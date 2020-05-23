/*

FASTYBIRD MODULE CONFIGURATION

Copyright (C) 2018 FastyBird Ltd. <info@fastybird.com>

*/

#pragma once

//------------------------------------------------------------------------------
// FASTYBIRD - General data
//------------------------------------------------------------------------------

#ifndef FASTYBIRD_SUPPORT
    #define FASTYBIRD_SUPPORT                               1                       // Enable FastyBird connection by default
#endif

#ifndef FASTYBIRD_NODES_SUPPORT
    #define FASTYBIRD_NODES_SUPPORT                         0                       // Child nodes support dissabled by default
#endif

#ifndef FASTYBIRD_MAX_CHANNELS
    #define FASTYBIRD_MAX_CHANNELS                          0                       // Maximum device channels count
#endif

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

#define FASTYBIRD_SWITCH_PAYLOAD_ON                         "on"
#define FASTYBIRD_SWITCH_PAYLOAD_OFF                        "off"
#define FASTYBIRD_SWITCH_PAYLOAD_TOGGLE                     "toggle"

#define FASTYBIRD_BTN_PAYLOAD_PRESS                         "press"
#define FASTYBIRD_BTN_PAYLOAD_CLICK                         "click"
#define FASTYBIRD_BTN_PAYLOAD_DBL_CLICK                     "dbl_click"
#define FASTYBIRD_BTN_PAYLOAD_TRIPLE_CLICK                  "triple_click"
#define FASTYBIRD_BTN_PAYLOAD_LNG_CLICK                     "lng_click"
#define FASTYBIRD_BTN_PAYLOAD_LNG_LNG_CLICK                 "lng_lng_click"

#define FASTYBIRD_PROPERTY_DATA_TYPE_FLOAT                  "float"
#define FASTYBIRD_PROPERTY_DATA_TYPE_INTEGER                "integer"
#define FASTYBIRD_PROPERTY_DATA_TYPE_BOOLEAN                "boolean"
#define FASTYBIRD_PROPERTY_DATA_TYPE_STRING                 "string"
#define FASTYBIRD_PROPERTY_DATA_TYPE_ENUM                   "enum"
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
#define FASTYBIRD_PROPERTY_STATE                            "state"

#define FASTYBIRD_HARDWARE_MAC_ADDRESS                      "mac-address"

#define FASTYBIRD_CHANNEL_ARRAY_SUFFIX                      "-%d"

//------------------------------------------------------------------------------
// FASTYBIRD - Device initialization sequentions
//------------------------------------------------------------------------------

#define FASTYBIRD_PUB_CONNECTION                            0
#define FASTYBIRD_PUB_NAME                                  1
#define FASTYBIRD_PUB_PARENT                                2
#define FASTYBIRD_PUB_DEVICE_PROPERTIES                     3
#define FASTYBIRD_PUB_HARDWARE                              4
#define FASTYBIRD_PUB_FIRMWARE                              5
#define FASTYBIRD_PUB_CHANNELS                              6
#define FASTYBIRD_PUB_CONTROL_STRUCTURE                     7
#define FASTYBIRD_PUB_CONFIGURATION_SCHEMA                  8
#define FASTYBIRD_PUB_INITIALIZE_CHANNELS                   9
#define FASTYBIRD_PUB_READY                                 10
#define FASTYBIRD_PUB_CONFIGURATION                         11
#define FASTYBIRD_PUB_CHANNELS_CONFIGURATION                12
#define FASTYBIRD_PUB_HEARTBEAT                             13

//------------------------------------------------------------------------------
// FASTYBIRD - Channel initialization sequentions
//------------------------------------------------------------------------------

#define FASTYBIRD_PUB_CHANNEL_NAME                          0
#define FASTYBIRD_PUB_CHANNEL_PROPERTIES                    1
#define FASTYBIRD_PUB_CHANNEL_PROPERTY                      2
#define FASTYBIRD_PUB_CHANNEL_CONTROL_STRUCTURE             3
#define FASTYBIRD_PUB_CHANNEL_CONFIGURATION_SCHEMA          4
#define FASTYBIRD_PUB_CHANNEL_DONE                          5

