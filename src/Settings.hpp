/*
 * General options
 */

#define BOARD_FIRMWARE_VERSION        "F0.0.1"
#define BOARD_HARDWARE_VERSION        "H0.0.1"

#define BOARD_NAME                    "ESPER device"        // Name of your product. Should match App Export request info.
#define BOARD_VENDOR                  "Company Name"        // Name of your company. Should match App Export request info.

#define PRODUCT_WIFI_SSID             "ESPER device"        // Name of the device, to be displayed during configuration. Should match export request info.
#define BOARD_CONFIG_AP_URL           "esper.iot"           // Config page will be available in a browser at 'http://our-product.cc/'

#define OTA_TOPIC                     "esper/ota"           // MQTT topic to trigger OTA updte on compatible devices
                                                            // Compativility validated by product hame and hardware version
                                                            // File name without spaces <BoartName><HardwareVersion>.bin

#define ECHO_TOPIC                    "esper/echo"          // MQTT topic to collect all online devices status
#define OHCE_TOPIC                    "esper/ohce"          // MQTT topic to collect all online devices status

/*
 * Board configuration (see examples below).
 */

#ifdef USE_WEMOS_D1_MINI_BOARD

  #warning ( "ESP8266 Wemos D1 Mini Board selected" )

  // Custom board configuration
  #define BOARD_BUTTON_PIN            0                     // Pin where user button is attached
  #define BOARD_BUTTON_ACTIVE_LOW     true                  // true if button is "active-low"

  #define BOARD_LED_PIN               2                     // Set LED pin - if you have a single-color LED attached
  #define BOARD_LED_INVERSE           false                 // true if LED is common anode, false if common cathode
  #define BOARD_LED_BRIGHTNESS        32                    // 0..255 brightness control

#else
  #error "No board selected"
#endif


/*
 * Advanced options
 */

#define BUTTON_HOLD_TIME_INDICATION   3000
#define BUTTON_HOLD_TIME_ACTION       10000

#define BOARD_PWM_MAX                 1023

#define WIFI_NET_CONNECT_TIMEOUT      30000
#define WIFI_AP_CONFIG_PORT           80
#define WIFI_AP_IP                    IPAddress(192, 168, 4, 1)
#define WIFI_AP_Subnet                IPAddress(255, 255, 255, 0)


class CEsperId {
private:
  String id;
public:
  void init() {
    id = WiFi.macAddress();
    id.replace(":", "");
  }

  String get() {return id;}
  const char* c_str() {return id.c_str();}
};

CEsperId EsperId;
