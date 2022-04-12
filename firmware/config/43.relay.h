/*

RELAY MODULE CONFIGURATION

Copyright (C) 2018 FastyBird s.r.o. <code@fastybird.com>

*/

#pragma once

#include <bitset>

// =============================================================================
// MODULE DEFAULTS
// =============================================================================

#ifndef RELAY_PROVIDER
    #define RELAY_PROVIDER                      RELAY_PROVIDER_NONE
#endif

#ifndef DUMMY_RELAY_COUNT
    #define DUMMY_RELAY_COUNT                   0
#endif

// Default boot mode: 0 means OFF, 1 ON and 2 whatever was before
#ifndef RELAY_BOOT_MODE
    #define RELAY_BOOT_MODE                     RELAY_BOOT_OFF
#endif

// 0 means ANY, 1 zero or one and 2 one and only one
#ifndef RELAY_SYNC
    #define RELAY_SYNC                          RELAY_SYNC_ANY
#endif

// Default pulse mode: 0 means no pulses, 1 means normally off, 2 normally on
#ifndef RELAY_PULSE_MODE
    #define RELAY_PULSE_MODE                    RELAY_PULSE_NONE
#endif

// Default pulse time in seconds
#ifndef RELAY_PULSE_TIME
    #define RELAY_PULSE_TIME                    1.0
#endif

// Relay requests flood protection window - in seconds
#ifndef RELAY_FLOOD_WINDOW
    #define RELAY_FLOOD_WINDOW                  3
#endif

// Allowed actual relay changes inside requests flood protection window
#ifndef RELAY_FLOOD_CHANGES
    #define RELAY_FLOOD_CHANGES                 5
#endif

// Pulse with in milliseconds for a latched relay
#ifndef RELAY_LATCHING_PULSE
    #define RELAY_LATCHING_PULSE                10
#endif

// Do not save relay state after these many milliseconds
#ifndef RELAY_SAVE_DELAY
    #define RELAY_SAVE_DELAY                    1000
#endif

// TODO Only single EEPROM address is used to store state, which is 1 byte
// Relay status is stored using bitfield.
// This means that, atm, we are only storing the status of the first 8 relays.
#define RELAY_SAVE_MASK_MAX                     8

// =============================================================================
// MODULE TYPES
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
#define RELAY_PROVIDER_RFBRIDGE                 3
#define RELAY_PROVIDER_STM                      4

// =============================================================================
// MODULE PROTOTYPES
// =============================================================================

typedef std::function<void(unsigned int)> relay_on_event_callback_t;
void relayOnEventRegister(relay_on_event_callback_t callback, uint8_t relay);

typedef struct {
    // Configuration variables
    uint8_t pin;            // GPIO pin for the relay
    uint8_t type;           // RELAY_TYPE_NORMAL, RELAY_TYPE_INVERSE, RELAY_TYPE_LATCHED or RELAY_TYPE_LATCHED_INVERSE
    uint8_t reset_pin;      // GPIO to reset the relay if RELAY_TYPE_LATCHED
    uint32_t delay_on;      // Delay to turn relay ON
    uint32_t delay_off;     // Delay to turn relay OFF
    uint8_t pulse;          // RELAY_PULSE_NONE, RELAY_PULSE_OFF or RELAY_PULSE_ON
    uint32_t pulse_ms;      // Pulse length in millis

    // Status variables
    bool current_status;    // Holds the current (physical) status of the relay
    bool target_status;     // Holds the target status
    uint32_t fw_start;      // Flood window start time
    uint8_t fw_count;       // Number of changes within the current flood window
    uint32_t change_time;   // Scheduled time to change
    bool report;            // Whether to report to own topic

    // Helping objects
    Ticker pulseTicker;     // Holds the pulse back timer

    std::vector<relay_on_event_callback_t> callbacks;
} relay_t;

// =============================================================================
// MODULE DEFAULTS
// =============================================================================

#ifndef RELAY1_PIN
    #define RELAY1_PIN                          GPIO_NONE
#endif

#ifndef RELAY2_PIN
    #define RELAY2_PIN                          GPIO_NONE
#endif

#ifndef RELAY3_PIN
    #define RELAY3_PIN                          GPIO_NONE
#endif

#ifndef RELAY4_PIN
    #define RELAY4_PIN                          GPIO_NONE
#endif

#ifndef RELAY5_PIN
    #define RELAY5_PIN                          GPIO_NONE
#endif

#ifndef RELAY6_PIN
    #define RELAY6_PIN                          GPIO_NONE
#endif

#ifndef RELAY7_PIN
    #define RELAY7_PIN                          GPIO_NONE
#endif

#ifndef RELAY8_PIN
    #define RELAY8_PIN                          GPIO_NONE
#endif

// -----------------------------------------------------------------------------

#ifndef RELAY1_TYPE
    #define RELAY1_TYPE                         RELAY_TYPE_NORMAL
#endif

#ifndef RELAY2_TYPE
    #define RELAY2_TYPE                         RELAY_TYPE_NORMAL
#endif

#ifndef RELAY3_TYPE
    #define RELAY3_TYPE                         RELAY_TYPE_NORMAL
#endif

#ifndef RELAY4_TYPE
    #define RELAY4_TYPE                         RELAY_TYPE_NORMAL
#endif

#ifndef RELAY5_TYPE
    #define RELAY5_TYPE                         RELAY_TYPE_NORMAL
#endif

#ifndef RELAY6_TYPE
    #define RELAY6_TYPE                         RELAY_TYPE_NORMAL
#endif

