/*

SUPPORTED HARDWARE DEFINITIONS

Copyright (C) 2018 FastyBird Ltd. <info@fastybird.com>

*/

// -----------------------------------------------------------------------------
// FastyBird Core
// -----------------------------------------------------------------------------

#if defined(FASTYBIRD_CORE)

    // This is a special firmware configuration targeted to generate
    // a light-weight binary image meant to be able to do two-step-updates

    // Info
    #define MANUFACTURER                "ESPRESSIF"
    #define THING                       "FASTYBIRD_CORE"
    #define HARWARE_VERSION             "0.0.1"

    // Disable non-core modules
    #define ALEXA_SUPPORT               0
    #define I2C_SUPPORT                 0
    #define NTP_SUPPORT                 0
    #define SCHEDULER_SUPPORT           0
    #define SENSOR_SUPPORT              0
    #define WEB_SUPPORT                 0

// -----------------------------------------------------------------------------
// FastyBird things
// -----------------------------------------------------------------------------

#elif FASTYBIRD_WIFI_GATEWAY

    // Info
    #define MANUFACTURER                "FASTYBIRD"
    #define THING                       "WIFI_GW"
    #define HARWARE_VERSION             "0.0.1"

    // BUTTONs
    #define BUTTON1_PIN                 0
    #define BUTTON1_MODE                BUTTON_PUSHBUTTON | BUTTON_DEFAULT_HIGH

    // LEDs
    #define LED1_PIN                    2
    #define LED1_PIN_INVERSE            1

    // I2C interface
    #define I2C_SDA_PIN                 4  // D2
    #define I2C_SCL_PIN                 5  // D1

    #define NODES_GATEWAY_SUPPORT       1
    #define NODES_GATEWAY_TX_PIN        12
    #define NODES_GATEWAY_RX_PIN        4

    // FastyBird cloud with gateway support
    #define FASTYBIRD_GATEWAY_SUPPORT   1

// -----------------------------------------------------------------------------
// Itead Studio boards
// -----------------------------------------------------------------------------

#elif defined(ITEAD_SONOFF_BASIC)

    // Info
    #define MANUFACTURER                "ITEAD"
    #define THING                       "SONOFF_BASIC"
    #define HARWARE_VERSION             "0.0.1"

    // Buttons
    #define BUTTON1_PIN                 0
    #define BUTTON1_MODE                BUTTON_PUSHBUTTON | BUTTON_DEFAULT_HIGH

    // Relays
    #define RELAY_PROVIDER              RELAY_PROVIDER_RELAY

    #define RELAY1_PIN                  12
    #define RELAY1_TYPE                 RELAY_TYPE_NORMAL
    #define RELAY1_BTN                  1

    // LEDs
    #define LED1_PIN                    13
    #define LED1_PIN_INVERSE            1

#elif defined(ITEAD_SONOFF_DUAL)

    // Info
    #define MANUFACTURER                "ITEAD"
    #define THING                       "SONOFF_DUAL"
    #define HARWARE_VERSION             "0.0.1"

    #define SERIAL_BAUDRATE             19230
    #define DEBUG_SERIAL_SUPPORT        0

    // Relays
    #define RELAY_PROVIDER              RELAY_PROVIDER_DUAL
    #define DUMMY_RELAY_COUNT           2

    // Buttons
    #define BUTTON3_RELAY               1

    // LEDs
    #define LED1_PIN                    13
    #define LED1_PIN_INVERSE            1

#elif defined(ITEAD_SONOFF_DUAL_R2)

    #define MANUFACTURER                "ITEAD"
    #define THING                       "SONOFF_DUAL_R2"
    #define HARWARE_VERSION             "0.0.2"

    // Buttons
    #define BUTTON1_PIN                 0       // Button 0 on header
    #define BUTTON1_MODE                BUTTON_SWITCH | BUTTON_SET_PULLUP | BUTTON_DEFAULT_HIGH
    #define BUTTON2_PIN                 9       // Button 1 on header
    #define BUTTON2_MODE                BUTTON_SWITCH | BUTTON_SET_PULLUP | BUTTON_DEFAULT_HIGH
    #define BUTTON3_PIN                 10      // Physical button
    #define BUTTON3_MODE                BUTTON_PUSHBUTTON | BUTTON_DEFAULT_HIGH

    // Relays
    #define RELAY_PROVIDER              RELAY_PROVIDER_RELAY

    #define RELAY1_PIN                  12
    #define RELAY1_TYPE                 RELAY_TYPE_NORMAL
    #define RELAY1_BTN                  1
    #define RELAY2_PIN                  5
    #define RELAY2_TYPE                 RELAY_TYPE_NORMAL
    #define RELAY2_BTN                  2

    // LEDs
    #define LED1_PIN                    13
    #define LED1_PIN_INVERSE            1

