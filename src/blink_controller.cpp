#include <blink_controller.h>

BlinkController::BlinkController(uint8_t pin) : pin(pin)
{
    blinkState = BlinkPattern(1);
}

void BlinkController::init()
{
    pinMode(LED_PIN, OUTPUT);
}

void BlinkController::setNextState()
{
    switch (blinkState)
    {
    case BlinkPattern::BLINK:
        blinkState = BlinkPattern::ONLINE;
        digitalWrite(pin, LedState::ON);
        break;
    case BlinkPattern::ONLINE:
        blinkState = BlinkPattern::OFFLINE;
        digitalWrite(pin, LedState::OFF);
        break;
    default:
        blinkState = BlinkPattern::BLINK;
        break;
    }
}

BlinkPattern BlinkController::getCurrentState()
{
    return blinkState;
}

void BlinkController::refresh()
{
    uint32_t now = millis();
    if (blinkState != BlinkPattern::BLINK) return;

    if (now > lastToggleMs + blinkIntervalMs)
    {
        toggleLedState();
        lastToggleMs = now;
    }
}

void BlinkController::toggleLedState()
{
    currentLedState = currentLedState == LedState::ON ? LedState::OFF : LedState::ON;
    digitalWrite(pin, currentLedState);
}