#include <Arduino.h>

constexpr uint8_t PIN_RELAY_CONTROL = 4;
constexpr uint8_t PIN_RELAY_CIRCUIT_READER = 5;

constexpr uint32_t RELAY_CHANGE_INTERVAL_MS = 1000;
constexpr uint32_t RELAY_DEBOUNCE_MS = 50;

// task said 10 measurements, but I tuned it down for quicker debug cycles
constexpr uint8_t MAX_MEASUREMENTS = 5;

// whether we received a meaningful signal from realy's NO-COM circuit
volatile bool hasRelaySentValue = false;
volatile uint32_t lastRelayReadMs = 0;

// whether we send IN signal to relay
volatile bool isRelayEnabled = false;

uint32_t lastRelayControlChangeMs = 0;

uint8_t measurementIterationCounter = 0;
bool experimentEnded = false;


void IRAM_ATTR isr()
{
    // while we essentially only nead RISING event,
    // it is more convenient to handle CHANGE because of the debounce.
    // If it is FALLING, we need to ignore completely,
    // if it is RISING, we need to do a regular debounce.
    uint32_t now = millis();

    if (isRelayEnabled)
    {
        return;
    }

    if (now - lastRelayReadMs >= RELAY_DEBOUNCE_MS)
    {
        hasRelaySentValue = true;
        lastRelayReadMs = now;
    }
}


void setup()
{
    Serial.begin(9600);

    pinMode(PIN_RELAY_CONTROL, OUTPUT);
    pinMode(PIN_RELAY_CIRCUIT_READER, INPUT_PULLDOWN);

    digitalWrite(PIN_RELAY_CONTROL, LOW);

    attachInterrupt(
        digitalPinToInterrupt(PIN_RELAY_CIRCUIT_READER),
        isr,
        CHANGE
    );

    delay(100);
}


void loop()
{
    if (experimentEnded)
    {
        return;
    }

    uint32_t now = millis();

    if (hasRelaySentValue)
    {
        hasRelaySentValue = false;

        Serial.println("Relay writes value!");

        Serial.print("MCU sent signal to relay at: ");
        Serial.print(lastRelayControlChangeMs);
        Serial.println(" ms");

        Serial.print("MCU received response at: ");
        Serial.print(lastRelayReadMs);
        Serial.println(" ms");

        Serial.print("Diff: ");
        Serial.print(lastRelayReadMs - lastRelayControlChangeMs);
        Serial.println(" ms");

        Serial.println("============");

        measurementIterationCounter++;

        if (measurementIterationCounter >= MAX_MEASUREMENTS)
        {
            // Stop only the interrupt used by this experiment instead of
            // disabling interrupts required by the ESP32 runtime.
            detachInterrupt(
                digitalPinToInterrupt(PIN_RELAY_CIRCUIT_READER)
            );

            digitalWrite(PIN_RELAY_CONTROL, LOW);
            isRelayEnabled = false;

            experimentEnded = true;

            Serial.println("Experiment ended");
            return;
        }
    }

    if (now - lastRelayControlChangeMs >= RELAY_CHANGE_INTERVAL_MS)
    {
        isRelayEnabled = !isRelayEnabled;

        digitalWrite(
            PIN_RELAY_CONTROL,
            isRelayEnabled ? HIGH : LOW
        );

        lastRelayControlChangeMs = now;
    }
}