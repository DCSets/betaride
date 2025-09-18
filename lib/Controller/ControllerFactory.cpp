#include "Controller.h"
#include "ELRSController.h"
#include "PS5_Controller.h"
#include "PS4_Controller.h"

Controller* ControllerFactory::createControllerFromJson(const String& json) {
    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, json);
    
    if (err) {
        Serial.println(F("JSON parse error - controller config"));
        return nullptr;
    }
    
    ControllerType controllerType = static_cast<ControllerType>(doc["controllerType"] | -1);
    
    switch (controllerType) {
        case ControllerType::ELRS: {
            ELRSConfig config(json);
            return new ELRSController(config);
        }
        case ControllerType::PS5: {
            PS5ControllerConfig config(json);
            return new PS5_Controller(config);
        }
        case ControllerType::PS4_DUALSHOCK: {
            // TODO: NEED TO SWITCH TO BLUEPAD32
            // PS4ControllerConfig config(json);
            // return new PS4_Controller(config);
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
        case ControllerType::PS5: {
            const PS5ControllerConfig& ps5Config = static_cast<const PS5ControllerConfig&>(config);
            return new PS5_Controller(ps5Config);
        }

        case ControllerType::PS4_DUALSHOCK: {
            // const PS4ControllerConfig& ps4Config = static_cast<const PS4ControllerConfig&>(config);
            // return new PS4_Controller(ps4Config);
        }

        default:
            Serial.println(F("Unknown controller type"));
            return nullptr;
    }
}
