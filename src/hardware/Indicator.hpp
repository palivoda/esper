
void indicator_run();

class Indicator {
public:

  Indicator() {
    m_Counter = 0;
    initLED();
  }

  uint32_t run() {
    if (g_buttonPressed) {
      if (millis() - g_buttonPressTime > BUTTON_HOLD_TIME_ACTION)     { return beatLED((int[]){ 100, 100 }); }
      if (millis() - g_buttonPressTime > BUTTON_HOLD_TIME_INDICATION) { return waveLED(1000); }
    }
    switch (EsperState::get()) {
    case MODE_RESET_CONFIG:
    case MODE_WAIT_CONFIG:       return beatLED((int[]){ 50, 500 });
    case MODE_CONFIGURING:       return beatLED((int[]){ 200, 200 });
    case MODE_CONNECTING_NET:    return beatLED((int[]){ 50, 500 });
    case MODE_CONNECTING_MQTT:  return beatLED((int[]){ 100, 100 });
    case MODE_RUNNING:           return waveLED(5000);
    case MODE_OTA_UPGRADE:       return beatLED((int[]){ 50, 50 });
    default:                     return beatLED((int[]){ 80, 100, 80, 1000 } );
    }
  }

  void updateState() {
    m_Counter = 0;
    indicator_run();
  }

protected:

  void initLED() {
    pinMode(BOARD_LED_PIN, OUTPUT);
  }

  void setLED(uint32_t brightness) {
    #if BOARD_LED_INVERSE
      analogWrite(BOARD_LED_PIN, BOARD_PWM_MAX - brightness);
    #else
      analogWrite(BOARD_LED_PIN, brightness);
    #endif
  }

  uint32_t skipLED() {
    return 20;
  }

  template<typename T>
  uint32_t beatLED(const T& beat) {
    const uint8_t cnt = sizeof(beat)/sizeof(beat[0]);
    setLED((m_Counter % 2 == 0) ? BOARD_PWM_MAX : 0);
    uint32_t next = beat[m_Counter];
    m_Counter = (m_Counter+1) % cnt;
    return next;
  }

  uint32_t waveLED(unsigned breathePeriod) {
    uint8_t brightness = (m_Counter < 128) ? m_Counter : 255 - m_Counter;

    setLED(BOARD_PWM_MAX * ((float)brightness / (BOARD_PWM_MAX/2)));

    // This function relies on the 8-bit, unsigned m_Counter rolling over.
    m_Counter = (m_Counter+1) % 256;
    return breathePeriod / 256;
  }

private:
  uint8_t m_Counter;
};

Indicator indicator;

/*
 * Animation timers
 */

#include <Ticker.h>

Ticker indicatorTimer;

void indicator_run() {
  uint32_t returnTime = indicator.run();
  if (returnTime) {
    indicatorTimer.attach_ms(returnTime, indicator_run);
  }
}

void indicator_init() {
  indicatorTimer.attach_ms(100, indicator_run);
}
