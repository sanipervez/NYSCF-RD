#include <stdio.h>
#include <wiringPi.h>
#include <unistd.h> // For usleep()

#define PWM_PIN 6 // WiringPi pin 7 corresponds to GPIO 13

int main(void) {
    if (wiringPiSetup() == -1) {
        fprintf(stderr, "WiringPi setup failed.\n");
        return 1;
    }

    // Set GPIO 13 as an output
    pinMode(PWM_PIN, OUTPUT);

    // Duration to run the loop (5 seconds)
    unsigned long startTime = millis();
    unsigned long runTime = 5000; // 5 seconds in milliseconds

    // Loop for 5 seconds
    while (millis() - startTime < runTime) {
        // Turn the GPIO pin on
        digitalWrite(PWM_PIN, HIGH);
        usleep(100000); // Sleep for 100 ms

        // Turn the GPIO pin off
        digitalWrite(PWM_PIN, LOW);
        usleep(100000); // Sleep for 100 ms
    }

    return 0;
}
