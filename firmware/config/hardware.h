/*

SUPPORTED HARDWARE DEFINITIONS

Copyright (C) 2018 FastyBird s.r.o. <info@fastybird.com>

*/

// -----------------------------------------------------------------------------
// FastyBird Core
// -----------------------------------------------------------------------------

#if defined(FASTYBIRD_CORE)

    // This is a special firmware configuration targeted to generate
    // a light-weight binary image meant to be able to do two-step-updates

    // Info
    #define MANUFACTURER                "ESPRESSIF"
    #define DEVICE                      "FASTYBIRD_CORE"
    #define HARWARE_VERSION             "0.0.1"

    // Disable non-core modules
    #define ALEXA_SUPPORT               0
    #define I2C_SUPPORT                 0
    #define SENSOR_SUPPORT              0
    #define WEB_SUPPORT                 0

// -----------------------------------------------------------------------------
// FastyBird devices
// -----------------------------------------------------------------------------

#elif FASTYBIRD_WIFI_GATEWAY

    // Info
    #define MANUFACTURER                "FASTYBIRD"
    #define DEVICE                      "WIFI_GW"
    #define HARWARE_VERSION             "0.0.1"
    #define FASTYBIRD_MAX_CHANNELS      1

    // BUTTONs
    #define BUTTON_SUPPORT              1

    #define BUTTON1_PIN                 0
    #define BUTTON1_MODE                BUTTON_PUSHBUTTON | BUTTON_DEFAULT_HIGH

    // LEDs
    #define STATUS_LED_PIN              2
    #define STATUS_LED_PIN_INVERSE      1

    // I2C interface
    #define I2C_SDA_PIN                 4  // D2
    #define I2C_SCL_PIN                 5  // D1

    #define NODES_GATEWAY_SUPPORT       1
    #define NODES_GATEWAY_TX_PIN        12
    #define NODES_GATEWAY_RX_PIN        4

    // FastyBird cloud with gateway support
    #define FASTYBIRD_GATEWAY_SUPPORT   1

#elif defined(FASTYBIRD_3CH_POWER_STRIP_R1)

    // Info
    #define MANUFACTURER                "FASTYBIRD"
    #define DEVICE                      "3CH_POWER_STRIP_R1"
    #define HARWARE_VERSION             "0.0.1"
    #define FASTYBIRD_MAX_CHANNELS      3

    // BUTTONs
    #define BUTTON_SUPPORT              1

    #define BUTTON1_PIN                 0
    #define BUTTON1_MODE                BUTTON_PUSHBUTTON | BUTTON_DEFAULT_HIGH

    #define BUTTON2_PIN                 9
    #define BUTTON2_MODE                BUTTON_PUSHBUTTON | BUTTON_DEFAULT_HIGH

    #define BUTTON3_PIN                 10
    #define BUTTON3_MODE                BUTTON_PUSHBUTTON | BUTTON_DEFAULT_HIGH

    #define BUTTON4_PIN                 14
    #define BUTTON4_MODE                BUTTON_PUSHBUTTON | BUTTON_DEFAULT_HIGH

    #define SETTINGS_FACTORY_BTN        4
    #define SYSTEM_RESET_BTN            4
    #define WIFI_AP_BTN                 4

    // RELAYs
    #define RELAY_PROVIDER              RELAY_PROVIDER_RELAY

    #define RELAY1_PIN                  12
    #define RELAY1_TYPE                 RELAY_TYPE_NORMAL
    #define RELAY1_BTN                  1
    #define RELAY1_CHANNEL              1

    #define RELAY2_PIN                  5
    #define RELAY2_TYPE                 RELAY_TYPE_NORMAL
    #define RELAY2_BTN                  2
    #define RELAY2_CHANNEL              2

    #define RELAY3_PIN                  4
    #define RELAY3_TYPE                 RELAY_TYPE_NORMAL
    #define RELAY3_BTN                  3
    #define RELAY3_CHANNEL              3

    // LEDs
    #define STATUS_LED_PIN              13
    #define STATUS_LED_PIN_INVERSE      1

    // CHANNELs
    #define CHANNEL1_NAME               "socket-one"
    #define CHANNEL2_NAME               "socket-two"
    #define CHANNEL3_NAME               "socket-three"

// -----------------------------------------------------------------------------
// Itead Studio boards
// -----------------------------------------------------------------------------

#elif defined(ITEAD_SONOFF_BASIC)

    // Info
    #define MANUFACTURER                "ITEAD"
    #define DEVICE                      "SONOFF_BASIC"
    #define HARWARE_VERSION             "0.0.1"
    #define FASTYBIRD_MAX_CHANNELS      1

    // BUTTONs
    #define BUTTON_SUPPORT              1

    #define BUTTON1_PIN                 0
    #define BUTTON1_MODE                BUTTON_PUSHBUTTON | BUTTON_DEFAULT_HIGH

    // RELAYs
    #define RELAY_PROVIDER              RELAY_PROVIDER_RELAY

    #define RELAY1_PIN                  12
    #define RELAY1_TYPE                 RELAY_TYPE_NORMAL
    #define RELAY1_BTN                  1
    #define RELAY1_CHANNEL              1

    // LEDs
    #define STATUS_LED_PIN              13
    #define STATUS_LED_PIN_INVERSE      1

    // CHANNELs
    #define CHANNEL1_NAME               "output"

