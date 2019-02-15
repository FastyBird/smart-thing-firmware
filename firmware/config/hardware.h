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

    // Disable non-core modules
    #define ALEXA_SUPPORT               0
    #define I2C_SUPPORT                 0
    #define NTP_SUPPORT                 0
    #define SCHEDULER_SUPPORT           0
    #define DIRECT_CONTROL_SUPPORT      0
    #define SENSOR_SUPPORT              0
    #define WEB_SUPPORT                 0

// -----------------------------------------------------------------------------
// FastyBird things
// -----------------------------------------------------------------------------

#elif FASTYBIRD_WIFI_GATEWAY

    // Info
    #define MANUFACTURER                "FASTYBIRD"
    #define THING                       "WIFI_GW"

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

#elif defined(ITEAD_SONOFF_SLAMPHER)

    // Info
    #define MANUFACTURER                "ITEAD"
    #define THING                       "SONOFF_SLAMPHER"

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

#endif
