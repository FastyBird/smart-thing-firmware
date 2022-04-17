/*

SUPPORTED HARDWARE DEFINITIONS

Copyright (C) 2018 FastyBird s.r.o. <code@fastybird.com>

*/

#pragma once

// -----------------------------------------------------------------------------
// FastyBird Core
// -----------------------------------------------------------------------------

#if defined(FASTYBIRD_CORE)

    // This is a special firmware configuration targeted to generate
    // a light-weight binary image meant to be able to do two-step-updates

    // Info
    #define HARDWARE_MANUFACTURER           "ESPRESSIF"
    #define HARDWARE_MODEL                  "FASTYBIRD_CORE"
    #define HARDWARE_VERSION                "0.0.1"

// -----------------------------------------------------------------------------
// FastyBird devices
// -----------------------------------------------------------------------------

#elif defined(FASTYBIRD_3CH_POWER_STRIP_R1)

    // DEVICE
    #define HARDWARE_MANUFACTURER           "FASTYBIRD"
    #define HARDWARE_MODEL                  "FASTYBIRD_3CH_POWER_STRIP_R1"
    #define HARDWARE_VERSION                "0.0.1"

    // SYSTEM
    #define SYSTEM_RESET_BTN_INDEX          3

    // SETTINGS
    #define SETTINGS_FACTORY_BTN_INDEX      3

    // WIFI
    #define WIFI_SUPPORT                    1

    #define WIFI_AP_BTN_INDEX               3

    // WEB
    #define WEB_SUPPORT                     1

    // WS
    #define WS_SUPPORT                      1

    // MQTT
    #define MQTT_SUPPORT                    1

    // BUTTON
    #define BUTTON_SUPPORT                  1

    #define BUTTON1_PIN                     0
    #define BUTTON1_MODE                    BUTTON_PUSHBUTTON | BUTTON_DEFAULT_HIGH

    #define BUTTON2_PIN                     9
    #define BUTTON2_MODE                    BUTTON_PUSHBUTTON | BUTTON_DEFAULT_HIGH

    #define BUTTON3_PIN                     10
    #define BUTTON3_MODE                    BUTTON_PUSHBUTTON | BUTTON_DEFAULT_HIGH

    #define BUTTON4_PIN                     14
    #define BUTTON4_MODE                    BUTTON_PUSHBUTTON | BUTTON_DEFAULT_HIGH

    // RELAY
    #define RELAY_PROVIDER                  RELAY_PROVIDER_RELAY

    #define RELAY1_PIN                      12
    #define RELAY1_TYPE                     RELAY_TYPE_NORMAL
    #define RELAY1_BTN_INDEX                0

    #define RELAY2_PIN                      5
    #define RELAY2_TYPE                     RELAY_TYPE_NORMAL
    #define RELAY2_BTN_INDEX                1

    #define RELAY3_PIN                      4
    #define RELAY3_TYPE                     RELAY_TYPE_NORMAL
    #define RELAY3_BTN_INDEX                2

    // LED
    #define LED_SUPPORT                     1

    #define STATUS_LED_PIN                  13
    #define STATUS_LED_PIN_INVERSE          1

    // FASTYBIRD
    #define FASTYBIRD_SUPPORT               1

    #define FASTYBIRD_MAX_CHANNELS          3

    #define FASTYBIRD_BUTTON1_CHANNEL_INDEX 0
    #define FASTYBIRD_BUTTON2_CHANNEL_INDEX 1
    #define FASTYBIRD_BUTTON3_CHANNEL_INDEX 2

    #define FASTYBIRD_RELAY1_CHANNEL_INDEX  0
    #define FASTYBIRD_RELAY2_CHANNEL_INDEX  1
    #define FASTYBIRD_RELAY3_CHANNEL_INDEX  2

// -----------------------------------------------------------------------------
// Itead Studio devices
// -----------------------------------------------------------------------------

