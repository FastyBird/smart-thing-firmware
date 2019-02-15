/*

FIRMWARE GENERAL SETTINGS

Copyright (C) 2018 FastyBird Ltd. <info@fastybird.com>

*/

//------------------------------------------------------------------------------
// GENERAL
//------------------------------------------------------------------------------

#define THING_NAME                      MANUFACTURER "_" THING      // Concatenate both to get a unique thing name

#ifndef ADMIN_PASS
#define ADMIN_PASS                      "fibonacci"                 // Default password (WEB, OTA, WIFI SoftAP)
#endif

#ifndef USE_PASSWORD
#define USE_PASSWORD                    1                           // Insecurity caution! Disabling this will disable password querying completely.
#endif

#ifndef LOOP_DELAY_TIME
#define LOOP_DELAY_TIME                 10                          // Delay for this millis in the main loop [0-250]
#endif

//------------------------------------------------------------------------------
// FASTYBIRD
//------------------------------------------------------------------------------

#ifndef FASTYBIRD_SUPPORT
#define FASTYBIRD_SUPPORT               1                           // Enable FastyBird cloud connection
#endif

#ifndef FASTYBIRD_GATEWAY_SUPPORT
#define FASTYBIRD_GATEWAY_SUPPORT       0                           // Gateway support dissabled by default
#endif

#ifndef FASTYBIRD_MQTT_BASE_TOPIC
#define FASTYBIRD_MQTT_BASE_TOPIC       "/v2/{cloudThingId}"
#endif

#ifndef FASTYBIRD_MQTT_NODE_BASE_TOPIC
#define FASTYBIRD_MQTT_NODE_BASE_TOPIC  "/v2/{cloudThingId}/child/{nodeId}"
#endif

//------------------------------------------------------------------------------
// NETWORK
//------------------------------------------------------------------------------

#ifndef NETWORK_ASYNC_TCP_SSL_ENABLED
#define NETWORK_ASYNC_TCP_SSL_ENABLED   0                           // Enable SSL
#endif

//------------------------------------------------------------------------------
// DEBUG
//------------------------------------------------------------------------------

#ifndef DEBUG_SUPPORT
#define DEBUG_SUPPORT                   1                           // Enable serial debug log
#endif

#ifndef DEBUG_SERIAL_SUPPORT
#define DEBUG_SERIAL_SUPPORT            1                           // Enable serial debug log
#endif

#ifndef DEBUG_PORT
#define DEBUG_PORT                      Serial                      // Default debugging port
#endif

#ifndef DEBUG_WEB_SUPPORT
#define DEBUG_WEB_SUPPORT               1                           // Enable web debug log (will only work if WEB_SUPPORT is also 1)
#endif

#ifndef DEBUG_MQTT_SUPPORT
#define DEBUG_MQTT_SUPPORT              1                           // Enable mqtt debug log
#endif

#ifndef SERIAL_BAUDRATE
#define SERIAL_BAUDRATE                 115200                      // Default baudrate
#endif

#ifndef DEBUG_ADD_TIMESTAMP
#define DEBUG_ADD_TIMESTAMP             1                           // Add timestamp to debug messages
                                                                    // (in millis overflowing every 1000 seconds)
#endif

#define TERMINAL_BUFFER_SIZE            128                         // Max size for commands commands

// Second serial port (used for RX)

#ifndef SERIAL_RX_ENABLED
#define SERIAL_RX_ENABLED               0                           // Secondary serial port for RX
#endif

#ifndef SERIAL_RX_PORT
#define SERIAL_RX_PORT                  Serial                      // This setting is usually defined
                                                                    // in the hardware.h file for those
                                                                    // boards that require it
#endif

#ifndef SERIAL_RX_BAUDRATE
#define SERIAL_RX_BAUDRATE              115200                      // Default baudrate
#endif

//------------------------------------------------------------------------------
// STABILTY CHECK
//------------------------------------------------------------------------------

#ifndef STABILTY_CHECK_ENABLED
#define STABILTY_CHECK_ENABLED          1                           // Enable crash check by default
#endif

#ifndef STABILTY_CHECK_TIME
#define STABILTY_CHECK_TIME             60000                       // The system is considered stable after these many millis
#endif

#ifndef STABILTY_CHECK_MAX
#define STABILTY_CHECK_MAX              5                           // After this many crashes on boot
                                                                    // the system is flagged as unstable
#endif

//------------------------------------------------------------------------------
// EEPROM
//------------------------------------------------------------------------------

#define EEPROM_SIZE                     SPI_FLASH_SEC_SIZE          // EEPROM size in bytes (1 sector = 4096 bytes)

//#define EEPROM_RORATE_SECTORS         2                           // Number of sectors to use for EEPROM rotation
                                                                    // If not defined the firmware will use a number based
                                                                    // on the number of available sectors

