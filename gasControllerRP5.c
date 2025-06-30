#include "ui/landing_page.h"

#include "ui/calibration_page.h"

#include "ui/file_setup_page.h"

#include "ui/passcode_page.h"

#include <gtk/gtk.h> //Used to create graphical user interface 

#include <stdio.h>

#include <stdint.h>

#include <wiringPi.h> //Responsible for communication between Raspberry Pi 5 

#include <wiringPiI2C.h> //as well as the sensors 

#include "scd4x_i2c.h"

#include "sensirion_i2c_hal.h"  //Carbon Dioxide/ Temperature/ Humidity Sensor 

#include "sensirion_common.h" //Repositories

#include "sensirion_i2c.h"

#include "sensirion_config.h"

#include <string.h>

#include <stdlib.h>

#include <linux/i2c-dev.h>

#include <fcntl.h>

#include <unistd.h> //Repository from GitHub was originally in Python. AI was 

#include <sys/ioctl.h> //used in order to make conversion, these were the libraries

#include <i2c/smbus.h> //that most aligned with the previously used headers.

#include <time.h>


#define SCD4xI2CADDRESS 0x62         //SCD41 Sensor- Co2/ temperature/ humidity Address

#define ADDRESS3 0x73

#define OXYGENDATAREGISTER 0x03

#define USERSETREGISTER 0x08 //Oxygen sensor related addresses

#define AUTUALSETREGISTER 0x09

#define GETKEYREGISTER 0x0A


#define COMPRESSEDAIRPIN 27 //GPIO 16 - Physical Pin 36

#define NITROGENPIN 5 //GPIO 24 - Physical Pin 18

#define CO2PIN 6 //GPIO 25 - Physical Pin 22


// GPIO Pins for buttons

#define INCREASEO2 0 // WiringPi pin 0 (GPIO 17) Physical Pin 11

#define DECREASEO2 2 // WiringPi pin 2 (GPIO 27) Physical Pin 13

#define INCREASECO2 3 // WiringPi pin 3 (GPIO 22) Physical Pin 15
#include "passcode_page.h"
#define DECREASECO2 12 // WiringPi pin 12 (GPIO 10) Physical Pin 19


GtkWidget *labelco2;

GtkWidget *labelo2;     

GtkWidget *labeltemp;

GtkWidget *labelhum;

GtkWidget *labelsample; // Pointers- address of buttons or labels 

int counter = 1;



void initialize_gpio_pwm() {

    if (wiringPiSetup() == -1) {

        printf("wiringPiSetup failed. Are you running as root?\n");

        exit(1);

    }


    pinMode(NITROGENPIN, OUTPUT);

    pinMode(COMPRESSEDAIRPIN, OUTPUT);

    pinMode(CO2PIN, OUTPUT);


    pinMode(INCREASEO2, INPUT);

    pinMode(DECREASEO2, INPUT);

    pinMode(INCREASECO2, INPUT);

    pinMode(DECREASECO2, INPUT);


    pullUpDnControl(INCREASEO2, PUD_UP); // LOW when button is selected- otherwise HIGH

    pullUpDnControl(DECREASEO2, PUD_UP);

    pullUpDnControl(INCREASECO2, PUD_UP);

    pullUpDnControl(DECREASECO2, PUD_UP);

}

void user(float duty_cycle, uint8_t pin) {

    int cycleTime = 1000; // 1 second

    int on =(cycleTime * duty_cycle); // Duty cycle duration

    int off = cycleTime - on; // Remaining duration


    for (int i = 0; i < 10; i++) { // Loop for 10 cycles ?? 

        digitalWrite(pin, HIGH); 

        usleep(on*100); 

        digitalWrite(pin,LOW);

        usleep(off*100); 

    }

}


void toggle_relay(uint8_t pin) {

    if (pin == COMPRESSEDAIRPIN) {

        user(0.20, COMPRESSEDAIRPIN); // 20% duty cycle

    } else if (pin == NITROGENPIN) {

        user(0.77, NITROGENPIN); // 77% duty cycle

    } else if (pin == CO2PIN) {

        user(0.02, CO2PIN); // 2% duty cycle

    }


}


void handle_gpio_input() {

    if (digitalRead(INCREASEO2) == LOW) {

        toggle_relay(COMPRESSEDAIRPIN);

        printf("Increase O2 button pressed\n"); // Writes in Script

    }

    if (digitalRead(DECREASEO2) == LOW) {

        toggle_relay(NITROGENPIN);

        printf("Decrease O2 button pressed\n");

    }

    if (digitalRead(INCREASECO2) == LOW) {

        toggle_relay(CO2PIN);

        printf("Increase CO2 button pressed\n");

    }

    if (digitalRead(DECREASECO2) == LOW) {

        toggle_relay(NITROGENPIN);

        printf("Decrease CO2 button pressed\n");

    }

}


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

    }}


void custom_delay(int milliseconds) { // seconds being converted to milliseconds

    struct timespec ts;

    ts.tv_sec = milliseconds / 1000;

    ts.tv_nsec = (milliseconds % 1000) * 1000000;

    nanosleep(&ts, NULL);

}


