/*

PROGMEM DEFINITIONS

Copyright (C) 2018 FastyBird s.r.o. <code@fastybird.com>

*/

#pragma once

// =============================================================================
// MODULE DEFAULTS
// =============================================================================

#ifndef FASTYBIRD_SUPPORT
    #define FASTYBIRD_SUPPORT                               0
#endif

#ifndef FASTYBIRD_MAX_CHANNELS
    #define FASTYBIRD_MAX_CHANNELS                          0                       // Maximum device channels count
#endif

#define FASTYBIRD_MAIN_DEVICE_INDEX                         0

//------------------------------------------------------------------------------
// FASTYBIRD - Dependencies
//------------------------------------------------------------------------------

#if FASTYBIRD_SUPPORT
    #undef MQTT_SUPPORT
    #define MQTT_SUPPORT                                    1                       // FastyBird needs MQTT
#endif

//------------------------------------------------------------------------------
// FASTYBIRD - API configuration
//------------------------------------------------------------------------------

#define FASTYBIRD_STATUS_INIT                               "init"
#define FASTYBIRD_STATUS_READY                              "ready"
#define FASTYBIRD_STATUS_DISCONNECTED                       "disconnected"
#define FASTYBIRD_STATUS_SLEEPING                           "sleeping"
#define FASTYBIRD_STATUS_LOST                               "lost"
#define FASTYBIRD_STATUS_ALERT                              "alert"

#define FASTYBIRD_SWITCH_PAYLOAD_ON                         "switch-on"
#define FASTYBIRD_SWITCH_PAYLOAD_OFF                        "switch-off"
#define FASTYBIRD_SWITCH_PAYLOAD_TOGGLE                     "switch-toggle"

#define FASTYBIRD_BTN_PAYLOAD_PRESS                         "btn-pressed"
#define FASTYBIRD_BTN_PAYLOAD_CLICK                         "btn-clicked"
#define FASTYBIRD_BTN_PAYLOAD_DBL_CLICK                     "btn-double-clicked"
#define FASTYBIRD_BTN_PAYLOAD_TRIPLE_CLICK                  "btn-triple-clicked"
#define FASTYBIRD_BTN_PAYLOAD_LNG_CLICK                     "btn-long-clicked"
#define FASTYBIRD_BTN_PAYLOAD_LNG_LNG_CLICK                 "btn-extra-long-clicked"

#define FASTYBIRD_PROPERTY_DATA_TYPE_FLOAT                  "float"
#define FASTYBIRD_PROPERTY_DATA_TYPE_INTEGER                "int"
#define FASTYBIRD_PROPERTY_DATA_TYPE_BOOLEAN                "bool"
#define FASTYBIRD_PROPERTY_DATA_TYPE_STRING                 "string"
#define FASTYBIRD_PROPERTY_DATA_TYPE_ENUM                   "enum"
#define FASTYBIRD_PROPERTY_DATA_TYPE_SWITCH                 "switch"
#define FASTYBIRD_PROPERTY_DATA_TYPE_BUTTON                 "button"
#define FASTYBIRD_PROPERTY_DATA_TYPE_COLOR                  "color"

#define FASTYBIRD_PROPERTY_IP_ADDRESS                       "ip-address"
#define FASTYBIRD_PROPERTY_TEMPERATURE                      "temperature"
#define FASTYBIRD_PROPERTY_HUMIDITY                         "humidity"
#define FASTYBIRD_PROPERTY_NOISE_LEVEL                      "noise-level"
#define FASTYBIRD_PROPERTY_LIGHT_LEVEL                      "light-level"
#define FASTYBIRD_PROPERTY_AIR_QUALITY_LEVEL                "air-quality-level"
#define FASTYBIRD_PROPERTY_ENERGY_DELTA                     "energy-delta"
#define FASTYBIRD_PROPERTY_ENERGY_TOTAL                     "energy-total"
#define FASTYBIRD_PROPERTY_CURRENT                          "current"
#define FASTYBIRD_PROPERTY_VOLTAGE                          "voltage"
#define FASTYBIRD_PROPERTY_ACTIVE_POWER                     "active-power"
#define FASTYBIRD_PROPERTY_REACTIVE_POWER                   "reactive-power"
#define FASTYBIRD_PROPERTY_APPARENT_POWER                   "apparent-power"
#define FASTYBIRD_PROPERTY_POWER_FACTOR                     "power-factor"
#define FASTYBIRD_PROPERTY_VALUE                            "value"
#define FASTYBIRD_PROPERTY_INTERVAL                         "interval"
#define FASTYBIRD_PROPERTY_UPTIME                           "uptime"
#define FASTYBIRD_PROPERTY_FREE_HEAP                        "free-heap"
#define FASTYBIRD_PROPERTY_CPU_LOAD                         "cpu-load"
#define FASTYBIRD_PROPERTY_VCC                              "vcc"
#define FASTYBIRD_PROPERTY_SSID                             "ssid"
#define FASTYBIRD_PROPERTY_RSSI                             "rssi"
#define FASTYBIRD_PROPERTY_BUTTON                           "button"
#define FASTYBIRD_PROPERTY_SWITCH                           "switch"

