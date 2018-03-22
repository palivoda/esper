#define USE_WEMOS_D1_MINI_BOARD     // Board pinout selection as defined in Setting.hpp
#define ESPER_DEBUG                 // Comment this out to disable debug prints
//#define ESPER_PRINT Serial        // Debug output can be redefined

#include <Esper.h>

void setup() {

  ESPER_DEBUG_SETUP(74880); //ESP8266 baud rate

  //Init EEPROM
  Esper.begin(2); //2 seconds for tic tac

}

void loop() {

  //Important for WiFi and MQTT connection setup
  Esper.run();

}

void connected() {
  E_DEBUG("Connected...");
}

void disconnecting() {
  E_DEBUG("Disconnecting...");
}

void tictac()
{

    String message = "Lets go party! There are " + millis() + " people!";

    E_DEBUG("Sending %s ", message.c_str());

    mqtt.publish("hello/world", message);

    E_DEBUG_ASSERT(millis() > 1000);

    if (millis() > 1000) {
      E_DEBUG("Party face control set :)");
      E_DEBUG_BREAK();
    }

}
