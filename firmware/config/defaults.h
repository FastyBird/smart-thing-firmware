/*

DEFAULT CONFIGURATION

Copyright (C) 2018 FastyBird s.r.o. <info@fastybird.com>

*/

// =============================================================================
// Hardware default values
// =============================================================================

#define GPIO_NONE               0x99

// =============================================================================
// BUTTONS MODULE
// =============================================================================

#ifndef BUTTON1_PIN
#define BUTTON1_PIN             GPIO_NONE
#endif

#ifndef BUTTON2_PIN
#define BUTTON2_PIN             GPIO_NONE
#endif

#ifndef BUTTON3_PIN
#define BUTTON3_PIN             GPIO_NONE
#endif

#ifndef BUTTON4_PIN
#define BUTTON4_PIN             GPIO_NONE
#endif

#ifndef BUTTON5_PIN
#define BUTTON5_PIN             GPIO_NONE
#endif

#ifndef BUTTON6_PIN
#define BUTTON6_PIN             GPIO_NONE
#endif

#ifndef BUTTON7_PIN
#define BUTTON7_PIN             GPIO_NONE
#endif

#ifndef BUTTON8_PIN
#define BUTTON8_PIN             GPIO_NONE
#endif

// =============================================================================
// RELAYS MODULE
// =============================================================================

#ifndef DUMMY_RELAY_COUNT
#define DUMMY_RELAY_COUNT       0
#endif

// -----------------------------------------------------------------------------

#ifndef RELAY1_PIN
#define RELAY1_PIN              GPIO_NONE
#endif

#ifndef RELAY2_PIN
#define RELAY2_PIN              GPIO_NONE
#endif

#ifndef RELAY3_PIN
#define RELAY3_PIN              GPIO_NONE
#endif

#ifndef RELAY4_PIN
#define RELAY4_PIN              GPIO_NONE
#endif

#ifndef RELAY5_PIN
#define RELAY5_PIN              GPIO_NONE
#endif

#ifndef RELAY6_PIN
#define RELAY6_PIN              GPIO_NONE
#endif

#ifndef RELAY7_PIN
#define RELAY7_PIN              GPIO_NONE
#endif

#ifndef RELAY8_PIN
#define RELAY8_PIN              GPIO_NONE
#endif

// -----------------------------------------------------------------------------

#ifndef RELAY1_TYPE
#define RELAY1_TYPE             RELAY_TYPE_NORMAL
#endif

#ifndef RELAY2_TYPE
#define RELAY2_TYPE             RELAY_TYPE_NORMAL
#endif

#ifndef RELAY3_TYPE
#define RELAY3_TYPE             RELAY_TYPE_NORMAL
#endif

#ifndef RELAY4_TYPE
#define RELAY4_TYPE             RELAY_TYPE_NORMAL
#endif

#ifndef RELAY5_TYPE
#define RELAY5_TYPE             RELAY_TYPE_NORMAL
#endif

#ifndef RELAY6_TYPE
#define RELAY6_TYPE             RELAY_TYPE_NORMAL
#endif

#ifndef RELAY7_TYPE
#define RELAY7_TYPE             RELAY_TYPE_NORMAL
#endif

#ifndef RELAY8_TYPE
#define RELAY8_TYPE             RELAY_TYPE_NORMAL
#endif

// -----------------------------------------------------------------------------

#ifndef RELAY1_BTN
#define RELAY1_BTN              0
#endif

#ifndef RELAY2_BTN
#define RELAY2_BTN              0
#endif

#ifndef RELAY3_BTN
#define RELAY3_BTN              0
#endif

#ifndef RELAY4_BTN
#define RELAY4_BTN              0
#endif

#ifndef RELAY5_BTN
#define RELAY5_BTN              0
#endif

#ifndef RELAY6_BTN
#define RELAY6_BTN              0
#endif

#ifndef RELAY7_BTN
#define RELAY7_BTN              0
#endif

#ifndef RELAY8_BTN
#define RELAY8_BTN              0
#endif

#ifndef RELAY1_BTN_EVENT
#define RELAY1_BTN_EVENT        BUTTON_EVENT_CLICK
#endif

#ifndef RELAY2_BTN_EVENT
#define RELAY2_BTN_EVENT        BUTTON_EVENT_CLICK
#endif

#ifndef RELAY3_BTN_EVENT
#define RELAY3_BTN_EVENT        BUTTON_EVENT_CLICK
#endif

#ifndef RELAY4_BTN_EVENT
#define RELAY4_BTN_EVENT        BUTTON_EVENT_CLICK
#endif