#elif defined(ITEAD_SONOFF_SLAMPHER)

    // Info
    #define MANUFACTURER                "ITEAD"
    #define THING                       "SONOFF_SLAMPHER"
    #define HARWARE_VERSION             "0.0.1"

    // Buttons
    #define BUTTON1_PIN                 0
    #define BUTTON1_MODE                BUTTON_PUSHBUTTON | BUTTON_DEFAULT_HIGH

    // Relays
    #define RELAY_PROVIDER              RELAY_PROVIDER_RELAY

    #define RELAY1_PIN                  12
    #define RELAY1_TYPE                 RELAY_TYPE_NORMAL
    #define RELAY1_BTN                  1

    // LEDs
    #define LED1_PIN                    13
    #define LED1_PIN_INVERSE            1

#elif defined(ITEAD_SONOFF_S20)

    // Info
    #define MANUFACTURER                "ITEAD"
    #define THING                       "SONOFF_S20"
    #define HARWARE_VERSION             "0.0.1"

    // Buttons
    #define BUTTON1_PIN                 0
    #define BUTTON1_MODE                BUTTON_PUSHBUTTON | BUTTON_DEFAULT_HIGH

    // Relays
    #define RELAY_PROVIDER              RELAY_PROVIDER_RELAY

    #define RELAY1_PIN                  12
    #define RELAY1_TYPE                 RELAY_TYPE_NORMAL
    #define RELAY1_BTN                  1

    // LEDs
    #define LED1_PIN                    13
    #define LED1_PIN_INVERSE            1

#elif defined(ITEAD_SONOFF_POW)

    // Info
    #define MANUFACTURER                "ITEAD"
    #define THING                       "SONOFF_POW"
    #define HARWARE_VERSION             "0.0.1"

    // Buttons
    #define BUTTON1_PIN                 0
    #define BUTTON1_MODE                BUTTON_PUSHBUTTON | BUTTON_DEFAULT_HIGH

    // Relays
    #define RELAY_PROVIDER              RELAY_PROVIDER_RELAY

    #define RELAY1_PIN                  12
    #define RELAY1_TYPE                 RELAY_TYPE_NORMAL
    #define RELAY1_BTN                  1

    // LEDs
    #define LED1_PIN                    15
    #define LED1_PIN_INVERSE            0

    // HLW8012
    #ifndef HLW8012_SUPPORT
        #define HLW8012_SUPPORT         1
    #endif

    #define HLW8012_SEL_PIN             5
    #define HLW8012_CF1_PIN             13
    #define HLW8012_CF_PIN              14

#elif defined(ITEAD_SONOFF_POW_R2)

    // Info
    #define MANUFACTURER                "ITEAD"
    #define THING                       "SONOFF_POW_R2"
    #define HARWARE_VERSION             "0.0.2"

    // Buttons
    #define BUTTON1_PIN                 0
    #define BUTTON1_MODE                BUTTON_PUSHBUTTON | BUTTON_DEFAULT_HIGH

    // Relays
    #define RELAY_PROVIDER              RELAY_PROVIDER_RELAY

    #define RELAY1_PIN                  12
    #define RELAY1_TYPE                 RELAY_TYPE_NORMAL
    #define RELAY1_BTN                  1

    // LEDs
    #define LED1_PIN                    13
    #define LED1_PIN_INVERSE            1

    // Disable UART noise
    #define DEBUG_SERIAL_SUPPORT        0

    // CSE7766
    #ifndef CSE7766_SUPPORT
        #define CSE7766_SUPPORT         1
    #endif

    #define CSE7766_PIN                 1

#endif
