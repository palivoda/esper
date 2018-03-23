//callbacks for implementation in sketch
void connected();
void tictac();
void disconnecting();

#include <Arduino.h>
#include <ESP8266WiFi.h>

#include "utils\Debug.hpp"
#include "Settings.hpp"
#include "EsperState.hpp"
#include "EsperBeat.hpp"
#include "ConfigStore.hpp"
#include "hardware\ResetButton.hpp"
#include "hardware\Indicator.hpp"
#include "EsperOTA.hpp"
#include "EsperScript.hpp"
#include "EsperMqttClient.hpp"
#include "ConfigMode.hpp"

inline void EsperState::set(State m) {
  if (state != m) {
    E_DEBUG("ESPER state: %s => %s", ESPER_STATE[state], ESPER_STATE[m]);
    if (is(MODE_RUNNING)) disconnecting();
    state = m;
    indicator.updateState();
    if (is(MODE_RUNNING)) connected(); //TODO: reconnect scenario will cause call multiple times
  }
}

class EsperLib {
public:

  void begin()
  {
    E_DEBUG("");
    E_DEBUG("Hardware v%s, Firmware %s", BOARD_HARDWARE_VERSION, BOARD_FIRMWARE_VERSION);

    randomSeed(ESP.getChipId());

    indicator_init();
    button_init();
    config_init();
    beat_init();
    EsperId.init();

    if (EsperConfig.flagConfig) {
      EsperState::set(MODE_CONNECTING_NET);
    } else {
      EsperState::set(MODE_WAIT_CONFIG);
    }

  }

  void run() {
    switch (EsperState::get()) {
    case MODE_WAIT_CONFIG:
    case MODE_CONFIGURING:       esperConfigMode();    break;
    case MODE_CONNECTING_NET:    esperConnectNet();    break;
    case MODE_CONNECTING_MQTT:   esperConnectMqtt();   break;
    case MODE_RUNNING:           esperRun();           break;
    case MODE_OTA_UPGRADE:       esperOTA();           break;
    case MODE_SWITCH_TO_STA:     esperSwitchToSTA();   break;
    case MODE_RESET_CONFIG:      esperResetConfig();   break;
    default:                     esperError();         break;
    }
  }

};

EsperLib Esper;
