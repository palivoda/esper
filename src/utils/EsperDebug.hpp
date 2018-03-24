#ifndef __ESPER_DEBUG__
#define __ESPER_DEBUG__

#ifdef ESPER_DEBUG
    #include <stdio.h>
    #include <gdbstub.h>

    #ifndef ESPER_PRINT
      #ifdef DEBUG_ESP_PORT
        #define ESPER_PRINT DEBUG_ESP_PORT
      #else
        #define ESPER_PRINT Serial
      #endif
    #endif

    void logtime() {
        ESPER_PRINT.print('[');
        ESPER_PRINT.print(millis());
        ESPER_PRINT.print("] ");
    }

    //TODO: ESPER_PRINT.printf(String(F(format)).c_str(), ##__VA_ARGS__);
    #define E_DEBUG(format, ...) logtime(); ESPER_PRINT.printf(format, ##__VA_ARGS__); ESPER_PRINT.println();
    #define E_DEBUG_BREAK()    { E_DEBUG("Hit breakpoint..."); for(;;) delay(50); }
    #define E_DEBUG_ASSERT(expr)   { if(!(expr)) { E_DEBUG("Assertion failed: ", #expr); E_DEBUG_BREAK() } }
    #define ESPER_DEBUG_SETUP(speed) ESPER_PRINT.begin(speed); Serial.setDebugOutput(true);

#else
    #define E_DEBUG(format, ...)
    #define E_DEBUG_BREAK()
    #define E_DEBUG_ASSERT(expr)
    #define ESPER_DEBUG_SETUP(speed)
#endif // ESPER_DEBUG

#endif //__ESPER_DEBUG__
