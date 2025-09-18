#ifndef CONFIGURATOR_HELPERS_H
#define CONFIGURATOR_HELPERS_H

#include <Arduino.h>
#include <ArduinoJson.h>

inline boolean validateJsonHelper(const char *json, JsonDocument &doc, const char *errorMessage)
{
    DeserializationError err = deserializeJson(doc, json);

    if (err)
    {
        Serial.println("JSON parse error: " + String(errorMessage));
        return false;
    }

    return true;
}

inline uint32_t usToDuty(int microseconds, int resolution, int frequency)
{
    // Calculate the maximum duty cycle value based on resolution
    // For n-bit resolution, max value is 2^n - 1
    uint32_t maxDuty = (1UL << resolution) - 1;

    // Convert microseconds to duty cycle
    // ESC typically expects 1000-2000 microseconds (1-2ms pulse width)
    // Map microseconds to the full duty cycle range
    return (microseconds * maxDuty) / (1000000L / frequency);
}

inline uint32_t speedToPWM(int speed, int frequency, int resolution)
{
    uint32_t maxDuty = (1UL << resolution) - 1;
    return map(speed, 0, 100, 0, maxDuty);
}

struct TimerWrapper {
    std::function<void()> callback;
    TimerHandle_t timer;
};

static void timerCallback(TimerHandle_t xTimer) {
    TimerWrapper* wrapper = static_cast<TimerWrapper*>(pvTimerGetTimerID(xTimer));
    wrapper->callback();
    xTimerDelete(xTimer, 0);
    delete wrapper;
}

inline void setTimeout(std::function<void()> func, int delayMs) {
    TimerWrapper* wrapper = new TimerWrapper{func, nullptr};
    wrapper->timer = xTimerCreate("timeout",
                                  pdMS_TO_TICKS(delayMs),
                                  pdFALSE,
                                  wrapper,
                                  timerCallback);
    xTimerStart(wrapper->timer, 0);
}
#endif