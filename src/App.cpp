#include <App.h>

App::App(ConfigStore *store) : _store(*store), _controller(nullptr)
{
    this->resetResources();
    this->loadResources();
}

App::~App()
{
    this->resetResources();
}

void App::testController()
{
    if (_controller == nullptr)
        return;

    _controller->loop();
    _controller->printAllChannels();
}
void App::loop()
{
    if(_ruleEngine == nullptr || !_ruleEngine->hasRules() || _controller == nullptr) {
        return;
    }

    _controller->loop();
    for(auto &rule : _ruleEngine->rules) {
        if(!_ruleEngine->checkCondition(rule.condition)) {
            continue;
        }
        if(rule.hasSubCondition && !_ruleEngine->checkCondition(rule.subCondition)) {
            continue;
        }

        if(rule.effect->count == 0) {
            continue;
        }

        for(int i = 0; i < rule.effect->count; i++) {
            String arduinoResourceId = rule.effect->resourceIds[i];
            std::string resourceId = arduinoResourceId.c_str();
            if(rule.effect->type() == TYPE_SERVOS) {
                auto it = _servos.find(resourceId);
                if(it != _servos.end()) {
                    int angle = _ruleEngine->getServoAngle(rule, it->second);
                    if(angle != -1) {
                        it->second->setAngle(angle);
                    }
                } 
            }
    
            if(rule.effect->type() == TYPE_MOTORS) {
                auto it = _motors.find(resourceId);
                if(it != _motors.end()) {
                    int speed = _ruleEngine->getMotorSpeed(rule);
                    MotorDirection direction = _ruleEngine->getMotorDirection(rule);
                    if(speed != -1) {
                        it->second->setThrottle(speed);
                    }
                    if(direction != MotorDirection::UNSPECIFIED) {
                        it->second->setDirection(direction);
                    }
                } 
            }
    
            if(rule.effect->type() == TYPE_BRUSHLESS_MOTORS) {
                auto it = _brushlessMotors.find(resourceId);
                if(it != _brushlessMotors.end()) {
                    int speed = _ruleEngine->getMotorSpeed(rule);
                    MotorDirection direction = _ruleEngine->getMotorDirection(rule);
                    if(speed != -1) {
                        it->second->setThrottle(speed);
                    }
                    if(direction != MotorDirection::UNSPECIFIED) {
                        it->second->setDirection(direction);
                    }
                } 
            }

        }
    }

    for(auto &servo : _servos) {
        servo.second->loop();
    }
    for(auto &motor : _motors) {
        motor.second->loop();
    }
    for(auto &motor : _brushlessMotors) {
        motor.second->loop();
    }
}

/* RESOURCES MANAGEMENT SECTION */
void App::loadResources()
{
    this->loadController();
    this->loadControllerRules();
    this->loadMotors();
    this->loadBrushlessMotors();
    this->loadServos();
}

void App::resetController()
{
    if (_controller != nullptr)
    {
        delete _controller;
        _controller = nullptr;
    }
}

void App::resetResources()
{
    for (auto& pair : _brushlessMotors) {
        delete pair.second;
    }
    for (auto& pair : _motors) {
        delete pair.second;
    }
    for (auto& pair : _servos) {
        delete pair.second;
    }
    if (_ruleEngine != nullptr)
    {
        delete _ruleEngine;
        _ruleEngine = nullptr;
    }

    _brushlessMotors.clear();
    _motors.clear();
    _servos.clear();
    this->resetController();
}

void App::loadController()
{
    // Refresh from NVS to reflect any recent saves
    _store.loadControllerConfig();
    this->resetController();

    ControllerConfig *controllerConfig = _store.getControllerConfig();
    if (controllerConfig == nullptr)
        return;

    _controller = ControllerFactory::createControllerFromConfig(*controllerConfig);
    _controller->begin();
}

void App::loadControllerRules()
{
    if (_controller == nullptr)
    {
        return;
    }

    _ruleEngine = new RuleEngine(_controller);

    // Refresh from NVS to reflect any recent saves
    _store.loadControllerRulesConfig();
    ControllerRule *rules = _store.getControllerRulesConfig();
    for (int i = 0; i < MAX_RULES; i++)
    {
        if (strlen(rules[i].id) > 0)
        {
            _ruleEngine->addRule(rules[i]);
        }
    }
}

void App::loadMotors()
{
    // Get the loaded brushless motor configs
    MotorConfig *configs = _store.getMotorsConfig();
    for (auto& pair : _motors) {
        delete pair.second;
    }
    _motors.clear();

    // Initialize Motor instances for each valid config
    for (int i = 0; i < MAX_MOTORS; i++)
    {
        // Check if this config slot has a valid ID (not empty)
        if (strlen(configs[i].id) > 0)
        {
            _motors[configs[i].id] = new Motor(configs[i]);
        }
    }
}

void App::loadBrushlessMotors()
{
    // Get the loaded brushless motor configs
    BrushlessMotorConfig *configs = _store.getBrushlessMotorsConfig();
    for (auto& pair : _brushlessMotors) {
        delete pair.second;
    }
    _brushlessMotors.clear();

    // Initialize Motor instances for each valid config
    for (int i = 0; i < MAX_BRUSHLESS_MOTORS; i++)
    {
        // Check if this config slot has a valid ID (not empty)
        if (strlen(configs[i].id) > 0)
        {
            _brushlessMotors[configs[i].id] = new BrushlessMotor(configs[i]);
        }
    }
}

void App::loadServos()
{
    // Get the loaded servos configs
    ServoConfig *configs = _store.getServosConfig();
    for (auto& pair : _servos) {
        delete pair.second;
    }
    _servos.clear();

    // Initialize Servos instances for each valid config
    for (int i = 0; i < MAX_SERVOS; i++)
    {
        // Check if this config slot has a valid ID (not empty)
        if (strlen(configs[i].id) > 0)
        {
            _servos[configs[i].id] = new Servo(configs[i]);
        }
    }

    Serial.println("Servos loaded: " + String(_servos.size()));
}