float get_flash(int file) {

    uint8_t rslt[1];

    read_i2c_block_data(file, GETKEYREGISTER, rslt, 1);

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

        write_i2c_block_data(file, USERSETREGISTER, txbuf, 1);

    } else {

        txbuf[0] = (uint8_t)((vol / mv) * 1000);

        write_i2c_block_data(file, AUTUALSETREGISTER, txbuf, 1);

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

        read_i2c_block_data(file, OXYGENDATAREGISTER, rslt, 3);

        oxygendata[0] = key * (rslt[0] + rslt[1] / 10.0 + rslt[2] / 100.0);

        if (*count < collect_num) {

            (*count)++;

        }

        return get_average_num(oxygendata, *count);

    } else if (collect_num > 100 || collect_num <= 0) {

        return -1;

    }

    return 0;

}


void update_sensor_readings() {

    int16_t error = 0;

    bool data_ready_flag = false;

    int file;


    sensirion_i2c_hal_sleep_usec(100000);

    error = scd4x_get_data_ready_status(&data_ready_flag);


    if (error || !data_ready_flag) {

        return;

    }


    uint16_t co2;

    int32_t temperature_raw, humidity_raw;
    
    error = scd4x_read_measurement(&co2, &temperature_raw, &humidity_raw);
    
    float temperature = temperature_raw / 1000.0f;
    
    float humidity = humidity_raw / 1000.0f;


    if (error || co2 == 0) {

        return;

    }


    file = init_i2c_bus("/dev/i2c-1");

    set_i2c_address(file, ADDRESS3);


    float OXYGEN_CONCENTRATION = 20.9;

    float OXYGEN_MV = 0;

    calibrate(file, OXYGEN_CONCENTRATION, OXYGEN_MV);


    float key = get_flash(file);

    int collect_num = 20;

    float oxygendata[101] = {0};

    int count = 0;


    float oxygen = get_oxygen_data(file, key, collect_num, oxygendata, &count);


    char buffer[128];

    snprintf(buffer, sizeof(buffer), "Sample: %d", counter); // decimal integer 

    gtk_label_set_text(GTK_LABEL(labelsample), buffer);


    snprintf(buffer, sizeof(buffer), "Carbon Dioxide: %.2f%%", co2 / 10000.0); 

// 2 decimal point floating value

    gtk_label_set_text(GTK_LABEL(labelco2), buffer);


    snprintf(buffer, sizeof(buffer), "Oxygen: %.1f%%", oxygen);

    gtk_label_set_text(GTK_LABEL(labelo2), buffer);


    snprintf(buffer, sizeof(buffer), "Temperature: %.1f °C", temperature);

    gtk_label_set_text(GTK_LABEL(labeltemp), buffer);


    snprintf(buffer, sizeof(buffer), "Humidity: %.0f %%", humidity);

    gtk_label_set_text(GTK_LABEL(labelhum), buffer);


    close(file);


    counter++;

    if (counter > 17280) { // 17,280 5 second increments in 24 hours. 5 seconds is how long it 

// takes sensor to collect and output readings 

        counter = 1;

    }

}


gboolean timer_callback(gpointer data) {

    update_sensor_readings();

    handle_gpio_input(); // Handle GPIO input every second

    return TRUE;

} // ensures sensor readings are being taken periodically


void increase_o2(GtkWidget *widget, gpointer data) {

    printf("Increase O2 button clicked\n");

    toggle_relay(COMPRESSEDAIRPIN);

}


void decrease_o2(GtkWidget *widget, gpointer data) {

    printf("Decrease O2 button clicked\n");

    toggle_relay(NITROGENPIN);

}


void increase_co2(GtkWidget *widget, gpointer data) {

    printf("Increase CO2 button clicked\n");

    toggle_relay(CO2PIN);}


void decrease_co2(GtkWidget *widget, gpointer data) {

    printf("Decrease CO2 button clicked\n");

    toggle_relay(NITROGENPIN);

}

 // responsible for the visual display of the GUI
static void on_dropdown_changed(GtkComboBoxText *combo, gpointer user_data) {
    GtkStack *stack = GTK_STACK(user_data);
    const gchar *selected = gtk_combo_box_text_get_active_text(combo);

    if (g_strcmp0(selected, "Home") == 0) {
        gtk_stack_set_visible_child_name(stack, "landing");
    } else if (g_strcmp0(selected, "Sensor Dashboard") == 0) {
        gtk_stack_set_visible_child_name(stack, "sensors");
    } else if (g_strcmp0(selected, "Calibration") == 0) {
        gtk_stack_set_visible_child_name(stack, "calibration");
    } else if (g_strcmp0(selected, "File Setup") == 0) {
        gtk_stack_set_visible_child_name(stack, "file_setup");
    } else if (g_strcmp0(selected, "Passcode") == 0) {
        gtk_stack_set_visible_child_name(stack, "passcode");
    }
}

