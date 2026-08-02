#include <Arduino.h>

#define BUTTON 4

int16_t counter = 0;
int16_t debounced_counter = 0;
int16_t debounce_ms = 100;
int16_t prev_pressed_at_ms = 0;

void IRAM_ATTR button_press()
{
    // naive counter - update value every time interrupt fires
    counter++;
    int16_t now = millis();
    Serial.println("\nButton Pressed (no debounce)! Count: " + String(counter));

    // debounced counter - skip if not enough time passed since the last event
    if (now > (prev_pressed_at_ms + debounce_ms))
    {
        debounced_counter++;
        Serial.println("\nButton Pressed! Count: " + String(debounced_counter));
        prev_pressed_at_ms = now;
    }
}

void setup()
{
    pinMode(BUTTON, INPUT);
    Serial.begin(115200);
    attachInterrupt(digitalPinToInterrupt(BUTTON), button_press, FALLING);
}

void loop()
{
    Serial.print("HELLO\r\n");
    delay(1000);
}