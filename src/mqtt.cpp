#include "mqtt.h"

WiFiClient wifiClient;
PubSubClient mqtt_client(wifiClient);
char msg[MQTT_BUFFER_LENGTH];
bool disconnected = false;
void (*hvac_state_function)(JsonDocument doc);

void mqtt_connect() {
  setup_wifi();
  setup_mqtt();
}

void setup_wifi() {
  WiFi.setAutoReconnect(true);
  WiFi.begin(MY_SSID, MY_PWD);
  unsigned long start = millis();
  unsigned long now = 0;
  while (true) {
    delay(500);
    now = millis();
    if (WiFi.status() == WL_CONNECTED)
    {
      // Serial.println("Connected");
      break;
    }    
    if (now - start >= WIFI_CONNECTION_TIMEOUT)
    {
      // Serial.println("Connection failed");
    }
  }
}

void reconnect_wifi()
{
  WiFi.disconnect();
  WiFi.reconnect();
  unsigned long start = millis();
  unsigned long now = 0;
  while (true)
  {
    now = millis();
    if (WiFi.status() == WL_CONNECTED)
    {
      break;
    }
    if (now - start >= WIFI_CONNECTION_TIMEOUT)
    {
      // Serial.println("Connection failed");
    }
  }
}

void mqtt_loop() {
  if (!WiFi.isConnected()) {
    disconnected = true;
    reconnect_wifi();
  }
  if (!mqtt_client.connected()) {
    disconnected = true;
    setup_mqtt();
  }
  mqtt_client.loop();
  // update_signal();
  // mqtt_client.publish(MY_MQTT_SIGNAL_TOPIC, signalStrength);
}

void setup_mqtt() {
  mqtt_client.setServer(MY_MQTT_URL, 1883);
  mqtt_client.setCallback(mqtt_callback);
  while (!mqtt_client.connected()) {
    String client_id = "midea-hvac-" + String(WiFi.macAddress());
    if (mqtt_client.connect(client_id.c_str())) {
      mqtt_client.subscribe(MY_MQTT_IN_TOPIC);
      if (disconnected)
      {
        disconnected = false;
        mqtt_client.publish(MY_MQTT_STATUS_TOPIC, "Reconnected");
      } else {
        mqtt_client.publish(MY_MQTT_STATUS_TOPIC, "Connected");
      }
    }
  }
}

void mqtt_callback(char *topic, byte *payload, unsigned int length) {
  char *output = reinterpret_cast<char *>(payload);
  JsonDocument doc;
  deserializeJson(doc, output);
  hvac_state_function(doc);
}

void mqtt_publish(char *topic, char *publishMessage) {
  snprintf(msg, MQTT_BUFFER_LENGTH, publishMessage);
  mqtt_client.publish(topic, msg);
}

void publish_state_mqtt(JsonDocument doc) {
  char output[MQTT_BUFFER_LENGTH];
  serializeJson(doc, output, MQTT_BUFFER_LENGTH);
  mqtt_publish(MY_MQTT_OUT_TOPIC, output);
}

void publish_capabilities_mqtt(JsonDocument doc) {
  char output[MQTT_BUFFER_LENGTH];
  serializeJson(doc, output, MQTT_BUFFER_LENGTH);
  mqtt_publish(MY_MQTT_CAPABILITIES_TOPIC, output);
}

void set_callback_function(void (*callback)(JsonDocument doc)) {
  hvac_state_function = callback;
}