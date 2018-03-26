# What is ESPER?
ESPER library takes care about your ESP8266 module setup and configuration. You focus on reading sensors, triggering relays and offcourse logic and UI setup in your Node-RED instance. 

Here is full list of features done so far:
* EEPROM configuration storage
* Setup WifI connection on deployment (SSID, password)
* Setup MQTT connection on deployment (host, port, user, password, retain)
* Configure MQTT topics for data esper/feed, as well for esper/init and esper/will
* Update firmware from URL (triggered by MQTT esper/ota message)
* Read device config and status via MQTT message (esper/echo message)
* Build in Timer trigger sensor readings when the board is online and ready (tictac)
* Debug macros with printf like formatting

All this you get by adding just a few lines in to your code template:

* Select the board and include h file of the library:
```sh
#define USE_WEMOS_D1_MINI_BOARD     //select board
#define ESPER_DEBUG                 //enable Debug
#include "Esper.hpp"                //main H file
```
* Then during setup call:
```sh
ESPER_DEBUG_SETUP(74880);           //default ESP baud rate
Esper.begin();                      //init EEPROM
```
* During loop you need to call only
```sh
Esper.run();
```
ESPER provides you 3 new callbacks that you can fill in with operation code:

| Function | Description |
| :---: | --- |
| connected() | Triggered by ESPER when device setup is completed - it's connected to WiFi and MQTT and connection parametrs saved to EEPROM. MQTT init message sent. |
| disconnect() | Triggered if WiFi, MQTT connection lost or ESPER OTA is initiated and device is going to reboot. |
| tictac() | Timer that is actived only when device is online and ready. Intended to sensor readigns. |

# Development environment 
* Git clone in to directory and open with [Platformio for Atom]. 
* Check [platformio.ini] file for setting up your board. 
* Connect ESP to USB and Run build. For ESP-01 it takes 10 seconds to start AP. 
* Connect to 'ESPER device' AP and open URL "esper.iot" to setup WiFi and MQTT settings. 
  * After save device will reboot and send esper/init MQTT mesage. 

# Setting up MQTT environment
As ESPER communicates via MQTT here is propsed software stack that you can get to kicstart your first DIY IoT project, like smart home:
* Orange PI Zero board with 8+ Gig SD card
* 8+ GB SD Card with (TODO: ready to burn SD card image):
  * Install [Armbian]
    * Dont forget to [setup WiFi] ;)
  * Install [Mosquitto] MQTT broker (port 1880)
  * Install [Node-RED] (http://192.168.x.x:1880/)
    * Install [Node-RED Dashboard] extension and access it on http://192.168.x.x:1880/ui/
    * Import ESPER Node-RED test flow [test-mqtt.json] if ok then test connection to ESP with [esper-core.json]
  * (optional) To provide OTA images you may want to install [NginX] web server
    * then you will may need SSH clinet with SFTP to upload new firmware files into /var/www/html
Steps:
* Burn the SD card image  and Boot OrangePi Zero
* Burn your ESP8266 firmware with DHT11 example, reboot, connect to "ESPER device" nework and configure it
* Now you should be able to see humidity and temperature charts.

# Supported Boards
By this time EPSER was delpoyed and working on
* [ESP-01S]
* [Wemos D1 Mini] and similar ESP8266MOD based PCBs

# Roadmap
* Kickstart application template
* MQTT Esper command
  * Define command implementation interface (abstract class with predefined call context)
  * Define command MQTT mesage format (format of string)
  * Create kick-start command implementation template
* Implement basic MQTT commands to manipulate with hardware pins (read, write, PWM)
  * Command to read pin with timer
  * Command to write pin, PWM on pin
  * Command to set Pin interrupt notification
* MQTT Esper process
  * Define command interface 
* Implement more internal process type of commands
  * Process to start trigger level logic
  * Process to start PID regilator  
* EEPROM encription

License
----

MIT

[//]: #

[ESP-01S]: [https://www.aliexpress.com/item/WIFI-module-ESP-01-ESP8266-8Mb-flash-memory/32733744011.html?spm=a2g0s.9042311.0.0.da9DRM&af=1696703&cv=25765197&cn=41p5y7w4do70a0xrzwt58ht5vxh30a4p&dp=v5_41p5y7w4do70a0xrzwt58ht5vxh30a4p&mall_affr=pr1&aff_platform=default&cpt=1521649012473&sk=jqf2Rf6&aff_trace_key=bed5cdc34a724c9d858342cacac3e055-1521649012473-09252-jqf2Rf6&terminal_id=897c247d304f494e95802c610d1d8751]

[Wemos D1 Mini]: https://www.aliexpress.com/item/D1-mini-Mini-NodeMcu-4M-bytes-Lua-WIFI-Internet-of-Things-development-board-based-ESP8266-by/32662942091.html?spm=a2g0s.9042311.0.0.bHIaLG&af=1696703&cv=25765197&cn=41p5y89ttwv6le8xkuvp5kn7tmz7c7gs&dp=v5_41p5y89ttwv6le8xkuvp5kn7tmz7c7gs&mall_affr=pr1&aff_platform=default&cpt=1521649505722&sk=jqf2Rf6&aff_trace_key=a9ea496c48aa41ac975edb51d96e5a8a-1521649505722-02062-jqf2Rf6&terminal_id=897c247d304f494e95802c610d1d8751

[Armbian]: https://docs.armbian.com/User-Guide_Getting-Started/

[setup WiFi]: https://cloudvedas.com/how-to-use-nmtui-and-nmcli-tool-to-configure-network-in-redhat-linux/

[Mosquitto]: https://www.digitalocean.com/community/tutorials/how-to-install-and-secure-the-mosquitto-mqtt-messaging-broker-on-debian-8

[Node-RED]: https://diyprojects.io/install-node-red-orange-pi-running-armbian/#.Wrfo-2puaUm

[Node-RED Dashboard]: https://flows.nodered.org/node/node-red-dashboard

[test-mqtt.json]: /examples/node-RED/test-mqtt.json

[esper-core.json]: /examples/node-RED/esper-core.json

[NginX]: https://www.digitalocean.com/community/tutorials/how-to-install-linux-nginx-mysql-php-lemp-stack-on-debian-8

[Platformio for Atom]: http://docs.platformio.org/en/latest/ide/atom.html#installation
