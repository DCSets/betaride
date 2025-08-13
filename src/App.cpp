#include <App.h>
#include <ELRSController.h>

App::App(ConfigStore *store) : _store(*store), _brushlessMotorCount(0), _controllerRulesCount(0), _controller(nullptr)
{
    for (int i = 0; i < MAX_MOTORS; i++)
    {
        _brushlessMotors[i] = nullptr;
    }
}

App::~App()
{
    // Clean up dynamically allocated BrushlessMotor instances
    for (int i = 0; i < MAX_MOTORS; i++)
    {
        if (_brushlessMotors[i] != nullptr)
        {
            delete _brushlessMotors[i];
            _brushlessMotors[i] = nullptr;
        }
    }
    if (_controller != nullptr)
    {
        delete _controller;
        _controller = nullptr;
    }
}

void App::loadResources()
{
    // this->loadMotors();
    // this->loadElrsRules();
    // this->loadController();

    // // Log how many resources were loaded
    // Serial.print("Loaded resources -> motors: ");
    // Serial.print(_brushlessMotorCount);
    // Serial.print(", rules: ");
    // Serial.print(_elrsRulesCount);
    // Serial.print(", controller: ");
    // Serial.println((_elrsRx != nullptr) ? 1 : 0);
}

void App::loadMotors()
{
    Serial.println("Cleaning motors (APP)");
    // Clean up any existing motors first
    for (int i = 0; i < MAX_MOTORS; i++)
    {
        if (_brushlessMotors[i] != nullptr)
        {
            delete _brushlessMotors[i];
            _brushlessMotors[i] = nullptr;
        }
    }
    Serial.println("Loading motors (APP)");
    // Get the loaded brushless motor configs
    BrushlessMotorConfig *configs = _store.getBrushlessMotorsConfig();
    _brushlessMotorCount = 0;

    // Initialize BrushlessMotor instances for each valid config
    for (int i = 0; i < MAX_MOTORS; i++)
    {
        // Check if this config slot has a valid ID (not empty)
        if (strlen(configs[i].id) > 0)
        {
            Serial.println("Loading motor (APP): " + String(configs[i].id));
            _brushlessMotors[i] = new BrushlessMotor(configs[i]);
            _brushlessMotorCount++;
        }
    }
}

void App::loadControllerRules()
{
    // Refresh from NVS to reflect any recent saves
    _store.loadControllerRulesConfig();

    // Clear local rules storage
    for (int i = 0; i < MAX_RULES; i++)
    {
        memset(&_controllerRules[i], 0, sizeof(ControllerRule));
    }

    ControllerRule *rules = _store.getControllerRulesConfig();
    _controllerRulesCount = 0;

    for (int i = 0; i < MAX_RULES; i++)
    {
        if (strlen(rules[i].id) > 0)
        {
            _controllerRules[i] = rules[i];
            _controllerRulesCount++;
        }
    }
}

void App::loadController()
{
    // Refresh from NVS to reflect any recent saves
    _store.loadELRSConfig();

    if (_controller != nullptr)
    {
        delete _controller;
        _controller = nullptr;
    }

    // Find first ELRS controller config and initialize controller
        ELRSConfig *controllers = _store.getELRSConfig();
    for (int i = 0; i < MAX_CONTROLLERS; i++)
    { 
        if (strlen(controllers[i].id) == 0)
            continue;

        // if (controllers[i].controllerType == ControllerType::ELRS)
        // {
            ELRSConfig *elrsConfig = reinterpret_cast<ELRSConfig *>(&controllers[i]);
            _controller = new ELRSController(*elrsConfig);
            _controller->begin();
            break;
        // }
    }
}

BrushlessMotor *App::findBrushlessById(const char *id)
{
    if (id == nullptr || strlen(id) == 0)
        return nullptr;
    for (int i = 0; i < MAX_MOTORS; i++)
    {
        if (_brushlessMotors[i] == nullptr)
            continue;
        if (strcmp(_brushlessMotors[i]->getId(), id) == 0)
        {
            return _brushlessMotors[i];
        }
    }
    return nullptr;
}

