#include <stdint.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "driver/uart.h"
#include "driver/gpio.h"

#define LED_PIN         GPIO_NUM_4
#define BTN_PIN         GPIO_NUM_5
#define TXD_PIN         GPIO_NUM_17
#define RXD_PIN         GPIO_NUM_18
#define UART_PORT       UART_NUM_1
#define UART_BAUD_RATE  115200

#define DEBOUNCE_US     20000
#define TX_PERIOD_US    50000
#define RX_TIMEOUT_US   250000

static const char *TAG = "BUTTON_UART";

static void init(void)
{
    const gpio_config_t led_config = {
        .pin_bit_mask = 1ULL << LED_PIN,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    ESP_ERROR_CHECK(gpio_config(&led_config));
    ESP_ERROR_CHECK(gpio_set_level(LED_PIN, 0));

    const gpio_config_t button_config = {
        .pin_bit_mask = 1ULL << BTN_PIN,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    ESP_ERROR_CHECK(gpio_config(&button_config));

    const uart_config_t uart_config = {
        .baud_rate = UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    ESP_ERROR_CHECK(uart_driver_install(UART_PORT, 2048, 0, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(UART_PORT, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(
        UART_PORT, TXD_PIN, RXD_PIN,
        UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
}

static void tx_task(void *arg)
{
    int previous_raw = gpio_get_level(BTN_PIN);
    int stable_level = 1;  // Start released; debounce initial input.
    int64_t changed_at = esp_timer_get_time();
    int64_t last_tx = changed_at;

    TickType_t poll_ticks = pdMS_TO_TICKS(10);
    if (poll_ticks == 0) {
        poll_ticks = 1;
    }
    TickType_t last_wake = xTaskGetTickCount();

    while (1) {
        const int64_t now = esp_timer_get_time();
        const int raw = gpio_get_level(BTN_PIN);

        if (raw != previous_raw) {
            previous_raw = raw;
            changed_at = now;
        }

        // Accept the input after it stays unchanged for 20 ms.
        if (now - changed_at >= DEBOUNCE_US) {
            stable_level = raw;
        }

        if (now - last_tx >= TX_PERIOD_US) {
            // Internal pull-up: LOW means pressed.
            const char state = (stable_level == 0) ? '1' : '0';

            if (uart_write_bytes(UART_PORT, &state, 1) != 1) {
                ESP_LOGE(TAG, "UART transmission failed");
            }
            last_tx = now;
        }

        vTaskDelayUntil(&last_wake, poll_ticks);
    }
}

static void rx_task(void *arg)
{
    uint8_t data[32];
    int led_state = 0;
    int64_t last_valid_rx = esp_timer_get_time();

    TickType_t read_ticks = pdMS_TO_TICKS(20);
    if (read_ticks == 0) {
        read_ticks = 1;
    }

    while (1) {
        const int count = uart_read_bytes(
            UART_PORT, data, sizeof(data), read_ticks);

        // UART is a byte stream; one read may contain multiple states.
        for (int i = 0; i < count; ++i) {
            if (data[i] != '0' && data[i] != '1') {
                continue;
            }

            last_valid_rx = esp_timer_get_time();
            const int received_state = (data[i] == '1');

            if (received_state != led_state) {
                led_state = received_state;
                ESP_ERROR_CHECK(gpio_set_level(LED_PIN, led_state));
                ESP_LOGI(TAG, "RX: LED %s", led_state ? "ON" : "OFF");
            }
        }

        // Turn off if valid state messages stop arriving.
        if (led_state &&
            esp_timer_get_time() - last_valid_rx >= RX_TIMEOUT_US) {
            led_state = 0;
            ESP_ERROR_CHECK(gpio_set_level(LED_PIN, 0));
            ESP_LOGW(TAG, "UART timeout: LED OFF");
        }
    }
}

void app_main(void)
{
    init();

    if (xTaskCreate(rx_task, "uart_rx", 3072, NULL, 5, NULL) != pdPASS) {
        abort();
    }
    if (xTaskCreate(tx_task, "uart_tx", 3072, NULL, 5, NULL) != pdPASS) {
        abort();
    }
}