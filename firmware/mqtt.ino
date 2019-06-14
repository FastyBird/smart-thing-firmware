/*

MQTT MODULE

Copyright (C) 2018 FastyBird Ltd. <info@fastybird.com>

*/

#if MQTT_SUPPORT

#include <EEPROM_Rotate.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ArduinoJson.h>
#include <vector>

#if MQTT_USE_ASYNC // Using AsyncMqttClient
    #include <AsyncMqttClient.h>
    
    AsyncMqttClient _mqtt;
#else // Using PubSubClient
    #include <PubSubClient.h>
    PubSubClient _mqtt;
    
    bool _mqtt_connected = false;

    WiFiClient _mqtt_client;

    #if NETWORK_ASYNC_TCP_SSL_ENABLED
        WiFiClientSecure _mqtt_client_secure;
    #endif // NETWORK_ASYNC_TCP_SSL_ENABLED
#endif // MQTT_USE_ASYNC

bool _mqtt_enabled = MQTT_ENABLED;
bool _mqtt_retain = MQTT_RETAIN;
uint32_t _mqtt_reconnect_delay = MQTT_RECONNECT_DELAY_MIN;
uint8_t _mqtt_qos = MQTT_QOS;
uint32_t _mqtt_keepalive = MQTT_KEEPALIVE;

char * _mqtt_user = 0;
char * _mqtt_pass = 0;
char * _mqtt_clientid;
char * _mqtt_will = "";
char * _mqtt_will_content;

#if MQTT_SKIP_RETAINED
    uint32_t _mqtt_connected_at = 0;
#endif

std::vector<mqtt_on_connect_callback_f> _mqtt_on_connect_callbacks;
std::vector<mqtt_on_disconnect_callback_f> _mqtt_on_disconnect_callbacks;

typedef struct {
    String topic;
    mqtt_on_message_callback_f callback;
} mqtt_topic_subscription_t;

std::vector<mqtt_topic_subscription_t> _mqtt_topic_subscriptions;

// -----------------------------------------------------------------------------
// MODULE PRIVATE
// -----------------------------------------------------------------------------

