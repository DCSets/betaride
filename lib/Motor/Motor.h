#ifndef MOTOR_H
#define MOTOR_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <structs.h>
#include <ConfiguratorHelpers.h>
#include "constants.h"
#include <enums.h>

struct MotorConfig : Resource
{
    int pin1;
    int pin2;
    int channel1;
    int channel2;
    int pwmFrequency;
    int pwmResolution;
    int reverseTimeout;

    MotorConfig() = default;
    MotorConfig(String json)
    {
        JsonDocument doc;
        if (!validateJsonHelper(json.c_str(), doc, "Motor config"))
            return;

        strncpy(this->id, doc["id"], sizeof(this->id));
        strncpy(this->name, doc["name"], sizeof(this->name));
        strncpy(this->type, doc["type"], sizeof(this->type));

        this->isNew = false;
        this->pin1 = doc["pin1"];
        this->pin2 = doc["pin2"];
        this->channel1 = doc["channel1"];
        this->channel2 = doc["channel2"];
        this->pwmFrequency = doc["pwmFrequency"];
        this->pwmResolution = doc["pwmResolution"];
        this->reverseTimeout = doc["reverseTimeout"];
    }

    void toJson(String &outJson) const
    {
        JsonDocument doc;

        doc["id"] = this->id;
        doc["isNew"] = this->isNew;
        doc["name"] = this->name;
        doc["type"] = this->type;
        doc["pin1"] = this->pin1;
        doc["pin2"] = this->pin2;
        doc["channel1"] = this->channel1;
        doc["channel2"] = this->channel2;
        doc["pwmFrequency"] = this->pwmFrequency;
        doc["pwmResolution"] = this->pwmResolution;
        doc["reverseTimeout"] = this->reverseTimeout;

        serializeJson(doc, outJson);
    }
};

class Motor
{
public:
    explicit Motor(const MotorConfig &config);

    void setThrottle(int percent);
    void setDirection(MotorDirection direction);
    const char *getId() const { return _config.id; }

    void loop();

private:
    MotorConfig _config;
    MotorDirection _direction;

    int _throttleLvl;
    int _reverseAt;
};

#endif
