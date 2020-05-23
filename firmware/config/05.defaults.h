/*

DEFAULT CONFIGURATION

Copyright (C) 2018 FastyBird Ltd. <info@fastybird.com>

*/

#pragma once

// =============================================================================
// Default values
// =============================================================================

#define GPIO_NONE                           0xFF
#define INDEX_NONE                          0xFF

// =============================================================================
// BUTTONS MODULE
// =============================================================================

#ifndef BUTTON1_PIN
    #define BUTTON1_PIN                     GPIO_NONE
#endif

#ifndef BUTTON2_PIN
    #define BUTTON2_PIN                     GPIO_NONE
#endif

#ifndef BUTTON3_PIN
    #define BUTTON3_PIN                     GPIO_NONE
#endif

#ifndef BUTTON4_PIN
    #define BUTTON4_PIN                     GPIO_NONE
#endif

#ifndef BUTTON5_PIN
    #define BUTTON5_PIN                     GPIO_NONE
#endif

#ifndef BUTTON6_PIN
    #define BUTTON6_PIN                     GPIO_NONE
#endif

#ifndef BUTTON7_PIN
    #define BUTTON7_PIN                     GPIO_NONE
#endif

#ifndef BUTTON8_PIN
    #define BUTTON8_PIN                     GPIO_NONE
#endif

// =============================================================================
// RELAYS MODULE
// =============================================================================

#ifndef RELAY_PROVIDER
    #define RELAY_PROVIDER                  RELAY_PROVIDER_NONE
#endif

#ifndef DUMMY_RELAY_COUNT
    #define DUMMY_RELAY_COUNT               0
#endif

// -----------------------------------------------------------------------------

#ifndef RELAY1_PIN
    #define RELAY1_PIN                      GPIO_NONE
#endif

#ifndef RELAY2_PIN
    #define RELAY2_PIN                      GPIO_NONE
#endif

#ifndef RELAY3_PIN
    #define RELAY3_PIN                      GPIO_NONE
#endif

#ifndef RELAY4_PIN
    #define RELAY4_PIN                      GPIO_NONE
#endif

#ifndef RELAY5_PIN
    #define RELAY5_PIN                      GPIO_NONE
#endif

#ifndef RELAY6_PIN
    #define RELAY6_PIN                      GPIO_NONE
#endif

#ifndef RELAY7_PIN
    #define RELAY7_PIN                      GPIO_NONE
#endif

#ifndef RELAY8_PIN
    #define RELAY8_PIN                      GPIO_NONE
#endif

// -----------------------------------------------------------------------------

#ifndef RELAY1_TYPE
    #define RELAY1_TYPE                     RELAY_TYPE_NORMAL
#endif

#ifndef RELAY2_TYPE
    #define RELAY2_TYPE                     RELAY_TYPE_NORMAL
#endif

#ifndef RELAY3_TYPE
    #define RELAY3_TYPE                     RELAY_TYPE_NORMAL
#endif

#ifndef RELAY4_TYPE
    #define RELAY4_TYPE                     RELAY_TYPE_NORMAL
#endif

#ifndef RELAY5_TYPE
    #define RELAY5_TYPE                     RELAY_TYPE_NORMAL
#endif

#ifndef RELAY6_TYPE
    #define RELAY6_TYPE                     RELAY_TYPE_NORMAL
#endif

#ifndef RELAY7_TYPE
    #define RELAY7_TYPE                     RELAY_TYPE_NORMAL
#endif

#ifndef RELAY8_TYPE
    #define RELAY8_TYPE                     RELAY_TYPE_NORMAL
#endif

// -----------------------------------------------------------------------------
// RELAYS BUTTONS DEFAULTS
// -----------------------------------------------------------------------------

#ifndef RELAY1_BTN_INDEX
    #define RELAY1_BTN_INDEX                INDEX_NONE
#endif

#ifndef RELAY2_BTN_INDEX
    #define RELAY2_BTN_INDEX                INDEX_NONE
#endif

#ifndef RELAY3_BTN_INDEX
    #define RELAY3_BTN_INDEX                INDEX_NONE
#endif

#ifndef RELAY4_BTN_INDEX
    #define RELAY4_BTN_INDEX                INDEX_NONE
#endif

#ifndef RELAY5_BTN_INDEX
    #define RELAY5_BTN_INDEX                INDEX_NONE
#endif

#ifndef RELAY6_BTN_INDEX
    #define RELAY6_BTN_INDEX                INDEX_NONE
#endif

#ifndef RELAY7_BTN_INDEX
    #define RELAY7_BTN_INDEX                INDEX_NONE
#endif

#ifndef RELAY8_BTN_INDEX
    #define RELAY8_BTN_INDEX                INDEX_NONE
#endif

// -----------------------------------------------------------------------------

#ifndef RELAY1_BTN_EVENT
    #define RELAY1_BTN_EVENT                BUTTON_EVENT_CLICK
#endif

#ifndef RELAY2_BTN_EVENT
    #define RELAY2_BTN_EVENT                BUTTON_EVENT_CLICK
#endif

#ifndef RELAY3_BTN_EVENT
    #define RELAY3_BTN_EVENT                BUTTON_EVENT_CLICK
#endif

#ifndef RELAY4_BTN_EVENT
    #define RELAY4_BTN_EVENT                BUTTON_EVENT_CLICK
#endif

#ifndef RELAY5_BTN_EVENT
    #define RELAY5_BTN_EVENT                BUTTON_EVENT_CLICK
