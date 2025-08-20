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

void ELRSController::printAllChannels()
{
    // to print only channels which were changed
    static int channels[16] = {-1};
    for (int ChannelNum = 0; ChannelNum <= 15; ChannelNum++)
    {
        auto it = _channels.find(ChannelNum);
        if (it != _channels.end() && channels[ChannelNum] != it->second)
        {
            Serial.printf("[rx@%d@%d]\n", ChannelNum, it->second);
            channels[ChannelNum] = it->second;
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