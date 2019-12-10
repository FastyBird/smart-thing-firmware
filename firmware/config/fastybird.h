// =============================================================================
// FASTYBIRD MQTT CONFIGURATION
// =============================================================================

#define FASTYBIRD_EMPTY_VALUE                               "none"

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
#define FASTYBIRD_PROPERTY_BUTTON                           "button"
#define FASTYBIRD_PROPERTY_SWITCH                           "switch"

#define FASTYBIRD_HARDWARE_MAC_ADDRESS                      "mac-address"

// =============================================================================
// BROADCAST TOPICS
// =============================================================================

#define FASTYBIRD_TOPIC_BROADCAST_ALERT                     "$broadcast/+"
#define FASTYBIRD_TOPIC_BROADCAST_INIT                      "$broadcast/init"

// =============================================================================
// DEVICE TOPICS
// =============================================================================

#define FASTYBIRD_TOPIC_DEVICE_NAME                         "$name"
#define FASTYBIRD_TOPIC_DEVICE_PROPERTIES_STRUCTURE         "$properties"
#define FASTYBIRD_TOPIC_DEVICE_HW_INFO                      "$hw/{hw}"
#define FASTYBIRD_TOPIC_DEVICE_FW_INFO                      "$fw/{fw}"
#define FASTYBIRD_TOPIC_DEVICE_CHANNELS                     "$channels"

#define FASTYBIRD_TOPIC_DEVICE_PROPERTY                     "{property}"
#define FASTYBIRD_TOPIC_DEVICE_PROPERTY_NAME                "{property}/$name"
#define FASTYBIRD_TOPIC_DEVICE_PROPERTY_SETTABLE            "{property}/$settable"
#define FASTYBIRD_TOPIC_DEVICE_PROPERTY_QUERYABLE           "{property}/$queryable"
#define FASTYBIRD_TOPIC_DEVICE_PROPERTY_DATA_TYPE           "{property}/$data-type"
#define FASTYBIRD_TOPIC_DEVICE_PROPERTY_FORMAT              "{property}/$format"
#define FASTYBIRD_TOPIC_DEVICE_PROPERTY_RECEIVE             "{property}/set"
#define FASTYBIRD_TOPIC_DEVICE_PROPERTY_QUERY               "{property}/query"

#define FASTYBIRD_TOPIC_DEVICE_CONTROL                      "$control"
#define FASTYBIRD_TOPIC_DEVICE_CONTROL_DATA                 "$control/{control}"
#define FASTYBIRD_TOPIC_DEVICE_CONTROL_SCHEMA               "$control/{control}/schema"
#define FASTYBIRD_TOPIC_DEVICE_CONTROL_RECEIVE              "$control/{control}/set"

// =============================================================================
// CHANNEL TOPICS
// =============================================================================

#define FASTYBIRD_TOPIC_CHANNEL_NAME		                "{channel}/$name"
#define FASTYBIRD_TOPIC_CHANNEL_PROPERTIES		            "{channel}/$properties"
#define FASTYBIRD_TOPIC_CHANNEL_ARRAY                       "{channel}/$array"

#define FASTYBIRD_TOPIC_CHANNEL_CONTROL                     "{channel}/$control"
#define FASTYBIRD_TOPIC_CHANNEL_CONTROL_DATA                "{channel}/$control/{control}"
#define FASTYBIRD_TOPIC_CHANNEL_CONTROL_SCHEMA              "{channel}/$control/{control}/schema"
#define FASTYBIRD_TOPIC_CHANNEL_CONTROL_RECEIVE             "{channel}/$control/{control}/set"

#define FASTYBIRD_TOPIC_CHANNEL_PROPERTY                    "{channel}/{property}"
#define FASTYBIRD_TOPIC_CHANNEL_PROPERTY_NAME               "{channel}/{property}/$name"
#define FASTYBIRD_TOPIC_CHANNEL_PROPERTY_TYPE               "{channel}/{property}/$type"
#define FASTYBIRD_TOPIC_CHANNEL_PROPERTY_SETTABLE           "{channel}/{property}/$settable"
#define FASTYBIRD_TOPIC_CHANNEL_PROPERTY_QUERYABLE          "{channel}/{property}/$queryable"
#define FASTYBIRD_TOPIC_CHANNEL_PROPERTY_DATA_TYPE          "{channel}/{property}/$datatype"
#define FASTYBIRD_TOPIC_CHANNEL_PROPERTY_FORMAT             "{channel}/{property}/$format"
#define FASTYBIRD_TOPIC_CHANNEL_PROPERTY_UNIT               "{channel}/{property}/$unit"
#define FASTYBIRD_TOPIC_CHANNEL_PROPERTY_RECEIVE            "{channel}/{property}/set"
#define FASTYBIRD_TOPIC_CHANNEL_PROPERTY_QUERY              "{channel}/{property}/query"

