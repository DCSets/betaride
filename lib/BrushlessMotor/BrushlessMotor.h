#ifndef ESC_H
#define ESC_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <ConfiguratorHelpers.h>
#include <structs.h>
#include <enums.h>

struct BrushlessMotorConfig : Resource
{
    int pin;
    int channel;
    int pwmFrequency;
    int pwmResolution;
    int throttleMin;
    int throttleMax;
    int throttleCenter;
    int reverseTimeout;
    int is3D;
    BrushlessMotorConfig() = default;
    BrushlessMotorConfig(String json)
    {
        JsonDocument doc;
        DeserializationError err = deserializeJson(doc, json);

        if (err)
        {
            Serial.println(F("JSON parse error - brushless motor config"));
            return;
        }

        strncpy(this->id, doc["id"], sizeof(this->id));
        strncpy(this->name, doc["name"], sizeof(this->name));
        strncpy(this->type, doc["type"], sizeof(this->type));

        this->isNew = false;
        this->pin = doc["pin"] | -1;
        this->channel = doc["channel"] | -1;
        this->pwmFrequency = doc["pwmFrequency"] | -1;
        this->pwmResolution = doc["pwmResolution"] | -1;
        this->throttleMin = doc["throttleMin"] | -1;
        this->throttleMax = doc["throttleMax"] | -1;
        this->throttleCenter = doc["throttleCenter"] | -1;
        this->reverseTimeout = doc["reverseTimeout"] | -1;
        this->is3D = doc["is3D"] | -1;
    }

    void toJson(String &outJson) const
    {
        JsonDocument doc;

        doc["id"] = this->id;
        doc["isNew"] = this->isNew;
        doc["name"] = this->name;
        doc["type"] = this->type;
        doc["pin"] = this->pin;
        doc["channel"] = this->channel;
        doc["pwmFrequency"] = this->pwmFrequency;
        doc["pwmResolution"] = this->pwmResolution;
        doc["throttleMin"] = this->throttleMin;
        doc["throttleMax"] = this->throttleMax;
        doc["throttleCenter"] = this->throttleCenter;
        doc["reverseTimeout"] = this->reverseTimeout;
        doc["is3D"] = this->is3D;

        serializeJson(doc, outJson);
    }
};

class BrushlessMotor
{
public:
    BrushlessMotor(const BrushlessMotorConfig &config);
    void setThrottle(int percent);
    void setDirection(MotorDirection direction);
    const char *getId() const { return _config.id; }

    void loop();

private:
    BrushlessMotorConfig _config;
    MotorDirection _direction;

    int _throttleLvl;
    int _reverseAt;
};

#endif