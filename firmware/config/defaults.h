/*

DEFAULT CONFIGURATION

Copyright (C) 2018 FastyBird s.r.o. <info@fastybird.com>

*/

// =============================================================================
// Default values
// =============================================================================

#define GPIO_NONE                           0x99
#define INDEX_NONE                          0x99

// =============================================================================
// BUTTONS MODULE
// =============================================================================

#ifndef BUTTON1_PIN
#define BUTTON1_PIN                         GPIO_NONE
#endif

#ifndef BUTTON2_PIN
#define BUTTON2_PIN                         GPIO_NONE
#endif

#ifndef BUTTON3_PIN
#define BUTTON3_PIN                         GPIO_NONE
#endif

#ifndef BUTTON4_PIN
#define BUTTON4_PIN                         GPIO_NONE
#endif

#ifndef BUTTON5_PIN
#define BUTTON5_PIN                         GPIO_NONE
#endif

#ifndef BUTTON6_PIN
#define BUTTON6_PIN                         GPIO_NONE
#endif

#ifndef BUTTON7_PIN
#define BUTTON7_PIN                         GPIO_NONE
#endif

#ifndef BUTTON8_PIN
#define BUTTON8_PIN                         GPIO_NONE
#endif

// -----------------------------------------------------------------------------
// BUTTONS FASTYBIRD DEFAULTS
// -----------------------------------------------------------------------------

#ifndef BUTTON1_CHANNEL
#define BUTTON1_CHANNEL                     INDEX_NONE
#endif

#ifndef BUTTON2_CHANNEL
#define BUTTON2_CHANNEL                     INDEX_NONE
#endif

#ifndef BUTTON3_CHANNEL
#define BUTTON3_CHANNEL                     INDEX_NONE
#endif

#ifndef BUTTON4_CHANNEL
#define BUTTON4_CHANNEL                     INDEX_NONE
#endif

#ifndef BUTTON5_CHANNEL
#define BUTTON5_CHANNEL                     INDEX_NONE
#endif

#ifndef BUTTON6_CHANNEL
#define BUTTON6_CHANNEL                     INDEX_NONE
#endif

#ifndef BUTTON7_CHANNEL
#define BUTTON7_CHANNEL                     INDEX_NONE
#endif

#ifndef BUTTON8_CHANNEL
#define BUTTON8_CHANNEL                     INDEX_NONE
#endif

// =============================================================================
// RELAYS MODULE
// =============================================================================

#ifndef DUMMY_RELAY_COUNT
#define DUMMY_RELAY_COUNT                   0
#endif

// -----------------------------------------------------------------------------

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
// RELAYS BUTTONS DEFAULTS
// -----------------------------------------------------------------------------

#ifndef RELAY1_BTN
#define RELAY1_BTN                          INDEX_NONE
#endif

#ifndef RELAY2_BTN
#define RELAY2_BTN                          INDEX_NONE
#endif

#ifndef RELAY3_BTN
#define RELAY3_BTN                          INDEX_NONE
#endif

#ifndef RELAY4_BTN
#define RELAY4_BTN                          INDEX_NONE
#endif

#ifndef RELAY5_BTN
#define RELAY5_BTN                          INDEX_NONE
#endif

#ifndef RELAY6_BTN
#define RELAY6_BTN                          INDEX_NONE
#endif

#ifndef RELAY7_BTN
#define RELAY7_BTN                          INDEX_NONE
#endif

#ifndef RELAY8_BTN
#define RELAY8_BTN                          INDEX_NONE
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

// -----------------------------------------------------------------------------
// RELAYS FASTYBIRD DEFAULTS
// -----------------------------------------------------------------------------

#ifndef RELAY1_CHANNEL
#define RELAY1_CHANNEL                      INDEX_NONE
#endif

#ifndef RELAY2_CHANNEL
#define RELAY2_CHANNEL                      INDEX_NONE
#endif

#ifndef RELAY3_CHANNEL
#define RELAY3_CHANNEL                      INDEX_NONE
#endif

#ifndef RELAY4_CHANNEL
#define RELAY4_CHANNEL                      INDEX_NONE
#endif

#ifndef RELAY5_CHANNEL
#define RELAY5_CHANNEL                      INDEX_NONE
#endif

