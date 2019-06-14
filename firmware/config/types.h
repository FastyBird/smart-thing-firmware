/*

DATA TYPES & CONSTANTS DEFINITION

Copyright (C) 2018 FastyBird Ltd. <info@fastybird.com>

*/

// =============================================================================
// RESET
// =============================================================================

#define CUSTOM_RESET_HARDWARE                   1       // Reset from hardware button
#define CUSTOM_RESET_WEB                        2       // Reset from web interface
#define CUSTOM_RESET_BROKER                     3       // Reset via broker
#define CUSTOM_RESET_OTA                        4       // Reset after successful OTA update
#define CUSTOM_RESET_NOFUSS                     5       // Reset after successful NOFUSS update
#define CUSTOM_RESET_UPGRADE                    6       // Reset after update from web interface
#define CUSTOM_RESET_FACTORY                    7       // Factory reset from terminal

#define CUSTOM_RESET_MAX                        7

// =============================================================================
// WIFI
// =============================================================================

#define WIFI_STATE_AP                           1
#define WIFI_STATE_STA                          2
#define WIFI_STATE_AP_STA                       3

// =============================================================================
// LED
// =============================================================================

#define LED_MODE_MQTT                           0       // LED will be managed from MQTT (OFF by default)
#define LED_MODE_WIFI                           1       // LED will blink according to the WIFI status
#define LED_MODE_ON                             2       // LED always ON
#define LED_MODE_OFF                            3       // LED always OFF

// =============================================================================
// BUTTONS
// =============================================================================

#define BUTTON_EVENT_NONE                       0
#define BUTTON_EVENT_PRESSED                    1
#define BUTTON_EVENT_RELEASED                   2
#define BUTTON_EVENT_CLICK                      2
#define BUTTON_EVENT_DBLCLICK                   3
#define BUTTON_EVENT_LNGCLICK                   4
#define BUTTON_EVENT_LNGLNGCLICK                5
#define BUTTON_EVENT_TRIPLECLICK                6

// Needed for ESP8285 boards under Windows using PlatformIO (?)
#ifndef BUTTON_PUSHBUTTON
#define BUTTON_PUSHBUTTON                       0
#define BUTTON_SWITCH                           1
#define BUTTON_DEFAULT_HIGH                     2
#define BUTTON_SET_PULLUP                       4
#endif

// =============================================================================
// GATEWAY PACKET NAMES
// =============================================================================

// Node searching
#define GATEWAY_PACKET_SEARCH_NEW_NODES         0x01
#define GATEWAY_PACKET_NODE_ADDRESS_CONFIRM     0x02

#define GATEWAY_PACKET_SEARCH_MAX               2

// Node addressing
#define GATEWAY_PACKET_SEARCH_NODES             0x03
#define GATEWAY_PACKET_ADDRESS_DISCARD          0x04

#define GATEWAY_PACKET_ADDRESS_MAX              2

// Node initialization
#define GATEWAY_PACKET_HW_MODEL                 0x11
#define GATEWAY_PACKET_HW_MANUFACTURER          0x12
#define GATEWAY_PACKET_HW_VERSION               0x13
#define GATEWAY_PACKET_FW_MODEL                 0x14
#define GATEWAY_PACKET_FW_MANUFACTURER          0x15
#define GATEWAY_PACKET_FW_VERSION               0x16

#define GATEWAY_PACKET_NODE_INIT_MAX            6

// Node registers initialization
#define GATEWAY_PACKET_REGISTERS_SIZE           0x21
#define GATEWAY_PACKET_AI_REGISTERS_STRUCTURE   0x22
#define GATEWAY_PACKET_AO_REGISTERS_STRUCTURE   0x23

#define GATEWAY_PACKET_REGISTERS_INIT_MAX       3

