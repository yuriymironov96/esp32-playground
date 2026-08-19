#pragma once

#include <Arduino.h>

constexpr uint8_t LED_PIN = 5;
constexpr uint16_t BLINK_TIME = 500;
constexpr uint8_t BTN_PIN = 8;

class Config
{
public:
    static const uint8_t SHORT_PRESS_BLINK_TIMES = 5;
    static const uint32_t NTH_ITERATION_PRINT = 1000;
    static const uint32_t DEBOUNCE_MS = 50;
    static const ulong BAUD_RATE = 9600;
};