#include <Arduino.h>
#include <config.h>
#include <button_controller.h>

ButtonStateController::ButtonStateController(
    uint8_t pin,
    uint32_t debounceMs) : pin(pin), debounceMs(debounceMs)
{
}

void ButtonStateController::init()
{
    pinMode(pin, INPUT);
    attachInterruptArg(digitalPinToInterrupt(pin), isr, this, FALLING);
}

bool ButtonStateController::consumePress()
{
    if (!buttonPressed)
        return false;

    buttonPressed = false;
    return true;
}

void IRAM_ATTR ButtonStateController::isr(void *self_ref)
{
    auto *self = static_cast<ButtonStateController *>(self_ref);
    self->onInterrupt();
}

void IRAM_ATTR ButtonStateController::onInterrupt()
{
    uint32_t now = millis();

    if (now - debounceMs < lastPressedMs)
    {
        return;
    }

    lastPressedMs = now;
    buttonPressed = true;
}