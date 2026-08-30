#pragma once
#include <stdio.h>
#include <driver/gpio.h>

#define BUTTON_1_PIN GPIO_NUM_4
#define BUTTON_2_PIN GPIO_NUM_5
#define BUTTON_3_PIN GPIO_NUM_6
#define BUTTON_4_PIN GPIO_NUM_7
#define MONITOR_1_PIN GPIO_NUM_10
#define MONITOR_2_PIN GPIO_NUM_11
#define MONITOR_3_PIN GPIO_NUM_12
#define MONITOR_4_PIN GPIO_NUM_13

extern const gpio_num_t BUTTON_PINS[];
extern const uint8_t BUTTON_PINS_SIZE;

extern const gpio_num_t MONITOR_PINS[];
extern const uint8_t MONITOR_PINS_SIZE;