/*

FIRMWARE DEBUG CONFIGURATION

Copyright (C) 2018 FastyBird s.r.o. <info@fastybird.com>

*/

#pragma once

// =============================================================================
// Debug
// =============================================================================

#ifndef DEBUG_SUPPORT
#define DEBUG_SUPPORT   DEBUG_SERIAL_SUPPORT || DEBUG_WEB_SUPPORT || DEBUG_MQTT_SUPPORT || DEBUG_NODES_SUPPORT
#endif

#if DEBUG_SUPPORT
    #define DEBUG_MSG(...) debugSend(__VA_ARGS__)

    #if NODES_GATEWAY_SUPPORT && DEBUG_NODES_SUPPORT
        #define DEBUG_GW_MSG(...) debugSend(__VA_ARGS__)
    #else
        #define DEBUG_GW_MSG(...)
    #endif
#endif

#if not DEBUG_SUPPORT
    #define DEBUG_MSG(...)

    #if NODES_GATEWAY_SUPPORT
        #define DEBUG_GW_MSG(...)
    #else
        #define DEBUG_GW_MSG(...)
    #endif
#endif
