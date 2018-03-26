# What is ESPER?
ESPER library takes care about your ESP microcontroller setup and configuration. You focus on reading sensors, triggering relays and off course logic and UI setup in your Node-RED instance.

Here is full list of features done so far:
* Arduino for ESP8266 based (all your sketches are portable)
* EEPROM configuration storage
* Setup Wi-Fi connection on deployment (SSID, password)
* Setup MQTT connection on deployment (host, port, user, password, retain)
* Configure MQTT topics for data esper/feed, as well for esper/init and esper/will
* Update firmware from URL (triggered by MQTT esper/ota message)
* Read device configuration and status via MQTT message (esper/echo message)
* Build in Timer trigger sensor readings when the board is online and ready (tictac)
* Debug macros with printf like formatting

All this you get by starting your project with [Esper Application]!

## Core Esper development:
* Git clone in to directory.
* Open directory as project in Paltformio
* In [platformio.ini] file set up your board and comment out ESPER_BULD_LIB parameter to include main.cpp content in to the build.
* You should be able to build Esper core and upload it in to the ESP chip.

## Extending Esper with new commands:
To be announced when functionality will be implemented.

# Supported Boards
By this time EPSER was deployed and working on
* [ESP-01S]
* [Wemos D1 Mini] and similar ESP8266MOD based PCBs

# Roadmap

* Kick start application template
* MQTT Esper command
  * Define command implementation interface (abstract class with predefined call context)
  * Define command MQTT message format (format of string)
  * Create kick-start command implementation template
* Implement basic MQTT commands to manipulate with hardware pins (read, write, PWM)
  * Command to read pin with timer
  * Command to write pin, PWM on pin
  * Command to set Pin interrupt notification
* Implement input and output value mappings
  * Basic points approximation mapper
  * Math function mapping
* MQTT Esper process
  * Define command interface
* Implement more internal process type of commands
  * Process to start trigger level logic
  * Process to start PID regulator  
* EEPROM encryption

License
----

MIT

[//]: #

[Esper Application]: https://github.com/palivoda/esper-app

[ESP-01S]: [https://www.aliexpress.com/item/WIFI-module-ESP-01-ESP8266-8Mb-flash-memory/32733744011.html?spm=a2g0s.9042311.0.0.da9DRM&af=1696703&cv=25765197&cn=41p5y7w4do70a0xrzwt58ht5vxh30a4p&dp=v5_41p5y7w4do70a0xrzwt58ht5vxh30a4p&mall_affr=pr1&aff_platform=default&cpt=1521649012473&sk=jqf2Rf6&aff_trace_key=bed5cdc34a724c9d858342cacac3e055-1521649012473-09252-jqf2Rf6&terminal_id=897c247d304f494e95802c610d1d8751]

[Wemos D1 Mini]: https://www.aliexpress.com/item/D1-mini-Mini-NodeMcu-4M-bytes-Lua-WIFI-Internet-of-Things-development-board-based-ESP8266-by/32662942091.html?spm=a2g0s.9042311.0.0.bHIaLG&af=1696703&cv=25765197&cn=41p5y89ttwv6le8xkuvp5kn7tmz7c7gs&dp=v5_41p5y89ttwv6le8xkuvp5kn7tmz7c7gs&mall_affr=pr1&aff_platform=default&cpt=1521649505722&sk=jqf2Rf6&aff_trace_key=a9ea496c48aa41ac975edb51d96e5a8a-1521649505722-02062-jqf2Rf6&terminal_id=897c247d304f494e95802c610d1d8751
