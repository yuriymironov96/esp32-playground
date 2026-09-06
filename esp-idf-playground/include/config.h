#define ADC_GPIO 11
#define LED_GPIO 10


#define ADC_RESOLUTION_BITS 12
#define ADC_MAX_DIGITAL_VALUE ((1U << ADC_RESOLUTION_BITS) - 1U)

#define ADC_ATTENUATION ADC_ATTEN_DB_12

// naive max value for uncalibrated voltage formula
#define MAX_VOLTAGE_MV 3300.0F

#define SAMPLE_PERIOD_MS 50U