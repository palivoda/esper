#include <Ticker.h>
#ifndef ESPER_BEAT_S
  #define ESPER_BEAT_S 1
#endif

Ticker esperBeat;

void beat_callback() {
  if (EsperState::is(MODE_RUNNING)) { tictac(); }
}

void beat_init() {
    esperBeat.attach(ESPER_BEAT_S, beat_callback);
}
