#include "ELRSController.h"

void ELRSController::begin()
{
    if (!this->_ready)
    {
        this->_crsfSerial.begin(CRSF_BAUDRATE, SERIAL_8N1, this->_config.rxPin, this->_config.txPin);
        if (!this->_crsfSerial)
        {
            while (1)
            {
                Serial.println("Invalid crsfSerial configuration");
            }
        }
        this->_crsf.begin(this->_crsfSerial);
        this->_ready = true;
        if(DEBUG) {
            Serial.println("ELRSController initialized");
        }
    }
}

void ELRSController::loop()
{
    if (!this->_ready)
    {
        return;
    }

    this->_crsf.update();
    if (!this->_crsf.isLinkUp())
    {
        for (int ChannelNum = 0; ChannelNum <= 15; ChannelNum++)
        {
            _channels[ChannelNum] = -1;
        }
        return;
    }

    for (int ChannelNum = 0; ChannelNum <= 15; ChannelNum++)
    {
        int value = this->_crsf.getChannel(ChannelNum + 1);
        _channels[ChannelNum] = value;
    }
}

void ELRSController::printAllChannels() {
    static uint32_t last = 0;
    if (millis() - last < 50) return; // 20 Hz
    last = millis();
    static int channels[16] = {-1};
    for (int i=0;i<16;i++) {
        auto it = _channels.find(i);
        if (it != _channels.end() && channels[i] != it->second) {
            Serial.printf("[rx@%d@%d]\n", i, it->second);
            channels[i] = it->second;
        }
    }
}

int ELRSController::getChannelPercent(int channel, int min, int max) {
    auto it = _channels.find(channel);
    if(it != _channels.end()) {
        return map(it->second, min, max, 0, 100);
    }
    return -1;
}