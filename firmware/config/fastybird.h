// =============================================================================
// FASTYBIRD MQTT CONFIGURATION
// =============================================================================

#define FASTYBIRD_STATUS_INIT                 "init"
#define FASTYBIRD_STATUS_READY                "ready"
#define FASTYBIRD_STATUS_DISCONNECTED         "disconnected"
#define FASTYBIRD_STATUS_SLEEPING             "sleeping"
#define FASTYBIRD_STATUS_LOST                 "lost"
#define FASTYBIRD_STATUS_ALERT                "alert"

#define FASTYBIRD_SWITCH_PAYLOAD_ON           "on"
#define FASTYBIRD_SWITCH_PAYLOAD_OFF          "off"
#define FASTYBIRD_SWITCH_PAYLOAD_TOGGLE       "toggle"
#define FASTYBIRD_SWITCH_PAYLOAD_QUERY        "query"

#define FASTYBIRD_LED_PAYLOAD_ON              "on"
#define FASTYBIRD_LED_PAYLOAD_OFF             "off"
#define FASTYBIRD_LED_PAYLOAD_TOGGLE          "toggle"

#define FASTYBIRD_BTN_PAYLOAD_PRESS           "press"
#define FASTYBIRD_BTN_PAYLOAD_CLICK           "click"
#define FASTYBIRD_BTN_PAYLOAD_DBL_CLICK       "dbl_click"
#define FASTYBIRD_BTN_PAYLOAD_TRIPLE_CLICK    "triple_click"
#define FASTYBIRD_BTN_PAYLOAD_LNG_CLICK       "lng_click"
#define FASTYBIRD_BTN_PAYLOAD_LNG_LNG_CLICK   "lng_lng_click"

#define FASTYBIRD_PROPERTY_IS_SETTABLE        "true"
#define FASTYBIRD_PROPERTY_IS_NOT_SETTABLE    "false"

#define FASTYBIRD_PROPERTY_DATA_TYPE_ENUM     "enum"
#define FASTYBIRD_PROPERTY_DATA_TYPE_FLOAT    "float"
#define FASTYBIRD_PROPERTY_DATA_TYPE_INTEGER  "integer"
#define FASTYBIRD_PROPERTY_DATA_TYPE_BOOLEAN  "boolean"

#define FASTYBIRD_PROPERTY_STATE              "state"
#define FASTYBIRD_PROPERTY_IP_ADDRESS         "ip-address"
#define FASTYBIRD_PROPERTY_STATUS_LED         "status-led"
#define FASTYBIRD_PROPERTY_TEMPERATURE        "temperature"
#define FASTYBIRD_PROPERTY_HUMIDITY           "humidity"
#define FASTYBIRD_PROPERTY_NOISE_LEVEL        "noise-level"
#define FASTYBIRD_PROPERTY_LIGHT_LEVEL        "light-level"
#define FASTYBIRD_PROPERTY_AIR_QUALITY_LEVEL  "air-quality-level"
#define FASTYBIRD_PROPERTY_ENERGY_DELTA       "energy-delta"
#define FASTYBIRD_PROPERTY_ENERGY_TOTAL       "energy-total"
#define FASTYBIRD_PROPERTY_CURRENT            "current"
#define FASTYBIRD_PROPERTY_VOLTAGE            "voltage"
#define FASTYBIRD_PROPERTY_ACTIVE_POWER       "active-power"
#define FASTYBIRD_PROPERTY_REACTIVE_POWER     "reactive-power"
#define FASTYBIRD_PROPERTY_APPARENT_POWER     "apparent-power"
#define FASTYBIRD_PROPERTY_POWER_FACTOR       "power-factor"
#define FASTYBIRD_PROPERTY_VALUE              "value"

#define FASTYBIRD_STAT_INTERVAL               "interval"
#define FASTYBIRD_STAT_UPTIME                 "uptime"
#define FASTYBIRD_STAT_FREE_HEAP              "free-heap"
#define FASTYBIRD_STAT_CPU_LOAD               "cpu-load"
#define FASTYBIRD_STAT_VCC                    "vcc"
#define FASTYBIRD_STAT_SSID                   "ssid"
#define FASTYBIRD_STAT_RSSI                   "rssi"

#define FASTYBIRD_HARDWARE_MAC_ADDRESS        "mac-address"

#define FASTYBIRD_ENABLE_CONFIGURATION        true
#define FASTYBIRD_ENABLE_RESET                true
#define FASTYBIRD_ENABLE_RECONNECT            true
#define FASTYBIRD_ENABLE_FACTORY_RESET        true

// =============================================================================
// THING TOPICS
// =============================================================================