#define FASTYBIRD_HARDWARE_MANUFACTURER                     "manufacturer"
#define FASTYBIRD_HARDWARE_MODEL                            "model"
#define FASTYBIRD_HARDWARE_VERSION                          "version"
#define FASTYBIRD_HARDWARE_MAC_ADDRESS                      "mac-address"
#define FASTYBIRD_FIRMWARE_MANUFACTURER                     "manufacturer"
#define FASTYBIRD_FIRMWARE_VERSION                          "version"

#define FASTYBIRD_CHANNEL_ARRAY_SUFFIX                      "-%d"

//------------------------------------------------------------------------------
// FASTYBIRD - Device initialization sequentions
//------------------------------------------------------------------------------

#define FASTYBIRD_PUB_DEVICE_INITIALIZE                     0
#define FASTYBIRD_PUB_DEVICE_NAME                           1
#define FASTYBIRD_PUB_DEVICE_HARDWARE                       2
#define FASTYBIRD_PUB_DEVICE_FIRMWARE                       3
#define FASTYBIRD_PUB_DEVICE_PROPERTIES                     4
#define FASTYBIRD_PUB_DEVICE_CONTROLS                       5
#define FASTYBIRD_PUB_DEVICE_CHANNELS                       6
#define FASTYBIRD_PUB_DEVICE_PROPERTY                       7
#define FASTYBIRD_PUB_DEVICE_CHANNEL                        8
#define FASTYBIRD_PUB_DEVICE_READY                          9

//------------------------------------------------------------------------------
// FASTYBIRD - Channel initialization sequentions
//------------------------------------------------------------------------------

#define FASTYBIRD_PUB_CHANNEL_NAME                          0
#define FASTYBIRD_PUB_CHANNEL_PROPERTIES                    1
#define FASTYBIRD_PUB_CHANNEL_CONTROLS                      2
#define FASTYBIRD_PUB_CHANNEL_PROPERTY                      3
#define FASTYBIRD_PUB_CHANNEL_DONE                          4

//------------------------------------------------------------------------------
// FASTYBIRD - Property initialization sequentions
//------------------------------------------------------------------------------

#define FASTYBIRD_PUB_PROPERTY_NAME                         0
#define FASTYBIRD_PUB_PROPERTY_TYPIE                        1
#define FASTYBIRD_PUB_PROPERTY_SETABLE                      2
#define FASTYBIRD_PUB_PROPERTY_QUERYABLE                    3
#define FASTYBIRD_PUB_PROPERTY_DATA_TYPE                    4
#define FASTYBIRD_PUB_PROPERTY_FORMAT                       5
#define FASTYBIRD_PUB_PROPERTY_UNIT                         6
#define FASTYBIRD_PUB_PROPERTY_DONE                         7

//------------------------------------------------------------------------------
// FASTYBIRD - Device controls
//------------------------------------------------------------------------------

#define FASTYBIRD_DEVICE_CONTROL_CONFIGURE                   "configure"
#define FASTYBIRD_DEVICE_CONTROL_REBOOT                      "reboot"
#define FASTYBIRD_DEVICE_CONTROL_FACTORY_RESET               "factory-reset"
#define FASTYBIRD_DEVICE_CONTROL_RECONNECT                   "reconnect"

//------------------------------------------------------------------------------
// FASTYBIRD - Channels names
//------------------------------------------------------------------------------

#define FASTYBIRD_CHANNEL_DEFAULT                           "channel-{no}"

#define FASTYBIRD_CHANNEL_ANALOG_SENSOR                     "analog-sensor"
#define FASTYBIRD_CHANNEL_ANALOG_ACTOR                      "analog-actor"
#define FASTYBIRD_CHANNEL_BINARY_SENSOR                     "digital-sensor"
#define FASTYBIRD_CHANNEL_BINARY_ACTOR                      "digital-actor"
#define FASTYBIRD_CHANNEL_EVENT                             "event"

// -----------------------------------------------------------------------------
// FASTYBIRD - MQTT API configuration
// -----------------------------------------------------------------------------