//------------------------------------------------------------------------------
// FASTYBIRD - Property initialization sequentions
//------------------------------------------------------------------------------

#define FASTYBIRD_PUB_PROPERTY_NAME                         0
#define FASTYBIRD_PUB_PROPERTY_SETABLE                      1
#define FASTYBIRD_PUB_PROPERTY_QUERYABLE                    2
#define FASTYBIRD_PUB_PROPERTY_DATA_TYPE                    3
#define FASTYBIRD_PUB_PROPERTY_UNIT                         4
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
#define FASTYBIRD_DEVICE_CONTROL_SEARCH_FOR_NODES            "search-nodes"
#define FASTYBIRD_DEVICE_CONTROL_DISCONNECT_NODE             "node-disconnect"

//------------------------------------------------------------------------------
// FASTYBIRD - Channel controls
//------------------------------------------------------------------------------

#define FASTYBIRD_CHANNEL_CONTROL_CONFIGURE                 "configure"

//------------------------------------------------------------------------------
// FASTYBIRD - Channels names
//------------------------------------------------------------------------------

#define FASTYBIRD_CHANNEL_ANALOG_SENSOR                     "analog-sensor"
#define FASTYBIRD_CHANNEL_ANALOG_ACTOR                      "analog-actor"
#define FASTYBIRD_CHANNEL_BINARY_SENSOR                     "digital-sensor"
#define FASTYBIRD_CHANNEL_BINARY_ACTOR                      "digital-actor"
#define FASTYBIRD_CHANNEL_EVENT                             "event"

//------------------------------------------------------------------------------
// FASTYBIRD - Default channels
//------------------------------------------------------------------------------

#ifndef FASTYBIRD_CHANNEL1_NAME
    #define FASTYBIRD_CHANNEL1_NAME                         "channel-1"
#endif

#ifndef FASTYBIRD_CHANNEL2_NAME
    #define FASTYBIRD_CHANNEL2_NAME                         "channel-2"
#endif

#ifndef FASTYBIRD_CHANNEL3_NAME
    #define FASTYBIRD_CHANNEL3_NAME                         "channel-3"
#endif

#ifndef FASTYBIRD_CHANNEL4_NAME
    #define FASTYBIRD_CHANNEL4_NAME                         "channel-4"
#endif

#ifndef FASTYBIRD_CHANNEL5_NAME
    #define FASTYBIRD_CHANNEL5_NAME                         "channel-5"
#endif

#ifndef FASTYBIRD_CHANNEL6_NAME
    #define FASTYBIRD_CHANNEL6_NAME                         "channel-6"
#endif

#ifndef FASTYBIRD_CHANNEL7_NAME
    #define FASTYBIRD_CHANNEL7_NAME                         "channel-7"
#endif

#ifndef FASTYBIRD_CHANNEL8_NAME
    #define FASTYBIRD_CHANNEL8_NAME                         "channel-8"
#endif

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

#define FASTYBIRD_TOPIC_DEVICE_NAME                         "$name"
#define FASTYBIRD_TOPIC_DEVICE_PARENT                       "$parent"
#define FASTYBIRD_TOPIC_DEVICE_PROPERTIES_STRUCTURE         "$properties"
#define FASTYBIRD_TOPIC_DEVICE_HW_INFO                      "$hw/{hw}"
#define FASTYBIRD_TOPIC_DEVICE_FW_INFO                      "$fw/{fw}"
#define FASTYBIRD_TOPIC_DEVICE_CHANNELS                     "$channels"
#define FASTYBIRD_TOPIC_DEVICE_STATE                        "$state"

#define FASTYBIRD_TOPIC_DEVICE_PROPERTY                     "$property/{property}"
#define FASTYBIRD_TOPIC_DEVICE_PROPERTY_NAME                "$property/{property}/$name"
#define FASTYBIRD_TOPIC_DEVICE_PROPERTY_SETTABLE            "$property/{property}/$settable"
#define FASTYBIRD_TOPIC_DEVICE_PROPERTY_QUERYABLE           "$property/{property}/$queryable"
#define FASTYBIRD_TOPIC_DEVICE_PROPERTY_DATA_TYPE           "$property/{property}/$data-type"
#define FASTYBIRD_TOPIC_DEVICE_PROPERTY_FORMAT              "$property/{property}/$format"
#define FASTYBIRD_TOPIC_DEVICE_PROPERTY_RECEIVE             "$property/{property}/set"
#define FASTYBIRD_TOPIC_DEVICE_PROPERTY_QUERY               "$property/{property}/query"

