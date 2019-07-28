// =============================================================================
// FASTYBIRD MQTT CONFIGURATION
// =============================================================================

#define FASTYBIRD_STATUS_INIT                               "init"
#define FASTYBIRD_STATUS_READY                              "ready"
#define FASTYBIRD_STATUS_DISCONNECTED                       "disconnected"
#define FASTYBIRD_STATUS_SLEEPING                           "sleeping"
#define FASTYBIRD_STATUS_LOST                               "lost"
#define FASTYBIRD_STATUS_ALERT                              "alert"

#define FASTYBIRD_SWITCH_PAYLOAD_ON                         "on"
#define FASTYBIRD_SWITCH_PAYLOAD_OFF                        "off"
#define FASTYBIRD_SWITCH_PAYLOAD_TOGGLE                     "toggle"

#define FASTYBIRD_LED_PAYLOAD_ON                            "on"
#define FASTYBIRD_LED_PAYLOAD_OFF                           "off"
#define FASTYBIRD_LED_PAYLOAD_TOGGLE                        "toggle"
#define FASTYBIRD_LED_PAYLOAD_RESTORE                       "restore"

#define FASTYBIRD_BTN_PAYLOAD_PRESS                         "press"
#define FASTYBIRD_BTN_PAYLOAD_CLICK                         "click"
#define FASTYBIRD_BTN_PAYLOAD_DBL_CLICK                     "dbl_click"
#define FASTYBIRD_BTN_PAYLOAD_TRIPLE_CLICK                  "triple_click"
#define FASTYBIRD_BTN_PAYLOAD_LNG_CLICK                     "lng_click"
#define FASTYBIRD_BTN_PAYLOAD_LNG_LNG_CLICK                 "lng_lng_click"

#define FASTYBIRD_PROPERTY_IS_SETTABLE                      "true"
#define FASTYBIRD_PROPERTY_IS_NOT_SETTABLE                  "false"

#define FASTYBIRD_PROPERTY_IS_QUERYABLE                     "true"
#define FASTYBIRD_PROPERTY_IS_NOT_QUERYABLE                 "false"

#define FASTYBIRD_PROPERTY_DATA_TYPE_FLOAT                  "float"
#define FASTYBIRD_PROPERTY_DATA_TYPE_INTEGER                "integer"
#define FASTYBIRD_PROPERTY_DATA_TYPE_BOOLEAN                "boolean"
#define FASTYBIRD_PROPERTY_DATA_TYPE_STRING                 "string"
#define FASTYBIRD_PROPERTY_DATA_TYPE_ENUM                   "enum"
#define FASTYBIRD_PROPERTY_DATA_TYPE_COLOR                  "color"

#define FASTYBIRD_PROPERTY_STATE                            "state"
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

#define FASTYBIRD_HARDWARE_MAC_ADDRESS                      "mac-address"

// =============================================================================
// THING TOPICS
// =============================================================================

#define FASTYBIRD_TOPIC_THING_NAME		                    "$name"
#define FASTYBIRD_TOPIC_THING_PROPERTIES_STRUCTURE          "$properties"
#define FASTYBIRD_TOPIC_THING_HW_INFO                       "$hw/{hw}"
#define FASTYBIRD_TOPIC_THING_FW_INFO                       "$fw/{fw}"
#define FASTYBIRD_TOPIC_THING_CHANNELS                      "$channels"

#define FASTYBIRD_TOPIC_THING_PROPERTY                      "{property}"
#define FASTYBIRD_TOPIC_THING_PROPERTY_NAME                 "{property}/$name"
#define FASTYBIRD_TOPIC_THING_PROPERTY_SETTABLE             "{property}/$settable"
#define FASTYBIRD_TOPIC_THING_PROPERTY_QUERYABLE            "{property}/$queryable"
#define FASTYBIRD_TOPIC_THING_PROPERTY_DATA_TYPE            "{property}/$data-type"
#define FASTYBIRD_TOPIC_THING_PROPERTY_FORMAT               "{property}/$format"
#define FASTYBIRD_TOPIC_THING_PROPERTY_MAPPING              "{property}/$mapping/{mapping}"
#define FASTYBIRD_TOPIC_THING_PROPERTY_RECEIVE              "{property}/set"
#define FASTYBIRD_TOPIC_THING_PROPERTY_QUERY                "{property}/query"

