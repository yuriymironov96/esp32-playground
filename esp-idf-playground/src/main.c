#include <stdio.h>
#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"


#define BUTTON_PIN  GPIO_NUM_4
#define MONITOR_PIN GPIO_NUM_10


volatile bool button_released = false;
bool monitor_state = false;


static void IRAM_ATTR button_isr(void *arg)
{
    button_released = true;
}


void app_main(void)
{
    gpio_config_t button_config = {
        .pin_bit_mask = 1ULL << BUTTON_PIN,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_POSEDGE,
    };

    gpio_config(&button_config);


    gpio_config_t monitor_config = {
        .pin_bit_mask = 1ULL << MONITOR_PIN,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    gpio_config(&monitor_config);

    gpio_set_level(MONITOR_PIN, 0);


    gpio_install_isr_service(0);

    gpio_isr_handler_add(
        BUTTON_PIN,
        button_isr,
        NULL
    );


    while (1)
    {
        if (button_released)
        {
            button_released = false;

            monitor_state = !monitor_state;

            gpio_set_level(
                MONITOR_PIN,
                (int)monitor_state
            );

            printf("Released -> %d\n", monitor_state);
        }

        vTaskDelay(1);
    }
}