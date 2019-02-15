/*

DATA TYPES & CONSTANTS DEFINITION

Copyright (C) 2018 FastyBird Ltd. <info@fastybird.com>

*/

// =============================================================================
// RESET
// =============================================================================

#define CUSTOM_RESET_HARDWARE       1       // Reset from hardware button
#define CUSTOM_RESET_WEB            2       // Reset from web interface
#define CUSTOM_RESET_MQTT           3       // Reset via MQTT
#define CUSTOM_RESET_OTA            4       // Reset after successful OTA update
#define CUSTOM_RESET_NOFUSS         5       // Reset after successful NOFUSS update
#define CUSTOM_RESET_UPGRADE        6       // Reset after update from web interface
#define CUSTOM_RESET_FACTORY        7       // Factory reset from terminal

#define CUSTOM_RESET_MAX            7

// =============================================================================
// WIFI
// =============================================================================

#define WIFI_STATE_AP               1
#define WIFI_STATE_STA              2
#define WIFI_STATE_AP_STA           3

// =============================================================================
// LED
// =============================================================================

#define LED_MODE_MQTT               0       // LED will be managed from MQTT (OFF by default)
#define LED_MODE_WIFI               2       // LED will blink according to the WIFI status
#define LED_MODE_ON                 3       // LED always ON
#define LED_MODE_OFF                4       // LED always OFF

// =============================================================================
// BUTTONS
// =============================================================================

#define BUTTON_EVENT_NONE           0
#define BUTTON_EVENT_PRESSED        1
#define BUTTON_EVENT_RELEASED       2
#define BUTTON_EVENT_CLICK          2
#define BUTTON_EVENT_DBLCLICK       3
#define BUTTON_EVENT_LNGCLICK       4
#define BUTTON_EVENT_LNGLNGCLICK    5
#define BUTTON_EVENT_TRIPLECLICK    6

// Needed for ESP8285 boards under Windows using PlatformIO (?)
#ifndef BUTTON_PUSHBUTTON
#define BUTTON_PUSHBUTTON           0
#define BUTTON_SWITCH               1
#define BUTTON_DEFAULT_HIGH         2
#define BUTTON_SET_PULLUP           4
#endif

// =============================================================================
// NODE PACKET NAMES
// =============================================================================

#define GATEWAY_PACKET_NONE                 0
#define GATEWAY_PACKET_UNKNOWN              1
#define GATEWAY_PACKET_ERROR                2
#define GATEWAY_PACKET_ACCEPTED             3
#define GATEWAY_PACKET_MASTER_LOOKUP        4
#define GATEWAY_PACKET_HEARTBEAT            5
#define GATEWAY_PACKET_PING                 6
#define GATEWAY_PACKET_PONG                 7
#define GATEWAY_PACKET_DATA                 8
#define GATEWAY_PACKET_INIT_START           9
#define GATEWAY_PACKET_INIT_END             10
#define GATEWAY_PACKET_INIT_RESTART         11
#define GATEWAY_PACKET_HARDWARE_INFO        12
#define GATEWAY_PACKET_FIRMWARE_INFO        13
#define GATEWAY_PACKET_SETTINGS_SCHEMA      14
#define GATEWAY_PACKET_CHANNELS_SCHEMA      15
#define GATEWAY_PACKET_WHO_ARE_YOU          16

#define GATEWAY_PACKETS_MAX                 17

#define GATEWAY_NODE_CHANNEL_EVENT          1
#define GATEWAY_NODE_CHANNEL_DIGITAL        2
#define GATEWAY_NODE_CHANNEL_ANALOG         3

#define GATEWAY_NODE_CHANNEL_TYPE_BUTTON    1
#define GATEWAY_NODE_CHANNEL_TYPE_INPUT     2
#define GATEWAY_NODE_CHANNEL_TYPE_OUTPUT    3
#define GATEWAY_NODE_CHANNEL_TYPE_SWITCH    4

#define GATEWAY_NODE_CHANNEL_FORMAT_ANALOG  1       // Input or Output value is FLOAT
#define GATEWAY_NODE_CHANNEL_FORMAT_DIGITAL 2       // Input or Output value is BOOLEAN
#define GATEWAY_NODE_CHANNEL_FORMAT_NUMERIC 3       // Input or Output value is INTEGER
#define GATEWAY_NODE_CHANNEL_FORMAT_EVENT   4       // Input or Output value is CHAR

// =============================================================================
// ENCODER
// =============================================================================

#define ENCODER_MODE_CHANNEL        0
#define ENCODER_MODE_RATIO          1

// =============================================================================
// RELAY
// =============================================================================

#define RELAY_BOOT_OFF              0
#define RELAY_BOOT_ON               1
#define RELAY_BOOT_SAME             2
#define RELAY_BOOT_TOGGLE           3

#define RELAY_TYPE_NORMAL           0
#define RELAY_TYPE_INVERSE          1
#define RELAY_TYPE_LATCHED          2
#define RELAY_TYPE_LATCHED_INVERSE  3

#define RELAY_SYNC_ANY              0
#define RELAY_SYNC_NONE_OR_ONE      1
#define RELAY_SYNC_ONE              2
#define RELAY_SYNC_SAME             3

#define RELAY_PULSE_NONE            0
#define RELAY_PULSE_OFF             1
#define RELAY_PULSE_ON              2

#define RELAY_PROVIDER_NONE         0
#define RELAY_PROVIDER_RELAY        1
#define RELAY_PROVIDER_DUAL         2
#define RELAY_PROVIDER_LIGHT        3
#define RELAY_PROVIDER_RFBRIDGE     4
#define RELAY_PROVIDER_STM          5

// =============================================================================
// LIGHT
// =============================================================================

// Available light providers
#define LIGHT_PROVIDER_NONE         0
#define LIGHT_PROVIDER_MY92XX       1       // works with MY9291 and MY9231
#define LIGHT_PROVIDER_DIMMER       2

// =============================================================================
// SCHEDULER
// =============================================================================

#define SCHEDULER_TYPE_SWITCH       1
#define SCHEDULER_TYPE_DIM          2

// =============================================================================
// IR
// =============================================================================

// IR Button modes
#define IR_BUTTON_MODE_NONE         0
#define IR_BUTTON_MODE_RGB          1
#define IR_BUTTON_MODE_HSV          2
#define IR_BUTTON_MODE_BRIGHTER     3
#define IR_BUTTON_MODE_STATE        4
#define IR_BUTTON_MODE_EFFECT       5
#define IR_BUTTON_MODE_TOGGLE       6

#define LIGHT_EFFECT_SOLID          0
#define LIGHT_EFFECT_FLASH          1
#define LIGHT_EFFECT_STROBE         2
#define LIGHT_EFFECT_FADE           3
#define LIGHT_EFFECT_SMOOTH         4