void _mqttConnect() {
    // Do not connect if disabled
    if (!_mqtt_enabled) {
        return;
    }

    // Do not connect if already connected
    if (_mqtt.connected()) {
        return;
    }

    // Check reconnect interval
    static uint32_t last = 0;

    if (millis() - last < _mqtt_reconnect_delay) {
        return;
    }

    last = millis();

    // Increase the reconnect delay
    _mqtt_reconnect_delay += MQTT_RECONNECT_DELAY_STEP;

    if (_mqtt_reconnect_delay > MQTT_RECONNECT_DELAY_MAX) {
        _mqtt_reconnect_delay = MQTT_RECONNECT_DELAY_MAX;
    }

    char * host = strdup(getSetting("mqttServer", MQTT_SERVER).c_str());
    int port = getSetting("mqttPort", MQTT_PORT).toInt();

    if (_mqtt_user) {
        free(_mqtt_user);
    }

    if (_mqtt_pass) {
        free(_mqtt_pass);
    }

    if (_mqtt_clientid) {
        free(_mqtt_clientid);
    }

    _mqtt_user = strdup(getSetting("mqttUser", MQTT_USER).c_str());
    _mqtt_pass = strdup(getSetting("mqttPassword", MQTT_PASS).c_str());
    _mqtt_clientid = strdup(getSetting("mqttClientID", getIdentifier()).c_str());

    if (strlen(_mqtt_user) == 0 || strlen(_mqtt_pass) == 0) {
        DEBUG_MSG(PSTR("[MQTT] Aborting attempt to connect. Mising username or password\n"));

        return;
    }

    DEBUG_MSG(PSTR("[MQTT] Connecting to broker at %s:%d\n"), host, port);

    #if MQTT_USE_ASYNC
        _mqtt.setServer(host, port);
        _mqtt.setClientId(_mqtt_clientid);
        _mqtt.setKeepAlive(_mqtt_keepalive);
        _mqtt.setCleanSession(false);

        if (strcmp(_mqtt_will, "") != 0) {
            _mqtt.setWill(_mqtt_will, _mqtt_qos, _mqtt_retain, _mqtt_will_content);
        }

        if ((strlen(_mqtt_user) > 0) && (strlen(_mqtt_pass) > 0)) {
            DEBUG_MSG(PSTR("[MQTT] Connecting as user %s\n"), _mqtt_user);

            _mqtt.setCredentials(_mqtt_user, _mqtt_pass);
        }

        #if NETWORK_ASYNC_TCP_SSL_ENABLED
            bool secure = MQTT_SSL_ENABLED == 1;

            _mqtt.setSecure(secure);

            if (secure) {
                DEBUG_MSG(PSTR("[MQTT] Using SSL\n"));

                uint8_t fp[20] = {0};

                if (sslFingerPrintArray(MQTT_SSL_FINGERPRINT, fp)) {
                    _mqtt.addServerFingerprint(fp);

                } else {
                    DEBUG_MSG(PSTR("[MQTT] Wrong fingerprint\n"));
                }
            }
        #endif // NETWORK_ASYNC_TCP_SSL_ENABLED

        DEBUG_MSG(PSTR("[MQTT] Client ID: %s\n"), _mqtt_clientid);
        DEBUG_MSG(PSTR("[MQTT] QoS: %d\n"), _mqtt_qos);
        DEBUG_MSG(PSTR("[MQTT] Retain flag: %d\n"), _mqtt_retain ? 1 : 0);
        DEBUG_MSG(PSTR("[MQTT] Keepalive time: %ds\n"), _mqtt_keepalive);

        if (strcmp(_mqtt_will, "") != 0) {
            DEBUG_MSG(PSTR("[MQTT] Will topic: %s\n"), _mqtt_will);
        }

        _mqtt.connect();

    #else // not MQTT_USE_ASYNC
        bool response = true;

        #if NETWORK_ASYNC_TCP_SSL_ENABLED
            bool secure = MQTT_SSL_ENABLED == 1;

            if (secure) {
                DEBUG_MSG(PSTR("[MQTT] Using SSL\n"));

                if (_mqtt_client_secure.connect(host, port)) {
                    char fp[60] = {0};

                    if (sslFingerPrintChar(MQTT_SSL_FINGERPRINT, fp)) {
                        if (_mqtt_client_secure.verify(fp, host)) {
                            _mqtt.setClient(_mqtt_client_secure);

                        } else {
                            DEBUG_MSG(PSTR("[MQTT] Invalid fingerprint\n"));
                            response = false;
                        }

                        _mqtt_client_secure.stop();

                        yield();

                    } else {
                        DEBUG_MSG(PSTR("[MQTT] Wrong fingerprint\n"));
                        response = false;
                    }

                } else {
                    DEBUG_MSG(PSTR("[MQTT] Thing connection failed\n"));

                    response = false;
                }

            } else {
                _mqtt.setClient(_mqtt_client);
            }

        #else // not NETWORK_ASYNC_TCP_SSL_ENABLED
            _mqtt.setClient(_mqtt_client);
        #endif // NETWORK_ASYNC_TCP_SSL_ENABLED

        if (response) {
            _mqtt.setServer(host, port);

            if ((strlen(_mqtt_user) > 0) && (strlen(_mqtt_pass) > 0)) {
                DEBUG_MSG(PSTR("[MQTT] Connecting as user %s\n"), _mqtt_user);

                response = _mqtt.connect(
                    _mqtt_clientid,
                    _mqtt_user,
                    _mqtt_pass,
                    _mqtt_will,
                    _mqtt_qos,
                    _mqtt_retain,
                    _mqtt_will_content
                );

            } else {
                response = _mqtt.connect(
                    _mqtt_clientid,
                    _mqtt_will,
                    _mqtt_qos,
                    _mqtt_retain,
                    _mqtt_will_content
                );
            }

            DEBUG_MSG(PSTR("[MQTT] Client ID: %s\n"), _mqtt_clientid);
            DEBUG_MSG(PSTR("[MQTT] QoS: %d\n"), _mqtt_qos);
            DEBUG_MSG(PSTR("[MQTT] Retain flag: %d\n"), _mqtt_retain ? 1 : 0);
            DEBUG_MSG(PSTR("[MQTT] Keepalive time: %ds\n"), _mqtt_keepalive);

            if (strcmp(_mqtt_will, "") != 0) {
                DEBUG_MSG(PSTR("[MQTT] Will topic: %s\n"), _mqtt_will);
            }
        }

        if (response) {
            _mqttOnConnect();

        } else {
            DEBUG_MSG(PSTR("[MQTT] Connection failed\n"));
        }
    #endif // MQTT_USE_ASYNC

    free(host);
}

// -----------------------------------------------------------------------------