#define EEPROM_CUSTOM_RESET             0                           // Address for the reset reason (1 byte)
#define EEPROM_CRASH_COUNTER            1                           // Address for the crash counter (1 byte)
#define EEPROM_MESSAGE_ID               2                           // Address for the MQTT message id (4 bytes)
#define EEPROM_ROTATE_DATA              6                           // Reserved for the EEPROM_ROTATE library (3 bytes)
#define EEPROM_RELAY_STATUS             9                           // Address for the relay status (1 byte)
#define EEPROM_ENERGY_COUNT             10                          // Address for the energy counter (4 bytes)
#define EEPROM_DATA_END                 14                          // End of custom EEPROM data block

//------------------------------------------------------------------------------
// HEARTBEAT
//------------------------------------------------------------------------------

#define HEARTBEAT_NONE                  0                           // Never send heartbeat
#define HEARTBEAT_ONCE                  1                           // Send it only once upon MQTT connection
#define HEARTBEAT_REPEAT                2                           // Send it upon MQTT connection and every HEARTBEAT_INTERVAL

#ifndef HEARTBEAT_MODE
#define HEARTBEAT_MODE                  HEARTBEAT_REPEAT
#endif

#ifndef HEARTBEAT_INTERVAL
#define HEARTBEAT_INTERVAL              300000                      // Interval between heartbeat messages (in ms)
#endif

#define UPTIME_OVERFLOW                 4294967295                  // Uptime overflow value

//------------------------------------------------------------------------------
// Load average
//------------------------------------------------------------------------------

#ifndef LOADAVG_INTERVAL
#define LOADAVG_INTERVAL                30000                       // Interval between calculating load average (in ms)
#endif

// -----------------------------------------------------------------------------
// SETTINGS
// -----------------------------------------------------------------------------

#ifndef SETTINGS_AUTOSAVE
#define SETTINGS_AUTOSAVE               1                           // Autosave settings or force manual commit
#endif

#define SETTINGS_MAX_LIST_COUNT         10                          // Maximum index for settings lists

// -----------------------------------------------------------------------------
// WIFI
// -----------------------------------------------------------------------------

#ifndef WIFI_SUPPORT
#define WIFI_SUPPORT                    1                           // Enable wifi support
#endif

#ifndef WIFI_CONNECT_TIMEOUT
#define WIFI_CONNECT_TIMEOUT            60000                       // Connecting timeout for WIFI in ms
#endif

#ifndef WIFI_RECONNECT_INTERVAL
#define WIFI_RECONNECT_INTERVAL         180000                      // If could not connect to WIFI, retry after this time in ms
#endif

#ifndef WIFI_MAX_NETWORKS
#define WIFI_MAX_NETWORKS               5                           // Max number of WIFI connection configurations
#endif

#ifndef WIFI_FALLBACK_APMODE
#define WIFI_FALLBACK_APMODE            1                           // Fallback to AP mode if no STA connection
#endif

#ifndef WIFI_SLEEP_MODE
#define WIFI_SLEEP_MODE                 WIFI_NONE_SLEEP             // WIFI_NONE_SLEEP, WIFI_LIGHT_SLEEP or WIFI_MODEM_SLEEP
#endif

#ifndef WIFI_SCAN_NETWORKS
#define WIFI_SCAN_NETWORKS              1                           // Perform a network scan before connecting
#endif

// Optional hardcoded configuration (up to 2 networks)
#ifndef WIFI1_SSID
#define WIFI1_SSID                      ""
#endif

#ifndef WIFI1_PASS
#define WIFI1_PASS                      ""
#endif

#ifndef WIFI1_IP
#define WIFI1_IP                        ""
#endif

#ifndef WIFI1_GW
#define WIFI1_GW                        ""
#endif

#ifndef WIFI1_MASK
#define WIFI1_MASK                      ""
#endif

#ifndef WIFI1_DNS
#define WIFI1_DNS                       ""
#endif

#ifndef WIFI2_SSID
#define WIFI2_SSID                      ""
#endif

#ifndef WIFI2_PASS
#define WIFI2_PASS                      ""
#endif

#ifndef WIFI2_IP
#define WIFI2_IP                        ""
#endif

#ifndef WIFI2_GW
#define WIFI2_GW                        ""
#endif

#ifndef WIFI2_MASK
#define WIFI2_MASK                      ""
#endif

#ifndef WIFI2_DNS
#define WIFI2_DNS                       ""
#endif

#ifndef WIFI_RSSI_1M
#define WIFI_RSSI_1M                    -30                         // Calibrate it with your router reading the RSSI at 1m
#endif

#ifndef WIFI_PROPAGATION_CONST
#define WIFI_PROPAGATION_CONST          4                           // This is typically something between 2.7 to 4.3 (free space is 2)
#endif

// -----------------------------------------------------------------------------
// SPIFFS
// -----------------------------------------------------------------------------

#ifndef SPIFFS_SUPPORT
#define SPIFFS_SUPPORT                  0                           // Do not add support for SPIFFS by default
#endif

