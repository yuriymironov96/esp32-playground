#include <conf.h>

const gpio_num_t BUTTON_PINS[] = {
    BUTTON_1_PIN,
    BUTTON_2_PIN,
    BUTTON_3_PIN,
    BUTTON_4_PIN,
};
const uint8_t BUTTON_PINS_SIZE = sizeof(BUTTON_PINS) / sizeof(BUTTON_1_PIN);

const gpio_num_t MONITOR_PINS[] = {
    MONITOR_1_PIN,
    MONITOR_2_PIN,
    MONITOR_3_PIN,
    MONITOR_4_PIN,
};
const uint8_t MONITOR_PINS_SIZE = sizeof(MONITOR_PINS) / sizeof(MONITOR_1_PIN);