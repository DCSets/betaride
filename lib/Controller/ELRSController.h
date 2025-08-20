#ifndef ELRS_CONTROLLER_H
#define ELRS_CONTROLLER_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <AlfredoCRSF.h>
#include <HardwareSerial.h>
#include <enums.h>
#include <ConfiguratorHelpers.h>
#include <structs.h>
#include <Controller.h>

struct ELRSConfig : public ControllerConfig
{
    int rxPin;
    int txPin;
    int uart;
    ELRSConfig() = default;
    ELRSConfig(String json)
    {
        JsonDocument doc;
        DeserializationError err = deserializeJson(doc, json);

        if (err)
        {
            Serial.println(F("JSON parse error - ELRS RX config"));
            return;
        }

        this->isNew = false;
        this->controllerType = ControllerType::ELRS;
        strncpy(this->id, doc["id"], sizeof(this->id));
        strncpy(this->name, doc["name"], sizeof(this->name));
        strncpy(this->type, doc["type"], sizeof(this->type));
        this->rxPin = doc["rxPin"] | -1;
        this->txPin = doc["txPin"] | -1;
        this->uart = doc["uart"] | -1;
    }

    void toJson(String &outJson) const override
    {
        JsonDocument doc;

        doc["id"] = this->id;
        doc["name"] = this->name;
        doc["controllerType"] = this->controllerType;
        doc["type"] = this->type;
        doc["rxPin"] = this->rxPin;
        doc["txPin"] = this->txPin;
        doc["uart"] = this->uart;
        doc["isNew"] = this->isNew;

        serializeJson(doc, outJson);
    }
};

class ELRSController : public Controller
{
private:
    ELRSConfig _config;
    AlfredoCRSF _crsf;
    HardwareSerial _crsfSerial;
    boolean _ready = false;

public:
    ELRSController(ELRSConfig config)
        : _config(config),
          _crsf(),
          _crsfSerial(config.uart) {}

    void begin() override;
    void loop() override;
    void printAllChannels() override;
    int getChannelPercent(int channel, int min = MIN_STICK_VALUE, int max = MAX_STICK_VALUE) override;
    bool isConnected() override { return _ready && _crsf.isLinkUp(); }
};

#endif
