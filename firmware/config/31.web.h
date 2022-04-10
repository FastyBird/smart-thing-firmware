/*

WEB MODULE CONFIGURATION

Copyright (C) 2018 FastyBird s.r.o. <code@fastybird.com>

*/

#pragma once

#include <ESPAsyncWebServer.h>

// =============================================================================
// MODULE DEFAULTS
// =============================================================================

#ifndef WEB_SUPPORT
    #define WEB_SUPPORT                     0
#endif

#ifndef WEB_EMBEDDED
    #define WEB_EMBEDDED                    1                                   // Build the firmware with the web interface embedded in
#endif

// This is not working at the moment!!
// Requires NETWORK_SSL_ENABLED to 1 and ESP8266 Arduino Core 2.4.0
#ifndef WEB_SSL_ENABLED
    #define WEB_SSL_ENABLED                 0                                   // Use HTTPS web interface
#endif

#ifndef WEB_USERNAME
    #define WEB_USERNAME                    "admin"                             // HTTP username
#endif

#ifndef WEB_PORT
    #define WEB_PORT                        80                                  // HTTP port
#endif

#ifndef WEB_REMOTE_DOMAIN
    #define WEB_REMOTE_DOMAIN               "*"                                 // Enable CORS for all domains
#endif

#ifndef WEB_API_DISCOVER
    #define WEB_API_DISCOVER                "/discover"                         //
#endif

#ifndef WEB_API_SIGN_IN
    #define WEB_API_SIGN_IN                 "/control/sign-in"                  //
#endif

#ifndef WEB_API_REPORT_CRASH
    #define WEB_API_REPORT_CRASH            "/control/report-crash"             // 
#endif

#ifndef WEB_API_INITIALIZE
    #define WEB_API_INITIALIZE              "/control/initialize"               //
#endif

#ifndef WEB_API_REBOOT
    #define WEB_API_REBOOT                  "/control/reboot"                   //
#endif

#ifndef WEB_API_FACTORY_RESET
    #define WEB_API_FACTORY_RESET           "/control/factory-reset"            //
#endif

#ifndef WEB_API_FIRMWARE_UPGRADE
    #define WEB_API_FIRMWARE_UPGRADE        "/control/upgrade-firmware"         //
#endif

#ifndef WEB_API_FIRMWARE_CONFIGURATION
    #define WEB_API_FIRMWARE_CONFIGURATION  "/control/configuration"            //
#endif

#ifndef WEB_API_NETWORK_RECONNECT
    #define WEB_API_NETWORK_RECONNECT       "/control/reconnect"                //
#endif

#ifndef WEB_API_NETWORK_CONFIGURATION
    #define WEB_API_NETWORK_CONFIGURATION   "/control/network-configuration"    //
#endif

#ifndef WEB_API_WS_DATA
    #define WEB_API_WS_DATA                 "/ws/data"                          //
#endif

#ifndef WEB_API_WS_AUTH
    #define WEB_API_WS_AUTH                 "/ws/auth"                          //
#endif

// =============================================================================
// MODULE PROTOTYPES
// =============================================================================

class AsyncClient;
class AsyncWebServer;

// Web server instance
AsyncWebServer * webServer();

typedef std::function<void(AsyncWebServer * server)> web_events_callback_t;
void webEventsRegister(web_events_callback_t callback);

// =============================================================================
// MODULE DEPENDENCIES CHECK & CONFIGURATION
// =============================================================================

#if not WEB_SUPPORT
    #undef DEBUG_WEB_SUPPORT
    #define DEBUG_WEB_SUPPORT           0
#endif