// -----------------------------------------------------------------------------
// OTA
// -----------------------------------------------------------------------------

#ifndef OTA_PORT
#define OTA_PORT                        8266                        // OTA port
#endif

#define OTA_GITHUB_FP                   "D7:9F:07:61:10:B3:92:93:E3:49:AC:89:84:5B:03:80:C1:9E:2F:8B"

// -----------------------------------------------------------------------------
// NOFUSS
// -----------------------------------------------------------------------------

#ifndef NOFUSS_SUPPORT
#define NOFUSS_SUPPORT                  0                           // Do not enable support for NoFuss by default (12.65Kb)
#endif

#ifndef NOFUSS_ENABLED
#define NOFUSS_ENABLED                  0                           // Do not perform NoFUSS updates by default
#endif

#ifndef NOFUSS_SERVER
#define NOFUSS_SERVER                   ""                          // Default NoFuss Server
#endif

#ifndef NOFUSS_INTERVAL
#define NOFUSS_INTERVAL                 3600000                     // Check for updates every hour
#endif

// -----------------------------------------------------------------------------
// WEB
// -----------------------------------------------------------------------------

#ifndef WEB_SUPPORT
#define WEB_SUPPORT                     1                           // Enable web support
#endif

// This is not working at the moment!!
// Requires NETWORK_ASYNC_TCP_SSL_ENABLED to 1 and ESP8266 Arduino Core 2.4.0
#ifndef WEB_SSL_ENABLED
#define WEB_SSL_ENABLED                 0                           // Use HTTPS web interface
#endif

#ifndef WEB_ROOT_USERNAME
#define WEB_ROOT_USERNAME               "root"                      // HTTP root username
#endif

#ifndef WEB_USERNAME
#define WEB_USERNAME                    "admin"                     // HTTP username
#endif

#ifndef WEB_PASSWORD
#define WEB_PASSWORD                    ""                          // HTTP password
#endif

#ifndef WEB_PORT
#define WEB_PORT                        80                          // HTTP port
#endif

// -----------------------------------------------------------------------------
// WEBSOCKETS
// -----------------------------------------------------------------------------

#ifndef WS_SUPPORT
#define WS_SUPPORT                      1                           // Enable WS support
#endif

#ifndef WS_BUFFER_SIZE
#define WS_BUFFER_SIZE                  5                           // Max number of secured websocket connections
#endif

#ifndef WS_TIMEOUT
#define WS_TIMEOUT                      1800000                     // Timeout for secured websocket
#endif

#ifndef WS_UPDATE_INTERVAL
#define WS_UPDATE_INTERVAL              30000                       // Update clients every 30 seconds
#endif

// -----------------------------------------------------------------------------
// MQTT
// -----------------------------------------------------------------------------

#ifndef MQTT_SUPPORT
#define MQTT_SUPPORT                    1                           // MQTT support (22.38Kb async, 12.48Kb sync)
#endif

#define MQTT_BASE_TOPIC                 ""

#ifndef MQTT_USE_ASYNC
#define MQTT_USE_ASYNC                  1                           // Use AysncMQTTClient (1) or PubSubClient (0)
#endif

// MQTT OVER SSL
// Using MQTT over SSL works pretty well but generates problems with the web interface.
// It could be a good idea to use it in conjuntion with WEB_SUPPORT=0.
// Requires NETWORK_ASYNC_TCP_SSL_ENABLED to 1 and ESP8266 Arduino Core 2.4.0.
//
// You can use SSL with MQTT_USE_ASYNC=1 (AsyncMqttClient library)
// but you might experience hiccups on the web interface, so my recommendation is:
// WEB_SUPPORT=0
//
// If you use SSL with MQTT_USE_ASYNC=0 (PubSubClient library)
// you will have to disable all the modules that use ESPAsyncTCP, that is:
// ALEXA_SUPPORT=0 and WEB_SUPPORT=0
//
// You will need the fingerprint for your MQTT server, example for CloudMQTT:
// $ echo -n | openssl s_client -connect m11.cloudmqtt.com:24055 > cloudmqtt.pem
// $ openssl x509 -noout -in cloudmqtt.pem -fingerprint -sha1

#ifndef MQTT_SSL_ENABLED
#define MQTT_SSL_ENABLED                0                           // By default MQTT over SSL will not be enabled
#endif

#ifndef MQTT_SSL_FINGERPRINT
#define MQTT_SSL_FINGERPRINT            ""                          // SSL fingerprint of the server
#endif

#ifndef MQTT_ENABLED
#define MQTT_ENABLED                    0                           // Do not enable MQTT connection by default
#endif

#ifndef MQTT_AUTOCONNECT
#define MQTT_AUTOCONNECT                1                           // If enabled will perform an autodiscover and
#endif

#ifndef MQTT_SERVER
#define MQTT_SERVER                     "io.fastybird.com"          // Default MQTT broker address
#endif