#ifndef RELAY7_TYPE
    #define RELAY7_TYPE                         RELAY_TYPE_NORMAL
#endif

#ifndef RELAY8_TYPE
    #define RELAY8_TYPE                         RELAY_TYPE_NORMAL
#endif

// -----------------------------------------------------------------------------
// RELAY BUTTON DEFAULTS
// -----------------------------------------------------------------------------

#ifndef RELAY1_BTN_INDEX
    #define RELAY1_BTN_INDEX                    INDEX_NONE
#endif

#ifndef RELAY2_BTN_INDEX
    #define RELAY2_BTN_INDEX                    INDEX_NONE
#endif

#ifndef RELAY3_BTN_INDEX
    #define RELAY3_BTN_INDEX                    INDEX_NONE
#endif

#ifndef RELAY4_BTN_INDEX
    #define RELAY4_BTN_INDEX                    INDEX_NONE
#endif

#ifndef RELAY5_BTN_INDEX
    #define RELAY5_BTN_INDEX                    INDEX_NONE
#endif

#ifndef RELAY6_BTN_INDEX
    #define RELAY6_BTN_INDEX                    INDEX_NONE
#endif

#ifndef RELAY7_BTN_INDEX
    #define RELAY7_BTN_INDEX                    INDEX_NONE
#endif

#ifndef RELAY8_BTN_INDEX
    #define RELAY8_BTN_INDEX                    INDEX_NONE
#endif

// -----------------------------------------------------------------------------

#ifndef RELAY1_BTN_EVENT
    #define RELAY1_BTN_EVENT                    BUTTON_EVENT_CLICK
#endif

#ifndef RELAY2_BTN_EVENT
    #define RELAY2_BTN_EVENT                    BUTTON_EVENT_CLICK
#endif

#ifndef RELAY3_BTN_EVENT
    #define RELAY3_BTN_EVENT                    BUTTON_EVENT_CLICK
#endif

#ifndef RELAY4_BTN_EVENT
    #define RELAY4_BTN_EVENT                    BUTTON_EVENT_CLICK
#endif

#ifndef RELAY5_BTN_EVENT
    #define RELAY5_BTN_EVENT                    BUTTON_EVENT_CLICK
#endif

#ifndef RELAY6_BTN_EVENT
    #define RELAY6_BTN_EVENT                    BUTTON_EVENT_CLICK
#endif

#ifndef RELAY7_BTN_EVENT
    #define RELAY7_BTN_EVENT                    BUTTON_EVENT_CLICK
#endif

#ifndef RELAY8_BTN_EVENT
    #define RELAY8_BTN_EVENT                    BUTTON_EVENT_CLICK
#endif

// -----------------------------------------------------------------------------

#ifndef RELAY1_RESET_PIN
    #define RELAY1_RESET_PIN                    GPIO_NONE
#endif

#ifndef RELAY2_RESET_PIN
    #define RELAY2_RESET_PIN                    GPIO_NONE
#endif

#ifndef RELAY3_RESET_PIN
    #define RELAY3_RESET_PIN                    GPIO_NONE
#endif

#ifndef RELAY4_RESET_PIN
    #define RELAY4_RESET_PIN                    GPIO_NONE
#endif

#ifndef RELAY5_RESET_PIN
    #define RELAY5_RESET_PIN                    GPIO_NONE
#endif

#ifndef RELAY6_RESET_PIN
    #define RELAY6_RESET_PIN                    GPIO_NONE
#endif

#ifndef RELAY7_RESET_PIN
    #define RELAY7_RESET_PIN                    GPIO_NONE
#endif

#ifndef RELAY8_RESET_PIN
    #define RELAY8_RESET_PIN                    GPIO_NONE
#endif

// -----------------------------------------------------------------------------

#ifndef RELAY1_DELAY_ON
    #define RELAY1_DELAY_ON                     0
#endif

#ifndef RELAY2_DELAY_ON
    #define RELAY2_DELAY_ON                     0
#endif

#ifndef RELAY3_DELAY_ON
    #define RELAY3_DELAY_ON                     0
#endif

#ifndef RELAY4_DELAY_ON
    #define RELAY4_DELAY_ON                     0
#endif

#ifndef RELAY5_DELAY_ON
    #define RELAY5_DELAY_ON                     0
#endif

#ifndef RELAY6_DELAY_ON
    #define RELAY6_DELAY_ON                     0
#endif

#ifndef RELAY7_DELAY_ON
    #define RELAY7_DELAY_ON                     0
#endif

#ifndef RELAY8_DELAY_ON
    #define RELAY8_DELAY_ON                     0
#endif

// -----------------------------------------------------------------------------

#ifndef RELAY1_DELAY_OFF
    #define RELAY1_DELAY_OFF                    0
#endif

#ifndef RELAY2_DELAY_OFF
    #define RELAY2_DELAY_OFF                    0
#endif

#ifndef RELAY3_DELAY_OFF
    #define RELAY3_DELAY_OFF                    0
#endif

#ifndef RELAY4_DELAY_OFF
    #define RELAY4_DELAY_OFF                    0
#endif

#ifndef RELAY5_DELAY_OFF
    #define RELAY5_DELAY_OFF                    0
#endif

#ifndef RELAY6_DELAY_OFF
    #define RELAY6_DELAY_OFF                    0
#endif

#ifndef RELAY7_DELAY_OFF
    #define RELAY7_DELAY_OFF                    0
#endif

#ifndef RELAY8_DELAY_OFF
    #define RELAY8_DELAY_OFF                    0
#endif
