/*

CONSTANTS DEFINITION

Copyright (C) 2018 FastyBird Ltd. <info@fastybird.com>

*/

#pragma once

// =============================================================================
// RESET
// =============================================================================

#define CUSTOM_RESET_WEB                        1       // Reset from web interface
#define CUSTOM_RESET_BROKER                     2       // Reset via broker
#define CUSTOM_RESET_BUTTON                     3       // Reset via hardware button

#define CUSTOM_FACTORY_WEB                      4       // Reset after factory reset from web interface
#define CUSTOM_FACTORY_BROKER                   5       // Reset after factory reset via broker
#define CUSTOM_FACTORY_BUTTON                   6       // Reset after factory reset via hardware button

#define CUSTOM_UPGRADE_WEB                      7       // Reset after successful update from web interface

#define CUSTOM_RESTORE_WEB                      8      // Reset after successful setting restore from web interface

#define CUSTOM_RESET_MAX                        8

// =============================================================================
// HEARTBEAT
// =============================================================================

#define HEARTBEAT_NONE                          0       // Never send heartbeat
#define HEARTBEAT_ONCE                          1       // Send it only once upon MQTT connection
#define HEARTBEAT_REPEAT                        2       // Send it upon MQTT connection and every HEARTBEAT_INTERVAL

// =============================================================================
// WIFI MODULE
// =============================================================================

#define WIFI_STATE_AP                           1
#define WIFI_STATE_STA                          2
#define WIFI_STATE_AP_STA                       3

// =============================================================================
// LED MODULE
// =============================================================================

#define LED_MODE_WIFI                           1       // LED will blink according to the WIFI status
#define LED_MODE_ON                             2       // LED always ON
#define LED_MODE_OFF                            3       // LED always OFF

// =============================================================================
// BUTTONS MODULE
// =============================================================================

#define BUTTON_EVENT_NONE                       0
#define BUTTON_EVENT_PRESSED                    1
#define BUTTON_EVENT_RELEASED                   2
#define BUTTON_EVENT_CLICK                      2
#define BUTTON_EVENT_DBL_CLICK                  3
#define BUTTON_EVENT_TRIPLE_CLICK               4
#define BUTTON_EVENT_LNG_CLICK                  5
#define BUTTON_EVENT_LNG_LNG_CLICK              6

// -----------------------------------------------------------------------------
// Needed for ESP8285 boards under Windows using PlatformIO (?)
// -----------------------------------------------------------------------------

#ifndef BUTTON_PUSHBUTTON
    #define BUTTON_PUSHBUTTON                   0
    #define BUTTON_SWITCH                       1
    #define BUTTON_DEFAULT_HIGH                 2
    #define BUTTON_SET_PULLUP                   4
#endif

// =============================================================================
// RELAY MODULE
// =============================================================================

#define RELAY_BOOT_OFF                          0
#define RELAY_BOOT_ON                           1
#define RELAY_BOOT_SAME                         2
#define RELAY_BOOT_TOGGLE                       3

#define RELAY_TYPE_NORMAL                       0
#define RELAY_TYPE_INVERSE                      1
#define RELAY_TYPE_LATCHED                      2
#define RELAY_TYPE_LATCHED_INVERSE              3

#define RELAY_SYNC_ANY                          0
#define RELAY_SYNC_NONE_OR_ONE                  1
#define RELAY_SYNC_ONE                          2
#define RELAY_SYNC_SAME                         3

#define RELAY_PULSE_NONE                        0
#define RELAY_PULSE_OFF                         1
#define RELAY_PULSE_ON                          2

#define RELAY_PROVIDER_NONE                     0
#define RELAY_PROVIDER_RELAY                    1
#define RELAY_PROVIDER_DUAL                     2
#define RELAY_PROVIDER_LIGHT                    3
#define RELAY_PROVIDER_RFBRIDGE                 4
#define RELAY_PROVIDER_STM                      5

// =============================================================================
// LIGHT MODULE
// =============================================================================

// Available light providers
#define LIGHT_PROVIDER_NONE                     0
#define LIGHT_PROVIDER_MY92XX                   1       // works with MY9291 and MY9231
#define LIGHT_PROVIDER_DIMMER                   2
