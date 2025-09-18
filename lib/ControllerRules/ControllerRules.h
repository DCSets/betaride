#ifndef CONTROLLER_RULES_H
#define CONTROLLER_RULES_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <memory> // for std::unique_ptr
#include <ConfiguratorHelpers.h>
#include <constants.h>
#include <enums.h>
#include <structs.h>

struct RuleCondition
{
    int channel;
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
            this->channel = doc["channel"] | -1;
            this->channelFunction = static_cast<ChannelFunction>(doc["channelFunction"] | -1);
            this->channelValue = doc["channelValue"] | -1;
            this->channelFrom = doc["channelFrom"] | -1;
            this->channelTo = doc["channelTo"] | -1;
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
    char resourceIds[10][13];
    int count; // actual number of resourceIds in the array
    int value; // for SPEED_PERCENT single-value usage
    int from;  // for SPEED_RANGE
    int to;    // for SPEED_RANGE

    RuleEffect() : count(0) {} // Initialize count to 0

    virtual ~RuleEffect() {}
    
    virtual const char *type() const = 0;

    virtual void fromJson(String json) = 0;

    virtual void toJson(String &outJson) const = 0;
};

struct RuleEffectMotor : public RuleEffect
{
    MotorDirection direction;
    MotorFunction function;

    const char *type() const override { return TYPE_MOTORS; }

    void fromJson(String json) override
    {
        JsonDocument doc;
        if (validateJsonHelper(json.c_str(), doc, "RuleEffect"))
        {
            // Clear the resourceIds array first
            memset(this->resourceIds, 0, sizeof(this->resourceIds));
            this->count = 0; // Reset count
            
            // Handle resourceIds as JSON array
            if (doc["resourceIds"].is<JsonArray>()) {
                JsonArray resourceIdsArray = doc["resourceIds"];
                int arrayCount = min((int)resourceIdsArray.size(), 10); // Limit to array size
                for (int i = 0; i < arrayCount; i++) {
                    const char* resourceId = resourceIdsArray[i];
                    if (resourceId) {
                        strncpy(this->resourceIds[i], resourceId, 12); // Leave space for null terminator
                        this->resourceIds[i][12] = '\0'; // Ensure null termination
                        this->count++; // Increment count for each valid resourceId
                    }
                }
            }
            
            this->value = doc["value"] | -1;
            this->from = doc["from"] | -1;
            this->to = doc["to"] | -1;
            this->direction = static_cast<MotorDirection>(doc["direction"] | -1);
            this->function = static_cast<MotorFunction>(doc["function"] | -1);
        }
    }
    void toJson(String &outJson) const override
    {
        JsonDocument doc;
        
        // Add base class data
        doc["type"] = type();
        
        // Serialize resourceIds array
        JsonArray resourceIdsArray = doc["resourceIds"].to<JsonArray>();
        for (int i = 0; i < 10; i++) {
            if (strlen(this->resourceIds[i]) > 0) {
                resourceIdsArray.add(this->resourceIds[i]);
            }
        }
        
        doc["value"] = this->value;
        doc["from"] = this->from;
        doc["to"] = this->to;
        
        // Add derived class data
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
        JsonDocument doc;
        if (validateJsonHelper(json.c_str(), doc, "RuleEffectBrushlessMotor"))
        {
            // Clear the resourceIds array first
            memset(this->resourceIds, 0, sizeof(this->resourceIds));
            this->count = 0; // Reset count
            
            // Handle resourceIds as JSON array
            if (doc["resourceIds"].is<JsonArray>()) {
                JsonArray resourceIdsArray = doc["resourceIds"];
                int arrayCount = min((int)resourceIdsArray.size(), 10); // Limit to array size
                for (int i = 0; i < arrayCount; i++) {
                    const char* resourceId = resourceIdsArray[i];
                    if (resourceId) {
                        strncpy(this->resourceIds[i], resourceId, 12); // Leave space for null terminator
                        this->resourceIds[i][12] = '\0'; // Ensure null termination
                        this->count++; // Increment count for each valid resourceId
                    }
                }
            }
            
            this->value = doc["value"] | -1;
            this->from = doc["from"] | -1;
            this->to = doc["to"] | -1;
            this->direction = static_cast<MotorDirection>(doc["direction"] | -1);
            this->function = static_cast<MotorFunction>(doc["function"] | -1);
        }
    }
    void toJson(String &outJson) const override
    {
        JsonDocument doc;
        
        // Add base class data
        doc["type"] = type();
        
        // Serialize resourceIds array
        JsonArray resourceIdsArray = doc["resourceIds"].to<JsonArray>();
        for (int i = 0; i < 10; i++) {
            if (strlen(this->resourceIds[i]) > 0) {
                resourceIdsArray.add(this->resourceIds[i]);
            }
        }
        
        doc["value"] = this->value;
        doc["from"] = this->from;
        doc["to"] = this->to;
        
        // Add derived class data
        doc["direction"] = static_cast<int>(this->direction);
        doc["function"] = static_cast<int>(this->function);
        
        serializeJson(doc, outJson);
    }
};

struct RuleEffectServo : public RuleEffect
{
    ServoFunction function;

    const char *type() const override { return TYPE_SERVOS; }