#define FASTYBIRD_TOPIC_THING_NAME		            "$name"
#define FASTYBIRD_TOPIC_THING_PROPERTIES		    "$properties"
#define FASTYBIRD_TOPIC_THING_HW_INFO               "$hw/{hw}"
#define FASTYBIRD_TOPIC_THING_FW_INFO               "$fw/{fw}"
#define FASTYBIRD_TOPIC_THING_CHANNELS              "$channels"
#define FASTYBIRD_TOPIC_THING_STATS_STRUCTURE       "$stats"
#define FASTYBIRD_TOPIC_THING_STATS                 "$stats/{stats}"

#define FASTYBIRD_TOPIC_THING_PROPERTY              "{property}"

#define FASTYBIRD_TOPIC_THING_CONTROL               "$control"
#define FASTYBIRD_TOPIC_THING_CONTROL_DATA          "$control/{control}"
#define FASTYBIRD_TOPIC_THING_CONTROL_SCHEMA        "$control/{control}/schema"
#define FASTYBIRD_TOPIC_THING_CONTROL_RECEIVE       "$control/{control}/set"

// =============================================================================
// CHANNEL TOPICS
// =============================================================================

#define FASTYBIRD_TOPIC_CHANNEL_NAME		        "{channel}/$name"
#define FASTYBIRD_TOPIC_CHANNEL_TYPE		        "{channel}/$type"
#define FASTYBIRD_TOPIC_CHANNEL_PROPERTIES		    "{channel}/$properties"
#define FASTYBIRD_TOPIC_CHANNEL_ARRAY               "{channel}/$array"

#define FASTYBIRD_TOPIC_CHANNEL_CONTROL             "{channel}/$control"
#define FASTYBIRD_TOPIC_CHANNEL_CONTROL_DATA        "{channel}/$control/{control}"
#define FASTYBIRD_TOPIC_CHANNEL_CONTROL_SCHEMA      "{channel}/$control/{control}/schema"
#define FASTYBIRD_TOPIC_CHANNEL_CONTROL_RECEIVE     "{channel}/$control/{control}/set"

#define FASTYBIRD_TOPIC_CHANNEL_PROPERTY            "{channel}/{property}"
#define FASTYBIRD_TOPIC_CHANNEL_PROPERTY_NAME       "{channel}/{property}/$name"
#define FASTYBIRD_TOPIC_CHANNEL_PROPERTY_SETTABLE   "{channel}/{property}/$settable"
#define FASTYBIRD_TOPIC_CHANNEL_PROPERTY_DATA_TYPE  "{channel}/{property}/$data-type"
#define FASTYBIRD_TOPIC_CHANNEL_PROPERTY_FORMAT     "{channel}/{property}/$format"
#define FASTYBIRD_TOPIC_CHANNEL_PROPERTY_MAPPING    "{channel}/{property}/$mapping/{mapping}"
#define FASTYBIRD_TOPIC_CHANNEL_PROPERTY_UNIT       "{channel}/{property}/$units"
#define FASTYBIRD_TOPIC_CHANNEL_PROPERTY_RECEIVE    "{channel}/{property}/set"

// =============================================================================
// CHANNELS NAMES
// =============================================================================

#define FASTYBIRD_CHANNEL_BUTTON                    "btn"
#define FASTYBIRD_CHANNEL_LED                       "led"
#define FASTYBIRD_CHANNEL_COUNTER                   "counter"
#define FASTYBIRD_CHANNEL_RELAY                     "relay"
#define FASTYBIRD_CHANNEL_RF_LEARN                  "rf-learn"
#define FASTYBIRD_CHANNEL_CLAP                      "clap"
#define FASTYBIRD_CHANNEL_ANALOG_INPUT              "analog-in"
#define FASTYBIRD_CHANNEL_DIGITAL_INPUT             "digital-in"
#define FASTYBIRD_CHANNEL_EVENTS                    "events"
#define FASTYBIRD_CHANNEL_DISTANCE                  "distance"
#define FASTYBIRD_CHANNEL_LIGHT                     "light"
#define FASTYBIRD_CHANNEL_ENERGY                    "energy"
#define FASTYBIRD_CHANNEL_MOVE                      "security-movement"
#define FASTYBIRD_CHANNEL_ENVIRONMENT               "env"

#define FASTYBIRD_CHANNEL_ARRAY_SUFFIX              "_%d"

// =============================================================================
// THING INITIALIZE SEQUENTIONS
// =============================================================================

#define FASTYBIRD_PUB_CONNECTION                            0
#define FASTYBIRD_PUB_NAME                                  1
#define FASTYBIRD_PUB_HARDWARE                              2
#define FASTYBIRD_PUB_FIRMWARE                              3
#define FASTYBIRD_PUB_CHANNELS                              4
#define FASTYBIRD_PUB_STATS                                 5
#define FASTYBIRD_PUB_CONTROL_STRUCTURE                     6
#define FASTYBIRD_PUB_CONFIGURATION_SCHEMA                  7
#define FASTYBIRD_PUB_INITIALIZE_CHANNELS                   8
#define FASTYBIRD_PUB_READY                                 9
#define FASTYBIRD_PUB_CONFIGURATION                         10
#define FASTYBIRD_PUB_CHANNELS_CONFIGURATION                11
#define FASTYBIRD_PUB_CHANNELS_DIRECT_CONTROL               12
#define FASTYBIRD_PUB_CHANNELS_SCHEDULE                     13
#define FASTYBIRD_PUB_HEARTBEAT                             14

