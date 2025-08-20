#include "Servo.h"

Servo::Servo(const ServoConfig &config) : _config(config)
{
    // TODO: use channels/pins manager to control this s**t
    ledcSetup(this->_config.channel, this->_config.pwmFrequency, this->_config.pwmResolution);
    ledcAttachPin(this->_config.pin, this->_config.channel);

    _angle = config.angleCenter; // Center servo on start
}

void Servo::setAngle(int degrees)
{
    _angle = constrain(degrees, 0, _config.maxAngle);
}

void Servo::loop()
{
    int pulseUs = map(_angle, 0, _config.maxAngle, _config.minUs, _config.maxUs);

    ledcWrite(this->_config.channel, usToDuty(pulseUs, this->_config.pwmResolution));
}

