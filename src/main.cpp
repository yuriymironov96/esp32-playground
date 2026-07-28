#include <Arduino.h>

// pins
constexpr uint8_t SLOWER_BUTTON_PIN = 0;
constexpr uint8_t FASTER_BUTTON_PIN = 36;
constexpr uint8_t RED_LED_PIN = 38;
constexpr uint8_t BLUE_LED_PIN = 45;

// blink intervals
constexpr unsigned long SLOW_BLINK_INTERVAL_MS = 2000;
constexpr unsigned long FAST_BLINK_INTERVAL_MS = 250;
constexpr unsigned long MIN_ACCELERATION_INTERVAL_MS = 100;

// rate of acceleration for the "special" mode
float ACCELERATION_RATIO = 0.8;

// button debounce
constexpr unsigned long DEBOUNCE_DELAY_MS = 50;

// button levels depend on the circuit wiring.
constexpr bool SLOWER_BUTTON_PRESSED_LEVEL = LOW;
constexpr bool FASTER_BUTTON_PRESSED_LEVEL = HIGH;

unsigned long blinkIntervalMs = SLOW_BLINK_INTERVAL_MS;
unsigned long previousBlinkTimeMs = 0;

bool redLedIsOn = true;
bool isAccelerating = false;

bool previousSlowerButtonState = !SLOWER_BUTTON_PRESSED_LEVEL;
bool previousFasterButtonState = !FASTER_BUTTON_PRESSED_LEVEL;

void updateBlinking();
void updateButtons();
void setLedStates();
void enableSlowMode();
void enableFastMode();
void enableAccelerationMode();

void setup()
{
    Serial.begin(9600);

    pinMode(SLOWER_BUTTON_PIN, INPUT);
    pinMode(FASTER_BUTTON_PIN, INPUT);

    pinMode(RED_LED_PIN, OUTPUT);
    pinMode(BLUE_LED_PIN, OUTPUT);

    setLedStates();

    Serial.println("ESP32 application started");
}

void loop()
{
    updateButtons();
    updateBlinking();
}

void updateBlinking()
{
    const unsigned long currentTimeMs = millis();

    if (currentTimeMs - previousBlinkTimeMs < blinkIntervalMs)
    {
        return;
    }

    previousBlinkTimeMs = currentTimeMs;

    redLedIsOn = !redLedIsOn;
    setLedStates();

    if (!isAccelerating)
    {
        return;
    }

    blinkIntervalMs =
        blinkIntervalMs * ACCELERATION_RATIO;

    if (blinkIntervalMs <= MIN_ACCELERATION_INTERVAL_MS)
    {
        blinkIntervalMs = SLOW_BLINK_INTERVAL_MS;
    }
}

void updateButtons()
{
    const bool currentSlowerButtonState =
        digitalRead(SLOWER_BUTTON_PIN);

    const bool currentFasterButtonState =
        digitalRead(FASTER_BUTTON_PIN);

    const bool slowerButtonIsHeld =
        currentSlowerButtonState == SLOWER_BUTTON_PRESSED_LEVEL;

    const bool fasterButtonIsHeld =
        currentFasterButtonState == FASTER_BUTTON_PRESSED_LEVEL;

    const bool slowerButtonWasPressed =
        slowerButtonIsHeld &&
        previousSlowerButtonState != SLOWER_BUTTON_PRESSED_LEVEL;

    const bool fasterButtonWasPressed =
        fasterButtonIsHeld &&
        previousFasterButtonState != FASTER_BUTTON_PRESSED_LEVEL;

    if (slowerButtonIsHeld && fasterButtonIsHeld)
    {
        if (!isAccelerating)
        {
            enableAccelerationMode();
        }
    }
    else if (slowerButtonWasPressed)
    {
        enableSlowMode();
    }
    else if (fasterButtonWasPressed)
    {
        enableFastMode();
    }

    previousSlowerButtonState = currentSlowerButtonState;
    previousFasterButtonState = currentFasterButtonState;

    delay(DEBOUNCE_DELAY_MS);
}

void setLedStates()
{
    digitalWrite(RED_LED_PIN, redLedIsOn ? HIGH : LOW);
    digitalWrite(BLUE_LED_PIN, redLedIsOn ? LOW : HIGH);
}

void enableSlowMode()
{
    blinkIntervalMs = SLOW_BLINK_INTERVAL_MS;
    isAccelerating = false;

    Serial.println("Slow mode enabled");
}

void enableFastMode()
{
    blinkIntervalMs = FAST_BLINK_INTERVAL_MS;
    isAccelerating = false;

    Serial.println("Fast mode enabled");
}

void enableAccelerationMode()
{
    blinkIntervalMs = SLOW_BLINK_INTERVAL_MS;
    isAccelerating = true;

    Serial.println("Acceleration mode enabled");
}