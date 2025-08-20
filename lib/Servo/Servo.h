#ifndef SERVO_SIMPLE_H
#define SERVO_SIMPLE_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <structs.h>


struct ServoConfig : Resource
{
    int pin;
    int channel;
    int frequency;
    int resolution;
    int angleMinUs;   // microseconds for min angle
    int angleMaxUs;   // microseconds for max angle
    int angleCenterUs; // center microseconds
    int angle;        // 0..180 degrees

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
        this->frequency = doc["frequency"] | -1;
        this->resolution = doc["resolution"] | -1;
        this->angleMinUs = doc["angleMinUs"] | -1;
        this->angleMaxUs = doc["angleMaxUs"] | -1;
        this->angleCenterUs = doc["angleCenterUs"] | -1;
        this->angle = doc["angle"] | -1;
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
        doc["frequency"] = this->frequency;
        doc["resolution"] = this->resolution;
        doc["angleMinUs"] = this->angleMinUs;
        doc["angleMaxUs"] = this->angleMaxUs;
        doc["angleCenterUs"] = this->angleCenterUs;
        doc["angle"] = this->angle;

        serializeJson(doc, outJson);
    }
};

class ServoSimple
{
public:
    explicit ServoSimple(const ServoConfig &config);

    // 0..180 degrees
    void setAngle(int degrees);
    const char *getId() const { return _config.id; }
    void loop();

private:
    ServoConfig _config;
    uint32_t usToDuty(int microseconds);
};

#endif