    void fromJson(String json) override
    {
        JsonDocument doc;
        if (validateJsonHelper(json.c_str(), doc, "RuleEffectServo"))
        {
            // Clear the resourceIds array first
            memset(this->resourceIds, 0, sizeof(this->resourceIds));
            this->count = 0; // Reset count
            
            // Handle resourceIds as JSON array
            if (doc["resourceIds"].is<JsonArray>()) {
                JsonArray resourceIdsArray = doc["resourceIds"];
                int arrayCount = min((int)resourceIdsArray.size(), 10); // Limit to array size
                for (int i = 0; i < arrayCount; i++) {
                    const char* resourceId = resourceIdsArray[i];
                    if (resourceId) {
                        strncpy(this->resourceIds[i], resourceId, 12); // Leave space for null terminator
                        this->resourceIds[i][12] = '\0'; // Ensure null termination
                        this->count++; // Increment count for each valid resourceId
                    }
                }
            }
            
            this->value = doc["value"] | -1;
            this->from = doc["from"] | -1;
            this->to = doc["to"] | -1;
            this->function = static_cast<ServoFunction>(doc["function"] | -1);
        }
    }
    void toJson(String &outJson) const override
    {
        JsonDocument doc;
        
        // Add base class data
        doc["type"] = type();
        
        // Serialize resourceIds array
        JsonArray resourceIdsArray = doc["resourceIds"].to<JsonArray>();
        for (int i = 0; i < 10; i++) {
            if (strlen(this->resourceIds[i]) > 0) {
                resourceIdsArray.add(this->resourceIds[i]);
            }
        }
        
        doc["value"] = this->value;
        doc["from"] = this->from;
        doc["to"] = this->to;
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
        this->isNew = doc["isNew"] | false;
        this->hasSubCondition = doc["hasSubCondition"] | false;

        // Parse condition as JSON object
        if ( doc["condition"].is<JsonObject>())
        {
            String conditionJson;
            serializeJson(doc["condition"], conditionJson);
            this->condition = RuleCondition(conditionJson);
        }

        // Parse subCondition as JSON object (if present)
        if (this->hasSubCondition && doc["subCondition"].is<JsonObject>())
        {
            String subConditionJson;
            serializeJson(doc["subCondition"], subConditionJson);
            this->subCondition = RuleCondition(subConditionJson);
        }

        // Parse effect as JSON object
        if (doc["effect"].is<JsonObject>())
        {
            String effectJson;
            serializeJson(doc["effect"], effectJson);
            
            // Try to determine effect type from the effect object
            String effectType = doc["effect"]["type"] | "";
            // If no type in effect, try to infer from the rule type or create a default
            if (effectType == TYPE_MOTORS)
            {
                effect.reset(new RuleEffectMotor());
            }
            else if (effectType == TYPE_BRUSHLESS_MOTORS)
            {
                effect.reset(new RuleEffectBrushlessMotor());
            }
            else if (effectType == TYPE_SERVOS)
            {
                effect.reset(new RuleEffectServo());
            }
            else
            {
                Serial.print("Unknown effect type: ");
                Serial.println(effectType);
                effect.reset();
            }

            if (effect)
            {
                effect->fromJson(effectJson);
            }
            else
            {
                Serial.println("No effect created");
            }
        }
        else
        {
            Serial.println("No effect found in JSON or not a JSON object");
        }
    }

    void toJson(String &outJson) const
    {
        JsonDocument doc;
        doc["id"] = this->id;
        doc["name"] = this->name;
        doc["type"] = this->type;
        doc["isNew"] = this->isNew;
        doc["hasSubCondition"] = this->hasSubCondition;

        // Create nested JSON objects instead of strings
        JsonObject conditionObj = doc["condition"].to<JsonObject>();
        conditionObj["channel"] = this->condition.channel;
        conditionObj["channelFunction"] = static_cast<int>(this->condition.channelFunction);
        conditionObj["channelValue"] = this->condition.channelValue;
        conditionObj["channelFrom"] = this->condition.channelFrom;
        conditionObj["channelTo"] = this->condition.channelTo;

        JsonObject subConditionObj = doc["subCondition"].to<JsonObject>();
        subConditionObj["channel"] = this->subCondition.channel;
        subConditionObj["channelFunction"] = static_cast<int>(this->subCondition.channelFunction);
        subConditionObj["channelValue"] = this->subCondition.channelValue;
        subConditionObj["channelFrom"] = this->subCondition.channelFrom;
        subConditionObj["channelTo"] = this->subCondition.channelTo;

        if (this->effect)
        {
            JsonObject effectObj = doc["effect"].to<JsonObject>();
            effectObj["type"] = this->effect->type();
            
            // Serialize resourceIds array
            JsonArray resourceIdsArray = effectObj["resourceIds"].to<JsonArray>();
            for (int i = 0; i < 10; i++) {
                if (strlen(this->effect->resourceIds[i]) > 0) {
                    resourceIdsArray.add(this->effect->resourceIds[i]);
                }
            }
            
            effectObj["value"] = this->effect->value;
            effectObj["from"] = this->effect->from;
            effectObj["to"] = this->effect->to;
            
            // Add derived class specific fields based on type
            String effectType = this->effect->type();
            if (effectType == TYPE_BRUSHLESS_MOTORS)
            {
                RuleEffectBrushlessMotor* brushlessEffect = static_cast<RuleEffectBrushlessMotor*>(this->effect.get());
                effectObj["direction"] = static_cast<int>(brushlessEffect->direction);
                effectObj["function"] = static_cast<int>(brushlessEffect->function);
            }
            else if (effectType == TYPE_MOTORS)
            {
                RuleEffectMotor* motorEffect = static_cast<RuleEffectMotor*>(this->effect.get());
                effectObj["direction"] = static_cast<int>(motorEffect->direction);
                effectObj["function"] = static_cast<int>(motorEffect->function);
            }
            else if (effectType == TYPE_SERVOS)
            {
                RuleEffectServo* servoEffect = static_cast<RuleEffectServo*>(this->effect.get());
                effectObj["function"] = static_cast<int>(servoEffect->function);
            }
        }

        serializeJson(doc, outJson);
    }
};

#endif