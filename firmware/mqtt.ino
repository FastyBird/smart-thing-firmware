/*

MQTT MODULE

Copyright (C) 2018 FastyBird s.r.o. <info@fastybird.com>

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

bool _mqtt_enabled = true;
uint32_t _mqtt_reconnect_delay = MQTT_RECONNECT_DELAY_MIN;

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
        _mqtt.setKeepAlive(MQTT_KEEPALIVE);
        _mqtt.setCleanSession(false);

        if (strcmp(_mqtt_will, "") != 0) {
            _mqtt.setWill(_mqtt_will, MQTT_QOS, true, _mqtt_will_content);
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
        DEBUG_MSG(PSTR("[MQTT] QoS: %d\n"), MQTT_QOS);
        DEBUG_MSG(PSTR("[MQTT] Keepalive time: %ds\n"), MQTT_KEEPALIVE);

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
                    MQTT_QOS,
                    true,
                    _mqtt_will_content
                );

            } else {
                response = _mqtt.connect(
                    _mqtt_clientid,
                    _mqtt_will,
                    MQTT_QOS,
                    true,
                    _mqtt_will_content
                );
            }

            DEBUG_MSG(PSTR("[MQTT] Client ID: %s\n"), _mqtt_clientid);
            DEBUG_MSG(PSTR("[MQTT] QoS: %d\n"), MQTT_QOS);
            DEBUG_MSG(PSTR("[MQTT] Keepalive time: %ds\n"), MQTT_KEEPALIVE);

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
    // Enable
    if (getSetting("mqttServer", MQTT_SERVER).length() == 0) {
        _mqtt_enabled = false;

    } else {
        _mqtt_enabled = true;
    }

    _mqtt_reconnect_delay = MQTT_RECONNECT_DELAY_MIN;
}

// -----------------------------------------------------------------------------

#if FASTYBIRD_SUPPORT || (WEB_SUPPORT && WS_SUPPORT)
    /**
     * Provide module configuration schema
     */
    void _mqttReportConfigurationSchema(
        JsonArray& configuration
    ) {
        // Configuration field
        JsonObject& server = configuration.createNestedObject();

        server["name"] = "mqtt_server";
        server["type"] = "text";
        server["default"] = MQTT_SERVER;

        JsonObject& server_port = configuration.createNestedObject();

        server_port["name"] = "mqtt_server_port";
        server_port["type"] = "number";
        server_port["min"] = 1;
        server_port["max"] = 36000;
        server_port["default"] = MQTT_PORT;

        JsonObject& username = configuration.createNestedObject();

        username["name"] = "mqtt_username";
        username["type"] = "text";
        username["default"] = MQTT_USER;

        JsonObject& password = configuration.createNestedObject();

        password["name"] = "mqtt_password";
        password["type"] = "text";
        password["default"] = MQTT_PASS;

        JsonObject& client_id = configuration.createNestedObject();

        client_id["name"] = "mqtt_client_id";
        client_id["type"] = "text";
        client_id["default"] = getIdentifier();

        #if ASYNC_TCP_SSL_ENABLED
            JsonObject& ssl_enabled = configuration.createNestedObject();

            ssl_enabled["name"] = "mqtt_ssl_enabled";
            ssl_enabled["type"] = "boolean";
            ssl_enabled["default"] = false;

            JsonObject& ssl_fp = configuration.createNestedObject();

            ssl_fp["name"] = "mqtt_ssl_fp";
            ssl_fp["type"] = "text";
            ssl_fp["default"] = "";
        #endif
    }

// -----------------------------------------------------------------------------

    /**
     * Report module configuration
     */
    void _mqttReportConfiguration(
        JsonObject& configuration
    ) {
        configuration["mqtt_server"] = getSetting("mqttServer", MQTT_SERVER);
        configuration["mqtt_server_port"] = getSetting("mqttPort", MQTT_PORT).toInt();
        configuration["mqtt_username"] = getSetting("mqttUser", MQTT_USER);
        configuration["mqtt_password"] = getSetting("mqttPassword", MQTT_PASS);
        configuration["mqtt_client_id"] = getSetting("mqttClientID", getIdentifier());
        
        #if ASYNC_TCP_SSL_ENABLED
            configuration["mqtt_use_ssl"] = getSetting("mqttUseSsl", 0).toInt() == 1;
            configuration["mqtt_fp"] = getSetting("mqttSslFp");
        #endif
    }

