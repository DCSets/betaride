#include "Controller.h"
#include "ELRSController.h"

Controller* ControllerFactory::createControllerFromJson(const String& json) {
    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, json);
    
    if (err) {
        Serial.println(F("JSON parse error - controller config"));
        return nullptr;
    }
    
    ControllerType controllerType = static_cast<ControllerType>(doc["controllerType"].as<int>());
    
    switch (controllerType) {
        case ControllerType::ELRS: {
            ELRSConfig config(json);
            return new ELRSController(config);
        }

        default:
            Serial.println(F("Unknown controller type"));
            return nullptr;
    }
}

Controller* ControllerFactory::createControllerFromConfig(const ControllerConfig& config) {
    switch (config.controllerType) {
        case ControllerType::ELRS: {
            const ELRSConfig& elrsConfig = static_cast<const ELRSConfig&>(config);
            return new ELRSController(elrsConfig);
        }

        default:
            Serial.println(F("Unknown controller type"));
            return nullptr;
    }
}
