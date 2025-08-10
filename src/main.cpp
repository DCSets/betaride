#include <Arduino.h>
#include <constants.h>
#include <ConfigStore.h>
#include <BrushlessMotor.h>
#include <Controller.h>
#include <ConfiguratorSerial.h>
#include <App.h>

ConfiguratorSerial * serial = nullptr;
ConfigStore * store = nullptr;
App *app = nullptr;

void setup()
{
  Serial.begin(115200);

  store = new ConfigStore();
  serial = new ConfiguratorSerial(store);



  // config = new CarConfig();
  // app = new App(config);
  // // Mock controller (ELRS)
  // ELRSConfig elrs("ctrl_1", "ELRS", DEFAULT_PIN_RX, DEFAULT_PIN_TX, DEFAULT_RX_UART);
  // config->saveControllerConfig(reinterpret_cast<ControllerConfig*>(&elrs));

  // // Mock brushless motors
  // BrushlessMotorConfig m1("m_1", "Motor1", DEFAULT_ESC_PIN, DEFAULT_ESC_CHANEL);
  // BrushlessMotorConfig m2("m_2", "Motor2", 19, DEFAULT_ESC_CHANEL);
  // BrushlessMotorConfig motors[2] = { m1, m2 };
  // config->saveBrushlessMotorsConfig(motors, 2);

  // // Mock ELRS rule: throttle channel full range -> brushless motor SPEED_FULL forward
  // ELRSRuleCondition cond(DEFAULT_THROTTLE_CHANNEL, ChannelFunction::FULL);
  // RuleEffectBrushlessMotor eff1(m1.id, MotorDirection::Forward, MotorFunction::SPEED_FULL);
  // ELRSRuleEffect re1(eff1);
  // ELRSRule r1("r_1", cond, re1);
  // config->saveElrsRulesConfig(&r1);

  // // Second rule: map same throttle channel to second motor too (or choose a different channel/effect)
  // RuleEffectBrushlessMotor eff2(m2.id, MotorDirection::Forward, MotorFunction::SPEED_FULL);
  // ELRSRuleEffect re2(eff2);
  // ELRSRule r2("r_2", cond, re2);
  // config->saveElrsRulesConfig(&r2);

  // config->loadMotorsConfig();
  // config->loadBrushlessMotorsConfig();
  // config->loadElrsRulesConfig();
  // config->loadControllerConfig();

  // Load resources into the app
  // app->loadResources();
}

void loop()
{
  serial->loop();
}