#elif defined(ITEAD_SONOFF_MINI)

    // Info
    #define MANUFACTURER                "ITEAD"
    #define DEVICE                      "SONOFF_MINI"
    #define HARWARE_VERSION             "0.0.1"
    #define FASTYBIRD_MAX_CHANNELS      1

    // BUTTONs
    #define BUTTON_SUPPORT              1

    #define BUTTON1_PIN                 0
    #define BUTTON1_MODE                BUTTON_PUSHBUTTON | BUTTON_DEFAULT_HIGH
    #define BUTTON1_RELAY               1
    #define BUTTON2_PIN                 4
    #define BUTTON2_MODE                BUTTON_SWITCH | BUTTON_SET_PULLUP | BUTTON_DEFAULT_HIGH
    #define BUTTON2_RELAY               1

    // RELAYs
    #define RELAY_PROVIDER              RELAY_PROVIDER_RELAY

    #define RELAY1_PIN                  12
    #define RELAY1_TYPE                 RELAY_TYPE_NORMAL

    // LEDs
    #define LED1_PIN                    13
    #define LED1_PIN_INVERSE            1

    // CHANNELs
    #define CHANNEL1_NAME               "output"

#elif defined(ITEAD_SONOFF_DUAL_R2)

    #define MANUFACTURER                "ITEAD"
    #define DEVICE                      "SONOFF_DUAL_R2"
    #define HARWARE_VERSION             "0.0.2"
    #define FASTYBIRD_MAX_CHANNELS      1

    // BUTTONs
    #define BUTTON_SUPPORT              1

    #define BUTTON1_PIN                 10
    #define BUTTON1_MODE                BUTTON_PUSHBUTTON | BUTTON_DEFAULT_HIGH

    // RELAYs
    #define RELAY_PROVIDER              RELAY_PROVIDER_RELAY

    #define RELAY1_PIN                  12
    #define RELAY1_TYPE                 RELAY_TYPE_NORMAL
    #define RELAY1_BTN                  1
    #define RELAY1_CHANNEL              1

    #define RELAY2_PIN                  5
    #define RELAY2_TYPE                 RELAY_TYPE_NORMAL
    #define RELAY2_BTN                  1
    #define RELAY2_BTN_EVENT            BUTTON_EVENT_DBLCLICK
    #define RELAY1_CHANNEL              2

    // LEDs
    #define STATUS_LED_PIN              13
    #define STATUS_LED_PIN_INVERSE      1

    // CHANNELs
    #define CHANNEL1_NAME               "output-one"
    #define CHANNEL2_NAME               "output-two"

#elif defined(ITEAD_SONOFF_SLAMPHER)

    // Info
    #define MANUFACTURER                "ITEAD"
    #define DEVICE                      "SONOFF_SLAMPHER"
    #define HARWARE_VERSION             "0.0.1"
    #define FASTYBIRD_MAX_CHANNELS      1

    // BUTTONs
    #define BUTTON_SUPPORT              1

    #define BUTTON1_PIN                 0
    #define BUTTON1_MODE                BUTTON_PUSHBUTTON | BUTTON_DEFAULT_HIGH

    // RELAYs
    #define RELAY_PROVIDER              RELAY_PROVIDER_RELAY

    #define RELAY1_PIN                  12
    #define RELAY1_TYPE                 RELAY_TYPE_NORMAL
    #define RELAY1_BTN                  1
    #define RELAY1_CHANNEL              1

    // LEDs
    #define STATUS_LED_PIN              13
    #define STATUS_LED_PIN_INVERSE      1

    // CHANNELs
    #define CHANNEL1_NAME               "light"

#elif defined(ITEAD_SONOFF_S20)

    // Info
    #define MANUFACTURER                "ITEAD"
    #define DEVICE                      "SONOFF_S20"
    #define HARWARE_VERSION             "0.0.1"
    #define FASTYBIRD_MAX_CHANNELS      1

    // BUTTONs
    #define BUTTON_SUPPORT              1

    #define BUTTON1_PIN                 0
    #define BUTTON1_MODE                BUTTON_PUSHBUTTON | BUTTON_DEFAULT_HIGH

    // RELAYs
    #define RELAY_PROVIDER              RELAY_PROVIDER_RELAY

    #define RELAY1_PIN                  12
    #define RELAY1_TYPE                 RELAY_TYPE_NORMAL
    #define RELAY1_BTN                  1
    #define RELAY1_CHANNEL              1

    // LEDs
    #define STATUS_LED_PIN              13
    #define STATUS_LED_PIN_INVERSE      1

    // CHANNELs
    #define CHANNEL1_NAME               "socket"

