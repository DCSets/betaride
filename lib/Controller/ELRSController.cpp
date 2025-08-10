#include "ELRSController.h"

void ELRSController::begin ()
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

void ELRSController::loop ()
{
    if (this->_ready)
    {
        this->_crsf.update();
    }
}



void ELRSController::getAllChannels (int* outChannels)
{
    for (int ChannelNum = 1; ChannelNum <= 16; ChannelNum++)
    {
        outChannels[ChannelNum - 1] = this->_crsf.getChannel(ChannelNum);
    }
}