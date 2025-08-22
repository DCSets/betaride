#include "Motor.h"

Motor::Motor(const MotorConfig &config) : _config(config),
                                          _direction(MotorDirection::Forward)
{
    if (this->_config.pin1 != -1 && this->_config.channel1 != -1)
    {
        ledcSetup(this->_config.channel1, this->_config.pwmFrequency, this->_config.pwmResolution);
        ledcAttachPin(this->_config.pin1, this->_config.channel1);
    }
    if (this->_config.pin2 != -1 && this->_config.channel2 != -1)
    {
        ledcSetup(this->_config.channel2, this->_config.pwmFrequency, this->_config.pwmResolution);
        ledcAttachPin(this->_config.pin2, this->_config.channel2);
    }
}

void Motor::setThrottle(int percent)
{
    this->_throttleLvl = percent;
}

void Motor::setDirection(MotorDirection direction)
{
    if (this->_config.pin1 == -1 || this->_config.pin2 == -1)
    {
        return;
    }

    if(this->_direction == direction) {
        return;
    }

    this->_direction = direction;
    this->_reverseAt = millis() + this->_config.reverseTimeout;
    this->_throttleLvl = 0;
}

void Motor::loop()
{
    if (this->_reverseAt != 0)
    {
        unsigned long currentTime = millis();
        if (currentTime >= this->_reverseAt)
        {
            this->_reverseAt = 0;
        }
        int zeroThrottle = 0;
        if (this->_config.pin1 != -1 && this->_config.channel1 != -1) {
            ledcWrite(this->_config.channel1, speedToPWM(zeroThrottle, this->_config.pwmFrequency, this->_config.pwmResolution));
        }
        if(this->_config.pin2 != -1 && this->_config.channel2 != -1) {
            ledcWrite(this->_config.channel2, speedToPWM(zeroThrottle, this->_config.pwmFrequency, this->_config.pwmResolution));
        }

        return;
    }

    uint32_t pwmValue = speedToPWM(this->_throttleLvl, this->_config.pwmFrequency, this->_config.pwmResolution);
    if (this->_config.pin1 != -1 && this->_config.channel1 != -1 && this->_config.pin2 != -1 && this->_config.channel2 != -1) {
        if(this->_direction == MotorDirection::Forward) {
            ledcWrite(this->_config.channel1, pwmValue);
            ledcWrite(this->_config.channel2, 0);
        } else {
            ledcWrite(this->_config.channel1, 0);
            ledcWrite(this->_config.channel2, pwmValue);
        }
    } else {
        // Ignore direction, just set the throttle
        if(this->_config.pin1 != -1 && this->_config.channel1 != -1) {
            ledcWrite(this->_config.channel1, pwmValue);
        }
        if(this->_config.pin2 != -1 && this->_config.channel2 != -1) {
            ledcWrite(this->_config.channel2, pwmValue);
        }
    }
}
