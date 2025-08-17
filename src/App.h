#ifndef APP_H
#define APP_H

#include <Arduino.h>
#include <ConfigStore.h>
#include <ControllerRules.h>
#include <BrushlessMotor.h>
#include <Motor.h>
#include <Controller.h>

class App
{
public:
    App(ConfigStore* store);
    ~App();
    void loadResources();
    void loop();
    ConfigStore& getStore() { return _store; }
    
private:
    ConfigStore _store;

    BrushlessMotor* _brushlessMotors[MAX_BRUSHLESS_MOTORS];
    Motor* _motors[MAX_MOTORS];
    ControllerRule _controllerRules[MAX_RULES];
    
    int _brushlessMotorCount;
    int _controllerRulesCount;
    Controller* _controller;



    void resetController();
    void resetResources();

    void loadMotors();
    void loadControllerRules();
    void loadController();

    BrushlessMotor* findBrushlessById(const char* id);
};

#endif