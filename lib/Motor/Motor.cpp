#include "Motor.h"

Motor::Motor(const MotorConfig &config) : _config(config),
                                          _direction(MotorDirection::Forward)
{
    // TODO: CHECK BOTH PINS ARE ATTACHED ( H - BRIDGE)

    ledcSetup(this->_config.channel1, this->_config.pwmFrequency, this->_config.pwmResolution);
    ledcAttachPin(this->_config.pin1, this->_config.channel1);
    ledcSetup(this->_config.channel2, this->_config.pwmFrequency, this->_config.pwmResolution);
    ledcAttachPin(this->_config.pin2, this->_config.channel2);
}

void Motor::setThrottle(int percent)
{
    this->_throttleLvl = percent;
}

void Motor::setDirection(MotorDirection direction)
{
   // TODO: CHECK BOTH PINS ARE ATTACHED

    this->_direction = direction;
    this->_reverseAt = millis() + this->_config.reverseTimeout;
    this->_throttleLvl = 0;
}

void Motor::loop()
{
}