#ifndef MQTT_PORT
#define MQTT_PORT                       1883                        // MQTT broker port
#endif

#ifndef MQTT_USER
#define MQTT_USER                       ""                          // Default MQTT broker usename
#endif

#ifndef MQTT_PASS
#define MQTT_PASS                       ""                          // Default MQTT broker password
#endif

#ifndef MQTT_RETAIN
#define MQTT_RETAIN                     true                        // MQTT retain flag
#endif

#ifndef MQTT_QOS
#define MQTT_QOS                        1                           // MQTT QoS value for all messages
#endif

#ifndef MQTT_KEEPALIVE
#define MQTT_KEEPALIVE                  300                         // MQTT keepalive value
#endif

#ifndef MQTT_RECONNECT_DELAY_MIN
#define MQTT_RECONNECT_DELAY_MIN        5000                        // Try to reconnect in 5 seconds upon disconnection
#endif

#ifndef MQTT_RECONNECT_DELAY_STEP
#define MQTT_RECONNECT_DELAY_STEP       5000                        // Increase the reconnect delay in 5 seconds after each failed attempt
#endif

#ifndef MQTT_RECONNECT_DELAY_MAX
#define MQTT_RECONNECT_DELAY_MAX        120000                      // Set reconnect time to 2 minutes at most
#endif

#ifndef MQTT_SKIP_RETAINED
#define MQTT_SKIP_RETAINED              1                           // Skip retained messages on connection
#endif

#ifndef MQTT_SKIP_TIME
#define MQTT_SKIP_TIME                  1000                        // Skip messages for 1 second anter connection
#endif

// -----------------------------------------------------------------------------
// NTP
// -----------------------------------------------------------------------------

#ifndef NTP_SUPPORT
#define NTP_SUPPORT                     1               // Build with NTP support by default (6.78Kb)
#endif

#ifndef NTP_SERVER
#define NTP_SERVER                      "pool.ntp.org"  // Default NTP server
#endif

#ifndef NTP_TIMEOUT
#define NTP_TIMEOUT                     1000            // Set NTP request timeout to 2 seconds (issue #452)
#endif

#ifndef NTP_TIME_OFFSET
#define NTP_TIME_OFFSET                 60              // Default timezone offset (GMT+1)
#endif

#ifndef NTP_DAY_LIGHT
#define NTP_DAY_LIGHT                   1               // Enable daylight time saving by default
#endif

#ifndef NTP_SYNC_INTERVAL
#define NTP_SYNC_INTERVAL               60              // NTP initial check every minute
#endif

#ifndef NTP_UPDATE_INTERVAL
#define NTP_UPDATE_INTERVAL             1800            // NTP check every 30 minutes
#endif

#ifndef NTP_START_DELAY
#define NTP_START_DELAY                 1000            // Delay NTP start 1 second
#endif

#ifndef NTP_DST_REGION
#define NTP_DST_REGION                  0               // 0 for Europe, 1 for USA (defined in NtpClientLib)
#endif

//------------------------------------------------------------------------------
// BUTTON
//------------------------------------------------------------------------------

#ifndef BUTTON_SUPPORT
#define BUTTON_SUPPORT                  1
#endif

#ifndef BUTTON_DEBOUNCE_DELAY
#define BUTTON_DEBOUNCE_DELAY           50                          // Debounce delay (ms)
#endif

#ifndef BUTTON_DBLCLICK_DELAY
#define BUTTON_DBLCLICK_DELAY           500                         // Time in ms to wait for a second (or third...) click
#endif

#ifndef BUTTON_LNGCLICK_DELAY
#define BUTTON_LNGCLICK_DELAY           1000                        // Time in ms holding the button down to get a long click
#endif

#ifndef BUTTON_LNGLNGCLICK_DELAY
#define BUTTON_LNGLNGCLICK_DELAY        10000                       // Time in ms holding the button down to get a long-long click
#endif

//------------------------------------------------------------------------------
// ENCODER
//------------------------------------------------------------------------------

#ifndef ENCODER_SUPPORT
#define ENCODER_SUPPORT             0
#endif

//------------------------------------------------------------------------------
// LED
//------------------------------------------------------------------------------

#ifndef LED_SUPPORT
#define LED_SUPPORT                 1
#endif

//------------------------------------------------------------------------------
// RELAY
//------------------------------------------------------------------------------

// Default boot mode: 0 means OFF, 1 ON and 2 whatever was before
#ifndef RELAY_BOOT_MODE
#define RELAY_BOOT_MODE             RELAY_BOOT_OFF
#endif

// 0 means ANY, 1 zero or one and 2 one and only one
#ifndef RELAY_SYNC
#define RELAY_SYNC                  RELAY_SYNC_ANY
#endif

// Default pulse mode: 0 means no pulses, 1 means normally off, 2 normally on
#ifndef RELAY_PULSE_MODE
#define RELAY_PULSE_MODE            RELAY_PULSE_NONE
#endif