#define FASTYBIRD_TOPIC_DEVICE_CONTROL                      "$control"
#define FASTYBIRD_TOPIC_DEVICE_CONTROL_DATA                 "$control/{control}"
#define FASTYBIRD_TOPIC_DEVICE_CONTROL_SCHEMA               "$control/{control}/$schema"
#define FASTYBIRD_TOPIC_DEVICE_CONTROL_RECEIVE              "$control/{control}/set"

// -----------------------------------------------------------------------------
// FASTYBIRD - MQTT API channel topics
// -----------------------------------------------------------------------------

#define FASTYBIRD_TOPIC_CHANNEL_NAME		                "$channel/{channel}/$name"
#define FASTYBIRD_TOPIC_CHANNEL_PROPERTIES		            "$channel/{channel}/$properties"

#define FASTYBIRD_TOPIC_CHANNEL_CONTROL                     "$channel/{channel}/$control"
#define FASTYBIRD_TOPIC_CHANNEL_CONTROL_DATA                "$channel/{channel}/$control/{control}"
#define FASTYBIRD_TOPIC_CHANNEL_CONTROL_SCHEMA              "$channel/{channel}/$control/{control}/$schema"
#define FASTYBIRD_TOPIC_CHANNEL_CONTROL_RECEIVE             "$channel/{channel}/$control/{control}/set"

#define FASTYBIRD_TOPIC_CHANNEL_PROPERTY                    "$channel/{channel}/$property/{property}"
#define FASTYBIRD_TOPIC_CHANNEL_PROPERTY_NAME               "$channel/{channel}/$property/{property}/$name"
#define FASTYBIRD_TOPIC_CHANNEL_PROPERTY_SETTABLE           "$channel/{channel}/$property/{property}/$settable"
#define FASTYBIRD_TOPIC_CHANNEL_PROPERTY_QUERYABLE          "$channel/{channel}/$property/{property}/$queryable"
#define FASTYBIRD_TOPIC_CHANNEL_PROPERTY_DATA_TYPE          "$channel/{channel}/$property/{property}/$datatype"
#define FASTYBIRD_TOPIC_CHANNEL_PROPERTY_FORMAT             "$channel/{channel}/$property/{property}/$format"
#define FASTYBIRD_TOPIC_CHANNEL_PROPERTY_UNIT               "$channel/{channel}/$property/{property}/$unit"
#define FASTYBIRD_TOPIC_CHANNEL_PROPERTY_RECEIVE            "$channel/{channel}/$property/{property}/set"
#define FASTYBIRD_TOPIC_CHANNEL_PROPERTY_QUERY              "$channel/{channel}/$property/{property}/query"

// -----------------------------------------------------------------------------
// FASTYBIRD - MQTT API in topic part count
// -----------------------------------------------------------------------------

#define FASTYBIRD_TOPIC_PART_COUNT_BROADCAST                4
#define FASTYBIRD_TOPIC_PART_COUNT_DEVICE_CONTROL           6
#define FASTYBIRD_TOPIC_PART_COUNT_CHANNEL_PROPERTY         8
#define FASTYBIRD_TOPIC_PART_COUNT_CHANNEL_CONTROL          8

// -----------------------------------------------------------------------------
// FASTYBIRD - MQTT API in topic part positions
// -----------------------------------------------------------------------------

#define FASTYBIRD_TOPIC_POSITION_BROADCAST_PREFIX           2
#define FASTYBIRD_TOPIC_POSITION_BROADCAST_ACTION           3
#define FASTYBIRD_TOPIC_POSITION_DEVICE                     2
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