// -----------------------------------------------------------------------------

    /**
     * Update module configuration via WS or MQTT etc.
     */
    bool _mqttUpdateConfiguration(
        JsonObject& configuration
    ) {
        DEBUG_MSG(PSTR("[MQTT] Updating module\n"));

        bool is_updated = false;

        if (
            configuration.containsKey("mqtt_server")
            && configuration["mqtt_server"].as<char *>() != getSetting("mqttServer").c_str()
        )  {
            DEBUG_MSG(PSTR("[MQTT] Setting: \"mqtt_server\" to: %s\n"), configuration["mqtt_server"].as<char *>());

            setSetting("mqttServer", configuration["mqtt_server"].as<char *>());

            is_updated = true;
        }

        if (
            configuration.containsKey("mqtt_server_port")
            && configuration["mqtt_server_port"].as<uint16_t>() != getSetting("mqttPort").toInt()
        )  {
            DEBUG_MSG(PSTR("[MQTT] Setting: \"mqtt_server_port\" to: %d\n"), configuration["mqtt_server_port"].as<uint16_t>());

            setSetting("mqttPort", configuration["mqtt_server_port"].as<uint16_t>());

            is_updated = true;
        }

        if (
            configuration.containsKey("mqtt_username")
            && configuration["mqtt_username"].as<char *>() != getSetting("mqttUser").c_str()
        )  {
            DEBUG_MSG(PSTR("[MQTT] Setting: \"mqtt_username\" to: %s\n"), configuration["mqtt_username"].as<char *>());

            setSetting("mqttUser", configuration["mqtt_username"].as<char *>());

            is_updated = true;
        }

        if (
            configuration.containsKey("mqtt_password")
            && configuration["mqtt_password"].as<char *>() != getSetting("mqttPassword").c_str()
        )  {
            DEBUG_MSG(PSTR("[MQTT] Setting: \"mqtt_password\" to: %s\n"), configuration["mqtt_password"].as<char *>());

            setSetting("mqttPassword", configuration["mqtt_password"].as<char *>());

            is_updated = true;
        }

        if (
            configuration.containsKey("mqtt_client_id")
            && configuration["mqtt_client_id"].as<char *>() != getSetting("mqttClientID").c_str()
        )  {
            DEBUG_MSG(PSTR("[MQTT] Setting: \"mqtt_client_id\" to: %s\n"), configuration["mqtt_client_id"].as<char *>());

            setSetting("mqttClientID", configuration["mqtt_client_id"].as<char *>());

            is_updated = true;
        }

        #if ASYNC_TCP_SSL_ENABLED
            if (
                configuration.containsKey("mqtt_ssl_enabled")
                && configuration["mqtt_ssl_enabled"].as<bool>() != (getSetting("mqttUseSsl").toInt() == 1)
            )  {
                DEBUG_MSG(PSTR("[MQTT] Setting: \"mqtt_ssl_enabled\" to: %d\n"), (configuration["mqtt_ssl_enabled"].as<bool>() ? 1 : 0));

                setSetting("mqttUseSsl", configuration["mqtt_ssl_enabled"].as<bool>() ? 1 : 0);

                is_updated = true;
            }

            if (
                configuration.containsKey("mqtt_ssl_fp")
                && configuration["mqtt_ssl_fp"].as<char *>() != getSetting("mqttSslFp").c_str()
            )  {
                setSetting("mqttSslFp", configuration["mqtt_ssl_fp"].as<char *>());

                is_updated = true;
            }
        #else
            delSetting("mqttUseSsl");
            delSetting("mqttSslFp");
        #endif

        return is_updated;
    }
#endif // FASTYBIRD_SUPPORT || (WEB_SUPPORT && WS_SUPPORT)

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

        // Data container
        JsonObject& data = module.createNestedObject("data");

        data["status"] = mqttConnected() ? true : false;

        // Configuration container
        JsonObject& configuration = module.createNestedObject("config");

        // Configuration values container
        JsonObject& configuration_values = configuration.createNestedObject("values");

        _mqttReportConfiguration(configuration_values);
        
        // Configuration schema container
        JsonArray& configuration_schema = configuration.createNestedArray("schema");

        _mqttReportConfigurationSchema(configuration_schema);
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

                if (
                    configuration.containsKey("values")
                    && _mqttUpdateConfiguration(configuration["values"])
                ) {
                    wsSend_P(clientId, PSTR("{\"message\": \"mqtt_updated\"}"));

                    // Reload & cache settings
                    firmwareReload();

                    // MQTT confiugration has changed, force reconnet
                    mqttDisconnect();
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
        wsSend(_mqttWSOnUpdate);
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
        wsSend(_mqttWSOnUpdate);
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
            uint8_t _packet_id = _mqtt.publish(topic, MQTT_QOS, retain, message);
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
    return mqttSend(topic, message, true);
}

// -----------------------------------------------------------------------------

uint8_t mqttSubscribe(
    const char * topic,
    mqtt_on_message_callback_f callback
) {
    if (_mqtt.connected() && (strlen(topic) > 0)) {
        uint8_t packet_id;

        #if MQTT_USE_ASYNC
            packet_id = _mqtt.subscribe(topic, MQTT_QOS);
            DEBUG_MSG(PSTR("[MQTT] Subscribing %s (PID %d)\n"), topic, packet_id);
        #else
            _mqtt.subscribe(topic, MQTT_QOS);
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
