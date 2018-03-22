/**************************************************************
 * How to trigger an OTA update?
 *  https://arduino-esp8266.readthedocs.io/en/latest/ota_updates/readme.html
 */

#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
void esperRestart();

String overTheAirURL;

void esperOTA() {

  EsperState::set(MODE_OTA_UPGRADE);

  E_DEBUG("Firmware update URL: %s", overTheAirURL.c_str());
  switch (ESPhttpUpdate.update(overTheAirURL, BOARD_FIRMWARE_VERSION)) {
  case HTTP_UPDATE_FAILED:
    E_DEBUG("Firmware update failed (error %i): %s ", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
    EsperState::set(MODE_ERROR);
    break;
  case HTTP_UPDATE_NO_UPDATES:
    E_DEBUG("No firmware updates available.");
    EsperState::set(MODE_CONNECTING_MQTT);
    break;
  case HTTP_UPDATE_OK:
    E_DEBUG("Firmware update: OK.");
    delay(10);
    esperRestart();
    break;
  }
}
