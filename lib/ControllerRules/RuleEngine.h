#ifndef RULE_ENGINE_H
#define RULE_ENGINE_H

#include <ControllerRules.h>
#include <Controller.h>
#include <vector>
#include <constants.h>
#include <enums.h>
#include <Servo.h>

class RuleEngine {
    public:
        RuleEngine(Controller *controller): _controller(controller) {}
        std::vector<ControllerRule> rules;
        void addRule(const ControllerRule& rule) {
            rules.push_back(rule);
        }
        bool hasRules() { return rules.size() > 0; }
        bool checkCondition(const RuleCondition& cond);

        MotorDirection getMotorDirection(const ControllerRule& rule) {
            if(rule.effect->type() == TYPE_MOTORS) {
                return static_cast<RuleEffectMotor*>(rule.effect.get())->direction;
            } else if(rule.effect->type() == TYPE_BRUSHLESS_MOTORS) {
                return static_cast<RuleEffectBrushlessMotor*>(rule.effect.get())->direction;
            }
            
            return MotorDirection::UNSPECIFIED;
        }
        int getMotorSpeed(const ControllerRule& rule);
        int getServoAngle(const ControllerRule& rule, Servo* servo);
    
    private:
        Controller *_controller;
    };

#endif