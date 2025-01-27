
#include "hvac.h"
#include "mqtt.h"

void setup() {
  setup_hvac();
  mqtt_connect();
  send_capabilities();
}

void loop() {
  loop_hvac();
  mqtt_loop();
  delay(10);
}