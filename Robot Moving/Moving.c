#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

// Define motor control pins
#define IN1 26
#define IN2 27
#define IN3 14
#define IN4 12

// Function to configure GPIOs
void setup_gpio() {
    gpio_set_direction(IN1, GPIO_MODE_OUTPUT);
    gpio_set_direction(IN2, GPIO_MODE_OUTPUT);
    gpio_set_direction(IN3, GPIO_MODE_OUTPUT);
    gpio_set_direction(IN4, GPIO_MODE_OUTPUT);
}

// Function to move forward
void move_forward() {
    gpio_set_level(IN1, 1);
    gpio_set_level(IN2, 0);
    gpio_set_level(IN3, 1);
    gpio_set_level(IN4, 0);
}

// Function to turn left
void turn_left() {
    gpio_set_level(IN1, 0);
    gpio_set_level(IN2, 1);
    gpio_set_level(IN3, 1);
    gpio_set_level(IN4, 0);
}

// Function to turn right
void turn_right() {
    gpio_set_level(IN1, 1);
    gpio_set_level(IN2, 0);
    gpio_set_level(IN3, 0);
    gpio_set_level(IN4, 1);
}

// Function to stop the car
void stop_car() {
    gpio_set_level(IN1, 0);
    gpio_set_level(IN2, 0);
    gpio_set_level(IN3, 0);
    gpio_set_level(IN4, 0);
}

// Main task
void app_main() {
    setup_gpio();

    while (1) {
        move_forward();
        vTaskDelay(pdMS_TO_TICKS(1000));  // Move forward for 1 second

        turn_left();
        vTaskDelay(pdMS_TO_TICKS(500));   // Turn left for 0.5 seconds

        move_forward();
        vTaskDelay(pdMS_TO_TICKS(1000));  // Move forward for 1 second

        turn_right();
        vTaskDelay(pdMS_TO_TICKS(500));   // Turn right for 0.5 seconds
    }
}