#ifndef RELAY6_CHANNEL
#define RELAY6_CHANNEL                      INDEX_NONE
#endif

#ifndef RELAY7_CHANNEL
#define RELAY7_CHANNEL                      INDEX_NONE
#endif

#ifndef RELAY8_CHANNEL
#define RELAY8_CHANNEL                      INDEX_NONE
#endif

// =============================================================================
// SENSORS MODULE
// =============================================================================

#ifndef SENSOR1_CHANNEL
#define SENSOR1_CHANNEL                     INDEX_NONE
#endif

// =============================================================================
// LEDs MODULE
// =============================================================================

#ifndef STATUS_LED_PIN
#define STATUS_LED_PIN                      GPIO_NONE
#endif

#ifndef STATUS_LED_PIN_INVERSE
#define STATUS_LED_PIN_INVERSE              0
#endif

#ifndef STATUS_LED_MODE
#define STATUS_LED_MODE                     LED_MODE_WIFI
#endif

// =============================================================================
// GATEWAY MODULE
// =============================================================================

#ifndef NODES_GATEWAY_TX_PIN
#define NODES_GATEWAY_TX_PIN                GPIO_NONE
#endif

#ifndef NODES_GATEWAY_RX_PIN
#define NODES_GATEWAY_RX_PIN                GPIO_NONE
#endif

// =============================================================================
// SETTINGS MODULE
// =============================================================================

#ifndef SETTINGS_FACTORY_BTN
    #ifdef BUTTON1_PIN
        #define SETTINGS_FACTORY_BTN        1
    #else
        #define SETTINGS_FACTORY_BTN        0
    #endif
#endif

#ifndef SETTINGS_FACTORY_BTN_EVENT
    #define SETTINGS_FACTORY_BTN_EVENT      BUTTON_EVENT_LNGLNGCLICK
#endif

// =============================================================================
// SYSTEM MODULE
// =============================================================================

#ifndef SYSTEM_RESET_BTN
    #ifdef BUTTON1_PIN
        #define SYSTEM_RESET_BTN            1
    #else
        #define SYSTEM_RESET_BTN            INDEX_NONE
    #endif
#endif

#ifndef SYSTEM_RESET_BTN_EVENT
    #define SYSTEM_RESET_BTN_EVENT          BUTTON_EVENT_LNGCLICK
#endif

// =============================================================================
// WIFI MODULE
// =============================================================================

#ifndef WIFI_AP_BTN
    #ifdef BUTTON1_PIN
        #define WIFI_AP_BTN                 1
    #else
        #define WIFI_AP_BTN                 INDEX_NONE
    #endif
#endif

#ifndef WIFI_AP_BTN_EVENT
    #define WIFI_AP_BTN_EVENT               BUTTON_EVENT_DBLCLICK
#endif

// =============================================================================
// GENERAL SETTINGS
// =============================================================================

// Relay providers
#ifndef RELAY_PROVIDER
    #define RELAY_PROVIDER                  RELAY_PROVIDER_NONE
#endif

// Light provider
#ifndef LIGHT_PROVIDER
    #define LIGHT_PROVIDER                  LIGHT_PROVIDER_NONE
#endif

// =============================================================================
// FASTYBIRD SETTINGS
// =============================================================================

#ifndef CHANNEL1_NAME
#define CHANNEL1_NAME                       "channel-1"
#endif

#ifndef CHANNEL2_NAME
#define CHANNEL2_NAME                       "channel-2"
#endif

#ifndef CHANNEL3_NAME
#define CHANNEL3_NAME                       "channel-3"
#endif

#ifndef CHANNEL4_NAME
#define CHANNEL4_NAME                       "channel-4"
#endif

#ifndef CHANNEL5_NAME
#define CHANNEL5_NAME                       "channel-5"
#endif

#ifndef CHANNEL6_NAME
#define CHANNEL6_NAME                       "channel-6"
#endif

#ifndef CHANNEL7_NAME
#define CHANNEL7_NAME                       "channel-7"
#endif

#ifndef CHANNEL8_NAME
#define CHANNEL8_NAME                       "channel-8"
#endif

// =============================================================================
// NODES GATEWAY SETTINGS
// =============================================================================

#ifndef NODES_GATEWAY_MASTER_ID
#define NODES_GATEWAY_MASTER_ID             254
#endif