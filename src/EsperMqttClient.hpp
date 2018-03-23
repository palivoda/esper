/*
*   EsperMqttClient class overrides PubSubClient popular MQTT library for:
*   1) User frindly error messages MQTT_STATE during MQTT connection setup
*   2) Trigger OTA update from URL (esper/ota)
*   3) Read read device state and config (esper/echo -> esper->ohce)
*   4) Read and Write hardware pin values
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* MQTT_STATE[10] = {
  "MQTT_CONNECTION_TIMEOUT",     //-4
  "MQTT_CONNECTION_LOST",        //-3
  "MQTT_CONNECT_FAILED",         //-2
  "MQTT_DISCONNECTED",           //-1
  "MQTT_CONNECTED",               //0
  "MQTT_CONNECT_BAD_PROTOCOL",    //1
  "MQTT_CONNECT_BAD_CLIENT_ID",   //2
  "MQTT_CONNECT_UNAVAILABLE",     //3
  "MQTT_CONNECT_BAD_CREDENTIALS", //4
  "MQTT_CONNECT_UNAUTHORIZED",    //5
};

class EsperMqttClient : public PubSubClient {
public:
  static void topicsTrace(char* topic, byte* payload, unsigned int length);
  void topicEcho(char* payload);
  void topicOTA(char* payload);

  EsperMqttClient(WiFiClient& client) {
    setClient(client);
    PubSubClient::setCallback(topicsTrace);
  }

  PubSubClient& setCallback(MQTT_CALLBACK_SIGNATURE) {
      this->callback = callback;
      return *this;
  }

  const char* stateName() {
    return MQTT_STATE[state()+4];
  }

  boolean subscribe(const char* topic) {
      E_DEBUG("MQTT: %s subscribed", topic);
      return PubSubClient::subscribe(topic, 0);
  }

private:
  MQTT_CALLBACK_SIGNATURE; //we keep own pointer to call back
};

WiFiClient wifiClient;
EsperMqttClient mqtt(wifiClient);

void EsperMqttClient::topicsTrace(char* topic, byte* p, unsigned int len) {

    E_DEBUG("MQTT %s (len=%i)", topic, len);

    // Route topics
    p[len] = 0x00; //ensure end of string
    char* payload = reinterpret_cast<char*>(p);

    if (strcmp(topic, OTA_TOPIC) == 0)
      mqtt.topicOTA(payload);
    else if (strncmp(topic, JS_TOPIC, strlen(JS_TOPIC)) == 0)
      esper_script(payload);
    else if (strcmp(topic, ECHO_TOPIC)  == 0)
      mqtt.topicEcho(payload); //respond to initial request
    else {
      E_DEBUG("MQTT: pass througth");
      if (mqtt.callback == NULL)
        mqtt.callback(topic, p, len); //pass forward non-OTA topics
    }

}

void EsperMqttClient::topicEcho(char* p) {

  char mqttBuff[MQTT_MAX_PACKET_SIZE];
  String payload(p);

  if (payload.startsWith("topics")) {

    E_DEBUG("topics");

    snprintf(mqttBuff, MQTT_MAX_PACKET_SIZE,
      R"json({"data":"%s","off":"%s","on":"%s","mac":"%s"})json",
      EsperConfig.mqttFeedTopic,
      EsperConfig.mqttOfflineTopic,
      EsperConfig.mqttOnlineTopic,
      EsperId.c_str()
    );

  } else if (payload.startsWith("mqtt")) {

    E_DEBUG("mqtt");

    snprintf(mqttBuff, MQTT_MAX_PACKET_SIZE,
      R"json({"user":"%s","pass":"%s","host":"%s","port":"%i","retain":"%i","mac":"%s"})json",
      EsperConfig.mqttUser,
      EsperConfig.mqttPass,
      EsperConfig.mqttHost,
      EsperConfig.mqttPort,
      EsperConfig.mqttRetain,
      EsperId.c_str()
    );

  } else if (payload.startsWith("wifi")) {

    E_DEBUG("wifi");

    snprintf(mqttBuff, MQTT_MAX_PACKET_SIZE,
      R"json({"ip":"%s","ssid":"%s","ch":"%i","gw":"%s","mac":"%s"})json",
      WiFi.localIP().toString().c_str(),
      WiFi.SSID().c_str(),
      WiFi.channel(),
      WiFi.gatewayIP().toString().c_str(),
      EsperId.c_str()
    );

  } else {

    E_DEBUG("ota");

    snprintf(mqttBuff, MQTT_MAX_PACKET_SIZE,
      R"json({"board":"%s","fw":"%s","hw":"%s","ms":"%i","mac":"%s"})json",
      BOARD_NAME,
      BOARD_FIRMWARE_VERSION,
      BOARD_HARDWARE_VERSION,
      millis() / 1000,
      EsperId.c_str()
      );

  }

  E_DEBUG("MQTT: %s response %s", OHCE_TOPIC, mqttBuff);

  mqtt.publish(OHCE_TOPIC, mqttBuff);
}

void EsperMqttClient::topicOTA(char* p) {

  String payload(p);

  //URL validation
  if (!payload.startsWith("http"))
    {E_DEBUG("Not http URL"); return;};
  if (!payload.indexOf(BOARD_NAME))
    {E_DEBUG("Expected board %s!", BOARD_NAME); return;};
  if (!payload.indexOf(BOARD_HARDWARE_VERSION))
    {E_DEBUG("Expected board v%s!", BOARD_HARDWARE_VERSION); return;};

  payload.replace(" ","");
  overTheAirURL = payload;

  mqtt.disconnect();
  delay(1000);
  EsperState::set(MODE_OTA_UPGRADE);

}