// Default pulse time in seconds
#ifndef RELAY_PULSE_TIME
#define RELAY_PULSE_TIME            1.0
#endif

// Relay requests flood protection window - in seconds
#ifndef RELAY_FLOOD_WINDOW
#define RELAY_FLOOD_WINDOW          3
#endif

// Allowed actual relay changes inside requests flood protection window
#ifndef RELAY_FLOOD_CHANGES
#define RELAY_FLOOD_CHANGES         5
#endif

// Pulse with in milliseconds for a latched relay
#ifndef RELAY_LATCHING_PULSE
#define RELAY_LATCHING_PULSE        10
#endif

// Do not save relay state after these many milliseconds
#ifndef RELAY_SAVE_DELAY
#define RELAY_SAVE_DELAY            1000
#endif

// -----------------------------------------------------------------------------
// LIGHT
// -----------------------------------------------------------------------------

// LIGHT_PROVIDER_DIMMER can have from 1 to 5 different channels.
// They have to be defined for each device in the hardware.h file.
// If 3 or more channels first 3 will be considered RGB.
// Usual configurations are:
// 1 channels => W
// 2 channels => WW
// 3 channels => RGB
// 4 channels => RGBW
// 5 channels => RGBWW

#ifndef LIGHT_SAVE_ENABLED
#define LIGHT_SAVE_ENABLED      1           // Light channel values saved by default after each change
#endif

#ifndef LIGHT_SAVE_DELAY
#define LIGHT_SAVE_DELAY        5           // Persist color after 5 seconds to avoid wearing out
#endif

#ifndef LIGHT_MAX_PWM

#if LIGHT_PROVIDER == LIGHT_PROVIDER_MY92XX
#define LIGHT_MAX_PWM           255
#endif

#if LIGHT_PROVIDER == LIGHT_PROVIDER_DIMMER
#define LIGHT_MAX_PWM           10000        // 10000 * 200ns => 2 kHz
#endif

#endif // LIGHT_MAX_PWM

#ifndef LIGHT_LIMIT_PWM
#define LIGHT_LIMIT_PWM         LIGHT_MAX_PWM   // Limit PWM to this value (prevent 100% power)
#endif

#ifndef LIGHT_MAX_VALUE
#define LIGHT_MAX_VALUE         255         // Maximum light value
#endif

#ifndef LIGHT_MAX_BRIGHTNESS
#define LIGHT_MAX_BRIGHTNESS    255         // Maximun brightness value
#endif

#define LIGHT_MIN_MIREDS        153      // Default to the Philips Hue value that HA also use.
#define LIGHT_MAX_MIREDS        500      // https://developers.meethue.com/documentation/core-concepts

#ifndef LIGHT_STEP
#define LIGHT_STEP              32          // Step size
#endif

#ifndef LIGHT_USE_COLOR
#define LIGHT_USE_COLOR         1           // Use 3 first channels as RGB
#endif

#ifndef LIGHT_USE_WHITE
#define LIGHT_USE_WHITE         0           // Use the 4th channel as (Warm-)White LEDs
#endif

#ifndef LIGHT_USE_CCT
#define LIGHT_USE_CCT           0           // Use the 5th channel as Coldwhite LEDs, LIGHT_USE_WHITE must be 1.
#endif

// Used when LIGHT_USE_WHITE AND LIGHT_USE_CCT is 1 - (1000000/Kelvin = MiReds)
// Warning! Don't change this yet, NOT FULLY IMPLEMENTED!
#define LIGHT_COLDWHITE_MIRED   153         // Coldwhite Strip, Value must be __BELOW__ W2!! (Default: 6535 Kelvin/153 MiRed)
#define LIGHT_WARMWHITE_MIRED   500         // Warmwhite Strip, Value must be __ABOVE__ W1!! (Default: 2000 Kelvin/500 MiRed)

#ifndef LIGHT_USE_GAMMA
#define LIGHT_USE_GAMMA         0           // Use gamma correction for color channels
#endif

#ifndef LIGHT_USE_CSS
#define LIGHT_USE_CSS           1           // Use CSS style to report colors (1=> "#FF0000", 0=> "255,0,0")
#endif

#ifndef LIGHT_USE_RGB
#define LIGHT_USE_RGB           0           // Use RGB color selector (1=> RGB, 0=> HSV)
#endif

#ifndef LIGHT_WHITE_FACTOR
#define LIGHT_WHITE_FACTOR      1           // When using LIGHT_USE_WHITE with uneven brightness LEDs,
                                            // this factor is used to scale the white channel to match brightness
#endif


#ifndef LIGHT_USE_TRANSITIONS
#define LIGHT_USE_TRANSITIONS   1           // Transitions between colors
#endif

#ifndef LIGHT_TRANSITION_STEP
#define LIGHT_TRANSITION_STEP   10          // Time in millis between each transtion step
#endif

#ifndef LIGHT_TRANSITION_TIME
#define LIGHT_TRANSITION_TIME   500         // Time in millis from color to color
#endif