#ifndef RELAY5_BTN_EVENT
#define RELAY5_BTN_EVENT        BUTTON_EVENT_CLICK
#endif

#ifndef RELAY6_BTN_EVENT
#define RELAY6_BTN_EVENT        BUTTON_EVENT_CLICK
#endif

#ifndef RELAY7_BTN_EVENT
#define RELAY7_BTN_EVENT        BUTTON_EVENT_CLICK
#endif

#ifndef RELAY8_BTN_EVENT
#define RELAY8_BTN_EVENT        BUTTON_EVENT_CLICK
#endif

// -----------------------------------------------------------------------------

#ifndef RELAY1_RESET_PIN
#define RELAY1_RESET_PIN        GPIO_NONE
#endif

#ifndef RELAY2_RESET_PIN
#define RELAY2_RESET_PIN        GPIO_NONE
#endif

#ifndef RELAY3_RESET_PIN
#define RELAY3_RESET_PIN        GPIO_NONE
#endif

#ifndef RELAY4_RESET_PIN
#define RELAY4_RESET_PIN        GPIO_NONE
#endif

#ifndef RELAY5_RESET_PIN
#define RELAY5_RESET_PIN        GPIO_NONE
#endif

#ifndef RELAY6_RESET_PIN
#define RELAY6_RESET_PIN        GPIO_NONE
#endif

#ifndef RELAY7_RESET_PIN
#define RELAY7_RESET_PIN        GPIO_NONE
#endif

#ifndef RELAY8_RESET_PIN
#define RELAY8_RESET_PIN        GPIO_NONE
#endif

// -----------------------------------------------------------------------------

#ifndef RELAY1_DELAY_ON
#define RELAY1_DELAY_ON         0
#endif

#ifndef RELAY2_DELAY_ON
#define RELAY2_DELAY_ON         0
#endif

#ifndef RELAY3_DELAY_ON
#define RELAY3_DELAY_ON         0
#endif

#ifndef RELAY4_DELAY_ON
#define RELAY4_DELAY_ON         0
#endif

#ifndef RELAY5_DELAY_ON
#define RELAY5_DELAY_ON         0
#endif

#ifndef RELAY6_DELAY_ON
#define RELAY6_DELAY_ON         0
#endif

#ifndef RELAY7_DELAY_ON
#define RELAY7_DELAY_ON         0
#endif

#ifndef RELAY8_DELAY_ON
#define RELAY8_DELAY_ON         0
#endif

// -----------------------------------------------------------------------------

#ifndef RELAY1_DELAY_OFF
#define RELAY1_DELAY_OFF        0
#endif

#ifndef RELAY2_DELAY_OFF
#define RELAY2_DELAY_OFF        0
#endif

#ifndef RELAY3_DELAY_OFF
#define RELAY3_DELAY_OFF        0
#endif

#ifndef RELAY4_DELAY_OFF
#define RELAY4_DELAY_OFF        0
#endif

#ifndef RELAY5_DELAY_OFF
#define RELAY5_DELAY_OFF        0
#endif

#ifndef RELAY6_DELAY_OFF
#define RELAY6_DELAY_OFF        0
#endif

#ifndef RELAY7_DELAY_OFF
#define RELAY7_DELAY_OFF        0
#endif

#ifndef RELAY8_DELAY_OFF
#define RELAY8_DELAY_OFF        0
#endif

// =============================================================================
// LEDs MODULE
// =============================================================================

#ifndef STATUS_LED_PIN
#define STATUS_LED_PIN          GPIO_NONE
#endif

#ifndef STATUS_LED_PIN_INVERSE
#define STATUS_LED_PIN_INVERSE  0
#endif

#ifndef STATUS_LED_MODE
#define STATUS_LED_MODE         LED_MODE_WIFI
#endif

// =============================================================================
// GATEWAY MODULE
// =============================================================================

#ifndef NODES_GATEWAY_TX_PIN
#define NODES_GATEWAY_TX_PIN    GPIO_NONE
#endif

#ifndef NODES_GATEWAY_RX_PIN
#define NODES_GATEWAY_RX_PIN    GPIO_NONE
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
        #define SYSTEM_RESET_BTN            0
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
        #define WIFI_AP_BTN                 0
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
// RF ENCODER
// =============================================================================

#ifndef ENCODER1_PIN1
#define ENCODER1_PIN1           GPIO_NONE
#endif

#ifndef ENCODER2_PIN1
#define ENCODER2_PIN1           GPIO_NONE
#endif

#ifndef ENCODER3_PIN1
#define ENCODER3_PIN1           GPIO_NONE
#endif

