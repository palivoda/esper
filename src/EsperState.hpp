enum State {
  MODE_WAIT_CONFIG,
  MODE_CONFIGURING,
  MODE_CONNECTING_NET,
  MODE_CONNECTING_MQTT,
  MODE_RUNNING,
  MODE_OTA_UPGRADE,
  MODE_SWITCH_TO_STA,
  MODE_RESET_CONFIG,
  MODE_ERROR,

  MODE_MAX_VALUE
};

#if defined(ESPER_DEBUG)
const char* ESPER_STATE[MODE_MAX_VALUE] = {
  "WAIT_CONFIG",
  "CONFIGURING",
  "CONNECTING_NET",
  "CONNECTING_MQTT",
  "RUNNING",
  "OTA_UPGRADE",
  "SWITCH_TO_STA",
  "RESET_CONFIG",
  "ERROR"
};
#endif

namespace EsperState
{
  volatile State state;

  State get()        { return state; }
  bool  is (State m) { return (state == m); }
  void  set(State m);
};
