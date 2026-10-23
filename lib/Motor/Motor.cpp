#include "Motor.h"

Motor::Motor(const MotorConfig &config) : _config(config),
                                          _direction(MotorDirection::Forward)
{
    // Arduino-ESP32 3.x: ledcSetup/ledcAttachPin -> ledcAttachChannel
    // Write via ledcWrite(pin): ledcWriteChannel() passes the raw channel to ledc_set_duty and fails for channels >= 8
    if (this->_config.pin1 != -1 && this->_config.channel1 != -1)
    {
        ledcAttachChannel(this->_config.pin1, this->_config.pwmFrequency, this->_config.pwmResolution, this->_config.channel1);
    }
    if (this->_config.pin2 != -1 && this->_config.channel2 != -1)
    {
        ledcAttachChannel(this->_config.pin2, this->_config.pwmFrequency, this->_config.pwmResolution, this->_config.channel2);
    }

    setThrottle(0);
    loop();
}

void Motor::setThrottle(int percent)
{
    this->_throttleLvl = percent;
    this->_lastInputTime = millis();
}

void Motor::setDirection(MotorDirection direction, const char *source)
{
    if (this->_config.pin1 == -1 || this->_config.pin2 == -1)
    {
        return;
    }

    if(this->_direction == direction) {
        if (DEBUG) {
            strncpy(this->_directionSource, source ? source : "?", sizeof(this->_directionSource) - 1);
        }
        return;
    }

    if (DEBUG) {
        logDirectionChange(direction, source);
    }

    this->_direction = direction;
    this->_reverseAt = millis() + this->_config.reverseTimeout;
    this->_throttleLvl = 0;
}

static const char *directionName(MotorDirection direction)
{
    return direction == MotorDirection::Forward ? "Forward" : "Backward";
}

// Printed at most every 500ms: rules fighting over the direction show up as a high change count instead of flooding Serial
void Motor::logDirectionChange(MotorDirection direction, const char *source)
{
    if (!source) {
        source = "?";
    }
    this->_directionChanges++;

    unsigned long now = millis();
    if (now - this->_directionLogAt >= 500) {
        Serial.printf("[motor %s] direction %s -> %s by rule '%s' (previous rule: '%s'), %d change(s) since last log\n",
                      this->_config.name, directionName(this->_direction), directionName(direction),
                      source, this->_directionSource, this->_directionChanges);
        this->_directionLogAt = now;
        this->_directionChanges = 0;
    }

    strncpy(this->_directionSource, source, sizeof(this->_directionSource) - 1);
}

void Motor::loop()
{
    // No rule has set the throttle recently (e.g. stick released) - stop instead of keeping the last value
    if (millis() - this->_lastInputTime > 300)
    {
        this->_throttleLvl = 0;
    }

    if (this->_reverseAt != 0)
    {
        unsigned long currentTime = millis();
        if (currentTime >= this->_reverseAt)
        {
            this->_reverseAt = 0;
        }
        int zeroThrottle = 0;
        if (this->_config.pin1 != -1 && this->_config.channel1 != -1) {
            ledcWrite(this->_config.pin1, speedToPWM(zeroThrottle, this->_config.pwmFrequency, this->_config.pwmResolution));
        }
        if(this->_config.pin2 != -1 && this->_config.channel2 != -1) {
            ledcWrite(this->_config.pin2, speedToPWM(zeroThrottle, this->_config.pwmFrequency, this->_config.pwmResolution));
        }

        return;
    }

    uint32_t pwmValue = speedToPWM(this->_throttleLvl, this->_config.pwmFrequency, this->_config.pwmResolution);

    if (this->_config.pin1 != -1 && this->_config.channel1 != -1 && this->_config.pin2 != -1 && this->_config.channel2 != -1) {
        if(this->_direction == MotorDirection::Forward) {
            ledcWrite(this->_config.pin1, pwmValue);
            ledcWrite(this->_config.pin2, 0);
        } else {
            ledcWrite(this->_config.pin1, 0);
            ledcWrite(this->_config.pin2, pwmValue);
        }
    } else {
        // Ignore direction, just set the throttle
        if(this->_config.pin1 != -1 && this->_config.channel1 != -1) {
            ledcWrite(this->_config.pin1, pwmValue);
        }
        if(this->_config.pin2 != -1 && this->_config.channel2 != -1) {
            ledcWrite(this->_config.pin2, pwmValue);
        }
    }
}
