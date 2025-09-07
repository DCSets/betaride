#include "RuleEngine.h"

bool RuleEngine::checkCondition(const RuleCondition &cond)
{
    if (!_controller->isConnected())
    {
        return false;
    }
    int val = _controller->getChannel(cond.channel);
    if (val == -1)
    {
        return false;
    }
    switch (cond.channelFunction)
    {
    case ChannelFunction::EXACT:
        return val >= cond.channelValue - _controller->drift && val <= cond.channelValue + _controller->drift;
    case ChannelFunction::ABOVE:
        return val > cond.channelValue;
    case ChannelFunction::ABOVE_OR_EQUAL:
        return val >= cond.channelValue;
    case ChannelFunction::BELOW:
        return val < cond.channelValue;
    case ChannelFunction::BELOW_OR_EQUAL:
        return val <= cond.channelValue;
    case ChannelFunction::RANGE:
        return val >= cond.channelFrom && val <= cond.channelTo;
    case ChannelFunction::FULL:
        return true;
    default:
        return false;
    }
}

int RuleEngine::getMotorSpeed(const ControllerRule &rule)
{
    MotorFunction function = MotorFunction::UNSPECIFIED;
    if (rule.effect->type() == TYPE_MOTORS)
    {
        function = static_cast<RuleEffectMotor *>(rule.effect.get())->function;
    }
    else if (rule.effect->type() == TYPE_BRUSHLESS_MOTORS)
    {
        function = static_cast<RuleEffectBrushlessMotor *>(rule.effect.get())->function;
    }
    else
    {
        Serial.println("Wrong effect");
        return -1;
    }

    if (function == MotorFunction::SPEED_PERCENT)
    {
        return rule.effect->value;
    }

    int channelPercent = -1;
    if (rule.condition.channelFunction == ChannelFunction::FULL)
         channelPercent = _controller->getChannelPercent(rule.condition.channel);

    if (rule.condition.channelFunction == ChannelFunction::RANGE) {
         channelPercent = _controller->getChannelPercent(rule.condition.channel, rule.condition.channelFrom, rule.condition.channelTo);
    }
    if (channelPercent != -1)
    {
        if (function == MotorFunction::SPEED_FULL)
            return map(channelPercent, 0, 100, 0, 100);

        if (function == MotorFunction::SPEED_RANGE)
            return map(channelPercent, 0, 100, rule.effect->from, rule.effect->to);
    }

    return -1;
}

int RuleEngine::getServoAngle(const ControllerRule &rule, Servo *servo)
{
    ServoFunction function = ServoFunction::UNSPECIFIED;
    if (rule.effect->type() == TYPE_SERVOS)
    {
        function = static_cast<RuleEffectServo *>(rule.effect.get())->function;
    }
    else
    {

        Serial.println("Wrong effect");
        return -1;
    }

    if (function == ServoFunction::ANGLE)
    {
        return constrain(rule.effect->value, 0, servo->getMaxAngle());
    }

    int channelPercent = -1;
    if (rule.condition.channelFunction == ChannelFunction::FULL)
    {
        channelPercent = _controller->getChannelPercent(rule.condition.channel);
    }

    if (rule.condition.channelFunction == ChannelFunction::RANGE)
        channelPercent = _controller->getChannelPercent(rule.condition.channel, rule.condition.channelFrom, rule.condition.channelTo);

    if (channelPercent != -1)
    {
        if (function == ServoFunction::ANGLE_FULL)
            return map(channelPercent, 0, 100, 0, servo->getMaxAngle());

        if (function == ServoFunction::ANGLE_RANGE)
            return map(channelPercent, 0, 100, rule.effect->from, rule.effect->to);
    }

    return -1;
}
