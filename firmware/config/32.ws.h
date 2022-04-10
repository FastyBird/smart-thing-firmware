/*

WS MODULE CONFIGURATION

Copyright (C) 2018 FastyBird s.r.o. <code@fastybird.com>

*/

#pragma once

// =============================================================================
// MODULE DEFAULTS
// =============================================================================

#ifndef WS_SUPPORT
    #define WS_SUPPORT                      0
#endif

#ifndef WS_BUFFER_SIZE
    #define WS_BUFFER_SIZE                  5                                   // Max number of secured websocket connections
#endif

#ifndef WS_TIMEOUT
    #define WS_TIMEOUT                      300000                              // Timeout for secured websocket
#endif

#ifndef WS_UPDATE_INTERVAL
    #define WS_UPDATE_INTERVAL              30000                               // Update clients every x seconds
#endif

// =============================================================================
// MODULE PROTOTYPES
// =============================================================================

typedef std::function<void(JsonObject&)> ws_on_connect_callback_t;
void wsOnConnectRegister(ws_on_connect_callback_t callback);

typedef std::function<void(JsonObject&)> ws_on_update_callback_t;
void wsOnUpdateRegister(ws_on_update_callback_t callback);

typedef std::function<void(uint32_t, const char *, JsonObject&)> ws_on_action_callback_t;
void wsOnActionRegister(ws_on_action_callback_t callback);

typedef std::function<void(uint32_t, JsonObject&)> ws_on_configure_callback_t;
void wsOnConfigureRegister(ws_on_configure_callback_t callback);

// =============================================================================
// MODULE DEPENDENCIES CHECK & CONFIGURATION
// =============================================================================

#if not WEB_SUPPORT
    #undef WS_SUPPORT
    #define WS_SUPPORT                      0                                   // WS support requires web support
#endif