void App::loop()
{
    // Quick checks
    if (_controller == nullptr)
        return;
    if (_controllerRulesCount == 0)
        return;

    // Update controller state
    _controller->loop();

    int channels[16] = {0};
    _controller->getAllChannels(channels);

    // Evaluate rules
    for (int i = 0; i < _controllerRulesCount && i < MAX_RULES; i++)
    {
        ControllerRule &rule = _controllerRules[i];
        if (strlen(rule.id) == 0)
            continue;

            auto evalCond = [&](const RuleCondition &c)
        {
            // channel expected as string number like "1".."16"
            int chIndex = c.channel;
            if (chIndex < 1 || chIndex > 16)
                return false;
            int value = channels[chIndex - 1];
            switch (c.channelFunction)
            {
            case ChannelFunction::FULL:
                return true;
            case ChannelFunction::RANGE:
                return value >= c.channelFrom && value <= c.channelTo;
            case ChannelFunction::EXACT:
                return value == c.channelValue;
            case ChannelFunction::ABOVE:
                return value > c.channelValue;
            case ChannelFunction::BELOW:
                return value < c.channelValue;
            case ChannelFunction::ABOVE_OR_EQUAL:
                return value >= c.channelValue;
            case ChannelFunction::BELOW_OR_EQUAL:
                return value <= c.channelValue;
            default:
                return false;
            }
        };

        if (!evalCond(rule.condition))
            continue;
        if (rule.hasSubCondition && !evalCond(rule.subCondition))
            continue;

        // Apply effect
        // switch (rule.effect->type())
        // {
        // case RuleEffectType::Motor:
        // case RuleEffectType::BrushlessMotor:
        // {
        //     const char *targetId = (rule.effect.type == RuleEffectType::Motor)
        //                                ? rule.effect.motor.resourceId
        //                                : rule.effect.brushless.resourceId;
        //     BrushlessMotor *motor = findBrushlessById(targetId);
        //     if (motor != nullptr)
        //     {
        //         const MotorDirection dir = (rule.effect.type == RuleEffectType::Motor)
        //                                        ? rule.effect.motor.direction
        //                                        : rule.effect.brushless.direction;
        //         const MotorFunction func = (rule.effect.type == RuleEffectType::Motor)
        //                                        ? rule.effect.motor.function
        //                                        : rule.effect.brushless.function;
        //         int value = (rule.effect.type == RuleEffectType::Motor)
        //                         ? rule.effect.motor.value
        //                         : rule.effect.brushless.value;
        //         int from = (rule.effect.type == RuleEffectType::Motor)
        //                        ? rule.effect.motor.from
        //                        : rule.effect.brushless.from;
        //         int to = (rule.effect.type == RuleEffectType::Motor)
        //                      ? rule.effect.motor.to
        //                      : rule.effect.brushless.to;

        //         motor->setDirection(dir);

        //         int throttlePercent = 0;
        //         switch (func)
        //         {
        //         case MotorFunction::SPEED_PERCENT:
        //             throttlePercent = constrain(value, 0, 100);
        //             break;
        //         case MotorFunction::SPEED_RANGE:
        //         {
        //             // Map current channel value to range [from..to] into 0..100
        //             int chIndex = atoi(rule.condition.channel);
        //             int chVal = (chIndex >= 1 && chIndex <= 16) ? channels[chIndex - 1] : 0;
        //             throttlePercent = map(constrain(chVal, from, to), from, to, 0, 100);
        //             break;
        //         }
        //         case MotorFunction::SPEED_FULL:
        //         {
        //             // Map full stick range to 0..100
        //             int chIndex = atoi(rule.condition.channel);
        //             int chVal = (chIndex >= 1 && chIndex <= 16) ? channels[chIndex - 1] : 0;
        //             throttlePercent = map(chVal, ElrsRx::MIN_STICK_VALUE, ElrsRx::MAX_STICK_VALUE, 0, 100);
        //             throttlePercent = constrain(throttlePercent, 0, 100);
        //             break;
        //         }
        //         default:
        //             break;
        //         }
        //         motor->setThrottle(throttlePercent);
        //     }
        //     break;
        // }
        // case RuleEffectType::Servo:
        // {
        //     // TODO: implement servo handling once Servo resources exist
        //     break;
        // }
        // default:
        //     break;
        // }
    }

    // Tick motors after applying effects
    for (int i = 0; i < MAX_MOTORS; i++)
    {
        if (_brushlessMotors[i] != nullptr)
        {
            _brushlessMotors[i]->loop();
        }
    }
}