void _mqttConfigure() {
    // MQTT options
    _mqtt_qos = getSetting("mqttQoS", MQTT_QOS).toInt();
    _mqtt_retain = getSetting("mqttRetain", MQTT_RETAIN).toInt() == 1;
    _mqtt_keepalive = getSetting("mqttKeep", MQTT_KEEPALIVE).toInt();

    // Enable
    if (getSetting("mqttServer", MQTT_SERVER).length() == 0) {
        mqttEnabled(false);

    } else {
        _mqtt_enabled = getSetting("mqttEnabled", MQTT_ENABLED).toInt() == 1;
    }

    _mqtt_reconnect_delay = MQTT_RECONNECT_DELAY_MIN;
}

// -----------------------------------------------------------------------------
// MODULE CALLBACKS
// -----------------------------------------------------------------------------

void _mqttOnConnect() {
    DEBUG_MSG(PSTR("[MQTT] Connected!\n"));

    _mqtt_reconnect_delay = MQTT_RECONNECT_DELAY_MIN;

    #if MQTT_SKIP_RETAINED
        _mqtt_connected_at = millis();
    #endif

    // Clean subscriptions
    mqttUnsubscribe("#");

    // Callbacks
    for (uint8_t i = 0; i < _mqtt_on_connect_callbacks.size(); i++) {
        (_mqtt_on_connect_callbacks[i])();
    }
    
    #if WEB_SUPPORT && WS_SUPPORT
        wsSend_P(PSTR("{\"mqttStatus\": true}"));
    #endif
}

// -----------------------------------------------------------------------------

void _mqttOnDisconnect() {
    DEBUG_MSG(PSTR("[MQTT] Disconnected!\n"));

    // Callbacks
    for (uint8_t i = 0; i < _mqtt_on_disconnect_callbacks.size(); i++) {
        (_mqtt_on_disconnect_callbacks[i])();
    }

    #if WEB_SUPPORT && WS_SUPPORT
        wsSend_P(PSTR("{\"mqttStatus\": false}"));
    #endif
}

// -----------------------------------------------------------------------------

void _mqttOnMessage(
    char * topic,
    char * payload,
    const uint8_t len
) {
    if (len == 0) {
        return;
    }

    char message[len + 1];

    strlcpy(message, (char *) payload, len + 1);

    #if MQTT_SKIP_RETAINED
        if (millis() - _mqtt_connected_at < MQTT_SKIP_TIME) {
            DEBUG_MSG(PSTR("[MQTT] Received %s > %s - SKIPPED\n"), topic, message);

            return;
        }
    #endif

    DEBUG_MSG(PSTR("[MQTT] Received %s > %s\n"), topic, message);

    // Callbacks
    for (uint8_t i = 0; i < _mqtt_topic_subscriptions.size(); i++) {
        if (_mqtt_topic_subscriptions[i].topic.equals(topic)) {
            DEBUG_MSG(PSTR("[MQTT] Found registered callback\n"));

            _mqtt_topic_subscriptions[i].callback(topic, message);
        }
    }
}

// -----------------------------------------------------------------------------