// =============================================================================
// CHANNEL INITIALIZE SEQUENTIONS
// =============================================================================

#define FASTYBIRD_PUB_CHANNEL_NAME                          0
#define FASTYBIRD_PUB_CHANNEL_TYPE                          1
#define FASTYBIRD_PUB_CHANNEL_PROPERTIES                    2
#define FASTYBIRD_PUB_CHANNEL_ARRAY                         3
#define FASTYBIRD_PUB_CHANNEL_PROPERTY_NAME                 4
#define FASTYBIRD_PUB_CHANNEL_PROPERTY_SETABLE              5
#define FASTYBIRD_PUB_CHANNEL_PROPERTY_DATA_TYPE            6
#define FASTYBIRD_PUB_CHANNEL_PROPERTY_FORMAT               7
#define FASTYBIRD_PUB_CHANNEL_PROPERTY_MAPPING              8
#define FASTYBIRD_PUB_CHANNEL_PROPERTY_UNIT                 9
#define FASTYBIRD_PUB_CHANNEL_CONTROL_STRUCTURE             10
#define FASTYBIRD_PUB_CHANNEL_CONFIGURATION_SCHEMA          11
#define FASTYBIRD_PUB_CHANNEL_DIRECT_CONTROL                12
#define FASTYBIRD_PUB_CHANNEL_SCHEDULE                      13
#define FASTYBIRD_PUB_CHANNEL_DONE                          14

// =============================================================================
// THING CONTROLS
// =============================================================================

#define FASTYBIRD_THING_CONTROL_CONFIGURATION               "config"
#define FASTYBIRD_THING_CONTROL_RESET                       "reset"
#define FASTYBIRD_THING_CONTROL_RECONNECT                   "reconnect"
#define FASTYBIRD_THING_CONTROL_FACTORY_RESET               "factory-reset"
#define FASTYBIRD_THING_CONTROL_VALUE_OTA                   "ota"

#define FASTYBIRD_THING_CONTROL_VALUE_CONFIGURATION         "config"
#define FASTYBIRD_THING_CONTROL_VALUE_RESET                 "reset"
#define FASTYBIRD_THING_CONTROL_VALUE_RECONNECT             "reconnect"
#define FASTYBIRD_THING_CONTROL_VALUE_FACTORY_RESET         "factory-reset"
#define FASTYBIRD_THING_CONTROL_VALUE_OTA                   "ota"

// =============================================================================
// CHANNEL CONTROLS
// =============================================================================

#define FASTYBIRD_CHANNEL_CONTROL_CONFIGURATION             "config"
#define FASTYBIRD_CHANNEL_CONTROL_DIRECT_CONTROL            "direct-control"
#define FASTYBIRD_CHANNEL_CONTROL_SCHEDULER                 "scheduler"

#define FASTYBIRD_CHANNEL_CONTROL_VALUE_CONFIGURATION       "config"
#define FASTYBIRD_CHANNEL_CONTROL_VALUE_DIRECT_CONTROL      "direct-controls"
#define FASTYBIRD_CHANNEL_CONTROL_VALUE_SCHEDULER           "schedules"

#define FASTYBIRD_DIRECT_CONTROL_EXPRESSION_EQ              "eq"
#define FASTYBIRD_DIRECT_CONTROL_EXPRESSION_NEQ             "neq"
#define FASTYBIRD_DIRECT_CONTROL_EXPRESSION_GT              "gt"
#define FASTYBIRD_DIRECT_CONTROL_EXPRESSION_LT              "lt"
#define FASTYBIRD_DIRECT_CONTROL_EXPRESSION_GTE             "gte"
#define FASTYBIRD_DIRECT_CONTROL_EXPRESSION_LTE             "lte"

// =============================================================================
// CHANNELS TYPES
// =============================================================================

#define FASTYBIRD_CHANNEL_TYPE_ANALOG_SENSOR                "analog_sensor"
#define FASTYBIRD_CHANNEL_TYPE_BINARY_SENSOR                "binary_sensor"
#define FASTYBIRD_CHANNEL_TYPE_BUTTON                       "button"
#define FASTYBIRD_CHANNEL_TYPE_ENERGY                       "energy"
#define FASTYBIRD_CHANNEL_TYPE_LED                          "led"
#define FASTYBIRD_CHANNEL_TYPE_LIGHT                        "light"
#define FASTYBIRD_CHANNEL_TYPE_SWITCH                       "switch"
