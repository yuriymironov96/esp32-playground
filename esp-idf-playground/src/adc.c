#include "adc.h"

struct app_adc_handlers init_adc()
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

    struct app_adc_handlers handlers;
    handlers.adc_handle = adc_handle;
    handlers.calibration_handle = calibration_handle;
    handlers.channel = channel;

    return handlers;
}

int get_calibrated_value(adc_oneshot_unit_handle_t adc_handle, adc_channel_t channel, adc_cali_handle_t calibration_handle)
{
    int raw;
    int calibrated_mv;

    ESP_ERROR_CHECK(adc_oneshot_read(adc_handle, channel, &raw));
    ESP_ERROR_CHECK(adc_cali_raw_to_voltage(calibration_handle, raw,
                                            &calibrated_mv));

    return calibrated_mv;
}