#if WEB_SUPPORT && WS_SUPPORT
    // WS client is connected
    void _mqttWSOnConnect(
        JsonObject& root
    ) {
        JsonArray& modules = root.containsKey("modules") ? root["modules"] : root.createNestedArray("modules");
        JsonObject& module = modules.createNestedObject();

        module["module"] = "mqtt";
        module["visible"] = true;

        // Configuration container
        JsonObject& configuration = module.createNestedObject("config");

        // Configuration values container
        JsonObject& configuration_values = configuration.createNestedObject("values");

        configuration_values["mqtt_server"] = getSetting("mqttServer", MQTT_SERVER);
        configuration_values["mqtt_server_port"] = getSetting("mqttPort", MQTT_PORT);
        configuration_values["mqtt_username"] = getSetting("mqttUser", MQTT_USER);
        configuration_values["mqtt_password"] = getSetting("mqttPassword", MQTT_PASS);
        configuration_values["mqtt_client_id"] = getSetting("mqttClientID", getIdentifier());
        
        configuration_values["mqtt_keep_alive"] = getSetting("mqttKeep", MQTT_KEEPALIVE).toInt();
        configuration_values["mqtt_retain"] = getSetting("mqttRetain", MQTT_RETAIN).toInt() == 1;
        configuration_values["mqtt_qos"] = getSetting("mqttQos", MQTT_QOS).toInt();

        #if ASYNC_TCP_SSL_ENABLED
            configuration_values["mqtt_use_ssl"] = getSetting("mqttUseSsl", 0).toInt() == 1;
            configuration_values["mqtt_fp"] = getSetting("mqttSslFp");
        #endif
        
        // Configuration schema container
        JsonArray& configuration_schema = configuration.createNestedArray("schema");

        // Configuration field
        JsonObject& keep_alive = configuration_schema.createNestedObject();

        keep_alive["name"] = "mqtt_keep_alive";
        keep_alive["type"] = "number";
        keep_alive["min"] = 10;
        keep_alive["max"] = 3600;
        keep_alive["step"] = 10;
        keep_alive["default"] = MQTT_KEEPALIVE;

        JsonObject& retain = configuration_schema.createNestedObject();

        retain["name"] = "mqtt_retain";
        retain["type"] = "boolean";
        retain["default"] = MQTT_RETAIN;

        JsonObject& qos = configuration_schema.createNestedObject();

        qos["name"] = "mqtt_qos";
        qos["type"] = "select";
        qos["default"] = MQTT_RETAIN;

        JsonArray& qosValues = qos.createNestedArray("values");

        JsonObject& qosValue0 = qosValues.createNestedObject();
        qosValue0["value"] = 0;
        qosValue0["name"] = "at_most_once";

        JsonObject& qosValue1 = qosValues.createNestedObject();
        qosValue1["value"] = 1;
        qosValue1["name"] = "at_least_once";

        JsonObject& qosValue2 = qosValues.createNestedObject();
        qosValue2["value"] = 2;
        qosValue2["name"] = "exactly_once";

        #if ASYNC_TCP_SSL_ENABLED
            JsonObject& ssl_enabled = configuration_schema.createNestedObject();

            ssl_enabled["name"] = "mqtt_ssl_enabled";
            ssl_enabled["type"] = "boolean";
            ssl_enabled["default"] = false;

            JsonObject& ssl_fp = configuration_schema.createNestedObject();

            ssl_fp["name"] = "mqtt_ssl_fp";
            ssl_fp["type"] = "text";
            ssl_fp["default"] = "";
        #endif
    }

// -----------------------------------------------------------------------------

    // WS client sent configure module request
    void _mqttWSOnConfigure(
        const uint32_t clientId, 
        JsonObject& module
    ) {
        if (module.containsKey("module") && module["module"] == "mqtt") {
            if (module.containsKey("config")) {
                // Extract configuration container
                JsonObject& configuration = module["config"].as<JsonObject&>();

                if (configuration.containsKey("values")) {
                    bool do_reconnect = false;

                    JsonObject& values = configuration["values"].as<JsonObject&>();

                    if (values.containsKey("mqtt_keep_alive") && values["mqtt_keep_alive"].as<uint8_t>() != getSetting("mqttKeep").toInt())  {
                        setSetting("mqttKeep", values["mqtt_keep_alive"].as<uint8_t>());
                    }

                    if (values.containsKey("mqtt_retain") && values["mqtt_retain"].as<bool>() != (getSetting("mqttRetain").toInt() == 1))  {
                        setSetting("mqttRetain", values["mqtt_retain"].as<bool>() ? 1 : 0);
                    }

                    if (values.containsKey("mqtt_qos") && values["mqtt_qos"].as<uint8_t>() != getSetting("mqttQos").toInt())  {
                        setSetting("mqttQos", values["mqtt_qos"].as<uint8_t>());
                    }

                    #if ASYNC_TCP_SSL_ENABLED
                        if (values.containsKey("mqtt_ssl_enabled") && values["mqtt_ssl_enabled"].as<bool>() != (getSetting("mqttUseSsl").toInt() == 1))  {
                            setSetting("mqttUseSsl", values["mqtt_ssl_enabled"].as<bool>() ? 1 : 0);
                        }

                        if (values.containsKey("mqtt_ssl_fp") && values["mqtt_ssl_fp"].as<char *>() != getSetting("mqttSslFp").c_str())  {
                            setSetting("mqttSslFp", values["mqtt_ssl_fp"].as<char *>());
                        }
                    #else
                        delSetting("mqttUseSsl");
                        delSetting("mqttSslFp");
                    #endif

                    if (values.containsKey("mqtt_server") && values["mqtt_server"].as<char *>() != getSetting("mqttServer").c_str())  {
                        setSetting("mqttServer", values["mqtt_server"].as<char *>());

                        do_reconnect = true;
                    }

                    if (values.containsKey("mqtt_server_port") && values["mqtt_server_port"].as<uint8_t>() != getSetting("mqttPort").toInt())  {
                        setSetting("mqttPort", values["mqtt_server_port"].as<uint8_t>());

                        do_reconnect = true;
                    }

                    if (values.containsKey("mqtt_username") && values["mqtt_username"].as<char *>() != getSetting("mqttUser").c_str())  {
                        setSetting("mqttUser", values["mqtt_username"].as<char *>());

                        do_reconnect = true;
                    }

                    if (values.containsKey("mqtt_password") && values["mqtt_password"].as<char *>() != getSetting("mqttPassword").c_str())  {
                        setSetting("mqttPassword", values["mqtt_password"].as<char *>());

                        do_reconnect = true;
                    }

                    if (values.containsKey("mqtt_client_id") && values["mqtt_client_id"].as<char *>() != getSetting("mqttClientID").c_str())  {
                        setSetting("mqttClientID", values["mqtt_client_id"].as<char *>());

                        do_reconnect = true;
                    }

                    wsSend_P(clientId, PSTR("{\"message\": \"mqtt_updated\"}"));

                    // Reload & cache settings
                    firmwareReload();

                    if (do_reconnect) {
                        mqttDisconnect();
                    }
                }
            }
        }
    }