// -----------------------------------------------------------------------------
// SCHEDULER
// -----------------------------------------------------------------------------

#ifndef SCHEDULER_SUPPORT
#define SCHEDULER_SUPPORT           1           // Enable scheduler (1.77Kb)
#endif

#ifndef SCHEDULER_MAX_SCHEDULES
#define SCHEDULER_MAX_SCHEDULES     10          // Max schedules alowed
#endif

// -----------------------------------------------------------------------------
// DIRECT CONTROLS
// -----------------------------------------------------------------------------

#ifndef DIRECT_CONTROL_SUPPORT
#define DIRECT_CONTROL_SUPPORT      1           // Enable direct control
#endif

#ifndef DIRECT_CONTROL_MAX_CONTROLS
#define DIRECT_CONTROL_MAX_CONTROLS 10          // Max direct controls alowed
#endif

// -----------------------------------------------------------------------------
// ALEXA
// -----------------------------------------------------------------------------

// This setting defines whether Alexa support should be built into the firmware
#ifndef ALEXA_SUPPORT
#define ALEXA_SUPPORT               1               // Enable Alexa support by default (10.84Kb)
#endif

// This is default value for the alexaEnabled setting that defines whether
// this device should be discoberable and respond to Alexa commands.
// Both ALEXA_SUPPORT and alexaEnabled should be 1 for Alexa support to work.
#ifndef ALEXA_ENABLED
#define ALEXA_ENABLED               1
#endif

// -----------------------------------------------------------------------------
// RFBRIDGE
// This module is not compatible with RF_SUPPORT=1
// -----------------------------------------------------------------------------

#ifndef RF_SEND_TIMES
#define RF_SEND_TIMES               4               // How many times to send the message
#endif

#ifndef RF_SEND_DELAY
#define RF_SEND_DELAY               500             // Interval between sendings in ms
#endif

#ifndef RF_RECEIVE_DELAY
#define RF_RECEIVE_DELAY            500             // Interval between recieving in ms (avoid debouncing)
#endif

#ifndef RF_RAW_SUPPORT
#define RF_RAW_SUPPORT              0               // RF raw codes require a specific firmware for the EFM8BB1
                                                    // https://github.com/rhx/RF-Bridge-EFM8BB1
#endif

// -----------------------------------------------------------------------------
// IR Bridge
// -----------------------------------------------------------------------------

#ifndef IR_SUPPORT
#define IR_SUPPORT                  0               // Do not build with IR support by default (10.25Kb)
#endif

//#define IR_RX_PIN                   5               // GPIO the receiver is connected to
//#define IR_TX_PIN                   4               // GPIO the transmitter is connected to

#ifndef IR_USE_RAW
#define IR_USE_RAW                  0               // Use raw codes
#endif

#ifndef IR_BUFFER_SIZE
#define IR_BUFFER_SIZE              1024
#endif

#ifndef IR_TIMEOUT
#define IR_TIMEOUT                  15U
#endif

#ifndef IR_REPEAT
#define IR_REPEAT                   1
#endif

#ifndef IR_DELAY
#define IR_DELAY                    100
#endif

#ifndef IR_DEBOUNCE
#define IR_DEBOUNCE                 500             // IR debounce time in milliseconds
#endif

#ifndef IR_BUTTON_SET
#define IR_BUTTON_SET               0               // IR button set to use (see below)
#endif

// -----------------------------------------------------------------------------

// Remote Buttons SET 1 (for the original Remote shipped with the controller)
#if IR_BUTTON_SET == 1