#define FASTYBIRD_MQTT_BASE_TOPIC                           "/fb/v1"

#define FASTYBIRD_PROPERTY_IS_SETTABLE                      "true"
#define FASTYBIRD_PROPERTY_IS_NOT_SETTABLE                  "false"

#define FASTYBIRD_PROPERTY_IS_QUERYABLE                     "true"
#define FASTYBIRD_PROPERTY_IS_NOT_QUERYABLE                 "false"

// -----------------------------------------------------------------------------
// FASTYBIRD - MQTT API topics parts
// -----------------------------------------------------------------------------

#define FASTYBIRD_TOPIC_PART_BROADCAST                      "$broadcast"
#define FASTYBIRD_TOPIC_PART_PROPERTY                       "$property"
#define FASTYBIRD_TOPIC_PART_CONTROL                        "$control"
#define FASTYBIRD_TOPIC_PART_CHANNEL		                "$channel"

#define FASTYBIRD_TOPIC_PART_SET    		                "set"
#define FASTYBIRD_TOPIC_PART_QUERY    		                "query"
#define FASTYBIRD_TOPIC_PART_INIT    		                "init"
#define FASTYBIRD_TOPIC_PART_RESET                          "reset"

// -----------------------------------------------------------------------------
// FASTYBIRD - MQTT API broadcast topics
// -----------------------------------------------------------------------------

#define FASTYBIRD_TOPIC_BROADCAST                           "$broadcast/+"

// -----------------------------------------------------------------------------
// FASTYBIRD - MQTT API device topics
// -----------------------------------------------------------------------------

#define FASTYBIRD_TOPIC_DEVICE_STATE                        "$state"
#define FASTYBIRD_TOPIC_DEVICE_NAME                         "$name"
#define FASTYBIRD_TOPIC_DEVICE_PROPERTIES                   "$properties"
#define FASTYBIRD_TOPIC_DEVICE_CONTROLS                     "$controls"
#define FASTYBIRD_TOPIC_DEVICE_CHANNELS                     "$channels"
#define FASTYBIRD_TOPIC_DEVICE_EXTENSIONS                    "$extensions"

#define FASTYBIRD_TOPIC_DEVICE_HW_INFO                      "$hw/{hw}"
#define FASTYBIRD_TOPIC_DEVICE_FW_INFO                      "$fw/{fw}"

#define FASTYBIRD_TOPIC_DEVICE_PROPERTY                     "$property/{property}"
#define FASTYBIRD_TOPIC_DEVICE_PROPERTY_NAME                "$property/{property}/$name"
#define FASTYBIRD_TOPIC_DEVICE_PROPERTY_TYPE                "$property/{property}/$type"
#define FASTYBIRD_TOPIC_DEVICE_PROPERTY_SETTABLE            "$property/{property}/$settable"
#define FASTYBIRD_TOPIC_DEVICE_PROPERTY_QUERYABLE           "$property/{property}/$queryable"
#define FASTYBIRD_TOPIC_DEVICE_PROPERTY_DATA_TYPE           "$property/{property}/$data-type"
#define FASTYBIRD_TOPIC_DEVICE_PROPERTY_FORMAT              "$property/{property}/$format"
#define FASTYBIRD_TOPIC_DEVICE_PROPERTY_UNIT                "$property/{property}/$unit"
#define FASTYBIRD_TOPIC_DEVICE_PROPERTY_SET                 "$property/{property}/set"
#define FASTYBIRD_TOPIC_DEVICE_PROPERTY_QUERY               "$property/{property}/query"

#define FASTYBIRD_TOPIC_DEVICE_CONTROL                      "$control/{control}"
#define FASTYBIRD_TOPIC_DEVICE_CONTROL_SET                  "$control/{control}/set"

// -----------------------------------------------------------------------------
// FASTYBIRD - MQTT API channel topics
// -----------------------------------------------------------------------------

#define FASTYBIRD_TOPIC_CHANNEL_NAME		                "$channel/{channel}/$name"
#define FASTYBIRD_TOPIC_CHANNEL_PROPERTIES		            "$channel/{channel}/$properties"
#define FASTYBIRD_TOPIC_CHANNEL_CONTROLS		            "$channel/{channel}/$controls"

