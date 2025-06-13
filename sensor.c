#include <stdio.h>
#include <stdint.h>
#include <bcm2835.h>
#include "scd4x_i2c.h"
#include "sensirion_i2c_hal.h"
#include "sensirion_common.h"
#include "sensirion_i2c.h"
#include "sensirion_config.h"
#include <string.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <i2c/smbus.h>
#include <time.h>

#define SCD4x_I2C_ADDRESS 0x62
#define ADDRESS_3 0x73

// Register for oxygen data
#define OXYGEN_DATA_REGISTER 0x03

// Register for users to configure key value manually
#define USER_SET_REGISTER 0x08

// Register for automatically configuring key value
#define AUTUAL_SET_REGISTER 0x09

// Register for obtaining key value
#define GET_KEY_REGISTER 0x0A

int init_i2c_bus(const char *filename) {
    int file;
    if ((file = open(filename, O_RDWR)) < 0) {
        perror("Failed to open the i2c bus");
        exit(1);
    }
    return file;
}

void set_i2c_address(int file, int addr) {
    if (ioctl(file, I2C_SLAVE, addr) < 0) {
        perror("Failed to acquire bus access and/or talk to slave");
        close(file);
        exit(1);
    }
}

void write_i2c_block_data(int file, int reg, const uint8_t *data, size_t length) {
    if (i2c_smbus_write_i2c_block_data(file, reg, length, data) < 0) {
        perror("Failed to write to the i2c bus");
        close(file);
        exit(1);
    }
}

void read_i2c_block_data(int file, int reg, uint8_t *data, size_t length) {
    if (i2c_smbus_read_i2c_block_data(file, reg, length, data) < 0) {
        perror("Failed to read from the i2c bus");
        close(file);
        exit(1);
    }
}

void custom_delay(int milliseconds) {
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    nanosleep(&ts, NULL);
}

float get_flash(int file) {
    uint8_t rslt[1];
    read_i2c_block_data(file, GET_KEY_REGISTER, rslt, 1);
    if (rslt[0] == 0) {
        return 20.9 / 120.0;
    } else {
        return rslt[0] / 1000.0;
    }
}

void calibrate(int file, float vol, float mv) {
    uint8_t txbuf[1];
    if (mv == 0) {
        txbuf[0] = (uint8_t)(vol * 10);
        write_i2c_block_data(file, USER_SET_REGISTER, txbuf, 1);
    } else {
        txbuf[0] = (uint8_t)((vol / mv) * 1000);
        write_i2c_block_data(file, AUTUAL_SET_REGISTER, txbuf, 1);
    }
}

float get_average_num(float *array, int len) {
    float temp = 0.0;
    for (int i = 0; i < len; i++) {
        temp += array[i];
    }
    return temp / (float)len;
}

float get_oxygen_data(int file, float key, int collect_num, float *oxygendata, int *count) {
    if (collect_num > 0) {
        for (int num = collect_num - 1; num > 0; num--) {
            oxygendata[num] = oxygendata[num - 1];
        }
        uint8_t rslt[3];
        read_i2c_block_data(file, OXYGEN_DATA_REGISTER, rslt, 3);
        oxygendata[0] = key * (rslt[0] + rslt[1] / 10.0 + rslt[2] / 100.0);
        if (*count < collect_num) {
            (*count)++;
        }
        return get_average_num(oxygendata, *count);
    } else if (collect_num > 100 || collect_num <= 0) {
        return -1;
    }
    return 0; // should never reach here
}

int main(void) {
    int16_t error = 0;

    sensirion_i2c_hal_init();

    // Clean up potential SCD41 states
    scd4x_wake_up();
    scd4x_stop_periodic_measurement();
    scd4x_reinit();

    uint16_t serial_0;
    uint16_t serial_1;
    uint16_t serial_2;
    error = scd4x_get_serial_number(&serial_0, &serial_1, &serial_2);
    if (error) {
        printf("Error executing scd4x_get_serial_number(): %i\n", error);
    } else {
        //printf("serial: 0x%04x%04x%04x\n", serial_0, serial_1, serial_2);
    }

    // Start Measurement
    error = scd4x_start_periodic_measurement();
    if (error) {
        printf("Error executing scd4x_start_periodic_measurement(): %i\n", error);
    }

    int sample_counter = 1;

    for (;;) {
        
        // Read Measurement if data is available
        bool data_ready_flag = false;
        sensirion_i2c_hal_sleep_usec(100000);
        error = scd4x_get_data_ready_flag(&data_ready_flag);
        if (error) {
            printf("Error executing scd4x_get_data_ready_flag(): %i\n", error);
            continue;
        }
        if (!data_ready_flag) {
            continue;
        }
        uint16_t co2;
        float temperature;
        float humidity;
        error = scd4x_read_measurement(&co2, &temperature, &humidity);
        if (error) {
            printf("Error executing scd4x_read_measurement(): %i\n", error);
        } else if (co2 == 0) {
            printf("Invalid sample detected, skipping.\n");
        } else {
            // Print sample number
            printf("Sample: %d\n", sample_counter);

            // Print the measurements
            printf("CO2: %.1f%%\n", co2 / 10000.0);
            printf("Temperature: %.1f °C\n", temperature);
            printf("Humidity: %.0f RH\n", humidity);

            // Increment the sample counter and reset if it reaches 5
          
        }
    
        int file = init_i2c_bus("/dev/i2c-1");
        set_i2c_address(file, ADDRESS_3);

        // Calibrate the sensor
        float OXYGEN_CONCENTRATION = 20.9; // The current concentration of oxygen in the air
        float OXYGEN_MV = 0; // The value marked on the sensor, assigned to 0
        calibrate(file, OXYGEN_CONCENTRATION, OXYGEN_MV);
        //printf("Sensor has been calibrated.\n");

        float key = get_flash(file);
        int collect_num = 20; // Example collect number
        float oxygendata[101] = {0};
        int count = 0;

        float oxygen = get_oxygen_data(file, key, collect_num, oxygendata, &count);
        printf("Oxygen concentration: %.1f%%\n", oxygen);
        custom_delay(1000); // Delay for 1 second
        printf("\n");
          sample_counter++;
            if (sample_counter > 17280) {
                sample_counter = 1;
            }
    }
    
    return 0;
}