// -----------------------------------------------------------------------------

    void _mqttWSOnUpdate(
        JsonObject& root
    ) {
        JsonArray& modules = root.containsKey("modules") ? root["modules"] : root.createNestedArray("modules");
        JsonObject& module = modules.createNestedObject();

        module["module"] = "mqtt";

        // Data container
        JsonObject& data = module.createNestedObject("data");

        data["status"] = mqttConnected() ? true : false;
    }
#endif

// -----------------------------------------------------------------------------
// MODULE API
// -----------------------------------------------------------------------------

void mqttOnConnectRegister(
    mqtt_on_connect_callback_f callback
) {
    _mqtt_on_connect_callbacks.push_back(callback);
}

// -----------------------------------------------------------------------------

void mqttOnDisconnectRegister(
    mqtt_on_disconnect_callback_f callback
) {
    _mqtt_on_disconnect_callbacks.push_back(callback);
}

// -----------------------------------------------------------------------------

uint8_t mqttSend(
    const char * topic,
    const char * message,
    bool retain
) {
    if (_mqtt.connected()) {
        #if MQTT_USE_ASYNC
            uint8_t _packet_id = _mqtt.publish(topic, _mqtt_qos, retain, message);
            DEBUG_MSG(PSTR("[MQTT] Sending %s => %s (PID %d)\n"), topic, message, _packet_id);

            return _packet_id;
        #else
            _mqtt.publish(topic, message, retain);
            DEBUG_MSG(PSTR("[MQTT] Sending %s => %s\n"), topic, message);

            return 999;
        #endif
    }

    return 0;
}

// -----------------------------------------------------------------------------

uint8_t mqttSend(
    const char * topic,
    const char * message
) {
    return mqttSend(topic, message, _mqtt_retain);
}

// -----------------------------------------------------------------------------

uint8_t mqttSubscribe(
    const char * topic,
    mqtt_on_message_callback_f callback
) {
    if (_mqtt.connected() && (strlen(topic) > 0)) {
        uint8_t packet_id;

        #if MQTT_USE_ASYNC
            packet_id = _mqtt.subscribe(topic, _mqtt_qos);
            DEBUG_MSG(PSTR("[MQTT] Subscribing %s (PID %d)\n"), topic, packet_id);
        #else
            _mqtt.subscribe(topic, _mqtt_qos);
            DEBUG_MSG(PSTR("[MQTT] Subscribing %s\n"), topic);

            packet_id = 999;
        #endif

        if (packet_id != 0) {
            _mqtt_topic_subscriptions.push_back({
                String(topic),
                callback
            });
        }

        return packet_id;
    }

    return 0;
}

// -----------------------------------------------------------------------------

uint8_t mqttUnsubscribe(
    const char * topic
) {
    if (_mqtt.connected() && (strlen(topic) > 0)) {
        #if MQTT_USE_ASYNC
            uint8_t _packet_id = _mqtt.unsubscribe(topic);
            DEBUG_MSG(PSTR("[MQTT] Unsubscribing %s (PID %d)\n"), topic, _packet_id);

            return _packet_id;
        #else
            _mqtt.unsubscribe(topic);
            DEBUG_MSG(PSTR("[MQTT] Unsubscribing %s\n"), topic);

            return 999;
        #endif
    }

    return 0;
}

// -----------------------------------------------------------------------------