/*
   +------+------+------+------+
   |  UP  | Down | OFF  |  ON  |
   +------+------+------+------+
   |  R   |  G   |  B   |  W   |
   +------+------+------+------+
   |  1   |  2   |  3   |FLASH |
   +------+------+------+------+
   |  4   |  5   |  6   |STROBE|
   +------+------+------+------+
   |  7   |  8   |  9   | FADE |
   +------+------+------+------+
   |  10  |  11  |  12  |SMOOTH|
   +------+------+------+------+
*/

    #define IR_BUTTON_COUNT 24

    const uint32_t IR_BUTTON[IR_BUTTON_COUNT][3] PROGMEM = {

        { 0xFF906F, IR_BUTTON_MODE_BRIGHTER, 1 },
        { 0xFFB847, IR_BUTTON_MODE_BRIGHTER, 0 },
        { 0xFFF807, IR_BUTTON_MODE_STATE, 0 },
        { 0xFFB04F, IR_BUTTON_MODE_STATE, 1 },

        { 0xFF9867, IR_BUTTON_MODE_RGB, 0xFF0000 },
        { 0xFFD827, IR_BUTTON_MODE_RGB, 0x00FF00 },
        { 0xFF8877, IR_BUTTON_MODE_RGB, 0x0000FF },
        { 0xFFA857, IR_BUTTON_MODE_RGB, 0xFFFFFF },

        { 0xFFE817, IR_BUTTON_MODE_RGB, 0xD13A01 },
        { 0xFF48B7, IR_BUTTON_MODE_RGB, 0x00E644 },
        { 0xFF6897, IR_BUTTON_MODE_RGB, 0x0040A7 },
        { 0xFFB24D, IR_BUTTON_MODE_EFFECT, LIGHT_EFFECT_FLASH },

        { 0xFF02FD, IR_BUTTON_MODE_RGB, 0xE96F2A },
        { 0xFF32CD, IR_BUTTON_MODE_RGB, 0x00BEBF },
        { 0xFF20DF, IR_BUTTON_MODE_RGB, 0x56406F },
        { 0xFF00FF, IR_BUTTON_MODE_EFFECT, LIGHT_EFFECT_STROBE },

        { 0xFF50AF, IR_BUTTON_MODE_RGB, 0xEE9819 },
        { 0xFF7887, IR_BUTTON_MODE_RGB, 0x00799A },
        { 0xFF708F, IR_BUTTON_MODE_RGB, 0x944E80 },
        { 0xFF58A7, IR_BUTTON_MODE_EFFECT, LIGHT_EFFECT_FADE },

        { 0xFF38C7, IR_BUTTON_MODE_RGB, 0xFFFF00 },
        { 0xFF28D7, IR_BUTTON_MODE_RGB, 0x0060A1 },
        { 0xFFF00F, IR_BUTTON_MODE_RGB, 0xEF45AD },
        { 0xFF30CF, IR_BUTTON_MODE_EFFECT, LIGHT_EFFECT_SMOOTH }

    };

#endif

//Remote Buttons SET 2 (another identical IR Remote shipped with another controller)
#if IR_BUTTON_SET == 2

/*
   +------+------+------+------+
   |  UP  | Down | OFF  |  ON  |
   +------+------+------+------+
   |  R   |  G   |  B   |  W   |
   +------+------+------+------+
   |  1   |  2   |  3   |FLASH |
   +------+------+------+------+
   |  4   |  5   |  6   |STROBE|
   +------+------+------+------+
   |  7   |  8   |  9   | FADE |
   +------+------+------+------+
   |  10  |  11  |  12  |SMOOTH|
   +------+------+------+------+
*/

    #define IR_BUTTON_COUNT 24

    const unsigned long IR_BUTTON[IR_BUTTON_COUNT][3] PROGMEM = {

        { 0xFF00FF, IR_BUTTON_MODE_BRIGHTER, 1 },
        { 0xFF807F, IR_BUTTON_MODE_BRIGHTER, 0 },
        { 0xFF40BF, IR_BUTTON_MODE_STATE, 0 },
        { 0xFFC03F, IR_BUTTON_MODE_STATE, 1 },

        { 0xFF20DF, IR_BUTTON_MODE_RGB, 0xFF0000 },
        { 0xFFA05F, IR_BUTTON_MODE_RGB, 0x00FF00 },
        { 0xFF609F, IR_BUTTON_MODE_RGB, 0x0000FF },
        { 0xFFE01F, IR_BUTTON_MODE_RGB, 0xFFFFFF },

        { 0xFF10EF, IR_BUTTON_MODE_RGB, 0xD13A01 },
        { 0xFF906F, IR_BUTTON_MODE_RGB, 0x00E644 },
        { 0xFF50AF, IR_BUTTON_MODE_RGB, 0x0040A7 },
        { 0xFFD02F, IR_BUTTON_MODE_EFFECT, LIGHT_EFFECT_FLASH },

        { 0xFF30CF, IR_BUTTON_MODE_RGB, 0xE96F2A },
        { 0xFFB04F, IR_BUTTON_MODE_RGB, 0x00BEBF },
        { 0xFF708F, IR_BUTTON_MODE_RGB, 0x56406F },
        { 0xFFF00F, IR_BUTTON_MODE_EFFECT, LIGHT_EFFECT_STROBE },

        { 0xFF08F7, IR_BUTTON_MODE_RGB, 0xEE9819 },
        { 0xFF8877, IR_BUTTON_MODE_RGB, 0x00799A },
        { 0xFF48B7, IR_BUTTON_MODE_RGB, 0x944E80 },
        { 0xFFC837, IR_BUTTON_MODE_EFFECT, LIGHT_EFFECT_FADE },

        { 0xFF28D7, IR_BUTTON_MODE_RGB, 0xFFFF00 },
        { 0xFFA857, IR_BUTTON_MODE_RGB, 0x0060A1 },
        { 0xFF6897, IR_BUTTON_MODE_RGB, 0xEF45AD },
        { 0xFFE817, IR_BUTTON_MODE_EFFECT, LIGHT_EFFECT_SMOOTH }

    };

#endif