#define FASTYBIRD_TOPIC_THING_CONTROL                       "$control"
#define FASTYBIRD_TOPIC_THING_CONTROL_DATA                  "$control/{control}"
#define FASTYBIRD_TOPIC_THING_CONTROL_SCHEMA                "$control/{control}/schema"
#define FASTYBIRD_TOPIC_THING_CONTROL_RECEIVE               "$control/{control}/set"

// =============================================================================
// CHANNEL TOPICS
// =============================================================================

#define FASTYBIRD_TOPIC_CHANNEL_NAME		                "{channel}/$name"
#define FASTYBIRD_TOPIC_CHANNEL_TYPE                        "{channel}/$type"
#define FASTYBIRD_TOPIC_CHANNEL_PROPERTIES		            "{channel}/$properties"
#define FASTYBIRD_TOPIC_CHANNEL_ARRAY                       "{channel}/$array"

#define FASTYBIRD_TOPIC_CHANNEL_CONTROL                     "{channel}/$control"
#define FASTYBIRD_TOPIC_CHANNEL_CONTROL_DATA                "{channel}/$control/{control}"
#define FASTYBIRD_TOPIC_CHANNEL_CONTROL_SCHEMA              "{channel}/$control/{control}/schema"
#define FASTYBIRD_TOPIC_CHANNEL_CONTROL_RECEIVE             "{channel}/$control/{control}/set"

#define FASTYBIRD_TOPIC_CHANNEL_PROPERTY                    "{channel}/{property}"
#define FASTYBIRD_TOPIC_CHANNEL_PROPERTY_NAME               "{channel}/{property}/$name"
#define FASTYBIRD_TOPIC_CHANNEL_PROPERTY_SETTABLE           "{channel}/{property}/$settable"
#define FASTYBIRD_TOPIC_CHANNEL_PROPERTY_QUERYABLE          "{channel}/{property}/$queryable"
#define FASTYBIRD_TOPIC_CHANNEL_PROPERTY_DATA_TYPE          "{channel}/{property}/$data-type"
#define FASTYBIRD_TOPIC_CHANNEL_PROPERTY_FORMAT             "{channel}/{property}/$format"
#define FASTYBIRD_TOPIC_CHANNEL_PROPERTY_MAPPING            "{channel}/{property}/$mapping/{mapping}"
#define FASTYBIRD_TOPIC_CHANNEL_PROPERTY_UNIT               "{channel}/{property}/$units"
#define FASTYBIRD_TOPIC_CHANNEL_PROPERTY_RECEIVE            "{channel}/{property}/set"
#define FASTYBIRD_TOPIC_CHANNEL_PROPERTY_QUERY              "{channel}/{property}/query"

#define FASTYBIRD_CHANNEL_ARRAY_SUFFIX                      "_%d"

// =============================================================================
// THING INITIALIZE SEQUENTIONS
// =============================================================================

#define FASTYBIRD_PUB_CONNECTION                            0
#define FASTYBIRD_PUB_NAME                                  1
#define FASTYBIRD_PUB_THING_PROPERTIES                      2
#define FASTYBIRD_PUB_HARDWARE                              3
#define FASTYBIRD_PUB_FIRMWARE                              4
#define FASTYBIRD_PUB_CHANNELS                              5
#define FASTYBIRD_PUB_CONTROL_STRUCTURE                     6
#define FASTYBIRD_PUB_CONFIGURATION_SCHEMA                  7
#define FASTYBIRD_PUB_INITIALIZE_CHANNELS                   8
#define FASTYBIRD_PUB_READY                                 9
#define FASTYBIRD_PUB_CONFIGURATION                         10
#define FASTYBIRD_PUB_CHANNELS_CONFIGURATION                11
#define FASTYBIRD_PUB_CHANNELS_SCHEDULE                     12
#define FASTYBIRD_PUB_HEARTBEAT                             13

// =============================================================================
// CHANNEL INITIALIZE SEQUENTIONS
// =============================================================================

