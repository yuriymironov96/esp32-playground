#include <math.h>
#include <stdio.h>

#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define ADC_GPIO 9


#define ADC_RESOLUTION_BITS 12
#define ADC_MAX_DIGITAL_VALUE ((1U << ADC_RESOLUTION_BITS) - 1U)

#define ADC_ATTENUATION ADC_ATTEN_DB_12

// naive max value for uncalibrated voltage formula
#define MAX_VOLTAGE_MV 3300.0F

#define SAMPLE_PERIOD_MS 500U

void app_main(void)
{
    // init pin-ADC coupling
    adc_unit_t unit;
    adc_channel_t channel;
    ESP_ERROR_CHECK(adc_oneshot_io_to_channel(ADC_GPIO, &unit, &channel));

    // init ADC driver instance
    adc_oneshot_unit_handle_t adc_handle;
    const adc_oneshot_unit_init_cfg_t unit_config = {
        .unit_id = unit,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&unit_config, &adc_handle));

    const adc_oneshot_chan_cfg_t channel_config = {
        .atten = ADC_ATTENUATION,
        .bitwidth = ADC_BITWIDTH_12,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, channel,
                                               &channel_config));

    // init a driver that will later help convert raw adc value into a calibrated one
    adc_cali_handle_t calibration_handle;
    const adc_cali_curve_fitting_config_t calibration_config = {
        .unit_id = unit,
        .chan = channel,
        .atten = ADC_ATTENUATION,
        .bitwidth = ADC_BITWIDTH_12,
    };
    ESP_ERROR_CHECK(adc_cali_create_scheme_curve_fitting(
        &calibration_config, &calibration_handle));
    
    // wait for serial monitor to set up
    vTaskDelay(pdMS_TO_TICKS(SAMPLE_PERIOD_MS));

    printf("GPIO: %d, resolution: %d bit, attenuation: 12 dB\n",
           ADC_GPIO, ADC_RESOLUTION_BITS);
    printf("RAW  U_manual(mV)  U_cali(mV)  Error(%%)\n");

    while (true) {
        int raw;
        int calibrated_mv;

        ESP_ERROR_CHECK(adc_oneshot_read(adc_handle, channel, &raw));
        ESP_ERROR_CHECK(adc_cali_raw_to_voltage(calibration_handle, raw,
                                                &calibrated_mv));

        const float manual_mv =
            (float)raw * MAX_VOLTAGE_MV / (float)ADC_MAX_DIGITAL_VALUE;

        float error_percent = 0.0F;
        if (calibrated_mv != 0) {
            error_percent = fabsf(manual_mv - calibrated_mv) /
                            calibrated_mv * 100.0F;
        }

        printf("%4d  %12.1f  %10d  %8.2f\n", raw, manual_mv,
               calibrated_mv, error_percent);

        vTaskDelay(pdMS_TO_TICKS(SAMPLE_PERIOD_MS));
    }
}