//Remote Buttons SET 3 (samsung AA59-00608A 8 Toggle Buttons for generic 8CH module)
#if IR_BUTTON_SET == 3
/*
   +------+------+------+
   |  1   |  2   |  3   |
   +------+------+------+
   |  4   |  5   |  6   |
   +------+------+------+
   |  7   |  8   |  9   |
   +------+------+------+
   |      |  0   |      |
   +------+------+------+
*/
#define IR_BUTTON_COUNT 10

 const unsigned long IR_BUTTON[IR_BUTTON_COUNT][3] PROGMEM = {

        { 0xE0E020DF, IR_BUTTON_MODE_TOGGLE, 0 }, // Toggle Relay #0
        { 0xE0E0A05F, IR_BUTTON_MODE_TOGGLE, 1 }, // Toggle Relay #1
        { 0xE0E0609F, IR_BUTTON_MODE_TOGGLE, 2 }, // Toggle Relay #2

        { 0xE0E010EF, IR_BUTTON_MODE_TOGGLE, 3 }, // Toggle Relay #3
        { 0xE0E0906F, IR_BUTTON_MODE_TOGGLE, 4 }, // Toggle Relay #4
        { 0xE0E050AF, IR_BUTTON_MODE_TOGGLE, 5 }, // Toggle Relay #5

        { 0xE0E030CF, IR_BUTTON_MODE_TOGGLE, 6 }, // Toggle Relay #6
        { 0xE0E0B04F, IR_BUTTON_MODE_TOGGLE, 7 } // Toggle Relay #7
      //{ 0xE0E0708F, IR_BUTTON_MODE_TOGGLE, 8 } //Extra Button

      //{ 0xE0E08877, IR_BUTTON_MODE_TOGGLE, 9 } //Extra Button
 };
#endif

//Remote Buttons SET 4
#if IR_BUTTON_SET == 4
/*
   +------+------+------+
   | OFF  | SRC  | MUTE |
   +------+------+------+
   ...
   +------+------+------+
*/
#define IR_BUTTON_COUNT 1

 const unsigned long IR_BUTTON[IR_BUTTON_COUNT][3] PROGMEM = {

        { 0xFFB24D, IR_BUTTON_MODE_TOGGLE, 0 } // Toggle Relay #0

 };

#endif

#ifndef IR_BUTTON_COUNT
#define IR_BUTTON_COUNT 0
#endif

//--------------------------------------------------------------------------------
// Custom RF module
// Check http://tinkerman.cat/adding-rf-to-a-non-rf-itead-sonoff/
// Enable support by passing RF_SUPPORT=1 build flag
// This module is not compatible with RFBRIDGE or SONOFF RF
//--------------------------------------------------------------------------------

#ifndef RF_SUPPORT
#define RF_SUPPORT                  0
#endif

#ifndef RF_PIN
#define RF_PIN                      14
#endif

#define RF_DEBOUNCE                 500
#define RF_LEARN_TIMEOUT            60000

//--------------------------------------------------------------------------------
// Custom RFM69 to MQTT bridge
// Check http://tinkerman.cat/rfm69-wifi-gateway/
// Enable support by passing RFM69_SUPPORT=1 build flag
//--------------------------------------------------------------------------------

#ifndef RFM69_SUPPORT
#define RFM69_SUPPORT               0
#endif

#ifndef RFM69_MAX_TOPICS
#define RFM69_MAX_TOPICS            50
#endif

#ifndef RFM69_DEFAULT_TOPIC
#define RFM69_DEFAULT_TOPIC         "/rfm69gw/{node}/{key}"
#endif

#ifndef RFM69_NODE_ID
#define RFM69_NODE_ID               1
#endif

#ifndef RFM69_GATEWAY_ID
#define RFM69_GATEWAY_ID            1
#endif

#ifndef RFM69_NETWORK_ID
#define RFM69_NETWORK_ID            164
#endif

#ifndef RFM69_PROMISCUOUS
#define RFM69_PROMISCUOUS           0
#endif

#ifndef RFM69_PROMISCUOUS_SENDS
#define RFM69_PROMISCUOUS_SENDS     0
#endif

#ifndef RFM69_FREQUENCY
#define RFM69_FREQUENCY             RF69_868MHZ
#endif

#ifndef RFM69_ENCRYPTKEY
#define RFM69_ENCRYPTKEY            "fibonacci0123456"
#endif

#ifndef RFM69_CS_PIN
#define RFM69_CS_PIN                SS
#endif

#ifndef RFM69_IRQ_PIN
#define RFM69_IRQ_PIN               5
#endif

#ifndef RFM69_RESET_PIN
#define RFM69_RESET_PIN             7
#endif

#ifndef RFM69_IS_RFM69HW
#define RFM69_IS_RFM69HW            0
#endif

// -----------------------------------------------------------------------------
// Nodes gateway
// -----------------------------------------------------------------------------

#ifndef NODES_GATEWAY_SUPPORT
#define NODES_GATEWAY_SUPPORT       0               // Do not build with gateway support by default
#endif
