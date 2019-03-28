#include <Arduino.h>
#include <ArduinoJson.h>
#include <functional>
#include <vector>
#include <pgmspace.h>
#include <core_version.h>

extern "C" {
    #include "user_interface.h"
}

// -----------------------------------------------------------------------------
// Debug
// -----------------------------------------------------------------------------
void debugSend(PGM_P format, ...);

extern "C" {
     void custom_crash_callback(struct rst_info*, uint32_t, uint32_t);
}

// Core version 2.4.2 and higher changed the cont_t structure to a pointer:
// https://github.com/esp8266/Arduino/commit/5d5ea92a4d004ab009d5f642629946a0cb8893dd#diff-3fa12668b289ccb95b7ab334833a4ba8L35
// Core version 2.5.0 introduced EspClass helper method:
// https://github.com/esp8266/Arduino/commit/0e0e34c614fe8a47544c9998201b1d9b3c24eb18
extern "C" {
    #include <cont.h>

    #if defined(ARDUINO_ESP8266_RELEASE_2_3_0) \
        || defined(ARDUINO_ESP8266_RELEASE_2_4_0) \
        || defined(ARDUINO_ESP8266_RELEASE_2_4_1)
        extern cont_t g_cont;
        #define getFreeStack() cont_get_free_stack(&g_cont)
    #elif defined(ARDUINO_ESP8266_RELEASE_2_4_2)
        extern cont_t* g_pcont;
        #define getFreeStack() cont_get_free_stack(g_pcont)
    #else
        #define getFreeStack() ESP.getFreeContStack()
    #endif
}

// -----------------------------------------------------------------------------
// EEPROM_ROTATE
// -----------------------------------------------------------------------------
#include <EEPROM_Rotate.h>

EEPROM_Rotate EEPROMr;

// -----------------------------------------------------------------------------
// GPIO
// -----------------------------------------------------------------------------
bool gpioValid(unsigned int gpio);
bool gpioGetLock(unsigned int gpio);
bool gpioReleaseLock(unsigned int gpio);

// -----------------------------------------------------------------------------
// I2C
// -----------------------------------------------------------------------------
void i2cScan();
void i2cClearBus();
bool i2cGetLock(unsigned int address);
bool i2cReleaseLock(unsigned int address);
unsigned int i2cFindAndLock(size_t size, unsigned int * addresses);

void i2c_wakeup(uint8_t address);
uint8_t i2c_write_buffer(uint8_t address, uint8_t * buffer, size_t len);
uint8_t i2c_write_uint8(uint8_t address, uint8_t value);
uint8_t i2c_write_uint8(uint8_t address, uint8_t reg, uint8_t value);
uint8_t i2c_write_uint8(uint8_t address, uint8_t reg, uint8_t value1, uint8_t value2);
uint8_t i2c_write_uint16(uint8_t address, uint16_t value);
uint8_t i2c_write_uint16(uint8_t address, uint8_t reg, uint16_t value);
uint8_t i2c_read_uint8(uint8_t address);
uint8_t i2c_read_uint8(uint8_t address, uint8_t reg);
uint16_t i2c_read_uint16(uint8_t address);
uint16_t i2c_read_uint16(uint8_t address, uint8_t reg);
uint16_t i2c_read_uint16_le(uint8_t address, uint8_t reg);
int16_t i2c_read_int16(uint8_t address, uint8_t reg);
int16_t i2c_read_int16_le(uint8_t address, uint8_t reg);
void i2c_read_buffer(uint8_t address, uint8_t * buffer, size_t len);

// -----------------------------------------------------------------------------
// OTA
// -----------------------------------------------------------------------------
#include "ESPAsyncTCP.h"

// -----------------------------------------------------------------------------
// Utils
// -----------------------------------------------------------------------------
char * ltrim(char * s);
void niceDelay(unsigned long ms);

#define ARRAYINIT(type, name, ...) type name[] = {__VA_ARGS__};

