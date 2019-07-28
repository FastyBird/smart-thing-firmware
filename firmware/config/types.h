/*

DATA TYPES & CONSTANTS DEFINITION

Copyright (C) 2018 FastyBird s.r.o. <info@fastybird.com>

*/

// =============================================================================
// RESET
// =============================================================================

#define CUSTOM_RESET_WEB                        1       // Reset from web interface
#define CUSTOM_RESET_BROKER                     2       // Reset via broker
#define CUSTOM_RESET_BUTTON                     3       // Reset via hardware button

#define CUSTOM_FACTORY_WEB                      4       // Reset after factory reset from web interface
#define CUSTOM_FACTORY_BROKER                   5       // Reset after factory reset via broker
#define CUSTOM_FACTORY_BUTTON                   6       // Reset after factory reset via hardware button

#define CUSTOM_UPGRADE_WEB                      7       // Reset after successful update from web interface
#define CUSTOM_UPGRADE_OTA                      8       // Reset after successful OTA update
#define CUSTOM_UPGRADE_NOFUSS                   9       // Reset after successful NOFUSS update

#define CUSTOM_RESTORE_WEB                      10      // Reset after successful setting restore from web interface

#define CUSTOM_RESET_MAX                        10

// =============================================================================
// WIFI
// =============================================================================

#define WIFI_STATE_AP                           1
#define WIFI_STATE_STA                          2
#define WIFI_STATE_AP_STA                       3

// =============================================================================
// LED
// =============================================================================

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

// =============================================================================
// ENVIRONMENTAL
// =============================================================================

// American Society of Heating, Refrigerating and Air-Conditioning Engineers suggests a range of 45% - 55% humidity to manage health effects and illnesses.
// Comfortable: 30% - 60%
// Recommended: 45% - 55%
// High       : 55% - 80%

#define HUMIDITY_NORMAL             0       // > %30
#define HUMIDITY_COMFORTABLE        1       // > %45
#define HUMIDITY_DRY                2       // < %30
#define HUMIDITY_WET                3       // > %70

// United States Environmental Protection Agency - UV Index Scale
// One UV Index unit is equivalent to 25 milliWatts per square meter.
#define UV_INDEX_LOW                0       // 0 to 2 means low danger from the sun's UV rays for the average person.
#define UV_INDEX_MODERATE           1       // 3 to 5 means moderate risk of harm from unprotected sun exposure.
#define UV_INDEX_HIGH               2       // 6 to 7 means high risk of harm from unprotected sun exposure. Protection against skin and eye damage is needed.
#define UV_INDEX_VERY_HIGH          3       // 8 to 10 means very high risk of harm from unprotected sun exposure.
                                            // Take extra precautions because unprotected skin and eyes will be damaged and can burn quickly.
#define UV_INDEX_EXTREME            4       // 11 or more means extreme risk of harm from unprotected sun exposure.
                                            // Take all precautions because unprotected skin and eyes can burn in minutes.

// =============================================================================
// UNITS
// =============================================================================

#define POWER_WATTS                 0
#define POWER_KILOWATTS             1

#define ENERGY_JOULES               0
#define ENERGY_KWH                  1

#define TMP_CELSIUS                 0
#define TMP_FAHRENHEIT              1
#define TMP_KELVIN                  2

// =============================================================================
// Sensor ID
// These should remain over time, do not modify them, only add new ones at the end
// =============================================================================

#define SENSOR_DHTXX_ID             1
#define SENSOR_DALLAS_ID            2
#define SENSOR_EMON_ANALOG_ID       3
#define SENSOR_EMON_ADC121_ID       4
#define SENSOR_EMON_ADS1X15_ID      5
#define SENSOR_HLW8012_ID           6
#define SENSOR_V9261F_ID            7
#define SENSOR_ECH1560_ID           8
#define SENSOR_ANALOG_ID            9
#define SENSOR_DIGITAL_ID           10
#define SENSOR_EVENTS_ID            11
#define SENSOR_PMSX003_ID           12
#define SENSOR_BMX280_ID            13
#define SENSOR_MHZ19_ID             14
#define SENSOR_SI7021_ID            15
#define SENSOR_SHT3X_I2C_ID         16
#define SENSOR_BH1750_ID            17
#define SENSOR_PZEM004T_ID          18
#define SENSOR_AM2320_ID            19
#define SENSOR_GUVAS12SD_ID         20
#define SENSOR_CSE7766_ID           21
#define SENSOR_TMP3X_ID             22
#define SENSOR_SONAR_ID             23
#define SENSOR_SENSEAIR_ID          24
#define SENSOR_GEIGER_ID            25
#define SENSOR_NTC_ID               26
#define SENSOR_SDS011_ID            27
#define SENSOR_MICS2710_ID          28
#define SENSOR_MICS5525_ID          29
#define SENSOR_PULSEMETER_ID        30
#define SENSOR_VEML6075_ID          31
#define SENSOR_VL53L1X_ID           32
#define SENSOR_EZOPH_ID             33
#define SENSOR_BMP180_ID            34
#define SENSOR_MAX6675_ID           35
#define SENSOR_LDR_ID               36
#define SENSOR_SONOFF_SC            37
#define SENSOR_SONOFF_SC_PRO        38

// =============================================================================
// Magnitudes
// These should remain over time, do not modify their values, only add new ones at the end
// =============================================================================

#define MAGNITUDE_NONE              0
#define MAGNITUDE_TEMPERATURE       1
#define MAGNITUDE_HUMIDITY          2
#define MAGNITUDE_PRESSURE          3
#define MAGNITUDE_CURRENT           4
#define MAGNITUDE_VOLTAGE           5
#define MAGNITUDE_POWER_ACTIVE      6
#define MAGNITUDE_POWER_APPARENT    7
#define MAGNITUDE_POWER_REACTIVE    8
#define MAGNITUDE_POWER_FACTOR      9
#define MAGNITUDE_ENERGY            10
#define MAGNITUDE_ENERGY_DELTA      11
#define MAGNITUDE_ANALOG            12
#define MAGNITUDE_DIGITAL           13
#define MAGNITUDE_EVENT             14
#define MAGNITUDE_PM1dot0           15
#define MAGNITUDE_PM2dot5           16
#define MAGNITUDE_PM10              17
#define MAGNITUDE_CO2               18
#define MAGNITUDE_LUX               19
#define MAGNITUDE_UVA               20
#define MAGNITUDE_UVB               21
#define MAGNITUDE_UVI               22
#define MAGNITUDE_DISTANCE          23
#define MAGNITUDE_HCHO              24
#define MAGNITUDE_GEIGER_CPM        25
#define MAGNITUDE_GEIGER_SIEVERT    26
#define MAGNITUDE_COUNT             27
#define MAGNITUDE_NO2               28
#define MAGNITUDE_CO                29
#define MAGNITUDE_RESISTANCE        30
#define MAGNITUDE_PH                31

#define MAGNITUDE_MAX               32