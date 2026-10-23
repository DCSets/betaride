#include "BrushlessMotor.h"

BrushlessMotor::BrushlessMotor(const BrushlessMotorConfig &config) : _config(config),
                                                                     _direction(MotorDirection::Forward)
{
    // Arduino-ESP32 3.x: ledcSetup/ledcAttachPin -> ledcAttachChannel
    // Write via ledcWrite(pin): ledcWriteChannel() passes the raw channel to ledc_set_duty and fails for channels >= 8
    ledcAttachChannel(this->_config.pin, this->_config.pwmFrequency, this->_config.pwmResolution, this->_config.channel);

    setThrottle(0);
    loop();
}

void BrushlessMotor::setThrottle(int percent)
{
    this->_throttleLvl = percent;
    this->_lastInputTime = millis();
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
    if(millis() - this->_lastInputTime > 300) {
        this->_throttleLvl = 0;
    }

    if (this->_reverseAt != 0)
    {
        unsigned long currentTime = millis();
        if (currentTime >= this->_reverseAt)
        {
            this->_reverseAt = 0;
        }
        int zeroThrottle = (this->_config.is3D == 1) ? this->_config.throttleCenter : this->_config.throttleMin;
        ledcWrite(this->_config.pin, usToDuty(zeroThrottle, this->_config.pwmResolution, this->_config.pwmFrequency));
        return;
    }

    if (this->_config.is3D == 1)
    {
        if (this->_direction == MotorDirection::Forward)
        {
            int throttleUs = map(this->_throttleLvl, 0, 100, this->_config.throttleCenter, this->_config.throttleMax);
            ledcWrite(this->_config.pin, usToDuty(throttleUs, this->_config.pwmResolution, this->_config.pwmFrequency));
            return;
        }

        // Move backward
        int throttleUs = map(this->_throttleLvl, 0, 100, this->_config.throttleCenter, this->_config.throttleMin);
        ledcWrite(this->_config.pin, usToDuty(throttleUs, this->_config.pwmResolution, this->_config.pwmFrequency));
        return;
    }

    int throttleUs = map(this->_throttleLvl, 0, 100, this->_config.throttleMin, this->_config.throttleMax);
    ledcWrite(this->_config.pin, usToDuty(throttleUs, this->_config.pwmResolution, this->_config.pwmFrequency));
}
