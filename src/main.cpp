#include <Arduino.h>
#include <config.h>
#include <button_controller.h>
#include <blink_controller.h>
#include <monitor.h>

BlinkController blinkController = BlinkController(LED_PIN);
ButtonStateController buttonStateController = ButtonStateController(BTN_PIN);

void setup()
{
    Serial.begin(Config::BAUD_RATE);
    blinkController.init();
    buttonStateController.init();

}

void loop()
{
    if (buttonStateController.consumePress())
    {
        blinkController.setNextState();
    }
    blinkController.refresh();

    if (printIfNthIteration()) {
        Serial.println("Serial monitor print!");
    }
}
