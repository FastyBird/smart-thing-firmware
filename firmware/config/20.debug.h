/*

DEBUG MODULE CONFIGURATION

Copyright (C) 2018 FastyBird s.r.o. <code@fastybird.com>

*/

#pragma once

// =============================================================================
// MODULE DEFAULTS
// =============================================================================

#ifndef DEBUG_SUPPORT
    #define DEBUG_SUPPORT                   0
#endif

#ifndef DEBUG_SERIAL_SUPPORT
    #define DEBUG_SERIAL_SUPPORT            0                                   // Enable serial debug log
#endif

#ifndef DEBUG_PORT
    #define DEBUG_PORT                      Serial                              // Default debugging port
#endif

#ifndef DEBUG_WEB_SUPPORT
    #define DEBUG_WEB_SUPPORT               0                                   // Enable web debug log (will only work if WEB_SUPPORT is also 1)
#endif

#ifndef DEBUG_MQTT_SUPPORT
    #define DEBUG_MQTT_SUPPORT              0                                   // Enable mqtt debug log
#endif

#ifndef SERIAL_BAUDRATE
    #define SERIAL_BAUDRATE                 115200                              // Default baudrate
#endif

#ifndef DEBUG_ADD_TIMESTAMP
    #define DEBUG_ADD_TIMESTAMP             1                                   // Add timestamp to debug messages
                                                                                // (in millis overflowing every 1000 seconds)
#endif

// Second serial port (used for RX)

#ifndef SERIAL_RX_ENABLED
    #define SERIAL_RX_ENABLED               0                                   // Secondary serial port for RX
#endif

#ifndef SERIAL_RX_PORT
    #define SERIAL_RX_PORT                  Serial                              // This setting is usually defined
                                                                                // in the hardware.h file for those
                                                                                // boards that require it
#endif

#ifndef SERIAL_RX_BAUDRATE
    #define SERIAL_RX_BAUDRATE              115200                              // Default baudrate
#endif

// =============================================================================
// MODULE DEPENDENCIES CHECK & CONFIGURATION
// =============================================================================

#ifndef DEBUG_SUPPORT
    #define DEBUG_SUPPORT   DEBUG_SERIAL_SUPPORT || DEBUG_WEB_SUPPORT || DEBUG_MQTT_SUPPORT
#endif

#if DEBUG_SUPPORT
    #define DEBUG_MSG(...) debugSend(__VA_ARGS__)
#endif

#if not DEBUG_SUPPORT
    #define DEBUG_MSG(...)
#endif
