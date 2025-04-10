#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "driver/ledc.h"
#include "esp_log.h"
#include <string.h>

#define TAG "METAL_CAR"

// Motor GPIOs
#define IN1 GPIO_NUM_2
#define IN2 GPIO_NUM_4
#define IN3 GPIO_NUM_19
#define IN4 GPIO_NUM_18

#define ENA GPIO_NUM_32
#define ENB GPIO_NUM_22

// Metal detection input
#define WIRE1 GPIO_NUM_33
#define WIRE2 GPIO_NUM_34

// UART settings
#define GPS_TXD GPIO_NUM_26
#define GPS_RXD GPIO_NUM_25
#define GPS_UART UART_NUM_1
#define BUF_SIZE (1024)

// PWM settings
#define PWM_FREQ 1000
#define PWM_DUTY 128
#define PWM_RESOLUTION LEDC_TIMER_8_BIT

void init_pwm()
{
    ledc_timer_config_t timer = {
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
        .duty_resolution = PWM_RESOLUTION,
        .freq_hz = PWM_FREQ,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ledc_timer_config(&timer);

    ledc_channel_config_t ena_channel = {
        .gpio_num = ENA,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .timer_sel = LEDC_TIMER_0,
        .duty = PWM_DUTY,
        .hpoint = 0
    };
    ledc_channel_config(&ena_channel);

    ledc_channel_config_t enb_channel = ena_channel;
    enb_channel.gpio_num = ENB;
    enb_channel.channel = LEDC_CHANNEL_1;
    ledc_channel_config(&enb_channel);
}

void init_gpio()
{
    gpio_config_t io_conf = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1ULL << IN1) | (1ULL << IN2) | (1ULL << IN3) | (1ULL << IN4)
    };
    gpio_config(&io_conf);

    gpio_set_direction(WIRE1, GPIO_MODE_INPUT);
    gpio_set_direction(WIRE2, GPIO_MODE_INPUT);
}

void init_uart()
{
    const uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_driver_install(GPS_UART, BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(GPS_UART, &uart_config);
    uart_set_pin(GPS_UART, GPS_TXD, GPS_RXD, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}

void move_forward()
{
    gpio_set_level(IN1, 1);
    gpio_set_level(IN2, 0);
    gpio_set_level(IN3, 1);
    gpio_set_level(IN4, 0);
}

void move_left()
{
    gpio_set_level(IN1, 0);
    gpio_set_level(IN2, 1);
    gpio_set_level(IN3, 1);
    gpio_set_level(IN4, 0);
}

void move_right()
{
    gpio_set_level(IN1, 1);
    gpio_set_level(IN2, 0);
    gpio_set_level(IN3, 0);
    gpio_set_level(IN4, 1);
}

void stop_motors()
{
    gpio_set_level(IN1, 0);
    gpio_set_level(IN2, 0);
    gpio_set_level(IN3, 0);
    gpio_set_level(IN4, 0);
}

void stop_car_and_log_gps()
{
    stop_motors();
    ESP_LOGI(TAG, "Detected condition! Stopping for 10 seconds.");

    // Read and log GPS data (this is where GPS decoding would happen)
    uint8_t data[BUF_SIZE];
    int len = uart_read_bytes(GPS_UART, data, BUF_SIZE, pdMS_TO_TICKS(500));
    if (len > 0) {
        data[len] = '\0';  // Null terminate
        ESP_LOGI(TAG, "GPS Raw: %s", (char *)data);
        // Add GPS parsing logic here
    }

    vTaskDelay(pdMS_TO_TICKS(10000));  // Delay 10 seconds
}

void app_main()
{
    init_gpio();
    init_pwm();
    init_uart();

    ESP_LOGI(TAG, "System started. Waiting for GPS signal...");

    while (1)
    {
        for (int j = 0; j < 3; j++)
        {
            for (int i = 0; i < 15; i++)
            {
                move_forward();
                ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, PWM_DUTY);
                ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
                ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, PWM_DUTY);
                ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1);
                vTaskDelay(pdMS_TO_TICKS(500));

                if (gpio_get_level(WIRE1) == 1 && gpio_get_level(WIRE2) == 0)
                {
                    stop_car_and_log_gps();
                }
            }

            move_right();
            vTaskDelay(pdMS_TO_TICKS(1250));

            for (int i = 0; i < 15; i++)
            {
                move_forward();
                ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, PWM_DUTY);
                ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
                ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, PWM_DUTY);
                ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1);
                vTaskDelay(pdMS_TO_TICKS(500));

                if (gpio_get_level(WIRE1) == 1 && gpio_get_level(WIRE2) == 0)
                {
                    stop_car_and_log_gps();
                }
            }

            move_left();
            vTaskDelay(pdMS_TO_TICKS(1250));
        }

        stop_motors();
    }
}
