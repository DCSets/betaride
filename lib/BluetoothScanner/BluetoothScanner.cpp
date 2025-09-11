#include "BluetoothScanner.h"

// Global callback function for device discovery
void BluetoothScanner::onDeviceFound(BTAdvertisedDevice *pDevice)
{
    String deviceName = pDevice->getName().c_str();
    uint8_t *address = *pDevice->getAddress().getNative();

    if (deviceName.length() > 0)
    {
        this->addControllerFound(deviceName.c_str(), address);
    }
}

void BluetoothScanner::stopScan(String reason)
{
    if (this->scanStartTime != 0)
    {
        this->scanStartTime = 0;
        SerialBT.discoverAsyncStop();
        if (DEBUG)
        {
            Serial.println("Bluetooth scan stopped: " + reason);
        }
    }
}

void BluetoothScanner::scan()
{
    if (!SerialBT.begin("ESP32_Scanner"))
    {
        Serial.println("Failed to initialize Bluetooth");
        return;
    }

    if (SerialBT.discoverAsync([this](BTAdvertisedDevice *pDevice)
                               { this->onDeviceFound(pDevice); }))
    {
        this->scanStartTime = millis();

        if (DEBUG)
        {
            Serial.println("Discovery started. Devices will be reported as they are found.");
        }

        setTimeout([this]()
                   { this->stopScan("Timeout"); }, this->MAX_SCAN_DURATION);
    }
    else
    {
        Serial.println("Error starting discovery.");
        return this->stopScan("Error starting discovery.");
    }
}

void BluetoothScanner::addControllerFound(std::string name, uint8_t *address)
{
    String addressString = String(address[0], HEX) + ":" + String(address[1], HEX) + ":" + String(address[2], HEX) + ":" + String(address[3], HEX) + ":" + String(address[4], HEX) + ":" + String(address[5], HEX);
    this->controllerFound.push_back(std::make_pair(name, addressString.c_str()));
    this->callback(name, addressString.c_str());
}
