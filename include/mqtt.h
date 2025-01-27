#include "secrets.h"
#include "mqtt_settings.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Appliance/AirConditioner/AirConditioner.h>

using namespace dudanov::midea::ac;

void mqtt_connect();

void setup_wifi();

void reconnect_wifi();

void mqtt_loop();

void setup_mqtt();

void mqtt_callback(char *topic, byte *payload, unsigned int length);

void mqtt_publish(char *topic, char *publishMessage);

void publish_state_mqtt(JsonDocument doc);

void publish_capabilities_mqtt(JsonDocument doc);

void set_callback_function(void (*callback)(JsonDocument doc));