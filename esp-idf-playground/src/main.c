#include <math.h>
#include <stdio.h>

#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "config.h"
#include "adc.h"
#include "hysteresis_moving_average.h"
#include "driver/gpio.h"

void app_main(void)
{
    struct app_adc_handlers adc_config = init_adc();
    adc_oneshot_unit_handle_t adc_handle = adc_config.adc_handle;

    struct app_avg_state state = init_moving_average();

    bool current_state = false;

    gpio_reset_pin(LED_GPIO);
    gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);
    
    // wait for serial monitor to set up
    vTaskDelay(pdMS_TO_TICKS(SAMPLE_PERIOD_MS));

    while (true) {
        int value = get_calibrated_value(adc_handle, adc_config.channel, adc_config.calibration_handle);

        printf("Calibrated LDR value: %d\n", value);

        update_moving_average(&state, value);

        if (state.current_state != current_state) {
            printf("State change detected: prev: %d, next: %d\n", current_state, state.current_state);
            current_state = state.current_state;
            gpio_set_level(LED_GPIO, !current_state);
        }

        vTaskDelay(pdMS_TO_TICKS(SAMPLE_PERIOD_MS));
    }
}
