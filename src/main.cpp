#include <Arduino.h>

const int adcPin = 4; // ESP32-S3 GPIO4: ADC1 channel

const int resolutions[] = {9, 10, 11, 12};

const adc_attenuation_t attenuations[] = {
    ADC_0db,
    ADC_2_5db,
    ADC_6db,
    ADC_11db
};

void calculateVoltage(int resolution, adc_attenuation_t attenuation);

void setup() {
    Serial.begin(115200);
    delay(1000);

    pinMode(adcPin, INPUT);
}

void loop() {
    for (int resolution : resolutions) {
        for (adc_attenuation_t attenuation : attenuations) {
            calculateVoltage(resolution, attenuation);
            delay(100);
        }
    }

    Serial.println("-----------------------------");
    delay(2000);
}

void calculateVoltage(
    int resolution,
    adc_attenuation_t attenuation
) {
    analogReadResolution(resolution);
    analogSetPinAttenuation(adcPin, attenuation);

    // pause after to let configs apply
    delay(10);

    int rawValue = analogRead(adcPin);

    // get max possible value for a given resultion by shifting bits to res+1, and subtracting 1.
    // Kind of like 2^n - 1
    int maxRawValue = (1 << resolution) - 1;

    // Rough theoretical conversion only.
    float calculatedVoltage =
        rawValue * (3.3f / static_cast<float>(maxRawValue));

    // ESP32 calibrated reading, returned in millivolts.
    uint32_t calibratedMilliVolts = analogReadMilliVolts(adcPin);
    float calibratedVoltage = calibratedMilliVolts / 1000.0f;

    float deviation = 0.0f;

    if (calibratedVoltage != 0.0f) {
        deviation =
            ((calculatedVoltage - calibratedVoltage)
             / calibratedVoltage)
            * 100.0f;
    }

    Serial.printf(
        "Resolution: %d-bit | Attenuation: %d | "
        "Raw: %d/%d | Calculated: %.3f V | "
        "Calibrated: %.3f V | Difference: %.2f%%\n",
        resolution,
        static_cast<int>(attenuation),
        rawValue,
        maxRawValue,
        calculatedVoltage,
        calibratedVoltage,
        deviation
    );
}