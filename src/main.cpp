#include <Arduino.h>
#include <constants.h>
#include <ConfigStore.h>
#include <BrushlessMotor.h>
#include <Controller.h>
#include <ConfiguratorSerial.h>
#include <App.h>
#include <ps5Controller.h>

ConfiguratorSerial *serial = nullptr;
ConfigStore *store = nullptr;
App *app = nullptr;

void setup()
{
  Serial.begin(115200);

  store = new ConfigStore();
  serial = new ConfiguratorSerial(store);
  app = new App(store);
}

void loop()
{     
  serial->loop();
  if(!serial->isConnected()) {
    app->loop();
  }
  if(serial->isControllerTesting()) {
    app->testController();
  }
}
