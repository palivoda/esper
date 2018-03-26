#ifndef ESPER_LIB_BULD

#define USE_WEMOS_D1_MINI_BOARD     // Uncomment the board you are using
#define ESPER_DEBUG                 // Comment this out to disable debug prints
#define ESPER_BEAT_S 3              //Sensor pull time
#include "Esper.hpp"

void setup() {
  ESPER_DEBUG_SETUP(74880);
  Esper.begin();
}

void loop() {
  Esper.run();
}

void connected() {
  E_DEBUG("Connected...");
}

void disconnecting() {
  E_DEBUG("Disconnecting...");
}

// This function invoked by Ticker timer every 3 seconds to send values to
// MQTT broker in JSON format. You can add other value recipients here.
void tictac()
{
  int up = millis() / 1000;
  E_DEBUG("Up: %i", up);

  //Push data to MQTT
  char mqttMsg[MQTT_MAX_PACKET_SIZE];

  snprintf(mqttMsg, MQTT_MAX_PACKET_SIZE,
    R"json({"up":"%i","id":"%s"})json",
    up, WiFi.macAddress().c_str());

  if (mqtt.connected())
    if (!mqtt.publish(EsperConfig.mqttFeedTopic, mqttMsg))
      {E_DEBUG("MQTT Failed.");}

}

#endif