#endif

#ifndef RELAY6_BTN_EVENT
    #define RELAY6_BTN_EVENT                BUTTON_EVENT_CLICK
#endif

#ifndef RELAY7_BTN_EVENT
    #define RELAY7_BTN_EVENT                BUTTON_EVENT_CLICK
#endif

#ifndef RELAY8_BTN_EVENT
    #define RELAY8_BTN_EVENT                BUTTON_EVENT_CLICK
#endif

// -----------------------------------------------------------------------------

#ifndef RELAY1_RESET_PIN
    #define RELAY1_RESET_PIN                GPIO_NONE
#endif

#ifndef RELAY2_RESET_PIN
    #define RELAY2_RESET_PIN                GPIO_NONE
#endif

#ifndef RELAY3_RESET_PIN
    #define RELAY3_RESET_PIN                GPIO_NONE
#endif

#ifndef RELAY4_RESET_PIN
    #define RELAY4_RESET_PIN                GPIO_NONE
#endif

#ifndef RELAY5_RESET_PIN
    #define RELAY5_RESET_PIN                GPIO_NONE
#endif

#ifndef RELAY6_RESET_PIN
    #define RELAY6_RESET_PIN                GPIO_NONE
#endif

#ifndef RELAY7_RESET_PIN
    #define RELAY7_RESET_PIN                GPIO_NONE
#endif

#ifndef RELAY8_RESET_PIN
    #define RELAY8_RESET_PIN                GPIO_NONE
#endif

// -----------------------------------------------------------------------------

#ifndef RELAY1_DELAY_ON
    #define RELAY1_DELAY_ON                 0
#endif

#ifndef RELAY2_DELAY_ON
    #define RELAY2_DELAY_ON                 0
#endif

#ifndef RELAY3_DELAY_ON
    #define RELAY3_DELAY_ON                 0
#endif

#ifndef RELAY4_DELAY_ON
    #define RELAY4_DELAY_ON                 0
#endif

#ifndef RELAY5_DELAY_ON
    #define RELAY5_DELAY_ON                 0
#endif

#ifndef RELAY6_DELAY_ON
    #define RELAY6_DELAY_ON                 0
#endif

#ifndef RELAY7_DELAY_ON
    #define RELAY7_DELAY_ON                 0
#endif

#ifndef RELAY8_DELAY_ON
    #define RELAY8_DELAY_ON                 0
#endif

// -----------------------------------------------------------------------------

#ifndef RELAY1_DELAY_OFF
    #define RELAY1_DELAY_OFF                0
#endif

#ifndef RELAY2_DELAY_OFF
    #define RELAY2_DELAY_OFF                0
#endif

#ifndef RELAY3_DELAY_OFF
    #define RELAY3_DELAY_OFF                0
#endif

#ifndef RELAY4_DELAY_OFF
    #define RELAY4_DELAY_OFF                0
#endif

#ifndef RELAY5_DELAY_OFF
    #define RELAY5_DELAY_OFF                0
#endif

#ifndef RELAY6_DELAY_OFF
    #define RELAY6_DELAY_OFF                0
#endif

#ifndef RELAY7_DELAY_OFF
    #define RELAY7_DELAY_OFF                0
#endif

#ifndef RELAY8_DELAY_OFF
    #define RELAY8_DELAY_OFF                0
#endif

// =============================================================================
// LIGHT MODULE
// =============================================================================

#ifndef LIGHT_PROVIDER
    #define LIGHT_PROVIDER                  LIGHT_PROVIDER_NONE
#endif

// =============================================================================
// LEDs MODULE
// =============================================================================

#ifndef STATUS_LED_PIN
    #define STATUS_LED_PIN                  GPIO_NONE
#endif

#ifndef STATUS_LED_PIN_INVERSE
    #define STATUS_LED_PIN_INVERSE          0
#endif

#ifndef STATUS_LED_MODE
    #define STATUS_LED_MODE                 LED_MODE_WIFI
#endif

// =============================================================================
// SETTINGS MODULE
// =============================================================================

#ifndef SETTINGS_FACTORY_BTN
    #ifdef BUTTON1_PIN
        #define SETTINGS_FACTORY_BTN        0
    #else
        #define SETTINGS_FACTORY_BTN        INDEX_NONE
    #endif
#endif

#ifndef SETTINGS_FACTORY_BTN_EVENT
    #define SETTINGS_FACTORY_BTN_EVENT      BUTTON_EVENT_LNG_LNG_CLICK
#endif

// =============================================================================
// SYSTEM MODULE
// =============================================================================

#ifndef SYSTEM_RESET_BTN
    #ifdef BUTTON1_PIN
        #define SYSTEM_RESET_BTN            0
    #else
        #define SYSTEM_RESET_BTN            INDEX_NONE
    #endif
#endif

#ifndef SYSTEM_RESET_BTN_EVENT
    #define SYSTEM_RESET_BTN_EVENT          BUTTON_EVENT_LNG_CLICK
#endif

// =============================================================================
// WIFI MODULE
// =============================================================================

#ifndef WIFI_AP_BTN
    #ifdef BUTTON1_PIN
        #define WIFI_AP_BTN                 0
    #else
        #define WIFI_AP_BTN                 INDEX_NONE
    #endif
#endif

#ifndef WIFI_AP_BTN_EVENT
    #define WIFI_AP_BTN_EVENT               BUTTON_EVENT_DBL_CLICK
#endif
