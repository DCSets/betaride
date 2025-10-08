#ifndef SERVO_SIMPLE_H
#define SERVO_SIMPLE_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <structs.h>
#include <ConfiguratorHelpers.h>

struct ServoConfig : Resource
{
    int pin;
    int channel;
    int pwmFrequency;
    int pwmResolution;
    int maxAngle; // 0..180 degrees by default
    int minUs;   // microseconds for min angle
    int maxUs;   // microseconds for max angle
    int angleCenter; // use to shift center

    ServoConfig() = default;
    ServoConfig(String json)
    {
        JsonDocument doc;
        DeserializationError err = deserializeJson(doc, json);

        if (err)
        {
            Serial.println(F("JSON parse error - servo config"));
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
        this->minUs = doc["minUs"] | -1;
        this->maxUs = doc["maxUs"] | -1;
        this->angleCenter = doc["angleCenter"] | -1;
        this->maxAngle = doc["maxAngle"] | -1;
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
        doc["minUs"] = this->minUs;
        doc["maxUs"] = this->maxUs;
        doc["angleCenter"] = this->angleCenter;
        doc["maxAngle"] = this->maxAngle;

        serializeJson(doc, outJson);
    }
};

class Servo
{
public:
    Servo(const ServoConfig &config);

    // 0..180 degrees
    void setAngle(int degrees);
    void loop();
    int getMaxAngle() { return _config.maxAngle; }
    int getPreviousAngle() { return _previousAngle; }
    int getAngle() { return _angle; }
    int getMidAngle() { return _config.angleCenter; }
private:
    ServoConfig _config;
    int _angle;
    int _previousAngle;
};

#endif

