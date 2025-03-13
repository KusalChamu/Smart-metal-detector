#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_http_client.h"
#include "driver/uart.h"

// Wi-Fi credentials
#define WIFI_SSID "Your_WiFi_SSID"
#define WIFI_PASSWORD "Your_WiFi_Password"

// Pin definitions
#define METAL_DETECTOR_PIN GPIO_NUM_34  // ADC1_CH6 (GPIO34)

// UART configuration for GPS
#define GPS_UART_PORT UART_NUM_2
#define GPS_TX_PIN GPIO_NUM_17
#define GPS_RX_PIN GPIO_NUM_16
#define BUF_SIZE 1024

// Server URL
#define SERVER_URL "http://yourserver.com/notify?message=%s"

// Tag for logging
static const char* TAG = "Metal_Detector";

// Function to initialize Wi-Fi
void wifi_init_sta() {
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASSWORD,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "Wi-Fi initialized, connecting to AP...");
    ESP_ERROR_CHECK(esp_wifi_connect());
}

// Function to send data to the server
void send_to_app(const char* message) {
    esp_http_client_config_t config = {
        .url = SERVER_URL,
        .method = HTTP_METHOD_GET,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);

    char url[256];
    snprintf(url, sizeof(url), SERVER_URL, message);
    esp_http_client_set_url(client, url);

    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "Data sent to server: %s", message);
    } else {
        ESP_LOGE(TAG, "Failed to send data: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
}

// Function to read GPS data
void read_gps_data(char* buffer, int size) {
    int length = uart_read_bytes(GPS_UART_PORT, (uint8_t*)buffer, size - 1, pdMS_TO_TICKS(100));
    if (length > 0) {
        buffer[length] = '\0';  // Null-terminate the string
    }
}

// Main application task
void app_main() {
    // Initialize NVS (Non-Volatile Storage)
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Initialize Wi-Fi
    wifi_init_sta();

    // Configure ADC for metal detection sensor
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_DB_11);

    // Configure UART for GPS
    uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };
    uart_param_config(GPS_UART_PORT, &uart_config);
    uart_set_pin(GPS_UART_PORT, GPS_TX_PIN, GPS_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(GPS_UART_PORT, BUF_SIZE * 2, 0, 0, NULL, 0);

    char gps_data[BUF_SIZE] = {0};
    int metal_detection_count = 0;

    while (1) {
        // Read metal detection sensor value
        int sensor_value = adc1_get_raw(ADC1_CHANNEL_6);
        ESP_LOGI(TAG, "Sensor Value: %d", sensor_value);

        if (sensor_value > 2000) {  // Example threshold for metal detection
            ESP_LOGI(TAG, "Metal Detected!");

            // Read GPS data
            read_gps_data(gps_data, sizeof(gps_data));
            ESP_LOGI(TAG, "GPS Data: %s", gps_data);

            // Send GPS data to server
            char message[512];
            snprintf(message, sizeof(message), "Metal Detected at: %s", gps_data);
            send_to_app(message);

            metal_detection_count++;
            ESP_LOGI(TAG, "Total Metal Detections: %d", metal_detection_count);
        }

        vTaskDelay(pdMS_TO_TICKS(100));  // Delay 100ms
    }
}
