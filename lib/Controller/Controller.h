#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <enums.h>
#include <constants.h>
#include <ConfiguratorHelpers.h>
#include <structs.h>

// Forward declarations
struct ELRSConfig;
class ELRSController;

struct ControllerConfig : Resource
{
    virtual ControllerType getControllerType() const = 0;

    virtual ~ControllerConfig() {}

    virtual void toJson(String &outJson) const = 0;
};



class Controller
{
public:
    virtual ~Controller() {}
    virtual void getAllChannels(int *outChannels) = 0;
    virtual void begin() = 0;
    virtual void loop() = 0;

private:
    boolean _ready = false;
};


class ControllerFactory {
public:
    static Controller* createControllerFromJson(const String& json);
    static Controller* createControllerFromConfig(const ControllerConfig& config);
};
#endif