#define FASTYBIRD_TOPIC_CHANNEL_PROPERTY                    "$channel/{channel}/$property/{property}"
#define FASTYBIRD_TOPIC_CHANNEL_PROPERTY_NAME               "$channel/{channel}/$property/{property}/$name"
#define FASTYBIRD_TOPIC_CHANNEL_PROPERTY_TYPE               "$channel/{channel}/$property/{property}/$type"
#define FASTYBIRD_TOPIC_CHANNEL_PROPERTY_SETTABLE           "$channel/{channel}/$property/{property}/$settable"
#define FASTYBIRD_TOPIC_CHANNEL_PROPERTY_QUERYABLE          "$channel/{channel}/$property/{property}/$queryable"
#define FASTYBIRD_TOPIC_CHANNEL_PROPERTY_DATA_TYPE          "$channel/{channel}/$property/{property}/$data-type"
#define FASTYBIRD_TOPIC_CHANNEL_PROPERTY_FORMAT             "$channel/{channel}/$property/{property}/$format"
#define FASTYBIRD_TOPIC_CHANNEL_PROPERTY_UNIT               "$channel/{channel}/$property/{property}/$unit"
#define FASTYBIRD_TOPIC_CHANNEL_PROPERTY_SET                "$channel/{channel}/$property/{property}/set"
#define FASTYBIRD_TOPIC_CHANNEL_PROPERTY_QUERY              "$channel/{channel}/$property/{property}/query"

#define FASTYBIRD_TOPIC_CHANNEL_CONTROL                     "$channel/{channel}/$control/{control}"
#define FASTYBIRD_TOPIC_CHANNEL_CONTROL_SET                 "$channel/{channel}/$control/{control}/set"

// -----------------------------------------------------------------------------
// FASTYBIRD - MQTT API in topic part count
// -----------------------------------------------------------------------------

#define FASTYBIRD_TOPIC_PART_COUNT_BROADCAST                4
#define FASTYBIRD_TOPIC_PART_COUNT_DEVICE_PROPERTY          6
#define FASTYBIRD_TOPIC_PART_COUNT_DEVICE_CONTROL           6
#define FASTYBIRD_TOPIC_PART_COUNT_CHANNEL_PROPERTY         8
#define FASTYBIRD_TOPIC_PART_COUNT_CHANNEL_CONTROL          8

// -----------------------------------------------------------------------------
// FASTYBIRD - MQTT API in topic part positions
// -----------------------------------------------------------------------------

#define FASTYBIRD_TOPIC_POSITION_BROADCAST_PREFIX           2
#define FASTYBIRD_TOPIC_POSITION_BROADCAST_ACTION           3

#define FASTYBIRD_TOPIC_POSITION_DEVICE_NAME                2
#define FASTYBIRD_TOPIC_POSITION_DEVICE_PROPERTY_PREFIX     3
#define FASTYBIRD_TOPIC_POSITION_DEVICE_PROPERTY_NAME       4
#define FASTYBIRD_TOPIC_POSITION_DEVICE_PROPERTY_ACTION     5
#define FASTYBIRD_TOPIC_POSITION_DEVICE_CONTROL_PREFIX      3
#define FASTYBIRD_TOPIC_POSITION_DEVICE_CONTROL_NAME        4
#define FASTYBIRD_TOPIC_POSITION_DEVICE_CONTROL_ACTION      5

#define FASTYBIRD_TOPIC_POSITION_CHANNEL_PREFIX             3
#define FASTYBIRD_TOPIC_POSITION_CHANNEL_NAME               4
#define FASTYBIRD_TOPIC_POSITION_CHANNEL_PROPERTY_PREFIX    5
#define FASTYBIRD_TOPIC_POSITION_CHANNEL_PROPERTY_NAME      6
#define FASTYBIRD_TOPIC_POSITION_CHANNEL_PROPERTY_ACTION    7
#define FASTYBIRD_TOPIC_POSITION_CHANNEL_CONTROL_PREFIX     5
#define FASTYBIRD_TOPIC_POSITION_CHANNEL_CONTROL_NAME       6
#define FASTYBIRD_TOPIC_POSITION_CHANNEL_CONTROL_ACTION     7

// -----------------------------------------------------------------------------
// FASTYBIRD - Buttons module
// -----------------------------------------------------------------------------

#ifndef FASTYBIRD_BUTTON1_CHANNEL_INDEX
    #define FASTYBIRD_BUTTON1_CHANNEL_INDEX                 INDEX_NONE
#endif

#ifndef FASTYBIRD_BUTTON2_CHANNEL_INDEX
    #define FASTYBIRD_BUTTON2_CHANNEL_INDEX                 INDEX_NONE
#endif

#ifndef FASTYBIRD_BUTTON3_CHANNEL_INDEX
    #define FASTYBIRD_BUTTON3_CHANNEL_INDEX                 INDEX_NONE
#endif

#ifndef FASTYBIRD_BUTTON4_CHANNEL_INDEX
    #define FASTYBIRD_BUTTON4_CHANNEL_INDEX                 INDEX_NONE