#define FASTYBIRD_CHANNEL_ARRAY_SUFFIX                      "_%d"

// =============================================================================
// DEVICE INITIALIZE SEQUENTIONS
// =============================================================================

#define FASTYBIRD_PUB_CONNECTION                            0
#define FASTYBIRD_PUB_NAME                                  1
#define FASTYBIRD_PUB_DEVICE_PROPERTIES                     2
#define FASTYBIRD_PUB_HARDWARE                              3
#define FASTYBIRD_PUB_FIRMWARE                              4
#define FASTYBIRD_PUB_CHANNELS                              5
#define FASTYBIRD_PUB_CONTROL_STRUCTURE                     6
#define FASTYBIRD_PUB_CONFIGURATION_SCHEMA                  7
#define FASTYBIRD_PUB_INITIALIZE_CHANNELS                   8
#define FASTYBIRD_PUB_READY                                 9
#define FASTYBIRD_PUB_CONFIGURATION                         10
#define FASTYBIRD_PUB_CHANNELS_CONFIGURATION                11
#define FASTYBIRD_PUB_HEARTBEAT                             12

// =============================================================================
// CHANNEL INITIALIZE SEQUENTIONS
// =============================================================================

#define FASTYBIRD_PUB_CHANNEL_NAME                          0
#define FASTYBIRD_PUB_CHANNEL_PROPERTIES                    1
#define FASTYBIRD_PUB_CHANNEL_ARRAY                         2
#define FASTYBIRD_PUB_CHANNEL_PROPERTY                      3
#define FASTYBIRD_PUB_CHANNEL_CONTROL_STRUCTURE             4
#define FASTYBIRD_PUB_CHANNEL_CONFIGURATION_SCHEMA          5
#define FASTYBIRD_PUB_CHANNEL_DONE                          6

// =============================================================================
// CHANNEL PROPERTY INITIALIZE SEQUENTIONS
// =============================================================================

#define FASTYBIRD_PUB_CHANNEL_PROPERTY_NAME                 0
#define FASTYBIRD_PUB_CHANNEL_PROPERTY_TYPE                 1
#define FASTYBIRD_PUB_CHANNEL_PROPERTY_SETABLE              2
#define FASTYBIRD_PUB_CHANNEL_PROPERTY_QUERYABLE            3
#define FASTYBIRD_PUB_CHANNEL_PROPERTY_DATA_TYPE            4
#define FASTYBIRD_PUB_CHANNEL_PROPERTY_UNITS                5
#define FASTYBIRD_PUB_CHANNEL_PROPERTY_FORMAT               6
#define FASTYBIRD_PUB_CHANNEL_PROPERTY_UNIT                 7
#define FASTYBIRD_PUB_CHANNEL_PROPERTY_DONE                 8

// =============================================================================
// DEVICE CONTROLS
// =============================================================================

#define FASTYBIRD_DEVICE_CONTROL_CONFIGURE                   "configure"
#define FASTYBIRD_DEVICE_CONTROL_REBOOT                      "reboot"
#define FASTYBIRD_DEVICE_CONTROL_FACTORY_RESET               "factory-reset"
#define FASTYBIRD_DEVICE_CONTROL_RECONNECT                   "reconnect"
#define FASTYBIRD_DEVICE_CONTROL_SEARCH_FOR_NODES            "search-nodes"
#define FASTYBIRD_DEVICE_CONTROL_DISCONNECT_NODE             "node-disconnect"

// =============================================================================
// CHANNEL CONTROLS
// =============================================================================

#define FASTYBIRD_CHANNEL_CONTROL_CONFIGURE                 "configure"

// =============================================================================
// CHANNELS NAMES
// =============================================================================

#define FASTYBIRD_CHANNEL_ANALOG_SENSOR                     "analog_sensor"
#define FASTYBIRD_CHANNEL_ANALOG_ACTOR                      "analog_actor"
#define FASTYBIRD_CHANNEL_BINARY_SENSOR                     "digital_sensor"
#define FASTYBIRD_CHANNEL_BINARY_ACTOR                      "digital_actor"
#define FASTYBIRD_CHANNEL_EVENT                             "event"