#elif defined(ITEAD_SONOFF_BASIC)

    // DEVICE
    #define HARDWARE_MANUFACTURER           "ITEAD"
    #define HARDWARE_MODEL                  "SONOFF_BASIC"
    #define HARDWARE_VERSION                "0.0.1"

    // SYSTEM
    #define SYSTEM_RESET_BTN_INDEX          0

    // SETTINGS
    #define SETTINGS_FACTORY_BTN_INDEX      0

    // WIFI
    #define WIFI_SUPPORT                    1

    #define WIFI_AP_BTN_INDEX               0

    // WEB
    #define WEB_SUPPORT                     1

    // WS
    #define WS_SUPPORT                      1

    // MQTT
    #define MQTT_SUPPORT                    1

    // BUTTON
    #define BUTTON_SUPPORT                  1

    #define BUTTON1_PIN                     0
    #define BUTTON1_MODE                    BUTTON_PUSHBUTTON | BUTTON_DEFAULT_HIGH

    // RELAY
    #define RELAY_PROVIDER                  RELAY_PROVIDER_RELAY

    #define RELAY1_PIN                      12
    #define RELAY1_TYPE                     RELAY_TYPE_NORMAL
    #define RELAY1_BTN_INDEX                0

    // LED
    #define LED_SUPPORT                     1

    #define STATUS_LED_PIN                  13
    #define STATUS_LED_PIN_INVERSE          1

    // FASTYBIRD
    #define FASTYBIRD_SUPPORT               1

    #define FASTYBIRD_MAX_CHANNELS          1

    #define FASTYBIRD_RELAY1_CHANNEL_INDEX  0

#elif defined(ITEAD_SONOFF_MINI)

    // DEVICE
    #define HARDWARE_MANUFACTURER           "ITEAD"
    #define HARDWARE_MODEL                  "SONOFF_MINI"
    #define HARDWARE_VERSION                "0.0.1"

    // SYSTEM
    #define SYSTEM_RESET_BTN_INDEX          0

    // SETTINGS
    #define SETTINGS_FACTORY_BTN_INDEX      0

    // WIFI
    #define WIFI_SUPPORT                    1

    #define WIFI_AP_BTN_INDEX               0

    // WEB
    #define WEB_SUPPORT                     1

    // WS
    #define WS_SUPPORT                      1

    // MQTT
    #define MQTT_SUPPORT                    1

    // BUTTON
    #define BUTTON_SUPPORT                  1

    #define BUTTON1_PIN                     0
    #define BUTTON1_MODE                    BUTTON_PUSHBUTTON | BUTTON_DEFAULT_HIGH
    
    #define BUTTON2_PIN                     4
    #define BUTTON2_MODE                    BUTTON_SWITCH | BUTTON_SET_PULLUP | BUTTON_DEFAULT_HIGH

    // RELAY
    #define RELAY_PROVIDER                  RELAY_PROVIDER_RELAY

    #define RELAY1_PIN                      12
    #define RELAY1_TYPE                     RELAY_TYPE_NORMAL
    #define RELAY1_BTN_INDEX                0

    // LED
    #define LED_SUPPORT                     1

    #define LED1_PIN                        13
    #define LED1_PIN_INVERSE                1

    // FASTYBIRD
    #define FASTYBIRD_SUPPORT               1

    #define FASTYBIRD_MAX_CHANNELS          1

    #define FASTYBIRD_RELAY1_CHANNEL_INDEX  0

