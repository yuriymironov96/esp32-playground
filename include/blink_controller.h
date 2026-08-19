#pragma once

#include <Arduino.h>
#include <config.h>

enum BlinkPattern {
    BLINK,
    ONLINE,
    OFFLINE
};

enum LedState {
    ON = HIGH,
    OFF = LOW,
};

class BlinkController
{
public:
    BlinkController(uint8_t pin);

    void init();

    void setNextState();

    BlinkPattern getCurrentState();

    void refresh();

private:
    uint8_t pin;
    BlinkPattern blinkState = BlinkPattern::OFFLINE;
    uint32_t lastToggleMs = 0;
    uint32_t blinkIntervalMs = 500;
    LedState currentLedState = LedState::OFF;
    void toggleLedState();
};
