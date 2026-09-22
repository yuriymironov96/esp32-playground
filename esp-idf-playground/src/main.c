#include <stdio.h>
#include <stdint.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <driver/ledc.h>
#include <driver/gpio.h>
#include <esp_err.h>

#define SERVO_PIN       18
#define ENCODER_A_PIN   11
#define ENCODER_B_PIN   12

#define SERVO_MIN_US    1000
#define SERVO_MAX_US    2000
#define SERVO_PERIOD_US 20000

#define LEDC_MODE       LEDC_LOW_SPEED_MODE
#define LEDC_TIMER      LEDC_TIMER_0
#define LEDC_CHANNEL    LEDC_CHANNEL_0
#define LEDC_RESOLUTION LEDC_TIMER_10_BIT

#define ENCODER_TRANSITIONS_PER_STEP 4
#define DEGREES_PER_STEP             10

static QueueHandle_t encoder_queue;

static void servo_set_angle(int angle)
{
    uint32_t pulse_us =
        SERVO_MIN_US +
        ((SERVO_MAX_US - SERVO_MIN_US) * angle) / 180;

    uint32_t duty = (pulse_us * 1024UL) / SERVO_PERIOD_US;

    ESP_ERROR_CHECK(
        ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, duty)
    );
    ESP_ERROR_CHECK(
        ledc_update_duty(LEDC_MODE, LEDC_CHANNEL)
    );
}

static uint8_t encoder_get_state(void)
{
    return ((uint8_t)gpio_get_level(ENCODER_A_PIN) << 1) |
           (uint8_t)gpio_get_level(ENCODER_B_PIN);
}

static void IRAM_ATTR encoder_isr(void *argument)
{
    uint8_t state =
        ((uint8_t)gpio_get_level(ENCODER_A_PIN) << 1) |
        (uint8_t)gpio_get_level(ENCODER_B_PIN);

    BaseType_t wake_task = pdFALSE;
    xQueueSendFromISR(encoder_queue, &state, &wake_task);

    if (wake_task) {
        portYIELD_FROM_ISR();
    }
}

void app_main(void)
{
    ledc_timer_config_t timer = {
        .speed_mode      = LEDC_MODE,
        .timer_num       = LEDC_TIMER,
        .duty_resolution = LEDC_RESOLUTION,
        .freq_hz         = 50,
        .clk_cfg         = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&timer));

    ledc_channel_config_t channel = {
        .gpio_num   = SERVO_PIN,
        .speed_mode = LEDC_MODE,
        .channel    = LEDC_CHANNEL,
        .timer_sel  = LEDC_TIMER,
        .intr_type  = LEDC_INTR_DISABLE,
        .duty       = 77,
        .hpoint     = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&channel));

    gpio_config_t encoder_config = {
        .pin_bit_mask =
            (1ULL << ENCODER_A_PIN) |
            (1ULL << ENCODER_B_PIN),
        .mode         = GPIO_MODE_INPUT,
        .pull_up_en   = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_ANYEDGE
    };
    ESP_ERROR_CHECK(gpio_config(&encoder_config));

    encoder_queue = xQueueCreate(32, sizeof(uint8_t));
    ESP_ERROR_CHECK(encoder_queue == NULL ? ESP_ERR_NO_MEM : ESP_OK);

    ESP_ERROR_CHECK(gpio_install_isr_service(0));
    ESP_ERROR_CHECK(
        gpio_isr_handler_add(ENCODER_A_PIN, encoder_isr, NULL)
    );
    ESP_ERROR_CHECK(
        gpio_isr_handler_add(ENCODER_B_PIN, encoder_isr, NULL)
    );

    /*
     * Quadrature transition table.
     * Reverse all signs if the direction is backwards.
     */
    static const int8_t transition_table[16] = {
         0, -1,  1,  0,
         1,  0,  0, -1,
        -1,  0,  0,  1,
         0,  1, -1,  0
    };

    int angle = 90;
    int transition_count = 0;
    uint8_t previous_state = encoder_get_state();

    servo_set_angle(angle);
    printf("Servo angle: %d\n", angle);

    while (1) {
        uint8_t current_state;

        if (xQueueReceive(
                encoder_queue,
                &current_state,
                portMAX_DELAY) == pdTRUE) {

            int8_t movement = transition_table[
                (previous_state << 2) | current_state
            ];

            previous_state = current_state;
            transition_count += movement;

            if (transition_count >= ENCODER_TRANSITIONS_PER_STEP) {
                transition_count = 0;

                if (angle < 180) {
                    angle += DEGREES_PER_STEP;
                    if (angle > 180) {
                        angle = 180;
                    }

                    servo_set_angle(angle);
                    printf("Servo angle: %d\n", angle);
                }
            } else if (
                transition_count <= -ENCODER_TRANSITIONS_PER_STEP
            ) {
                transition_count = 0;

                if (angle > 0) {
                    angle -= DEGREES_PER_STEP;
                    if (angle < 0) {
                        angle = 0;
                    }

                    servo_set_angle(angle);
                    printf("Servo angle: %d\n", angle);
                }
            }
        }
    }
}