#elif defined(ITEAD_SONOFF_DUAL_R2)

    // DEVICE
    #define HARDWARE_MANUFACTURER           "ITEAD"
    #define HARDWARE_MODEL                  "SONOFF_DUAL_R2"
    #define HARDWARE_VERSION                "0.0.2"

    // SYSTEM
    #define SYSTEM_RESET_BTN_INDEX          0

    // SETTINGS
    #define SETTINGS_FACTORY_BTN_INDEX      0

    // WIFI
    #define WIFI_SUPPORT                    1

    #define WIFI_AP_BTN_INDEX               0

    // WEB
    #define WEB_SUPPORT                     1

    // WS
    #define WS_SUPPORT                      1

    // MQTT
    #define MQTT_SUPPORT                    1

    // BUTTON
    #define BUTTON_SUPPORT                  1

    #define BUTTON1_PIN                     10
    #define BUTTON1_MODE                    BUTTON_PUSHBUTTON | BUTTON_DEFAULT_HIGH

    // RELAY
    #define RELAY_PROVIDER                  RELAY_PROVIDER_RELAY

    #define RELAY1_PIN                      12
    #define RELAY1_TYPE                     RELAY_TYPE_NORMAL
    #define RELAY1_BTN_INDEX                0

    #define RELAY2_PIN                      5
    #define RELAY2_TYPE                     RELAY_TYPE_NORMAL
    #define RELAY2_BTN_INDEX                0
    #define RELAY2_BTN_EVENT                BUTTON_EVENT_DBL_CLICK

    // LED
    #define LED_SUPPORT                     1

    #define STATUS_LED_PIN                  13
    #define STATUS_LED_PIN_INVERSE          1

    // FASTYBIRD
    #define FASTYBIRD_SUPPORT               1

    #define FASTYBIRD_MAX_CHANNELS          2

    #define FASTYBIRD_RELAY1_CHANNEL_INDEX  0
    #define FASTYBIRD_RELAY1_CHANNEL_INDEX  1

#elif defined(ITEAD_SONOFF_SLAMPHER)

    // DEVICE
    #define HARDWARE_MANUFACTURER           "ITEAD"
    #define HARDWARE_MODEL                  "SONOFF_SLAMPHER"
    #define HARDWARE_VERSION                "0.0.1"

    // SYSTEM
    #define SYSTEM_RESET_BTN_INDEX          0

    // SETTINGS
    #define SETTINGS_FACTORY_BTN_INDEX      0

    // WIFI
    #define WIFI_SUPPORT                    1

    #define WIFI_AP_BTN_INDEX               0

    // WEB
    #define WEB_SUPPORT                     1

    // WS
    #define WS_SUPPORT                      1

    // MQTT
    #define MQTT_SUPPORT                    1

    // BUTTON
    #define BUTTON_SUPPORT                  1

    #define BUTTON1_PIN                     0
    #define BUTTON1_MODE                    BUTTON_PUSHBUTTON | BUTTON_DEFAULT_HIGH

    // RELAY
    #define RELAY_PROVIDER                  RELAY_PROVIDER_RELAY

    #define RELAY1_PIN                      12
    #define RELAY1_TYPE                     RELAY_TYPE_NORMAL
    #define RELAY1_BTN_INDEX                0

    // LED
    #define LED_SUPPORT                     1

    #define STATUS_LED_PIN                  13
    #define STATUS_LED_PIN_INVERSE          1

    // FASTYBIRD
    #define FASTYBIRD_SUPPORT               1

    #define FASTYBIRD_MAX_CHANNELS          1

    #define FASTYBIRD_RELAY1_CHANNEL_INDEX  0

#elif defined(ITEAD_SONOFF_S20)

    // DEVICE
    #define HARDWARE_MANUFACTURER           "ITEAD"
    #define HARDWARE_MODEL                  "SONOFF_S20"
    #define HARDWARE_VERSION                "0.0.1"

    // SYSTEM
    #define SYSTEM_RESET_BTN_INDEX          0

    // SETTINGS
    #define SETTINGS_FACTORY_BTN_INDEX      0

    // WIFI
    #define WIFI_SUPPORT                    1

    #define WIFI_AP_BTN_INDEX               0

    // WEB
    #define WEB_SUPPORT                     1

    // WS
    #define WS_SUPPORT                      1

    // MQTT
    #define MQTT_SUPPORT                    1

    // BUTTON
    #define BUTTON_SUPPORT                  1

    #define BUTTON1_PIN                     0
    #define BUTTON1_MODE                    BUTTON_PUSHBUTTON | BUTTON_DEFAULT_HIGH

    // RELAY
    #define RELAY_PROVIDER                  RELAY_PROVIDER_RELAY

    #define RELAY1_PIN                      12
    #define RELAY1_TYPE                     RELAY_TYPE_NORMAL
    #define RELAY1_BTN_INDEX                0

    // LED
    #define LED_SUPPORT                     1

    #define STATUS_LED_PIN                  13
    #define STATUS_LED_PIN_INVERSE          1

    // FASTYBIRD
    #define FASTYBIRD_SUPPORT               1

    #define FASTYBIRD_MAX_CHANNELS          1

    #define FASTYBIRD_RELAY1_CHANNEL_INDEX  0

