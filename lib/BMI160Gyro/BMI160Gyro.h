#ifndef BMI160GYRO_H
#define BMI160GYRO_H

#include <Wire.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <structs.h>
#include <constants.h>
#include <enums.h>
#include <ConfiguratorHelpers.h>
#include <BMI160Gen.h>

struct BMI160GyroConfig : Resource
{
    GyroType gyroType;
    int sdaPin;
    int sclPin;
    int minThreshold;
    int desiredRotation;
    float rollOffset;
    float pitchOffset;
    float yawOffset;
    char servos[10][13];
    int servoCount;

    BMI160GyroConfig() = default;
    BMI160GyroConfig(String json)
    {
        JsonDocument doc;
        DeserializationError err = deserializeJson(doc, json);

        if (err)
        {
            Serial.println(F("JSON parse error - servo config"));
            return;
        }

        memset(this->servos, 0, sizeof(this->servos));
        strncpy(this->id, doc["id"], sizeof(this->id));
        strncpy(this->name, doc["name"], sizeof(this->name));
        strncpy(this->type, doc["type"], sizeof(this->type));

        this->isNew = false;
        this->gyroType = GyroType::BMI160;
        this->sdaPin = doc["sdaPin"] | -1;
        this->sclPin = doc["sclPin"] | -1;
        this->desiredRotation = doc["desiredRotation"] | 70;
        this->minThreshold = doc["minThreshold"] | 30;
        this->rollOffset = doc["rollOffset"] | 0.0;
        this->pitchOffset = doc["pitchOffset"] | 0.0;
        this->yawOffset = doc["yawOffset"] | 0.0;

        if (doc["servos"].is<JsonArray>()) {
            JsonArray resourceIdsArray = doc["servos"];
            int arrayCount = min((int)resourceIdsArray.size(), 10); // Limit to array size
            for (int i = 0; i < arrayCount; i++) {
                const char* resourceId = resourceIdsArray[i];
                if (resourceId) {
                    strncpy(this->servos[i], resourceId, 12); // Leave space for null terminator
                    this->servos[i][12] = '\0'; // Ensure null termination
                    this->servoCount++;
                }
            }
        }
    }

    void toJson(String &outJson) const
    {
        JsonDocument doc;

        doc["id"] = this->id;
        doc["isNew"] = this->isNew;
        doc["name"] = this->name;
        doc["type"] = this->type;
        doc["gyroType"] = static_cast<int>(this->gyroType);
        doc["sdaPin"] = this->sdaPin;
        doc["sclPin"] = this->sclPin;
        doc["desiredRotation"] = this->desiredRotation;
        doc["minThreshold"] = this->minThreshold;
        doc["rollOffset"] = this->rollOffset;
        doc["pitchOffset"] = this->pitchOffset;
        doc["yawOffset"] = this->yawOffset;

        // Serialize resourceIds array
        JsonArray resourceIdsArray = doc["servos"].to<JsonArray>();
        for (int i = 0; i < 10; i++) {
            if (strlen(this->servos[i]) > 0) {
                resourceIdsArray.add(this->servos[i]);
            }
        }

        serializeJson(doc, outJson);
    }
};

class BMI160Gyro
{
public:
    BMI160Gyro(const BMI160GyroConfig &config) : _config(config) {};
    void begin();
    void loop();

    // Getters for velocity measurements
    float getRollVelocity() const { return _roll_velocity; }
    float getPitchVelocity() const { return _pitch_velocity; }
    float getYawVelocity() const { return _yaw_velocity; }

    // Calibration methods
    void calibrate();
    bool isCalibrated() const { return _calibrated; }

    void setMeasureAngle(bool measure_angle) { this->measure_angle = measure_angle; }
    bool exceededRotationThreshold() const { return abs(_yaw_velocity) > this->_config.minThreshold; }
    void printAngles()
    {
        static uint32_t last = 0;
        if (millis() - last < 50)
            return; // 20 Hz
        last = millis();

        Serial.print("[gy@");
        Serial.print(this->yaw_angle);
        Serial.print("@");
        Serial.print(this->roll_angle);
        Serial.print("@");
        Serial.print(this->pitch_angle);
        Serial.println("]");
    }
    int calculateNewAngle(int currentAngle, int midAngle);
    void getServoIds(String *servos) const {
        for(int i = 0; i < this->_config.servoCount; i++) {
            servos[i] = this->_config.servos[i];
        }
    }
    
private:    
    const float C_ALPHA = 0.98; // Complimentary filter alpha
    float velocityThreshold = 0.5; // degrees per second
    bool measure_angle = false;

    unsigned long _last_update_time = 0;
    const int I2C_ADDR = 0x68;

    BMI160GyroConfig _config;
    TwoWire _wire = TwoWire(0);

    bool _ready = false;
    bool _calibrated = false;

    // Angular velocities (degrees per second)
    float _yaw_velocity = 0.0;   // degrees per second
    float _roll_velocity = 0.0;  // degrees per second
    float _pitch_velocity = 0.0; // degrees per second

    // Calibration offsets
    float _roll_offset = 0.0;  // degrees per second
    float _pitch_offset = 0.0; // degrees per second
    float _yaw_offset = 0.0;   // degrees per second

    // Helper functions
    float convertRawGyro(int gRaw);
    float unwrapAngle(float prev, float current);
    
    float yaw_angle = 0;
    float roll_angle = 0;
    float pitch_angle = 0;


    const unsigned long CONNECTION_ATTEMPT_INTERVAL = 3000;
    unsigned long _connection_attempt = 0;
    bool connectGyro();

    // TODO: Move it to config?
    const int MAX_CORRECTION_ANGLE = 15;
    const float CORRECTION_FACTOR = 0.3;
};

#endif