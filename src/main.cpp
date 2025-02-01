
#include "hvac.h"
#include "mqtt.h"

void setup() {
  mqtt_connect();
  setup_hvac();
}

void loop() {
  loop_hvac();
  mqtt_loop();
  delay(10);
}