#elif defined(ITEAD_SONOFF_POW)

    // DEVICE
    #define HARDWARE_MANUFACTURER           "ITEAD"
    #define HARDWARE_MODEL                  "SONOFF_POW"
    #define HARDWARE_VERSION                "0.0.1"

    // SYSTEM
    #define SYSTEM_RESET_BTN_INDEX          0

    // SETTINGS
    #define SETTINGS_FACTORY_BTN_INDEX      0

    // WIFI
    #define WIFI_SUPPORT                    1

    #define WIFI_AP_BTN_INDEX               0

    // WEB
    #define WEB_SUPPORT                     1

    // WS
    #define WS_SUPPORT                      1

    // MQTT
    #define MQTT_SUPPORT                    1

    // BUTTON
    #define BUTTON_SUPPORT                  1

    #define BUTTON1_PIN                     0
    #define BUTTON1_MODE                    BUTTON_PUSHBUTTON | BUTTON_DEFAULT_HIGH

    // RELAY
    #define RELAY_PROVIDER                  RELAY_PROVIDER_RELAY

    #define RELAY1_PIN                      12
    #define RELAY1_TYPE                     RELAY_TYPE_NORMAL
    #define RELAY1_BTN_INDEX                0

    // LED
    #define LED_SUPPORT                     1

    #define STATUS_LED_PIN                  15
    #define STATUS_LED_PIN_INVERSE          0

    // SENSORs
    #define HLW8012_SUPPORT                 1
    #define HLW8012_SEL_PIN                 5
    #define HLW8012_CF1_PIN                 13
    #define HLW8012_CF_PIN                  14

    // FASTYBIRD
    #define FASTYBIRD_SUPPORT               1

    #define FASTYBIRD_MAX_CHANNELS          1

    #define FASTYBIRD_RELAY1_CHANNEL_INDEX  0
    #define FASTYBIRD_SENSOR1_CHANNEL_INDEX 0

#elif defined(ITEAD_SONOFF_POW_R2)

    // DEVICE
    #define HARDWARE_MANUFACTURER           "ITEAD"
    #define HARDWARE_MODEL                  "SONOFF_POW_R2"
    #define HARDWARE_VERSION                "0.0.2"

    // SYSTEM
    #define SYSTEM_RESET_BTN_INDEX          0

    // SETTINGS
    #define SETTINGS_FACTORY_BTN_INDEX      0

    // WIFI
    #define WIFI_SUPPORT                    1

    #define WIFI_AP_BTN_INDEX               0

    // WEB
    #define WEB_SUPPORT                     1

    // WS
    #define WS_SUPPORT                      1

    // MQTT
    #define MQTT_SUPPORT                    1

    // BUTTON
    #define BUTTON_SUPPORT                  1

    #define BUTTON1_PIN                     0
    #define BUTTON1_MODE                    BUTTON_PUSHBUTTON | BUTTON_DEFAULT_HIGH

    // RELAY
    #define RELAY_PROVIDER                  RELAY_PROVIDER_RELAY

    #define RELAY1_PIN                      12
    #define RELAY1_TYPE                     RELAY_TYPE_NORMAL
    #define RELAY1_BTN_INDEX                0

    // LED
    #define LED_SUPPORT                     1

    #define STATUS_LED_PIN                  13
    #define STATUS_LED_PIN_INVERSE          1

    // SENSOR
    #define CSE7766_SUPPORT                 1
    #define CSE7766_PIN                     1

    // FASTYBIRD
    #define FASTYBIRD_SUPPORT               1

    #define FASTYBIRD_MAX_CHANNELS          1

    #define FASTYBIRD_RELAY1_CHANNEL_INDEX  0
    #define FASTYBIRD_SENSOR1_CHANNEL_INDEX 0

    // Disable UART noise
    #define DEBUG_SERIAL_SUPPORT            0

