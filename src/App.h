#ifndef APP_H
#define APP_H

#include <Arduino.h>
#include <map>
#include <string>
#include <ConfigStore.h>
#include <ControllerRules.h>
#include <BrushlessMotor.h>
#include <Motor.h>
#include <Controller.h>
#include <RuleEngine.h>
#include <Servo.h>
#include <BMI160Gyro.h>

class App
{
public:
    App(ConfigStore* store);
    ~App();

    void loadResources();
    void loop();
    void testController();
    void testGyro();
    void calibrateGyro();
    ConfigStore& getStore() { return _store; }
    
private:
    ConfigStore _store;
    RuleEngine* _ruleEngine = nullptr;
    Controller* _controller = nullptr;

    std::map<std::string, BrushlessMotor*> _brushlessMotors;
    std::map<std::string, Motor*> _motors;
    std::map<std::string, Servo*> _servos;
    std::map<std::string, BMI160Gyro*> _gyros;
    void resetController();
    void resetResources();

    void loadMotors();
    void loadBrushlessMotors();
    void loadServos();
    void loadGyros();
    void loadControllerRules();
    void loadController();
};

#endif