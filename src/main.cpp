#include <stdio.h>
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_bt_api.h"
#include "esp_bt_device.h"
#include "esp_spp_api.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Define motor control pins
#define MR1 GPIO_NUM_12
#define MR2 GPIO_NUM_14
#define ML1 GPIO_NUM_27
#define ML2 GPIO_NUM_26

// Bluetooth Serial object
esp_spp_cb_t spp_cb;

// Received character
char receivedChar;

// Function prototypes
void forward();
void backward();
void left();
void right();
void stop();
void spp_callback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param);

void app_main() {
    // Initialize GPIO pins for motor control
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << MR1) | (1ULL << MR2) | (1ULL << ML1) | (1ULL << ML2);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);

    // Initialize Bluetooth
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    esp_bt_controller_init(&bt_cfg);
    esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT);
    esp_bluedroid_init();
    esp_bluedroid_enable();
    esp_spp_register_callback(spp_callback);
    esp_spp_init(ESP_SPP_MODE_CB);

    // Set Bluetooth device name
    esp_bt_dev_set_device_name("Metal Detector Robots");

    while (1) {
        // Main loop
        vTaskDelay(20 / portTICK_PERIOD_MS);
    }
}

// Motor control functions
void forward() {
    gpio_set_level(MR1, 1); // Right motor forward
    gpio_set_level(MR2, 0);
    gpio_set_level(ML1, 0); // Left motor forward
    gpio_set_level(ML2, 1);
}

void backward() {
    gpio_set_level(MR1, 0); // Right motor backward
    gpio_set_level(MR2, 1);
    gpio_set_level(ML1, 1); // Left motor backward
    gpio_set_level(ML2, 0);
}

void left() {
    gpio_set_level(MR1, 1); // Right motor forward
    gpio_set_level(MR2, 0);
    gpio_set_level(ML1, 1); // Left motor backward
    gpio_set_level(ML2, 0);
}

void right() {
    gpio_set_level(MR1, 0); // Right motor backward
    gpio_set_level(MR2, 1);
    gpio_set_level(ML1, 0); // Left motor forward
    gpio_set_level(ML2, 1);
}

void stop() {
    gpio_set_level(MR1, 0); // Stop both motors
    gpio_set_level(MR2, 0);
    gpio_set_level(ML1, 0);
    gpio_set_level(ML2, 0);
}

// Bluetooth SPP callback function
void spp_callback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param) {
    if (event == ESP_SPP_DATA_IND_EVT) {
        receivedChar = param->data_ind.data[0];
        ESP_LOGI("BT", "Received: %c", receivedChar);

        switch (receivedChar) {
            case 'F':
                forward();
                break;
            case 'G':
                backward();
                break;
            case 'L':
                left();
                break;
            case 'R':
                right();
                break;
            case 'S':
                stop();
                break;
            default:
                break;
        }
    }
}