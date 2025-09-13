#include "PS5_Controller.h"

void PS5_Controller::begin()
{
    if (!this->_ready)
    {
        ps5.begin(this->_config.address.c_str());
        initializeChannelMappings();

        this->_ready = true;
        if(DEBUG) {
            Serial.println("PS5 DualSense controller initialized");
        }
    }
}

void PS5_Controller::loop()
{
    if (!this->isConnected())
    {
        return;
    }

    // Update all channels with current PS5 controller values
    for (int i = 0; i < 28; i++) {
        int value = getChannelValue(i);
        if (value != -300) {
            _channels[i] = value;
        }
    }
}

void PS5_Controller::printAllChannels() {
    static uint32_t last = 0;
    if (millis() - last < 50) return; // 20 Hz
    last = millis();
    static int channels[28] = {-300};
    for (int i=0;i<28;i++) {
        auto it = _channels.find(i);
        if (it != _channels.end() && channels[i] != it->second) {
            Serial.printf("[rx@%d@%d]\n", i, it->second);
            channels[i] = it->second;
        }
    }
}

int PS5_Controller::getChannelPercent(int channel, int min, int max) {
    auto it = _channels.find(channel);
    if(it != _channels.end()) {
        return map(it->second, min, max, 0, 100);
    }
    return -1;
}

void PS5_Controller::initializeChannelMappings() {
    // Initialize member function pointers for each channel mapping
    // Analog Sticks
    _channelMappings[0].getValue = &PS5_Controller::getLStickX;   // LStickX
    _channelMappings[1].getValue = &PS5_Controller::getLStickY;   // LStickY
    _channelMappings[2].getValue = &PS5_Controller::getRStickX;   // RStickX
    _channelMappings[3].getValue = &PS5_Controller::getRStickY;   // RStickY
    
    // Face Buttons
    _channelMappings[4].getValue = &PS5_Controller::getSquare;     // Square
    _channelMappings[5].getValue = &PS5_Controller::getCross;      // Cross
    _channelMappings[6].getValue = &PS5_Controller::getTriangle;   // Triangle
    _channelMappings[7].getValue = &PS5_Controller::getCircle;     // Circle
    
    // D-Pad Buttons
    _channelMappings[8].getValue = &PS5_Controller::getLeft;       // Left
    _channelMappings[9].getValue = &PS5_Controller::getRight;      // Right
    _channelMappings[10].getValue = &PS5_Controller::getUp;        // Up
    _channelMappings[11].getValue = &PS5_Controller::getDown;      // Down
    
    // Shoulder Buttons
    _channelMappings[12].getValue = &PS5_Controller::getL1;        // L1
    _channelMappings[13].getValue = &PS5_Controller::getL2;        // L2
    _channelMappings[14].getValue = &PS5_Controller::getR1;        // R1
    _channelMappings[15].getValue = &PS5_Controller::getR2;        // R2
    
    // Other Buttons
    _channelMappings[16].getValue = &PS5_Controller::getShare;     // Share
    _channelMappings[17].getValue = &PS5_Controller::getOptions;   // Options
    _channelMappings[18].getValue = &PS5_Controller::getL3;        // L3
    _channelMappings[19].getValue = &PS5_Controller::getR3;        // R3
    _channelMappings[20].getValue = &PS5_Controller::getPSButton;  // PSButton
    _channelMappings[21].getValue = &PS5_Controller::getTouchpad;  // Touchpad
    
    // Trigger Values
    _channelMappings[22].getValue = &PS5_Controller::getL2Value;   // L2Value
    _channelMappings[23].getValue = &PS5_Controller::getR2Value;   // R2Value
    
    // D-Pad Diagonal Buttons
    _channelMappings[24].getValue = &PS5_Controller::getUpRight;   // UpRight
    _channelMappings[25].getValue = &PS5_Controller::getDownRight; // DownRight
    _channelMappings[26].getValue = &PS5_Controller::getUpLeft;    // UpLeft
    _channelMappings[27].getValue = &PS5_Controller::getDownLeft;  // DownLeft
}

int PS5_Controller::getChannelValue(int channelIndex) {
    if (channelIndex < 0 || channelIndex >= 28) {
        return -300;
    }
    
    if (_channelMappings[channelIndex].getValue == nullptr) {
        return -300;
    }
    
    return (this->*(_channelMappings[channelIndex].getValue))();
}

int PS5_Controller::getChannelPercent(int channel)  {
    if(channel < 0 || channel >= 28) {
        return -1;
    }

    ChannelMapping channelMapping = _channelMappings[channel];
    return getChannelPercent(channel, channelMapping.minValue, channelMapping.maxValue);
}

void PS5_Controller::retryConnection()
{
    if (!ps5.isConnected()) {
        this->_ready = false;
        this->begin();
    }
}