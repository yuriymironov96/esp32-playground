#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <conf.h>
#include <init.h>

#include "esp_timer.h"


const uint32_t BUTTON_DEBOUNCE_MS = 50;
const uint32_t BUTTON_POLL_INTERVAL_MS = 5;


/*
 * BUTTON 1
 *
 * Interrupt only sets a flag.
 * Main loop processes the event.
 */

volatile bool should_toggle_button_1 = false;
bool is_button_1_high = false;


static void IRAM_ATTR button_1_isr(void *arg)
{
    should_toggle_button_1 = true;
}


/*
 * BUTTON 2
 *
 * Time-based debounce directly inside ISR.
 */

volatile bool should_toggle_button_2 = false;
bool is_button_2_high = false;

volatile int64_t last_button_2_toggle_ms = 0;


static void IRAM_ATTR button_2_isr(void *arg)
{
    int64_t now_ms = esp_timer_get_time() / 1000;

    if (
        now_ms - last_button_2_toggle_ms <
        BUTTON_DEBOUNCE_MS
    ) {
        return;
    }

    last_button_2_toggle_ms = now_ms;
    should_toggle_button_2 = true;
}


/*
 * BUTTON 3
 *
 * State-based debounce / event validation.
 *
 * ISR only reports that something happened.
 *
 * Main loop accepts the event only if the button
 * is currently released.
 *
 * Active-HIGH button:
 *
 *     pressed  = 1
 *     released = 0
 *
 * Therefore MONITOR_3_PIN toggles on release.
 */

volatile bool button_3_event = false;
bool is_button_3_high = false;


static void IRAM_ATTR button_3_isr(void *arg)
{
    button_3_event = true;
}


/*
 * BUTTON 4
 *
 * Polling + finite state machine debounce.
 */

enum BtnState
{
    OFF,
    IS_PRESSING,
    ON,
    IS_RELEASING,
};


enum BtnState btn_4_state = OFF;

uint32_t button_4_last_poll_ms = 0;
uint32_t button_4_debounce_started_ms = 0;

bool is_button_4_high = false;


void update_btn_4_state(void)
{
    uint32_t now_ms =
        (uint32_t)(esp_timer_get_time() / 1000);


    /*
     * Poll BUTTON_4_PIN only once every
     * BUTTON_POLL_INTERVAL_MS.
     */
    if (
        now_ms - button_4_last_poll_ms <
        BUTTON_POLL_INTERVAL_MS
    ) {
        return;
    }

    button_4_last_poll_ms = now_ms;


    int pin_state = gpio_get_level(BUTTON_4_PIN);


    switch (btn_4_state)
    {
        /*
         * Button is confirmed released.
         */
        case OFF:
        {
            /*
             * HIGH means a possible press started.
             */
            if (pin_state == 1)
            {
                btn_4_state = IS_PRESSING;
                button_4_debounce_started_ms = now_ms;
            }

            break;
        }


        /*
         * We detected HIGH and are checking
         * whether the button remains HIGH long
         * enough to be considered pressed.
         */
        case IS_PRESSING:
        {
            /*
             * It returned LOW too soon.
             * Treat that as bounce.
             */
            if (pin_state == 0)
            {
                btn_4_state = OFF;
            }
            else if (
                now_ms - button_4_debounce_started_ms >=
                BUTTON_DEBOUNCE_MS
            )
            {
                /*
                 * Button remained HIGH for the
                 * complete debounce period.
                 */
                btn_4_state = ON;
            }

            break;
        }


        /*
         * Button is confirmed pressed.
         */
        case ON:
        {
            /*
             * LOW means a possible release started.
             */
            if (pin_state == 0)
            {
                btn_4_state = IS_RELEASING;
                button_4_debounce_started_ms = now_ms;
            }

            break;
        }


        /*
         * We detected LOW and are checking
         * whether the button remains LOW long
         * enough to be considered released.
         */
        case IS_RELEASING:
        {
            /*
             * Returned HIGH too soon.
             * Treat that as bounce.
             */
            if (pin_state == 1)
            {
                btn_4_state = ON;
            }
            else if (
                now_ms - button_4_debounce_started_ms >=
                BUTTON_DEBOUNCE_MS
            )
            {
                /*
                 * Button remained LOW for the
                 * complete debounce period.
                 *
                 * Release is now confirmed.
                 */
                btn_4_state = OFF;

                is_button_4_high =
                    !is_button_4_high;

                gpio_set_level(
                    MONITOR_4_PIN,
                    (int)is_button_4_high
                );
            }

            break;
        }


        default:
        {
            btn_4_state = OFF;
            break;
        }
    }
}


void app_main(void)
{
    init_gpio();


    gpio_isr_handler_add(
        BUTTON_1_PIN,
        button_1_isr,
        NULL
    );

    gpio_isr_handler_add(
        BUTTON_2_PIN,
        button_2_isr,
        NULL
    );

    gpio_isr_handler_add(
        BUTTON_3_PIN,
        button_3_isr,
        NULL
    );


    printf("Hello world!\n");


    while (1)
    {
        if (should_toggle_button_1)
        {
            should_toggle_button_1 = false;

            is_button_1_high =
                !is_button_1_high;

            gpio_set_level(
                MONITOR_1_PIN,
                (int)is_button_1_high
            );
        }

        if (should_toggle_button_2)
        {
            should_toggle_button_2 = false;

            is_button_2_high =
                !is_button_2_high;

            gpio_set_level(
                MONITOR_2_PIN,
                (int)is_button_2_high
            );
        }

        if (button_3_event)
        {
            button_3_event = false;
            if (gpio_get_level(BUTTON_3_PIN) == 0)
            {
                is_button_3_high =
                    !is_button_3_high;

                gpio_set_level(
                    MONITOR_3_PIN,
                    (int)is_button_3_high
                );
            }
        }

        update_btn_4_state();


        vTaskDelay(pdMS_TO_TICKS(1));
    }
}