static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window;
    GtkWidget *main_vbox;
    GtkWidget *stack;
    GtkWidget *dropdown;

    // Load CSS
    GtkCssProvider *css_provider = gtk_css_provider_new();
    GdkDisplay *display = gdk_display_get_default();
    GdkScreen *screen = gdk_display_get_default_screen(display);
    GError *error = NULL;
    gtk_css_provider_load_from_path(css_provider, "style.css", &error);
    if (error) {
        g_warning("Failed to load CSS file: %s", error->message);
        g_error_free(error);
    }
    gtk_style_context_add_provider_for_screen(
        screen,
        GTK_STYLE_PROVIDER(css_provider),
        GTK_STYLE_PROVIDER_PRIORITY_USER
    );

    // Create the window
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Tri-Gas Controller");
    gtk_window_set_default_size(GTK_WINDOW(window), 1024, 600);
    gtk_window_fullscreen(GTK_WINDOW(window));

    // Main vertical box
    main_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window), main_vbox);

    // Dropdown for page navigation
    dropdown = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(dropdown), "Home");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(dropdown), "Sensor Dashboard");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(dropdown), "Calibration");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(dropdown), "File Setup");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(dropdown), "Passcode");
    gtk_combo_box_set_active(GTK_COMBO_BOX(dropdown), 0); // default to "Home"

    gtk_box_pack_start(GTK_BOX(main_vbox), dropdown, FALSE, FALSE, 5);

    // Create the stack
    stack = gtk_stack_new();
    gtk_stack_set_transition_type(GTK_STACK(stack), GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);
    gtk_stack_set_transition_duration(GTK_STACK(stack), 300);
    gtk_box_pack_start(GTK_BOX(main_vbox), stack, TRUE, TRUE, 0);

    // Home Page
    GtkWidget *landing_page = create_landing_page();
    

    // Sensor Dashboard Page (your original grid)
    GtkWidget *sensor_grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(sensor_grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(sensor_grid), 10);

    labelsample = gtk_label_new("Sample: 0");
    labelco2 = gtk_label_new("Carbon Dioxide: ");
    labelo2 = gtk_label_new("Oxygen: ");
    labeltemp = gtk_label_new("Temperature: ");
    labelhum = gtk_label_new("Humidity: ");

    gtk_grid_attach(GTK_GRID(sensor_grid), labelsample, 0, 0, 2, 1);
    gtk_grid_attach(GTK_GRID(sensor_grid), labeltemp, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(sensor_grid), labelhum, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(sensor_grid), labelco2, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(sensor_grid), labelo2, 1, 2, 1, 1);

    GtkWidget *binco2 = gtk_button_new_with_label("Increase O2");
    GtkWidget *bdeco2 = gtk_button_new_with_label("Decrease O2");
    GtkWidget *bincco2 = gtk_button_new_with_label("Increase CO2");
    GtkWidget *bdecco2 = gtk_button_new_with_label("Decrease CO2");

    gtk_grid_attach(GTK_GRID(sensor_grid), binco2, 0, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(sensor_grid), bdeco2, 1, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(sensor_grid), bincco2, 0, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(sensor_grid), bdecco2, 1, 4, 1, 1);

    g_signal_connect(binco2, "clicked", G_CALLBACK(increase_o2), NULL);
    g_signal_connect(bdeco2, "clicked", G_CALLBACK(decrease_o2), NULL);
    g_signal_connect(bincco2, "clicked", G_CALLBACK(increase_co2), NULL);
    g_signal_connect(bdecco2, "clicked", G_CALLBACK(decrease_co2), NULL);

    // Calibration Page
    GtkWidget *calibration = create_calibration_page();


    // File Setup Page
    GtkWidget *file_setup = create_file_setup_page();
    
    //Passcodem Page
    GtkWidget *passcode = create_passcode_page();   
    

    // Add pages to stack
    gtk_stack_add_titled(GTK_STACK(stack), landing_page, "landing", "Home");
    gtk_stack_add_titled(GTK_STACK(stack), sensor_grid, "sensors", "Sensor Dashboard");
    gtk_stack_add_titled(GTK_STACK(stack), calibration, "calibration", "Calibration");
    gtk_stack_add_titled(GTK_STACK(stack), file_setup, "file_setup", "File Setup");
    gtk_stack_add_titled(GTK_STACK(stack), passcode, "passcode", "Passcode");

    // Signal for dropdown navigation
    g_signal_connect(dropdown, "changed", G_CALLBACK(on_dropdown_changed), stack);

    gtk_widget_show_all(window);
    g_timeout_add(1000, timer_callback, NULL);
}




int main(int argc, char **argv) {

    GtkApplication *app;

    int status;

// Main initializes everything to ensure it is functioning properly. 


    initialize_gpio_pwm();


    sensirion_i2c_hal_init();


    app = gtk_application_new("org.gtk.example", G_APPLICATION_DEFAULT_FLAGS); //creates GUI environment 

    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

//activate is called so that the GUI appears the way we programmed

    status = g_application_run(G_APPLICATION(app), argc, argv);

    g_object_unref(app);

//clean shutdown

    return status;

}
