#include "BrushlessMotor.h"

BrushlessMotor::BrushlessMotor(const BrushlessMotorConfig &config) : _config(config),
                                                                     _direction(MotorDirection::Forward)
{
    // Configure LEDC channel and attach pin for this ESC
    ledcSetup(this->_config.channel, this->_config.pwmFrequency, this->_config.pwmResolution);
    ledcAttachPin(this->_config.pin, this->_config.channel);
}

void BrushlessMotor::setThrottle(int percent)
{
    this->_throttleLvl = percent;
}

void BrushlessMotor::setDirection(MotorDirection direction)
{
    if (this->_direction == direction || this->_config.is3D != 1)
    {
        return;
    }

    this->_direction = direction;
    this->_reverseAt = millis() + this->_config.reverseTimeout;
    this->_throttleLvl = 0;
}

void BrushlessMotor::loop()
{

    if (this->_reverseAt != 0)
    {
        unsigned long currentTime = millis();
        if (currentTime >= this->_reverseAt)
        {
            this->_reverseAt = 0;
        }
        int zeroThrottle = (this->_config.is3D == 1) ? this->_config.throttleCenter : this->_config.throttleMin;
        return ledcWrite(this->_config.channel, usToDuty(zeroThrottle, this->_config.pwmResolution, this->_config.pwmFrequency));
    }

    if (this->_config.is3D == 1)
    {
        if (this->_direction == MotorDirection::Forward)
        {
            int throttleUs = map(this->_throttleLvl, 0, 100, this->_config.throttleCenter, this->_config.throttleMax);
            return ledcWrite(this->_config.channel, usToDuty(throttleUs, this->_config.pwmResolution, this->_config.pwmFrequency));
        }

        // Move backward
        int throttleUs = map(this->_throttleLvl, 0, 100, this->_config.throttleCenter, this->_config.throttleMin);
        return ledcWrite(this->_config.channel, usToDuty(throttleUs, this->_config.pwmResolution, this->_config.pwmFrequency));
    }

    int throttleUs = map(this->_throttleLvl, 0, 100, this->_config.throttleMin, this->_config.throttleMax);
    return ledcWrite(this->_config.channel, usToDuty(throttleUs, this->_config.pwmResolution, this->_config.pwmFrequency));
}
