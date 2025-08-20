#include "RuleEngine.h"

bool RuleEngine::checkCondition(const RuleCondition &cond)
{
    if(!_controller->isConnected()) {
        Serial.println("Controller not connected");
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
        return val == cond.channelValue;
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
        return -1;
    }

    if (function == MotorFunction::SPEED_PERCENT)
    {
        return rule.effect->value;
    }

    int channelPercent = -1;
    if (rule.condition.channelFunction == ChannelFunction::FULL)
        int channelPercent = _controller->getChannelPercent(rule.condition.channel);

    if (rule.condition.channelFunction == ChannelFunction::RANGE)
        int channelPercent = _controller->getChannelPercent(rule.condition.channel, rule.condition.channelFrom, rule.condition.channelTo);

    if (channelPercent != -1)
    {
        if (function == MotorFunction::SPEED_FULL)
            return map(channelPercent, 0, 100, 0, 100);

        if (function == MotorFunction::SPEED_PERCENT)
            return map(channelPercent, 0, 100, rule.effect->from, rule.effect->to);
    }

    return -1;
}
