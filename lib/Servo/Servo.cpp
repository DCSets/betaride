#include "Servo.h"

Servo::Servo(const ServoConfig &config) : _config(config)
{
    // TODO: use channels/pins manager to control this s**t
    // Arduino-ESP32 3.x: ledcSetup/ledcAttachPin -> ledcAttachChannel
    // Write via ledcWrite(pin): ledcWriteChannel() passes the raw channel to ledc_set_duty and fails for channels >= 8
    ledcAttachChannel(this->_config.pin, this->_config.pwmFrequency, this->_config.pwmResolution, this->_config.channel);

    _angle = config.angleCenter; // Center servo on start
    _previousAngle = _angle;
}

void Servo::setAngle(int degrees)
{
    _previousAngle = _angle;
    _angle = constrain(degrees, 0, _config.maxAngle);
}

void Servo::loop()
{
    int pulseUs = map(_angle, 0, _config.maxAngle, _config.minUs, _config.maxUs);

    ledcWrite(this->_config.pin, usToDuty(pulseUs, this->_config.pwmResolution, this->_config.pwmFrequency));
}
