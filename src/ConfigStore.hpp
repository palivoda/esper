struct ErrorsStore {
  char      message[32];
} __attribute__((packed));

ErrorsStore EsperErrors;

struct ConfigStore {
  uint32_t  magic;
  char      version[9];
  uint8_t   flagConfig:1;
  uint8_t   flagApFail:1;
  uint8_t   flagSelfTest:1;

  char      wifiSSID[16];
  char      wifiPass[16];

  char      mqttHost[32];
  uint16_t  mqttPort;
  char      mqttUser[16];
  char      mqttPass[16];

  bool      mqttRetain;
  char      mqttFeedTopic[32];
  char      mqttOnlineTopic[32];
  char      mqttOfflineTopic[32];

  uint16_t  checksum;
} __attribute__((packed));

ConfigStore EsperConfig;

const ConfigStore configDefault = {
  0x20CECF77,
  BOARD_FIRMWARE_VERSION,
  0, 0, 0,

  "wifi",
  "pass",

  "192.168.3.102", 1883,
  "messanger",
  "comes",

  false,
  "esper/data",
  "esper/online",
  "esper/offline",

  0
};

#include <EEPROM.h>
#define EEPROM_CONFIG_START 0

void config_load()
{
  memset(&EsperErrors, 0, sizeof(EsperErrors));
  EEPROM.get(EEPROM_CONFIG_START, EsperErrors);
  E_DEBUG("Flashed error: %s", EsperErrors.message);

  memset(&EsperConfig, 0, sizeof(EsperConfig));
  EEPROM.get(EEPROM_CONFIG_START+sizeof(EsperErrors), EsperConfig);
  if (EsperConfig.magic != configDefault.magic) {
    E_DEBUG("Using default config.");
    EsperConfig = configDefault;
  }
}

bool config_save()
{
  memset(&EsperErrors, 0, sizeof(EsperErrors));
  EEPROM.put(EEPROM_CONFIG_START, EsperErrors);

  EEPROM.put(EEPROM_CONFIG_START+sizeof(EsperErrors), EsperConfig);
  EEPROM.commit();
  return true;
}

void esperResetConfig()
{
  E_DEBUG("Resetting configuration!");
  EsperConfig = configDefault;
  config_save();
  EsperState::set(MODE_WAIT_CONFIG);
}

bool config_init()
{
  EEPROM.begin(sizeof(ErrorsStore)+sizeof(ConfigStore));
  esperResetConfig(); //always reset config on start
  config_load();
  return true;
}

void flashError(const char* msg) {
  E_DEBUG("Flashing error: %s", msg);
  strncpy(EsperErrors.message, msg, sizeof(EsperErrors.message));
  EEPROM.put(EEPROM_CONFIG_START, EsperErrors);
  EEPROM.commit();
}

template<typename T, int size>
void CopyString(const String& s, T(&arr)[size]) {
  s.toCharArray(arr, size);
}
