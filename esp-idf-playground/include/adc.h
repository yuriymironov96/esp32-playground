#include "esp_adc/adc_oneshot.h"
#include "config.h"

struct app_adc_handlers
{
    adc_oneshot_unit_handle_t adc_handle;
    adc_cali_handle_t calibration_handle;
    adc_channel_t channel;
};

struct app_adc_handlers init_adc();

int get_calibrated_value(adc_oneshot_unit_handle_t adc_handle, adc_channel_t channel, adc_cali_handle_t calibration_handle);