// -----------------------------------------------------------------------------
// WebServer
// -----------------------------------------------------------------------------
#if WEB_SUPPORT
    #include <ESPAsyncWebServer.h>
    AsyncWebServer * webServer();

    typedef std::function<bool(AsyncWebServerRequest * request)> web_on_request_callback_f;
    void webOnRequestRegister(web_on_request_callback_f callback);
#else
    #define AsyncWebServerRequest void
    #define ArRequestHandlerFunction void
    #define AsyncWebSocketClient void
    #define AsyncWebSocket void
    #define AwsEventType void *

    #define web_on_request_callback_f void *
#endif

// -----------------------------------------------------------------------------
// WebSockets
// -----------------------------------------------------------------------------
#if WEB_SUPPORT && WS_SUPPORT
    typedef std::function<void(JsonObject&)> ws_on_connect_callback_f;
    void wsOnConnectRegister(ws_on_connect_callback_f callback);

    typedef std::function<void(JsonObject&)> ws_on_update_callback_f;
    void wsOnUpdateRegister(ws_on_update_callback_f callback);

    typedef std::function<void(uint32_t, const char *, JsonObject&)> ws_on_action_callback_f;
    void wsOnActionRegister(ws_on_action_callback_f callback);

    typedef std::function<void(uint32_t, JsonObject&)> ws_on_configure_callback_f;
    void wsOnConfigureRegister(ws_on_configure_callback_f callback);
#else
    #define ws_on_connect_callback_f void *
    #define ws_on_update_callback_f void *
    #define ws_on_action_callback_f void *
    #define ws_on_configure_callback_f void *
#endif

// -----------------------------------------------------------------------------
// WIFI
// -----------------------------------------------------------------------------
#if WIFI_SUPPORT
    #include "JustWifi.h"

    typedef std::function<void(justwifi_messages_t code, char * parameter)> wifi_callback_f;
    void wifiRegister(wifi_callback_f callback);
#else
    #define wifi_callback_f void *
#endif

// -----------------------------------------------------------------------------
// MQTT
// -----------------------------------------------------------------------------
#if MQTT_SUPPORT
    typedef std::function<void(const char *, const char *)> mqtt_on_message_callback_f;

    typedef std::function<void()> mqtt_on_connect_callback_f;
    void mqttOnConnectRegister(mqtt_on_connect_callback_f callback);

    typedef std::function<void()> mqtt_on_disconnect_callback_f;
    void mqttOnDisconnectRegister(mqtt_on_disconnect_callback_f callback);
#else
    #define mqtt_on_message_callback_f void *
    #define mqtt_on_connect_callback_f void *
    #define mqtt_on_disconnect_callback_f void *
#endif

// -----------------------------------------------------------------------------
// Settings
// -----------------------------------------------------------------------------
#include <Embedis.h>

template<typename T> bool setSetting(const String& key, T value);
template<typename T> bool setSetting(const String& key, unsigned int index, T value);
template<typename T> String getSetting(const String& key, T defaultValue);
template<typename T> String getSetting(const String& key, unsigned int index, T defaultValue);

// -----------------------------------------------------------------------------
// System
// -----------------------------------------------------------------------------

typedef std::function<void()> system_on_heartbeat_callback_f;
void systemOnHeartbeatRegister(system_on_heartbeat_callback_f callback);

// -----------------------------------------------------------------------------
// Button
// -----------------------------------------------------------------------------
#if BUTTON_SUPPORT
    #include <DebounceEvent.h>

    typedef std::function<void(unsigned int)> button_on_event_callback_f;
    void buttonOnEventRegister(button_on_event_callback_f callback, unsigned int button);
#else
    #define button_on_event_callback_f void *
#endif

