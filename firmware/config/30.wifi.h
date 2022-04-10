/*

WIFI MODULE CONFIGURATION

Copyright (C) 2018 FastyBird s.r.o. <code@fastybird.com>

*/

#pragma once

#include <JustWifi.h>

// =============================================================================
// MODULE DEFAULTS
// =============================================================================

#ifndef WIFI_SUPPORT
    #define WIFI_SUPPORT                        0
#endif

#ifndef WIFI_PASSWORD
    #define WIFI_PASSWORD                       ADMIN_PASSWORD                      // AP connect password
#endif

#ifndef WIFI_CONNECT_TIMEOUT
    #define WIFI_CONNECT_TIMEOUT                60000                               // Connecting timeout for WIFI in ms
#endif

#ifndef WIFI_RECONNECT_INTERVAL
    #define WIFI_RECONNECT_INTERVAL             180000                              // If could not connect to WIFI, retry after this time in ms
#endif

#ifndef WIFI_MAX_NETWORKS
    #define WIFI_MAX_NETWORKS                   5                                   // Max number of WIFI connection configurations
#endif

#ifndef WIFI_FALLBACK_APMODE
    #define WIFI_FALLBACK_APMODE                1                                   // Fallback to AP mode if no STA connection
#endif

#ifndef WIFI_SLEEP_MODE
    #define WIFI_SLEEP_MODE                     WIFI_NONE_SLEEP                     // WIFI_NONE_SLEEP, WIFI_LIGHT_SLEEP or WIFI_MODEM_SLEEP
#endif

#ifndef WIFI_SCAN_NETWORKS
    #define WIFI_SCAN_NETWORKS                  1                                   // Perform a network scan before connecting
#endif

#ifndef WIFI_PROPAGATION_CONST
    #define WIFI_PROPAGATION_CONST              4                                   // This is typically something between 2.7 to 4.3 (free space is 2)
#endif

#ifndef WIFI_AP_BTN_INDEX
    #define WIFI_AP_BTN_INDEX                   INDEX_NONE
#endif

#ifndef WIFI_AP_BTN_EVENT
    #define WIFI_AP_BTN_EVENT                   BUTTON_EVENT_DBL_CLICK
#endif

// =============================================================================
// MODULE TYPES
// =============================================================================

#define WIFI_STATE_AP                           1
#define WIFI_STATE_STA                          2
#define WIFI_STATE_AP_STA                       3

// =============================================================================
// MODULE PROTOTYPES
// =============================================================================

typedef std::function<void(justwifi_messages_t code, char * parameter)> wifi_callback_t;
void wifiRegister(wifi_callback_t callback);
