#pragma once

// -----------------------------------------------------------------------------
// Debug
// -----------------------------------------------------------------------------

#ifndef DEBUG_SUPPORT
#define DEBUG_SUPPORT   DEBUG_SERIAL_SUPPORT || DEBUG_WEB_SUPPORT || DEBUG_MQTT_SUPPORT
#endif

#if DEBUG_SUPPORT
    #define DEBUG_MSG(...) debugSend(__VA_ARGS__)
#endif

#if not DEBUG_SUPPORT
    #define DEBUG_MSG(...)
#endif