// -----------------------------------------------------------------------------
// FastyBird
// -----------------------------------------------------------------------------
#if FASTYBIRD_SUPPORT
    typedef std::function<void(unsigned int, const char *)> fastybird_channels_process_payload_f;

    typedef std::function<JsonArray&()> fastybird_channels_confiuration_schema_f;
    typedef std::function<void(unsigned int, JsonObject&)> fastybird_channels_configure_f;
    typedef std::function<void(unsigned int, JsonArray&)> fastybird_channels_confiure_direct_controls_f;
    typedef std::function<void(unsigned int, JsonArray&)> fastybird_channels_confiure_schedules_f;

    typedef struct {
        const char * from;
        const char * to;
    } fastybird_channel_property_mapping_t;

    typedef struct {
        const char * type;
        const char * name;

        bool settable;

        const char * dataType;

        std::vector<String> format;
        std::vector<fastybird_channel_property_mapping_t> mappings;

        fastybird_channels_process_payload_f payloadCallback;
    } fastybird_channel_property_t;

    typedef struct {
        const char * name;
        const char * type;
        unsigned int length;

        bool isConfigurable;
        bool hasDirectControl;
        bool hasScheduler;

        bool initialized;

        String configurationSchema;

        fastybird_channels_configure_f configureCallback;
        fastybird_channels_confiure_direct_controls_f configureDirectControlsCallback;
        fastybird_channels_confiure_schedules_f configureSchedulesCallback;

        std::vector<fastybird_channel_property_t> properties;
    } fastybird_channel_t;

    typedef std::function<void(JsonArray&)> fastybird_report_configuration_schema_callback_f;
    void fastybirdReportConfigurationSchemaRegister(fastybird_report_configuration_schema_callback_f callback);
    typedef std::function<void(JsonObject&)> fastybird_report_configuration_callback_f;
    void fastybirdReportConfigurationRegister(fastybird_report_configuration_callback_f callback);
    typedef std::function<void(JsonObject&)> fastybird_on_configure_callback_f;
    void fastybirdOnConfigureRegister(fastybird_on_configure_callback_f callback);

    typedef std::function<bool()> fastybird_channels_report_configuration_callback_f;
    void fastybirdChannelsReportConfigurationRegister(fastybird_channels_report_configuration_callback_f callback);
    typedef std::function<bool()> fastybird_channels_report_direct_controls_callback_f;
    void fastybirdChannelsReportDirectControlsRegister(fastybird_channels_report_direct_controls_callback_f callback);
    typedef std::function<bool()> fastybird_channels_report_scheduler_callback_f;
    void fastybirdChannelsReportSchedulerRegister(fastybird_channels_report_scheduler_callback_f callback);

    typedef struct {
        const char * key;
        const char * value;
    } fastybird_node_setting_t;

    typedef struct {
        const char * name;
        const char * version;
        const char * manufacturer;
    } fastybird_node_hardware_t;

    typedef struct {
        const char * name;
        const char * version;
        const char * manufacturer;
    } fastybird_node_software_t;

    typedef struct {
        // Node basic info
        const char * id;

        fastybird_node_hardware_t hardware;
        fastybird_node_software_t software;

        bool initialized;

        // Node channels
        std::vector<fastybird_channel_t> channels;

        std::vector<fastybird_node_setting_t> settings;
    } fastybird_node_t;

    void fastybirdRegisterNode(fastybird_node_t node);
    void fastybirdUnregisterNode(const char * nodeId);
#else
    #define fastybird_report_configuration_schema_callback_f void *
    #define fastybird_report_configuration_callback_f void *
    #define fastybird_on_configure_callback_f void *

    #define fastybird_channels_report_configuration_callback_f void *
    #define fastybird_channels_report_direct_controls_callback_f void *
    #define fastybird_channels_report_scheduler_callback_f void *
#endif

// -----------------------------------------------------------------------------
// Nodes gateway
// -----------------------------------------------------------------------------

#if NODES_GATEWAY_SUPPORT
    #define PJON_INCLUDE_TS

    #ifndef PJON_PACKET_MAX_LENGTH
        #define PJON_PACKET_MAX_LENGTH 80
    #endif

    #include <PJON.h>
#else
    #define PJON_Packet_Info void *
#endif