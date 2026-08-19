#pragma once

#include <Arduino.h>
#include <config.h>

class ButtonStateController
{
public:
    ButtonStateController(
        uint8_t pin,
        uint32_t debounceMs = Config::DEBOUNCE_MS);
    void init();

    bool consumePress();

private:
    uint8_t pin;
    uint32_t debounceMs = Config::DEBOUNCE_MS;

    volatile bool buttonPressed = false;
    volatile uint32_t lastPressedMs = 0;
    static void IRAM_ATTR isr(void *self_ref);

    void IRAM_ATTR onInterrupt();
};