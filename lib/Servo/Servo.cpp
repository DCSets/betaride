#include "Servo.h"

ServoSimple::ServoSimple(const ServoConfig &config) : _config(config)
{
    ledcSetup(this->_config.channel, this->_config.frequency, this->_config.resolution);
    ledcAttachPin(this->_config.pin, this->_config.channel);
}

void ServoSimple::setAngle(int degrees)
{
    this->_config.angle = constrain(degrees, 0, 180);
}

void ServoSimple::loop()
{
    // Map angle 0..180 to pulse width
    int pulseUs = map(this->_config.angle, 0, 180, this->_config.angleMinUs, this->_config.angleMaxUs);
    ledcWrite(this->_config.channel, this->usToDuty(pulseUs));
}

uint32_t ServoSimple::usToDuty(int microseconds)
{
    uint32_t maxDuty = (1UL << this->_config.resolution) - 1;
    return (microseconds * maxDuty) / 20000L;
}