#elif defined(ITEAD_SONOFF_SC)

    // DEVICE
    #define HARDWARE_MANUFACTURER           "ITEAD"
    #define HARDWARE_MODEL                  "SONOFF_SC"
    #define HARDWARE_VERSION                "0.0.1"

    // SYSTEM
    #define SYSTEM_RESET_BTN_INDEX          0

    // SETTINGS
    #define SETTINGS_FACTORY_BTN_INDEX      0

    // WIFI
    #define WIFI_SUPPORT                    1

    #define WIFI_AP_BTN_INDEX               0

    // WEB
    #define WEB_SUPPORT                     1

    // WS
    #define WS_SUPPORT                      1

    // MQTT
    #define MQTT_SUPPORT                    1

    // BUTTON
    #define BUTTON_SUPPORT                  1

    #define BUTTON1_PIN				        0
    #define BUTTON1_MODE			        BUTTON_PUSHBUTTON | BUTTON_DEFAULT_HIGH

    #define SERIAL_BAUDRATE                 19200

    // LED
    #define LED_SUPPORT                     1

    #define STATUS_LED_PIN  		        13
    #define STATUS_LED_PIN_INVERSE	        1

    // SENSOR
    #define FASTYBIRD_SENSOR1_CHANNEL_INDEX 0

    #define ITEAD_SONOFF_SC_SUPPORT         1

    // FASTYBIRD
    #define FASTYBIRD_SUPPORT               1

    #define FASTYBIRD_MAX_CHANNELS          1

    #define FASTYBIRD_SENSOR1_CHANNEL_INDEX 0

    // Disable UART noise
    #define DEBUG_SERIAL_SUPPORT            0

#elif defined(ITEAD_SONOFF_RFBRIDGE)

    // DEVICE
    #define HARDWARE_MANUFACTURER           "ITEAD"
    #define HARDWARE_MODEL                  "SONOFF_RFBRIDGE"
    #define HARDWARE_VERSION                "0.0.1"

    // SYSTEM
    #define SYSTEM_RESET_BTN_INDEX          0

    // SETTINGS
    #define SETTINGS_FACTORY_BTN_INDEX      0

    // WIFI
    #define WIFI_SUPPORT                    1

    #define WIFI_AP_BTN_INDEX               0

    // WEB
    #define WEB_SUPPORT                     1

    // WS
    #define WS_SUPPORT                      1

    // MQTT
    #define MQTT_SUPPORT                    1

    // BUTTON
    #define BUTTON_SUPPORT                  1

    #define BUTTON1_PIN                     0
    #define BUTTON1_MODE                    BUTTON_PUSHBUTTON | BUTTON_DEFAULT_HIGH

    // RELAY
    #ifndef DUMMY_RELAY_COUNT
        #define DUMMY_RELAY_COUNT           8   // Number of virtual switches
    #endif

    // LED
    #define LED_SUPPORT                     1

    #define LED1_PIN                        13
    #define LED1_PIN_INVERSE                1

    // RF
    #define RF_SUPPORT                      1

    // Only used when RFB_DIRECT=1
    #define RFB_RX_PIN                      4
    #define RFB_TX_PIN                      5

    // When using un-modified harware, firmware communicates with the secondary
    // MCU EFM8BB1 via UART at 19200 bps so we need to change the speed of
    // the port and remove UART noise on serial line
    #if not RFB_DIRECT
        #define SERIAL_BAUDRATE             19200
        #define DEBUG_SERIAL_SUPPORT        0
    #endif

#endif
