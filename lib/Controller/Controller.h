#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <enums.h>
#include <constants.h>
#include <ConfiguratorHelpers.h>
#include <structs.h>
#include <map>

// Forward declarations
struct ELRSConfig;
class ELRSController;

struct ControllerConfig : Resource
{
    ControllerType controllerType;

    virtual ~ControllerConfig() {}

    virtual void toJson(String &outJson) const = 0;
};



class Controller
{
public:
    virtual ~Controller() {}
    virtual void printAllChannels() = 0;
    virtual void begin() = 0;
    virtual void loop() = 0;
    virtual int getChannelPercent(int channel, int min = 0, int max = 100) = 0;
    virtual bool isConnected() { return false; }
    int getChannel(int channel) { 
        auto it = _channels.find(channel);
        if(it != _channels.end()) {
            return it->second;
        }
        return -1;
    }
    std::map<int, int> getChannels() { return _channels; }
protected:
    std::map<int, int> _channels;
private:
    boolean _ready = false;
};


class ControllerFactory {
public:
    static Controller* createControllerFromJson(const String& json);
    static Controller* createControllerFromConfig(const ControllerConfig& config);
};
#endif