#define FASTYBIRD_PUB_CHANNEL_NAME                          0
#define FASTYBIRD_PUB_CHANNEL_TYPE                          1
#define FASTYBIRD_PUB_CHANNEL_PROPERTIES                    2
#define FASTYBIRD_PUB_CHANNEL_ARRAY                         3
#define FASTYBIRD_PUB_CHANNEL_PROPERTY_NAME                 4
#define FASTYBIRD_PUB_CHANNEL_PROPERTY_SETABLE              5
#define FASTYBIRD_PUB_CHANNEL_PROPERTY_QUERYABLE            6
#define FASTYBIRD_PUB_CHANNEL_PROPERTY_DATA_TYPE            7
#define FASTYBIRD_PUB_CHANNEL_PROPERTY_FORMAT               8
#define FASTYBIRD_PUB_CHANNEL_PROPERTY_MAPPING              9
#define FASTYBIRD_PUB_CHANNEL_PROPERTY_UNIT                 10
#define FASTYBIRD_PUB_CHANNEL_CONTROL_STRUCTURE             11
#define FASTYBIRD_PUB_CHANNEL_CONFIGURATION_SCHEMA          12
#define FASTYBIRD_PUB_CHANNEL_SCHEDULE                      13
#define FASTYBIRD_PUB_CHANNEL_DONE                          14

// =============================================================================
// THING CONTROLS
// =============================================================================

#define FASTYBIRD_THING_CONTROL_CONFIGURE                   "configure"
#define FASTYBIRD_THING_CONTROL_REBOOT                      "reboot"
#define FASTYBIRD_THING_CONTROL_FACTORY_RESET               "factory-reset"
#define FASTYBIRD_THING_CONTROL_RECONNECT                   "reconnect"
#define FASTYBIRD_THING_CONTROL_SEARCH_FOR_NODES            "nodes-search"
#define FASTYBIRD_THING_CONTROL_DISCONNECT_NODE             "node-disconnect"

// =============================================================================
// CHANNEL CONTROLS
// =============================================================================

#define FASTYBIRD_CHANNEL_CONTROL_CONFIGURE                 "configure"
#define FASTYBIRD_CHANNEL_CONTROL_SCHEDULE                  "schedule"

// =============================================================================
// CHANNELS TYPES
// =============================================================================

#define FASTYBIRD_CHANNEL_TYPE_ANALOG_SENSOR                "analog_sensor"
#define FASTYBIRD_CHANNEL_TYPE_ANALOG_ACTOR                 "analog_actor"
#define FASTYBIRD_CHANNEL_TYPE_BINARY_SENSOR                "binary_sensor"
#define FASTYBIRD_CHANNEL_TYPE_BINARY_ACTOR                 "binary_actor"
#define FASTYBIRD_CHANNEL_TYPE_BUTTON                       "button"
#define FASTYBIRD_CHANNEL_TYPE_ENERGY                       "energy"
#define FASTYBIRD_CHANNEL_TYPE_LED                          "led"
#define FASTYBIRD_CHANNEL_TYPE_LIGHT                        "light"
#define FASTYBIRD_CHANNEL_TYPE_SWITCH                       "switch"
#define FASTYBIRD_CHANNEL_TYPE_EVENT                        "event"
#define FASTYBIRD_CHANNEL_TYPE_SENSOR                       "sensor"

// =============================================================================
// CHANNELS NAMES
// =============================================================================

#define FASTYBIRD_CHANNEL_BUTTON                            "btn"
#define FASTYBIRD_CHANNEL_LED                               "led"
#define FASTYBIRD_CHANNEL_COUNTER                           "counter"
#define FASTYBIRD_CHANNEL_SWITCH                            "switch"
#define FASTYBIRD_CHANNEL_RF_LEARN                          "rf-learn"
#define FASTYBIRD_CHANNEL_CLAP                              "clap"
#define FASTYBIRD_CHANNEL_ANALOG_INPUT                      "analog-in"
#define FASTYBIRD_CHANNEL_ANALOG_OUTPUT                     "analog-out"
#define FASTYBIRD_CHANNEL_DIGITAL_INPUT                     "digital-in"
#define FASTYBIRD_CHANNEL_DIGITAL_OUTPUT                    "digital-out"
#define FASTYBIRD_CHANNEL_EVENT                             "event"
#define FASTYBIRD_CHANNEL_DISTANCE                          "distance"
#define FASTYBIRD_CHANNEL_LIGHT                             "light"
#define FASTYBIRD_CHANNEL_ENERGY                            "energy"
#define FASTYBIRD_CHANNEL_MOVE                              "security-movement"
#define FASTYBIRD_CHANNEL_ENVIRONMENT                       "env"