#endif

#ifndef FASTYBIRD_BUTTON5_CHANNEL_INDEX
    #define FASTYBIRD_BUTTON5_CHANNEL_INDEX                 INDEX_NONE
#endif

#ifndef FASTYBIRD_BUTTON6_CHANNEL_INDEX
    #define FASTYBIRD_BUTTON6_CHANNEL_INDEX                 INDEX_NONE
#endif

#ifndef FASTYBIRD_BUTTON7_CHANNEL_INDEX
    #define FASTYBIRD_BUTTON7_CHANNEL_INDEX                 INDEX_NONE
#endif

#ifndef FASTYBIRD_BUTTON8_CHANNEL_INDEX
    #define FASTYBIRD_BUTTON8_CHANNEL_INDEX                 INDEX_NONE
#endif

// -----------------------------------------------------------------------------
// FASTYBIRD - Relays module
// -----------------------------------------------------------------------------

#ifndef FASTYBIRD_RELAY1_CHANNEL_INDEX
    #define FASTYBIRD_RELAY1_CHANNEL_INDEX                  INDEX_NONE
#endif

#ifndef FASTYBIRD_RELAY2_CHANNEL_INDEX
    #define FASTYBIRD_RELAY2_CHANNEL_INDEX                  INDEX_NONE
#endif

#ifndef FASTYBIRD_RELAY3_CHANNEL_INDEX
    #define FASTYBIRD_RELAY3_CHANNEL_INDEX                  INDEX_NONE
#endif

#ifndef FASTYBIRD_RELAY4_CHANNEL_INDEX
    #define FASTYBIRD_RELAY4_CHANNEL_INDEX                  INDEX_NONE
#endif

#ifndef FASTYBIRD_RELAY5_CHANNEL_INDEX
    #define FASTYBIRD_RELAY5_CHANNEL_INDEX                  INDEX_NONE
#endif

#ifndef FASTYBIRD_RELAY6_CHANNEL_INDEX
    #define FASTYBIRD_RELAY6_CHANNEL_INDEX                  INDEX_NONE
#endif

#ifndef FASTYBIRD_RELAY7_CHANNEL_INDEX
    #define FASTYBIRD_RELAY7_CHANNEL_INDEX                  INDEX_NONE
#endif

#ifndef FASTYBIRD_RELAY8_CHANNEL_INDEX
    #define FASTYBIRD_RELAY8_CHANNEL_INDEX                  INDEX_NONE
#endif

// -----------------------------------------------------------------------------
// FASTYBIRD - Sensors module
// -----------------------------------------------------------------------------

#ifndef FASTYBIRD_SENSOR1_CHANNEL_INDEX
    #define FASTYBIRD_SENSOR1_CHANNEL_INDEX                 INDEX_NONE
#endif

// -----------------------------------------------------------------------------
// FASTYBIRD - Prototypes
// -----------------------------------------------------------------------------

#include <Regexp.h>

// PROPERTIES
typedef std::function<void(const uint8_t, const uint8_t, const char *)> fastybird_properties_process_set_t;
typedef std::function<void(const uint8_t, const uint8_t)> fastybird_properties_process_query_t;

typedef struct {
    String name;

    bool settable;
    bool queryable;

    String dataType;
    String unit;

    String format;

    fastybird_properties_process_set_t set_callback;
    fastybird_properties_process_query_t query_callback;
} fastybird_property_t;

// -----------------------------------------------------------------------------

// CONTROLS
typedef std::function<void(const uint8_t, const char *)> fastybird_controls_process_call_t;

typedef struct {
    String name;

    fastybird_controls_process_call_t call_callback;
} fastybird_control_t;

// -----------------------------------------------------------------------------

// CHANNELS
typedef struct {
    String name;

    // Properties mapping via array indexes
    std::vector<uint8_t> properties;
    // Controls mapping via array indexes
    std::vector<uint8_t> controls;
} fastybird_channel_t;

// -----------------------------------------------------------------------------

// DEVICES
typedef struct {
    String name;
    String hardware_model;
    String hardware_manufacturer;
    String hardware_version;
    String firmware_manufacturer;
    String firmware_version;
    bool initialized;

    // Channels mapping via array indexes
    std::vector<uint8_t> channels;
    // Properties mapping via array indexes
    std::vector<uint8_t> properties;
    // Controls mapping via array indexes
    std::vector<uint8_t> controls;
} fastybird_device_t;

// -----------------------------------------------------------------------------

typedef std::function<void()> fastybird_on_connect_callback_t;
