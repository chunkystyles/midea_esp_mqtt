#include <Arduino.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <Appliance/AirConditioner/AirConditioner.h>
#include "mqtt_settings.h"
#include "mqtt.h"

void setup_hvac();

void send_capabilities();

void loop_hvac();

JsonDocument convert_ac_to_json(String updateType);

void callback_function();

void set_state(JsonDocument doc);