// Registers reading
#define GATEWAY_PACKET_READ_SINGLE_DI           0x31   // Master requested DI one regiter reading
#define GATEWAY_PACKET_READ_MULTI_DI            0x32   // Master requested DI multiple regiters reading
#define GATEWAY_PACKET_READ_SINGLE_DO           0x33   // Master requested DO one regiter reading
#define GATEWAY_PACKET_READ_MULTI_DO            0x34   // Master requested DO multiple regiters reading
#define GATEWAY_PACKET_READ_SINGLE_AI           0x35   // Master requested AI one regiter reading
#define GATEWAY_PACKET_READ_MULTI_AI            0x36   // Master requested AI multiple regiters reading
#define GATEWAY_PACKET_READ_SINGLE_AO           0x37   // Master requested AO one regiter reading
#define GATEWAY_PACKET_READ_MULTI_AO            0x38   // Master requested AO multiple regiters reading
#define GATEWAY_PACKET_READ_SINGLE_EV           0x39   // Master requested EV one regiter reading
#define GATEWAY_PACKET_READ_MULTI_EV            0x3A   // Master requested EV multiple regiters reading

#define GATEWAY_PACKET_REGISTERS_REDING_MAX     10

// Registers writing
#define GATEWAY_PACKET_WRITE_ONE_DO             0x41
#define GATEWAY_PACKET_WRITE_ONE_AO             0x42
#define GATEWAY_PACKET_WRITE_MULTI_DO           0x43
#define GATEWAY_PACKET_WRITE_MULTI_AO           0x44

#define GATEWAY_PACKET_REGISTERS_WRITING_MAX    4

// Node misc communication
#define GATEWAY_PACKET_NONE                     0xFF
#define GATEWAY_PACKET_GATEWAY_PING             0x51   // Master is testing node if is alive

#define GATEWAY_PACKET_MISC_MAX                 2

// -----------------------------------------------------------------------------

#define GATEWAY_DESCRIPTION_NOT_SET             "N/A"

// =============================================================================
// GATEWAY REGISTERS VALUES DATATYPES
// =============================================================================

#define GATEWAY_DATA_TYPE_UNKNOWN               0xFF
#define GATEWAY_DATA_TYPE_UINT8                 0x01
#define GATEWAY_DATA_TYPE_UINT16                0x02
#define GATEWAY_DATA_TYPE_UINT32                0x03
#define GATEWAY_DATA_TYPE_INT8                  0x04
#define GATEWAY_DATA_TYPE_INT16                 0x05
#define GATEWAY_DATA_TYPE_INT32                 0x06
#define GATEWAY_DATA_TYPE_FLOAT32               0x07

// =============================================================================
// GATEWAY REGISTERS TYPES
// =============================================================================

#define GATEWAY_REGISTER_DI                     0
#define GATEWAY_REGISTER_DO                     1
#define GATEWAY_REGISTER_AI                     2
#define GATEWAY_REGISTER_AO                     3
#define GATEWAY_REGISTER_EV                     4

// =============================================================================
// ENCODER
// =============================================================================

#define ENCODER_MODE_CHANNEL                    0
#define ENCODER_MODE_RATIO                      1

// =============================================================================
// RELAY
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
// LIGHT
// =============================================================================

// Available light providers
#define LIGHT_PROVIDER_NONE                     0
#define LIGHT_PROVIDER_MY92XX                   1       // works with MY9291 and MY9231
#define LIGHT_PROVIDER_DIMMER                   2

// =============================================================================
// SCHEDULER
// =============================================================================

#define SCHEDULER_TYPE_SWITCH                   1
#define SCHEDULER_TYPE_DIM                      2

// =============================================================================
// IR
// =============================================================================

// IR Button modes
#define IR_BUTTON_MODE_NONE                     0
#define IR_BUTTON_MODE_RGB                      1
#define IR_BUTTON_MODE_HSV                      2
#define IR_BUTTON_MODE_BRIGHTER                 3
#define IR_BUTTON_MODE_STATE                    4
#define IR_BUTTON_MODE_EFFECT                   5
#define IR_BUTTON_MODE_TOGGLE                   6

#define LIGHT_EFFECT_SOLID                      0
#define LIGHT_EFFECT_FLASH                      1
#define LIGHT_EFFECT_STROBE                     2
#define LIGHT_EFFECT_FADE                       3
#define LIGHT_EFFECT_SMOOTH                     4
