#ifndef PS5_CONTROLLER_H
#define PS5_CONTROLLER_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <enums.h>
#include <ConfiguratorHelpers.h>
#include <structs.h>
#include <Controller.h>

struct PS5ControllerConfig : public ControllerConfig
{
    int sdaPin;
    int sclPin;
    int i2cAddress;
    PS5ControllerConfig() = default;
    PS5ControllerConfig(String json)
    {
        JsonDocument doc;
        DeserializationError err = deserializeJson(doc, json);

        if (err)
        {
            Serial.println(F("JSON parse error - PS5 controller config"));
            return;
        }

        this->isNew = false;
        this->controllerType = ControllerType::PS5;
        strncpy(this->id, doc["id"], sizeof(this->id));
        strncpy(this->name, doc["name"], sizeof(this->name));
        strncpy(this->type, doc["type"], sizeof(this->type));

        this->sdaPin = doc["sdaPin"];
        this->sclPin = doc["sclPin"];
        this->i2cAddress = doc["i2cAddress"];
    }

    void toJson(String &outJson) const
    {
        JsonDocument doc;

        doc["id"] = this->id;
        doc["name"] = this->name;
        doc["controllerType"] = this->controllerType;
        doc["type"] = this->type;
        doc["sdaPin"] = this->sdaPin;
        doc["sclPin"] = this->sclPin;
        doc["i2cAddress"] = this->i2cAddress;
        doc["isNew"] = this->isNew;

        serializeJson(doc, outJson);
    }
};

class PS5Controller : public Controller
{
private:
    PS5ControllerConfig _config;
    boolean _ready = false;
    // Mock channel data for testing
    int _mockChannels[16] = {1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 
                             1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500};

public:
    PS5Controller(PS5ControllerConfig config)
        : _config(config) {}

    void begin() override;
    void loop() override;
    void getAllChannels(int *outChannels) override;
};

#endif
