#ifndef CONFIGURATOR_HELPERS_H
#define CONFIGURATOR_HELPERS_H

#include <Arduino.h>
#include <ArduinoJson.h>

inline boolean validateJsonHelper(const char *json, JsonDocument &doc, const char *errorMessage) {
    DeserializationError err = deserializeJson(doc, json);

    if (err) {
        Serial.println("JSON parse error: " + String(errorMessage));
        return false;
    }

    return true;
}

inline uint32_t usToDuty(int microseconds, int resolution)
{
    // Calculate the maximum duty cycle value based on resolution
    // For n-bit resolution, max value is 2^n - 1
    uint32_t maxDuty = (1UL << resolution) - 1;

    // Convert microseconds to duty cycle
    // ESC typically expects 1000-2000 microseconds (1-2ms pulse width)
    // Map microseconds to the full duty cycle range
    return (microseconds * maxDuty) / 20000L;
}

#endif