#elif defined(ITEAD_SONOFF_POW)

    // Info
    #define MANUFACTURER                "ITEAD"
    #define DEVICE                      "SONOFF_POW"
    #define HARWARE_VERSION             "0.0.1"
    #define FASTYBIRD_MAX_CHANNELS      1

    // BUTTONs
    #define BUTTON_SUPPORT              1

    #define BUTTON1_PIN                 0
    #define BUTTON1_MODE                BUTTON_PUSHBUTTON | BUTTON_DEFAULT_HIGH

    // RELAYs
    #define RELAY_PROVIDER              RELAY_PROVIDER_RELAY

    #define RELAY1_PIN                  12
    #define RELAY1_TYPE                 RELAY_TYPE_NORMAL
    #define RELAY1_BTN                  1
    #define RELAY1_CHANNEL              1

    // LEDs
    #define STATUS_LED_PIN              15
    #define STATUS_LED_PIN_INVERSE      0

    // SENSORs
    #define SENSOR_POWER_CHECK_STATUS   1
    #define SENSOR1_CHANNEL             1

    // HLW8012
    #ifndef HLW8012_SUPPORT
        #define HLW8012_SUPPORT         1
    #endif

    #define HLW8012_SEL_PIN             5
    #define HLW8012_CF1_PIN             13
    #define HLW8012_CF_PIN              14

    // CHANNELs
    #define CHANNEL1_NAME               "energy"

#elif defined(ITEAD_SONOFF_POW_R2)

    // Info
    #define MANUFACTURER                "ITEAD"
    #define DEVICE                      "SONOFF_POW_R2"
    #define HARWARE_VERSION             "0.0.2"
    #define FASTYBIRD_MAX_CHANNELS      1

    // BUTTONs
    #define BUTTON_SUPPORT              1

    #define BUTTON1_PIN                 0
    #define BUTTON1_MODE                BUTTON_PUSHBUTTON | BUTTON_DEFAULT_HIGH

    // RELAYs
    #define RELAY_PROVIDER              RELAY_PROVIDER_RELAY

    #define RELAY1_PIN                  12
    #define RELAY1_TYPE                 RELAY_TYPE_NORMAL
    #define RELAY1_BTN                  1
    #define RELAY1_CHANNEL              1

    // LEDs
    #define STATUS_LED_PIN              13
    #define STATUS_LED_PIN_INVERSE      1

    // SENSORs
    #define SENSOR1_CHANNEL             1
    #define SENSOR_POWER_CHECK_STATUS   1

    // CSE7766
    #ifndef CSE7766_SUPPORT
        #define CSE7766_SUPPORT         1
    #endif

    #define CSE7766_PIN                 1

    // Disable UART noise
    #define DEBUG_SERIAL_SUPPORT        0

    // CHANNELs
    #define CHANNEL1_NAME               "energy"

#elif defined(ITEAD_SONOFF_SC)

    // Info
    #define MANUFACTURER                "ITEAD"
    #define DEVICE                      "SONOFF_SC"
    #define HARWARE_VERSION             "0.0.1"
    #define FASTYBIRD_MAX_CHANNELS      1

    // BUTTONs
    #define BUTTON_SUPPORT              1

    #define BUTTON1_PIN				    0
    #define BUTTON1_MODE			    BUTTON_PUSHBUTTON | BUTTON_DEFAULT_HIGH

    #define SERIAL_BAUDRATE             19200

    // LEDs
    #define STATUS_LED_PIN  		    13
    #define STATUS_LED_PIN_INVERSE	    1

    // SENSORs
    #define SENSOR1_CHANNEL             1

  	// SONOFF_SC
    #ifndef ITEAD_SONOFF_SC_SUPPORT
        #define ITEAD_SONOFF_SC_SUPPORT 1
    #endif

    // Disable UART noise
    #define DEBUG_SERIAL_SUPPORT        0
    #define DEBUG_SUPPORT               0

    // CHANNELs
    #define CHANNEL1_NAME               "environment"

#elif defined(ITEAD_SONOFF_RFBRIDGE)

    // Info
    #define MANUFACTURER                "ITEAD"
    #define DEVICE                      "SONOFF_RFBRIDGE"
    #define HARWARE_VERSION             "0.0.1"
    #define FASTYBIRD_MAX_CHANNELS      8

    // Number of virtual switches
    #ifndef DUMMY_RELAY_COUNT
        #define DUMMY_RELAY_COUNT       8
    #endif

    // BUTTONs
    #define BUTTON1_PIN                 0
    #define BUTTON1_MODE                BUTTON_PUSHBUTTON | BUTTON_DEFAULT_HIGH

    // LEDs
    #define LED1_PIN                    13
    #define LED1_PIN_INVERSE            1

    #define RF_SUPPORT                  1

    // Only used when RFB_DIRECT=1
    #define RFB_RX_PIN                  4
    #define RFB_TX_PIN                  5

    // When using un-modified harware, firmware communicates with the secondary
    // MCU EFM8BB1 via UART at 19200 bps so we need to change the speed of
    // the port and remove UART noise on serial line
    #if not RFB_DIRECT
        #define SERIAL_BAUDRATE         19200
        #define DEBUG_SERIAL_SUPPORT    0
    #endif

#endif