#ifndef ENCODER4_PIN1
#define ENCODER4_PIN1           GPIO_NONE
#endif

#ifndef ENCODER5_PIN1
#define ENCODER5_PIN1           GPIO_NONE
#endif

// -----------------------------------------------------------------------------

#ifndef ENCODER1_PIN2
#define ENCODER1_PIN2           GPIO_NONE
#endif

#ifndef ENCODER2_PIN2
#define ENCODER2_PIN2           GPIO_NONE
#endif

#ifndef ENCODER3_PIN2
#define ENCODER3_PIN2           GPIO_NONE
#endif

#ifndef ENCODER4_PIN2
#define ENCODER4_PIN2           GPIO_NONE
#endif

#ifndef ENCODER5_PIN2
#define ENCODER5_PIN2           GPIO_NONE
#endif

// -----------------------------------------------------------------------------

#ifndef ENCODER1_BUTTON_PIN
#define ENCODER1_BUTTON_PIN     GPIO_NONE
#endif

#ifndef ENCODER2_BUTTON_PIN
#define ENCODER2_BUTTON_PIN     GPIO_NONE
#endif

#ifndef ENCODER3_BUTTON_PIN
#define ENCODER3_BUTTON_PIN     GPIO_NONE
#endif

#ifndef ENCODER4_BUTTON_PIN
#define ENCODER4_BUTTON_PIN     GPIO_NONE
#endif

#ifndef ENCODER5_BUTTON_PIN
#define ENCODER5_BUTTON_PIN     GPIO_NONE
#endif

// -----------------------------------------------------------------------------

#ifndef ENCODER1_BUTTON_LOGIC
#define ENCODER1_BUTTON_LOGIC   HIGH
#endif

#ifndef ENCODER2_BUTTON_LOGIC
#define ENCODER2_BUTTON_LOGIC   HIGH
#endif

#ifndef ENCODER3_BUTTON_LOGIC
#define ENCODER3_BUTTON_LOGIC   HIGH
#endif

#ifndef ENCODER4_BUTTON_LOGIC
#define ENCODER4_BUTTON_LOGIC   HIGH
#endif

#ifndef ENCODER5_BUTTON_LOGIC
#define ENCODER5_BUTTON_LOGIC   HIGH
#endif

// -----------------------------------------------------------------------------

#ifndef ENCODER1_BUTTON_MODE
#define ENCODER1_BUTTON_MODE    INPUT_PULLUP
#endif

#ifndef ENCODER2_BUTTON_MODE
#define ENCODER2_BUTTON_MODE    INPUT_PULLUP
#endif

#ifndef ENCODER3_BUTTON_MODE
#define ENCODER3_BUTTON_MODE    INPUT_PULLUP
#endif

#ifndef ENCODER4_BUTTON_MODE
#define ENCODER4_BUTTON_MODE    INPUT_PULLUP
#endif

#ifndef ENCODER5_BUTTON_MODE
#define ENCODER5_BUTTON_MODE    INPUT_PULLUP
#endif

// -----------------------------------------------------------------------------

#ifndef ENCODER1_MODE
#define ENCODER1_MODE           1
#endif

#ifndef ENCODER2_MODE
#define ENCODER2_MODE           1
#endif

#ifndef ENCODER3_MODE
#define ENCODER3_MODE           1
#endif

#ifndef ENCODER4_MODE
#define ENCODER4_MODE           1
#endif

#ifndef ENCODER5_MODE
#define ENCODER5_MODE           1
#endif

// -----------------------------------------------------------------------------

#ifndef ENCODER1_CHANNEL1
#define ENCODER1_CHANNEL1       0
#endif

#ifndef ENCODER2_CHANNEL1
#define ENCODER2_CHANNEL1       0
#endif

#ifndef ENCODER3_CHANNEL1
#define ENCODER3_CHANNEL1       0
#endif

#ifndef ENCODER4_CHANNEL1
#define ENCODER4_CHANNEL1       0
#endif

#ifndef ENCODER5_CHANNEL1
#define ENCODER5_CHANNEL1       0
#endif

// -----------------------------------------------------------------------------

#ifndef ENCODER1_CHANNEL2
#define ENCODER1_CHANNEL2       1
#endif

#ifndef ENCODER2_CHANNEL2
#define ENCODER2_CHANNEL2       1
#endif

#ifndef ENCODER3_CHANNEL2
#define ENCODER3_CHANNEL2       1
#endif

#ifndef ENCODER4_CHANNEL2
#define ENCODER4_CHANNEL2       1
#endif

#ifndef ENCODER5_CHANNEL2
#define ENCODER5_CHANNEL2       1
#endif
