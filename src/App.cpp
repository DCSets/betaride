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
            Serial.println("Rule condition not met");
            delay(1000);
            continue;
        }
        if(rule.hasSubCondition && !_ruleEngine->checkCondition(rule.subCondition)) {
            Serial.println("Rule subcondition not met");
            delay(1000);
            continue;
        }

        Serial.println("All good!");
        String resourceId = rule.effect->resourceId;
    }

    
    // // Tick motors after applying effects
    // for (int i = 0; i < MAX_MOTORS; i++)
    // {
    //     if (_brushlessMotors[i] != nullptr)
    //     {
    //         _brushlessMotors[i]->loop();
    //     }
    // }
}

/* RESOURCES MANAGEMENT SECTION */
void App::loadResources()
{
    this->loadController();
    this->loadControllerRules();
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
    if (_ruleEngine != nullptr)
    {
        delete _ruleEngine;
        _ruleEngine = nullptr;
    }

    _brushlessMotors.clear();
    _motors.clear();

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
    BrushlessMotorConfig *configs = _store.getBrushlessMotorsConfig();
    for (auto& pair : _motors) {
        delete pair.second;
    }
    _motors.clear();

    // Initialize BrushlessMotor instances for each valid config
    for (int i = 0; i < MAX_MOTORS; i++)
    {
        // Check if this config slot has a valid ID (not empty)
        if (strlen(configs[i].id) > 0)
        {
            _brushlessMotors[configs[i].id] = new BrushlessMotor(configs[i]);
        }
    }
}