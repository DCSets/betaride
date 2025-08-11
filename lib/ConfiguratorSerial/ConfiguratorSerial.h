#ifndef CONFIGURATOR_SERIAL_H
#define CONFIGURATOR_SERIAL_H

#include <vector>
#include <string>
#include <Arduino.h>
#include <ConfigStore.h>
#include <constants.h>
#include <enums.h>
#include <ELRSController.h>
#include <PS5Controller.h>

class ConfiguratorSerial
{
public:
    ConfiguratorSerial(ConfigStore *store){
        this->_store = store;
    };
    boolean isConnected();

    void loop();
    int parseCommand(String received, String outputChunks[], int maxChunks);
    void processCommand(String chunks[], int count);

    void printElrsChannels(int channels[16]);

private:
    boolean _isConnected = false;
    boolean _configTransfer = false;
    std::vector<std::pair<std::string, std::string>> _resources;
    ConfigStore *_store;

    void addResource(const std::string& key, const std::string& value) {
        _resources.push_back(std::make_pair(key, value));
    }
    const std::pair<std::string, std::string>& getResource(int index) const {
        return _resources[index];
    }
    void clear() {
        _resources.clear();
    }
    // Iterators
    std::vector<std::pair<std::string, std::string>>::iterator begin() {
        return _resources.begin();
    }

    std::vector<std::pair<std::string, std::string>>::iterator end() {
        return _resources.end();
    }

    std::vector<std::pair<std::string, std::string>>::const_iterator begin() const {
        return _resources.begin();
    }

    std::vector<std::pair<std::string, std::string>>::const_iterator end() const {
        return _resources.end();
    }

    static constexpr int MAX_CHUNKS = 5;
    static constexpr const char *_CMD_SERIAL = "serial";
    static constexpr const char *_CMD_REQUEST_RESOURCES = "rrequest";
    static constexpr const char *_CMD_TRANSFER = "transfer";
    static constexpr const char *_CMD_STORE_RESOURCES = "store";
};

#endif