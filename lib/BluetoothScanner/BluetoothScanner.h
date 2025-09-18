#ifndef BLUETOOTH_SCANNER_H
#define BLUETOOTH_SCANNER_H

#include <Arduino.h>
#include <vector>
#include <constants.h>
#include <ConfiguratorHelpers.h>
#include "BluetoothSerial.h"


class BluetoothScanner
{
public:
    BluetoothScanner(
        std::function<void(std::string, std::string)> deviceFoundCallback,
        std::function<void()> scanStoppedCallback
    ) { 
        this->deviceFoundCallback = deviceFoundCallback; 
        this->scanStoppedCallback = scanStoppedCallback;
    };
    void scan();
    void stopScan(String reason);
    boolean isScanning() { return this->scanStartTime != 0; };

private:
    const int MAX_SCAN_DURATION = 30000;
    uint scanStartTime = 0;
    std::function<void(std::string, std::string)> deviceFoundCallback;
    std::function<void()> scanStoppedCallback;
    BluetoothSerial SerialBT;
    void onDeviceFound(BTAdvertisedDevice* pDevice);
    std::vector<std::pair<std::string, std::string>> controllerFound;
    void addControllerFound(std::string name, uint8_t *address);
};

#endif