void mqttEnabled(
    const bool status
) {
    _mqtt_enabled = status;
}

// -----------------------------------------------------------------------------

bool mqttEnabled() {
    return _mqtt_enabled;
}

// -----------------------------------------------------------------------------

bool mqttConnected() {
    return _mqtt.connected();
}

// -----------------------------------------------------------------------------

void mqttDisconnect() {
    if (_mqtt.connected()) {
        DEBUG_MSG(PSTR("[MQTT] Disconnecting\n"));
        _mqtt.disconnect();
    }
}

// -----------------------------------------------------------------------------

void mqttReset() {
    _mqttConfigure();

    mqttDisconnect();
}

// -----------------------------------------------------------------------------

void mqttSetWill(
    char * topic,
    char * payload
) {
    _mqtt_will = strdup(topic);
    _mqtt_will_content = payload;
}

// -----------------------------------------------------------------------------

void mqttSetup() {
    DEBUG_MSG(PSTR("[MQTT] Async %s, SSL %s, Autoconnect %s\n"),
        MQTT_USE_ASYNC ? "ENABLED" : "DISABLED",
        ASYNC_TCP_SSL_ENABLED ? "ENABLED" : "DISABLED",
        MQTT_AUTOCONNECT ? "ENABLED" : "DISABLED"
    );

    #if MQTT_USE_ASYNC
        _mqtt.onConnect([](bool sessionPresent) {
            _mqttOnConnect();
        });

        _mqtt.onDisconnect([](AsyncMqttClientDisconnectReason reason) {
            if (reason == AsyncMqttClientDisconnectReason::TCP_DISCONNECTED) {
                DEBUG_MSG(PSTR("[MQTT] TCP Disconnected\n"));
            }

            if (reason == AsyncMqttClientDisconnectReason::MQTT_IDENTIFIER_REJECTED) {
                DEBUG_MSG(PSTR("[MQTT] Identifier Rejected\n"));
            }

            if (reason == AsyncMqttClientDisconnectReason::MQTT_SERVER_UNAVAILABLE) {
                DEBUG_MSG(PSTR("[MQTT] Server unavailable\n"));
            }

            if (reason == AsyncMqttClientDisconnectReason::MQTT_MALFORMED_CREDENTIALS) {
                DEBUG_MSG(PSTR("[MQTT] Malformed credentials\n"));
            }

            if (reason == AsyncMqttClientDisconnectReason::MQTT_NOT_AUTHORIZED) {
                DEBUG_MSG(PSTR("[MQTT] Not authorized\n"));
            }

            #if NETWORK_ASYNC_TCP_SSL_ENABLED
                if (reason == AsyncMqttClientDisconnectReason::TLS_BAD_FINGERPRINT) {
                    DEBUG_MSG(PSTR("[MQTT] Bad fingerprint\n"));
                }
            #endif

            _mqttOnDisconnect();
        });

        _mqtt.onMessage([](char * topic, char * payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
            _mqttOnMessage(topic, payload, len);
        });

        _mqtt.onSubscribe([](uint16_t packetId, uint8_t qos) {
            DEBUG_MSG(PSTR("[MQTT] Subscribe ACK for PID %d\n"), packetId);
        });

        _mqtt.onPublish([](uint16_t packetId) {
            DEBUG_MSG(PSTR("[MQTT] Publish ACK for PID %d\n"), packetId);
        });

    #else // not MQTT_USE_ASYNC
        _mqtt.setCallback([](char * topic, byte * payload, uint8_t length) {
            _mqttOnMessage(topic, (char *) payload, length);
        });
    #endif // MQTT_USE_ASYNC

    _mqttConfigure();

    #if WEB_SUPPORT && WS_SUPPORT
        wsOnConnectRegister(_mqttWSOnConnect);
        wsOnConfigureRegister(_mqttWSOnConfigure);
        wsOnUpdateRegister(_mqttWSOnUpdate);
    #endif

    // Register loop
    firmwareRegisterLoop(mqttLoop);
    firmwareRegisterReload(_mqttConfigure);
}

// -----------------------------------------------------------------------------

void mqttLoop() {
    if (WiFi.status() != WL_CONNECTED) {
        return;
    }

    #if MQTT_USE_ASYNC
        _mqttConnect();

    #else
        if (_mqtt.connected()) {
            _mqtt.loop();

        } else {
            if (_mqtt_connected) {
                _mqttOnDisconnect();
                _mqtt_connected = false;
            }

            _mqttConnect();
        }
    #endif
}

#endif // MQTT_SUPPORT
