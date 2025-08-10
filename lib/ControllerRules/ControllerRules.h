#ifndef CONTROLLER_RULES_H
#define CONTROLLER_RULES_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <memory> // for std::unique_ptr
#include <ConfiguratorHelpers.h>
#include <constants.h>

struct RuleCondition
{
    char channel[8];
    ChannelFunction channelFunction;
    int channelValue; // used for EXACT/ABOVE/BELOW/AE/BE
    int channelFrom;  // used for RANGE
    int channelTo;    // used for RANGE
    RuleCondition() = default;
    RuleCondition(String json)
    {
        JsonDocument doc;
        if (validateJsonHelper(json.c_str(), doc, "ELRS rule condition"))
        {
            strncpy(this->channel, doc["channel"], sizeof(this->channel));
            this->channelFunction = static_cast<ChannelFunction>(doc["channelFunction"].as<int>());
            this->channelValue = doc["channelValue"];
            this->channelFrom = doc["channelFrom"];
            this->channelTo = doc["channelTo"];
        }
    }

    void toJson(String &outJson) const
    {
        JsonDocument doc;
        doc["channel"] = this->channel;
        doc["channelFunction"] = static_cast<int>(this->channelFunction);
        doc["channelValue"] = this->channelValue;
        doc["channelFrom"] = this->channelFrom;
        doc["channelTo"] = this->channelTo;
        serializeJson(doc, outJson);
    }
};

struct RuleEffect
{
    char resourceId[32];
    int value; // for SPEED_PERCENT/SPEED_FULL single-value usage
    int from;  // for SPEED_RANGE
    int to;    // for SPEED_RANGE

    virtual ~RuleEffect() {}
    virtual const char *type() const = 0;

    virtual void fromJson(String json)
    {
        JsonDocument doc;
        if (validateJsonHelper(json.c_str(), doc, "RuleEffect"))
        {
            strncpy(this->resourceId, doc["resourceId"], sizeof(this->resourceId));
            this->value = doc["value"];
            this->from = doc["from"];
            this->to = doc["to"];
        }
    }

    virtual void toJson(String &outJson) const
    {
        JsonDocument doc;
        doc["type"] = type();
        doc["resourceId"] = this->resourceId;
        doc["value"] = this->value;
        doc["from"] = this->from;
        doc["to"] = this->to;
        serializeJson(doc, outJson);
    }
};

struct RuleEffectMotor : public RuleEffect
{
    MotorDirection direction;
    MotorFunction function;

    const char *type() const override { return TYPE_MOTORS; }

    void fromJson(String json) override
    {
        RuleEffect::fromJson(json);
        JsonDocument doc;
        if (validateJsonHelper(json.c_str(), doc, "RuleEffect"))
        {
            this->direction = static_cast<MotorDirection>(doc["direction"].as<int>());
            this->function = static_cast<MotorFunction>(doc["function"].as<int>());
        }
    }
    void toJson(String &outJson) const override
    {
        RuleEffect::toJson(outJson);
        JsonDocument doc;
        doc["direction"] = static_cast<int>(this->direction);
        doc["function"] = static_cast<int>(this->function);
        serializeJson(doc, outJson);
    }
};

struct RuleEffectBrushlessMotor : public RuleEffect
{
    MotorDirection direction;
    MotorFunction function;

    const char *type() const override { return TYPE_BRUSHLESS_MOTORS; }

    void fromJson(String json) override
    {
        RuleEffect::fromJson(json);
        JsonDocument doc;
        if (validateJsonHelper(json.c_str(), doc, "RuleEffectMotor"))
        {
            this->direction = static_cast<MotorDirection>(doc["direction"].as<int>());
            this->function = static_cast<MotorFunction>(doc["function"].as<int>());
        }
    }
    void toJson(String &outJson) const override
    {
        RuleEffect::toJson(outJson);
        JsonDocument doc;
        doc["direction"] = static_cast<int>(this->direction);
        doc["function"] = static_cast<int>(this->function);
        serializeJson(doc, outJson);
    }
};

struct RuleEffectServo : public RuleEffect
{
    // TODO: It's a mock (!)
    MotorDirection direction;
    MotorFunction function;

    const char *type() const override { return TYPE_SERVOS; }

    void fromJson(String json) override
    {
        RuleEffect::fromJson(json);
        JsonDocument doc;
        if (validateJsonHelper(json.c_str(), doc, "RuleEffectMotor"))
        {
            this->direction = static_cast<MotorDirection>(doc["direction"].as<int>());
            this->function = static_cast<MotorFunction>(doc["function"].as<int>());
        }
    }
    void toJson(String &outJson) const override
    {
        RuleEffect::toJson(outJson);
        JsonDocument doc;
        doc["direction"] = static_cast<int>(this->direction);
        doc["function"] = static_cast<int>(this->function);
        serializeJson(doc, outJson);
    }
};

struct ControllerRule : public Resource
{
    char id[32];
    char name[32];
    char type[32];
    bool isNew;
    bool hasSubCondition;
    RuleCondition condition;
    RuleCondition subCondition;
    std::shared_ptr<RuleEffect> effect;
    ControllerRule() = default;
    ControllerRule(String json)
    {
        JsonDocument doc;
        DeserializationError err = deserializeJson(doc, json);

        if (err)
        {
            Serial.println(F("JSON parse error - Controller rule"));
            return;
        }

        strncpy(this->id, doc["id"], sizeof(this->id));
        strncpy(this->name, doc["name"], sizeof(this->name));
        strncpy(this->type, doc["type"], sizeof(this->type));
        this->isNew = doc["isNew"];
        this->hasSubCondition = doc["hasSubCondition"];

        // Parse nested JSON strings
        String conditionJson = doc["condition"];
        String subConditionJson = doc["subCondition"];
        String effectJson = doc["effect"];

        String effectType = doc["effect"]["type"] | "";

        if (effectType == TYPE_MOTORS)
            effect.reset(new RuleEffectMotor());
        else if (effectType == TYPE_BRUSHLESS_MOTORS)
            effect.reset(new RuleEffectBrushlessMotor());
        else if (effectType == TYPE_SERVOS)
            effect.reset(new RuleEffectServo());
        else
            effect.reset();

        if (effect)
        {
            effect->fromJson(effectJson);
        }

        this->condition = RuleCondition(conditionJson);
        this->subCondition = RuleCondition(subConditionJson);
    }

    void toJson(String &outJson) const
    {
        JsonDocument doc;
        doc["id"] = this->id;
        doc["name"] = this->name;
        doc["type"] = this->type;
        doc["isNew"] = this->isNew;
        doc["hasSubCondition"] = this->hasSubCondition;

        String conditionJson, subConditionJson, effectJson;
        this->condition.toJson(conditionJson);
        this->subCondition.toJson(subConditionJson);
        if (this->effect)
        {
            this->effect->toJson(effectJson);
        }

        doc["condition"] = conditionJson;
        doc["subCondition"] = subConditionJson;
        doc["effect"] = effectJson;

        serializeJson(doc, outJson);
    }
};

#endif