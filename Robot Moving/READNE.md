Robot will follow zig zag movement continously

First Include headerfiles
#include <stdio.h> → Includes the standard input/output library for debugging with printf().
#include "freertos/FreeRTOS.h" → Includes FreeRTOS, the real-time operating system running on ESP32.
#include "freertos/task.h" → Provides task management functions like vTaskDelay(), which replaces delay() in Arduino.
#include "driver/gpio.h" → Gives access to ESP32’s GPIO functions for controlling pins

Defines the GPIO pins that control the L298 motor driver.

IN1, IN2 → Control the left motor.
IN3, IN4 → Control the right motor

Set up GPIO Configuration
gpio_set_direction(pin, GPIO_MODE_OUTPUT); → Configures each pin as an output so that the ESP32 can send control signals.

Motor control function
IN1 = HIGH, IN2 = LOW → Left motor rotates forward.
IN3 = HIGH, IN4 = LOW → Right motor rotates forward.
This makes the car move straight ahead.

IN1 = LOW, IN2 = HIGH → Left motor rotates backward.
IN3 = HIGH, IN4 = LOW → Right motor rotates forward.
This creates a left turn

IN1 = HIGH, IN2 = LOW → Left motor rotates forward.
IN3 = LOW, IN4 = HIGH → Right motor rotates backward.
This creates a right turn.

IN1 = LOW, IN2 = LOW, IN3 = LOW, IN4 = LOW → No voltage is sent to the motors.
The car stops completely

Main Function

setup_gpio(); → Initializes GPIO pins before starting the loop.
while (1) {...} → Runs an infinite loop so the car continuously moves.
move_forward(); → Moves the car straight.
vTaskDelay(pdMS_TO_TICKS(1000)); → Delays for 1000 milliseconds (1 second).
turn_left(); → Turns the car left.
vTaskDelay(pdMS_TO_TICKS(500)); → Delays for 0.5 seconds.
Repeats the cycle to create a zig-zag motion
