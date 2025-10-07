#include "BMI160Gyro.h"
#define ACCEL_SENSITIVITY 16384.0 // Sensitivity for ±2g in LSB/g (adjust based on your configuration)

void BMI160Gyro::begin()
{
    this->_wire.begin(this->_config.sdaPin, this->_config.sclPin);
    this->_wire.setTimeout(50);
    bool connected = this->connectGyro();
    if(connected) {
        this->calibrate();
    }
}

bool BMI160Gyro::connectGyro()
{
    if(millis() - this->_connection_attempt >= this->CONNECTION_ATTEMPT_INTERVAL) {
        this->_connection_attempt = millis();
    } else {
        return false;
    }

    this->_wire.beginTransmission(I2C_ADDR);
    uint8_t error = this->_wire.endTransmission();

    if (error != 0)
    {
        if (DEBUG)
            Serial.println("BMI160 not detected on I2C bus!");

        _ready = false;
        return false;
    }

    if (!BMI160.begin(BMI160GenClass::I2C_MODE, this->_wire, I2C_ADDR))
    {
        if (DEBUG)
            Serial.println("BMI160 initialization failed!");

        _ready = false;
    }
    else
    {
        if (DEBUG)
            Serial.println("BMI160 initialization successful!");

        // Set the gyroscope range to 250 degrees/second
        BMI160.setGyroRange(250);

        _ready = true;
    }

    return _ready;
}

void BMI160Gyro::loop()
{
    if (!this->_ready)
    {
        bool connected = this->connectGyro();
        if(connected) {
            this->calibrate();
        }
    }

    unsigned long current_time = micros();
    float dt = (current_time - _last_update_time) / 1000000.0; // Convert to seconds
    _last_update_time = current_time;

    // Skip if time delta is unreasonable (first run or overflow)
    if (dt > 0.1 || dt <= 0)
    {
        return;
    }

    // Read raw gyroscope measurements from device
    int gX = BMI160.readGyro(X_AXIS); // Roll
    int gY = BMI160.readGyro(Y_AXIS); // Pitch
    int gZ = BMI160.readGyro(Z_AXIS); // Yaw

    // Convert to degrees per second - this IS the angular velocity
    float raw_roll = convertRawGyro(gX);  // Raw roll velocity (degrees/sec)
    float raw_pitch = convertRawGyro(gY); // Raw pitch velocity (degrees/sec)
    float raw_yaw = convertRawGyro(gZ);   // Raw yaw velocity (degrees/sec)

    // Apply calibration offsets if calibrated
    if (_calibrated)
    {
        _roll_velocity = raw_roll - _roll_offset;
        _pitch_velocity = raw_pitch - _pitch_offset;
        _yaw_velocity = raw_yaw - _yaw_offset;
    }
    else
    {
        _roll_velocity = raw_roll;
        _pitch_velocity = raw_pitch;
        _yaw_velocity = raw_yaw;
    }

    if (this->measure_angle)
    {
        static int ax, ay, az;
        static float ax_g, ay_g, az_g, ap, ar;

        if (abs(_yaw_velocity) > velocityThreshold)
        {
            yaw_angle += _yaw_velocity * dt; // Use calibrated value
        }

        // Read accelerometer data FIRST
        BMI160.readAccelerometer(ax, ay, az);

        // Convert to g-force
        ax_g = ax / ACCEL_SENSITIVITY;
        ay_g = ay / ACCEL_SENSITIVITY;
        az_g = az / ACCEL_SENSITIVITY;

        // Calculate accelerometer angles
        ap = atan2(ay_g, sqrt(ax_g * ax_g + az_g * az_g)) * 180.0 / PI;
        ar = atan2(-ax_g, az_g) * 180.0 / PI;

        ap = unwrapAngle(pitch_angle, ap);
        ar = unwrapAngle(roll_angle, ar);

        // Use calibrated gyro velocities in complementary filter
        roll_angle = (roll_angle + _roll_velocity * dt) * C_ALPHA + ar * (1 - C_ALPHA);
        pitch_angle = (pitch_angle + _pitch_velocity * dt) * C_ALPHA + ap * (1 - C_ALPHA);
    }
}

void BMI160Gyro::calibrate()
{
    if (!this->_ready)
    {
        if (DEBUG)
            Serial.println("BMI160 not ready for calibration!");
        return;
    }

    if (DEBUG)
        Serial.println("Starting BMI160 calibration... Keep sensor still!");

    const int calibration_samples = 500;
    float roll_sum = 0.0, pitch_sum = 0.0, yaw_sum = 0.0;

    // Collect samples for calibration
    for (int i = 0; i < calibration_samples; i++)
    {
        int gX = BMI160.readGyro(X_AXIS);
        int gY = BMI160.readGyro(Y_AXIS);
        int gZ = BMI160.readGyro(Z_AXIS);

        roll_sum += convertRawGyro(gX);
        pitch_sum += convertRawGyro(gY);
        yaw_sum += convertRawGyro(gZ);

        delay(2); // Small delay between samples
    }

    // Calculate offsets (average of readings when stationary)
    _roll_offset = roll_sum / calibration_samples;
    _pitch_offset = pitch_sum / calibration_samples;
    _yaw_offset = yaw_sum / calibration_samples;

    this->_wire.beginTransmission(I2C_ADDR); // BMI160 I2C address
    this->_wire.write(0x7E);                 // CMD register
    this->_wire.write(0x37);                 // Accelerometer auto-calibration command
    this->_wire.endTransmission();

    // Give the sensor time to start calibration
    delay(100);
    _calibrated = true;

    Serial.println("[gyc@1]");
    if (DEBUG)
    {
        Serial.println("BMI160 calibration complete!");
        Serial.print("Roll offset: ");
        Serial.println(_roll_offset);
        Serial.print("Pitch offset: ");
        Serial.println(_pitch_offset);
        Serial.print("Yaw offset: ");
        Serial.println(_yaw_offset);
    }
}

float BMI160Gyro::convertRawGyro(int gRaw)
{
    // since we are using 250 degrees/seconds range
    // -250 maps to a raw value of -32768
    // +250 maps to a raw value of 32767
    float g = (gRaw * 250.0) / 32768.0;
    return g;
}

float BMI160Gyro::unwrapAngle(float prev, float current)
{
    float diff = current - prev;

    if (diff > 180.0f)
    {
        current -= 360.0f;
    }
    else if (diff < -180.0f)
    {
        current += 360.0f;
    }

    return current;
}

int BMI160Gyro::calculateNewAngle(int currentAngle, int midAngle) {
    
    float currentAngleVelocity = abs(_yaw_velocity);
    float desiredAngleVelocity = _config.desiredRotation;

    float error = desiredAngleVelocity - currentAngleVelocity;
    float correction = error * CORRECTION_FACTOR;
    correction = constrain(correction, -MAX_CORRECTION_ANGLE, MAX_CORRECTION_ANGLE);

    if(correction > 0) {
        // Under steering. Correct by increasing the angle.
        if(currentAngle > midAngle) {
            return currentAngle + correction;
        } else {
            return currentAngle - correction;
        }
    } else {
        // Over steering. Correct by decreasing the angle.
        if(currentAngle > midAngle) {
            return currentAngle - correction;
        } else {
            return currentAngle + correction;
        }
    }
}