#if FASTYBIRD_SUPPORT
    #include <Regexp.h>

    // PROPERTIES
    typedef std::function<void(const uint8_t, const uint8_t, const char *)> fastybird_properties_process_payload_f;
    typedef std::function<void(const uint8_t, const uint8_t)> fastybird_properties_process_query_f;

    typedef struct {
        String name;

        bool settable;
        bool queryable;

        String datatype;
        String unit;

        String format;

        fastybird_properties_process_payload_f payload_callback;
        fastybird_properties_process_query_f query_callback;
    } fastybird_property_t;

// -----------------------------------------------------------------------------

    // CHANNELS
    typedef std::function<void(const uint8_t, JsonArray&)> fastybird_on_report_channel_configuration_schema_callback_f;
    typedef std::function<void(const uint8_t, JsonObject&)> fastybird_on_report_channel_configuration_callback_f;
    typedef std::function<void(const uint8_t, JsonObject&)> fastybird_on_channel_configure_callback_f;

    typedef struct {
        String name;

        // Channel configuration callbacks mapping via array indexes
        std::vector<uint8_t> configurationSchemaCallbacks;
        std::vector<uint8_t> configurationCallbacks;
        std::vector<uint8_t> configureCallbacks;

        // Properties mapping via array indexes
        std::vector<uint8_t> properties;
    } fastybird_channel_t;

// -----------------------------------------------------------------------------

    // DEVICE
    typedef std::function<void(JsonArray&)> fastybird_on_report_configuration_schema_callback_f;
    typedef std::function<void(JsonObject&)> fastybird_on_report_configuration_callback_f;
    typedef std::function<void(JsonObject&)> fastybird_on_configure_callback_f;
    typedef std::function<void(const char *)> fastybird_control_callback_f;
    typedef std::function<void()> fastybird_on_connect_callback_f;

    typedef struct {
        fastybird_control_callback_f callback;
        String controlName;
    } fastybird_on_control_callback_t;

    void fastybirdReportConfigurationSchemaRegister(fastybird_on_report_configuration_schema_callback_f callback);
    void fastybirdReportConfigurationRegister(fastybird_on_report_configuration_callback_f callback);
    void fastybirdOnConfigureRegister(fastybird_on_configure_callback_f callback);
    void fastybirdOnControlRegister(fastybird_control_callback_f callback, String controlName);

// -----------------------------------------------------------------------------

    #if FASTYBIRD_NODES_SUPPORT
        // PROPERTIES
        typedef std::function<void(const uint8_t, const uint8_t, const uint8_t, const char *)> fastybird_node_properties_process_payload_f;
        typedef std::function<void(const uint8_t, const uint8_t, const uint8_t)> fastybird_node_properties_process_query_f;

        typedef struct {
            String name;

            bool settable;
            bool queryable;

            String datatype;
            String unit;

            String format;

            fastybird_node_properties_process_payload_f payload_callback;
            fastybird_node_properties_process_query_f query_callback;
        } fastybird_node_property_t;

// -----------------------------------------------------------------------------

        // CHANNELS
        typedef struct {
            String name;

            // Properties mapping via array indexes
            std::vector<uint8_t> properties;
        } fastybird_node_channel_t;
        
// -----------------------------------------------------------------------------

        // HARDWARE INFO
        typedef struct {
            char model[20];
            char version[20];
            char manufacturer[20];
        } fastybird_node_hardware_t;

// -----------------------------------------------------------------------------

        // FIRMWARE INFO
        typedef struct {
            char name[20];
            char version[20];
            char manufacturer[20];
        } fastybird_node_firmware_t;

// -----------------------------------------------------------------------------

        // NODE STRUCTURE
        typedef struct {
            char id[15]; // Representing node serial number

            fastybird_node_hardware_t hardware;
            fastybird_node_firmware_t firmware;

            bool initialized;
            bool disabled;
            bool ready;

            // Node channels
            std::vector<uint8_t> channels;
        } fastybird_node_t;
    #else
        #define fastybird_node_t void *
        #define fastybird_node_channel_t void *
        #define fastybird_node_property_t void *

        #define fastybird_node_properties_process_payload_f void *
        #define fastybird_node_properties_process_query_f void *
    #endif // FASTYBIRD_NODES_SUPPORT
#endif
