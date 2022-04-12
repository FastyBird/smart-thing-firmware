/*

MQTT MODULE

Copyright (C) 2018 FastyBird s.r.o. <code@fastybird.com>

*/

#if MQTT_SUPPORT

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>

AsyncMqttClient _mqtt;

bool _mqtt_enabled = true;

uint32_t _mqtt_connected_at = 0;
uint32_t _mqtt_reconnect_delay = MQTT_RECONNECT_DELAY_MIN;

char * _mqtt_host = "";
uint16_t _mqtt_port = MQTT_DEFAULT_PORT;

char * _mqtt_user = "";
char * _mqtt_pass = "";
char * _mqtt_clientId = "";

char * _mqtt_will = "";
char * _mqtt_will_content = "";

std::vector<mqtt_on_connect_callback_t> _mqtt_on_connect_callbacks;
std::vector<mqtt_on_disconnect_callback_t> _mqtt_on_disconnect_callbacks;
std::vector<mqtt_on_message_callback_t> _mqtt_on_message_callbacks;

// -----------------------------------------------------------------------------
// MODULE PRIVATE
// -----------------------------------------------------------------------------

void _mqttConnect()
{
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

    //if (strlen(_mqtt_user) == 0 || strlen(_mqtt_pass) == 0) {
    //    DEBUG_MSG(PSTR("[INFO][MQTT] Aborting attempt to connect. Mising username or password\n"));

    //    return;
    //}

    DEBUG_MSG(PSTR("[INFO][MQTT] Connecting to broker at %s:%d\n"), _mqtt_host, _mqtt_port);

    _mqtt.setServer(_mqtt_host, _mqtt_port);

    if (strlen(_mqtt_clientId) > 0) {
        _mqtt.setClientId(_mqtt_clientId);
    }

    _mqtt.setKeepAlive(MQTT_KEEPALIVE);
    _mqtt.setCleanSession(false);

    if (strlen(_mqtt_will) > 0) {
        _mqtt.setWill(_mqtt_will, MQTT_QOS, true, _mqtt_will_content);
    }

    if (strlen(_mqtt_user) > 0 && strlen(_mqtt_pass) > 0) {
        DEBUG_MSG(PSTR("[INFO][MQTT] Connecting as user %s\n"), _mqtt_user);

        _mqtt.setCredentials(_mqtt_user, _mqtt_pass);
    }

    #if NETWORK_SSL_ENABLED
        bool secure = MQTT_SSL_ENABLED == 1;

        _mqtt.setSecure(secure);

        if (secure) {
            DEBUG_MSG(PSTR("[INFO][MQTT] Using SSL\n"));

            uint8_t fp[20] = {0};

            if (sslFingerPrintArray(MQTT_SSL_FINGERPRINT, fp)) {
                _mqtt.addServerFingerprint(fp);

            } else {
                DEBUG_MSG(PSTR("[INFO][MQTT] Wrong fingerprint\n"));
            }
        }
    #endif // NETWORK_SSL_ENABLED

    DEBUG_MSG(PSTR("[INFO][MQTT] QoS: %d\n"), MQTT_QOS);
    DEBUG_MSG(PSTR("[INFO][MQTT] Keepalive time: %ds\n"), MQTT_KEEPALIVE);

    if (strlen(_mqtt_will) > 0) {
        DEBUG_MSG(PSTR("[INFO][MQTT] Will topic: %s\n"), _mqtt_will);
    }

    _mqtt.connect();
}

// -----------------------------------------------------------------------------

void _mqttConfigure()
{
    // Enable
    if (strlen(_mqtt_host) == 0) {
        _mqtt_enabled = false;

    } else {
        _mqtt_enabled = true;
    }

    _mqtt_reconnect_delay = MQTT_RECONNECT_DELAY_MIN;
}

// -----------------------------------------------------------------------------

#if WEB_SUPPORT && WS_SUPPORT
    /**
     * Provide module configuration schema
     */
    void _mqttReportConfigurationSchema(
        JsonArray& configuration
    ) {
        // Configuration field
        JsonObject& host = configuration.createNestedObject();

        host["name"] = "host";
        host["type"] = "text";
        host["default"] = "";

        JsonObject& port = configuration.createNestedObject();

        port["name"] = "port";
        port["type"] = "number";
        port["min"] = 1;
        port["max"] = 36000;
        port["default"] = MQTT_DEFAULT_PORT;

        JsonObject& username = configuration.createNestedObject();

        username["name"] = "username";
        username["type"] = "text";
        username["default"] = "";

        JsonObject& password = configuration.createNestedObject();

        password["name"] = "password";
        password["type"] = "text";
        password["default"] = "";

        #if ASYNC_TCP_SSL_ENABLED
            JsonObject& ssl_enabled = configuration.createNestedObject();

            ssl_enabled["name"] = "ssl_enabled";
            ssl_enabled["type"] = "boolean";
            ssl_enabled["default"] = false;

            JsonObject& ssl_fp = configuration.createNestedObject();

            ssl_fp["name"] = "ssl_fp";
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
        configuration["host"] = _mqtt_host;
        configuration["port"] = _mqtt_port;
        configuration["username"] = _mqtt_user;
        configuration["password"] = _mqtt_pass;
        configuration["client_id"] = _mqtt_clientId;

        #if ASYNC_TCP_SSL_ENABLED
            configuration["use_ssl"] = getSetting("mqtt_use_ssl", 0).toInt() == 1;
            configuration["fp"] = getSetting("mqtt_ssl_fp");
        #endif
    }

// -----------------------------------------------------------------------------

    /**
     * Update module configuration via WS or MQTT etc.
     */
    bool _mqttUpdateConfiguration(
        JsonObject& configuration
    ) {
        DEBUG_MSG(PSTR("[INFO][MQTT] Updating module\n"));

        bool is_updated = false;

        if (
            configuration.containsKey("host")
            && strcmp(configuration["host"].as<char *>(), _mqtt_host) != 0
        )  {
            DEBUG_MSG(PSTR("[INFO][MQTT] Setting: \"mqtt_host\" to: %s\n"), configuration["host"].as<char *>());

            setSetting("mqtt_host", configuration["host"].as<char *>());

            _mqtt_host = strdup(configuration["host"].as<char *>());

            is_updated = true;
        }

        if (
            configuration.containsKey("port")
            && configuration["port"].as<uint16_t>() != _mqtt_port
        )  {
            DEBUG_MSG(PSTR("[INFO][MQTT] Setting: \"mqtt_port\" to: %d\n"), configuration["port"].as<uint16_t>());

            setSetting("mqtt_port", configuration["port"].as<uint16_t>());

            _mqtt_port = configuration["port"].as<uint16_t>();

            is_updated = true;
        }

        if (
            configuration.containsKey("username")
            && strcmp(configuration["username"].as<char *>(), _mqtt_user) != 0
        )  {
            DEBUG_MSG(PSTR("[INFO][MQTT] Setting: \"mqtt_username\" to: %s\n"), configuration["username"].as<char *>());

            setSetting("mqtt_username", configuration["username"].as<char *>());

            _mqtt_user = strdup(configuration["username"].as<char *>());

            is_updated = true;
        }

        if (
            configuration.containsKey("password")
            && strcmp(configuration["password"].as<char *>(), _mqtt_pass) != 0
        )  {
            DEBUG_MSG(PSTR("[INFO][MQTT] Setting: \"mqtt_password\" to: %s\n"), configuration["password"].as<char *>());

            setSetting("mqtt_password", configuration["password"].as<char *>());

            _mqtt_pass = strdup(configuration["password"].as<char *>());

            is_updated = true;
        }

        if (
            configuration.containsKey("client_id")
            && strcmp(configuration["client_id"].as<char *>(), _mqtt_clientId) != 0
        )  {
            DEBUG_MSG(PSTR("[INFO][MQTT] Setting: \"mqtt_client_id\" to: %s\n"), configuration["client_id"].as<char *>());

            setSetting("mqtt_client_id", configuration["client_id"].as<char *>());

            _mqtt_clientId = strdup(configuration["client_id"].as<char *>());

            is_updated = true;
        }

        #if ASYNC_TCP_SSL_ENABLED
            if (
                configuration.containsKey("ssl_enabled")
                && configuration["ssl_enabled"].as<bool>() != (getSetting("mqtt_use_ssl").toInt() == 1)
            )  {
                DEBUG_MSG(PSTR("[INFO][MQTT] Setting: \"mqtt_ssl_enabled\" to: %d\n"), (configuration["ssl_enabled"].as<bool>() ? 1 : 0));

                setSetting("mqtt_use_ssl", configuration["ssl_enabled"].as<bool>() ? 1 : 0);

                is_updated = true;
            }

            if (
                configuration.containsKey("ssl_fp")
                && strcmp(configuration["ssl_fp"].as<char *>(), getSetting("mqtt_ssl_fp").c_str()) != 0
            )  {
                setSetting("mqtt_ssl_fp", configuration["ssl_fp"].as<char *>());

                is_updated = true;
            }
        #else
            delSetting("mqtt_use_ssl");
            delSetting("mqtt_ssl_fp");
        #endif

        return is_updated;
    }
#endif // WEB_SUPPORT && WS_SUPPORT

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

void _mqttOnConnect()
{
    DEBUG_MSG(PSTR("[INFO][MQTT] Connected!\n"));

    _mqtt_reconnect_delay = MQTT_RECONNECT_DELAY_MIN;

    _mqtt_connected_at = millis();

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

void _mqttOnDisconnect()
{
    DEBUG_MSG(PSTR("[INFO][MQTT] Disconnected!\n"));

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
            DEBUG_MSG(PSTR("[INFO][MQTT] Received %s > %s - SKIPPED\n"), topic, message);

            return;
        }
    #endif

    DEBUG_MSG(PSTR("[INFO][MQTT] Received %s > %s\n"), topic, message);

    // Callbacks
    for (uint8_t i = 0; i < _mqtt_on_message_callbacks.size(); i++) {
        _mqtt_on_message_callbacks[i](topic, message);
    }
}

// -----------------------------------------------------------------------------
// MODULE API
// -----------------------------------------------------------------------------

void mqttOnConnectRegister(
    mqtt_on_connect_callback_t callback
) {
    _mqtt_on_connect_callbacks.push_back(callback);
}

// -----------------------------------------------------------------------------

void mqttOnDisconnectRegister(
    mqtt_on_disconnect_callback_t callback
) {
    _mqtt_on_disconnect_callbacks.push_back(callback);
}

// -----------------------------------------------------------------------------

void mqttOnMessageRegister(
    mqtt_on_message_callback_t callback
) {
    _mqtt_on_message_callbacks.push_back(callback);
}

// -----------------------------------------------------------------------------

uint8_t mqttSend(
    const char * topic,
    const char * message,
    bool retain
) {
    if (_mqtt.connected()) {
        uint8_t _packet_id = _mqtt.publish(topic, MQTT_QOS, retain, message);
        DEBUG_MSG(PSTR("[INFO][MQTT] Sending %s => %s (PID %d)\n"), topic, message, _packet_id);

        return _packet_id;
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
    const char * topic
) {
    if (_mqtt.connected() && (strlen(topic) > 0)) {
        uint8_t packet_id;

        packet_id = _mqtt.subscribe(topic, MQTT_QOS);
        DEBUG_MSG(PSTR("[INFO][MQTT] Subscribing %s (PID %d)\n"), topic, packet_id);

        return packet_id;
    }

    return 0;
}

// -----------------------------------------------------------------------------

uint8_t mqttUnsubscribe(
    const char * topic
) {
    if (_mqtt.connected() && (strlen(topic) > 0)) {
        uint8_t _packet_id = _mqtt.unsubscribe(topic);
        DEBUG_MSG(PSTR("[INFO][MQTT] Unsubscribing %s (PID %d)\n"), topic, _packet_id);

        return _packet_id;
    }

    return 0;
}

// -----------------------------------------------------------------------------

bool mqttConnected()
{
    return _mqtt.connected();
}

// -----------------------------------------------------------------------------

void mqttDisconnect()
{
    if (_mqtt.connected()) {
        DEBUG_MSG(PSTR("[INFO][MQTT] Disconnecting\n"));
        _mqtt.disconnect();
    }
}

// -----------------------------------------------------------------------------

void mqttReset()
{
    _mqttConfigure();

    mqttDisconnect();
}

// -----------------------------------------------------------------------------

void mqttSetWill(
    char * topic,
    char * payload
) {
    _mqtt_will = strdup(topic);
    _mqtt_will_content = strdup(payload);
}

// -----------------------------------------------------------------------------

void mqttSetup()
{
    _mqtt_host = strdup(getSetting("mqtt_host").c_str());
    _mqtt_port = getSetting("mqtt_port", MQTT_DEFAULT_PORT).toInt();

    _mqtt_user = strdup(getSetting("mqtt_username").c_str());
    _mqtt_pass = strdup(getSetting("mqtt_password").c_str());
    _mqtt_clientId = strdup(getSetting("mqtt_client_id").c_str());

    DEBUG_MSG(PSTR("[INFO][MQTT] SSL %s, Autoconnect %s\n"),
        ASYNC_TCP_SSL_ENABLED ? "ENABLED" : "DISABLED",
        MQTT_AUTOCONNECT ? "ENABLED" : "DISABLED"
    );

    _mqtt.onConnect([](bool sessionPresent) {
        _mqttOnConnect();
    });

    _mqtt.onDisconnect([](AsyncMqttClientDisconnectReason reason) {
        if (reason == AsyncMqttClientDisconnectReason::TCP_DISCONNECTED) {
            DEBUG_MSG(PSTR("[INFO][MQTT] TCP Disconnected\n"));
        }

        if (reason == AsyncMqttClientDisconnectReason::MQTT_IDENTIFIER_REJECTED) {
            DEBUG_MSG(PSTR("[INFO][MQTT] Identifier Rejected\n"));
        }

        if (reason == AsyncMqttClientDisconnectReason::MQTT_SERVER_UNAVAILABLE) {
            DEBUG_MSG(PSTR("[INFO][MQTT] Server unavailable\n"));
        }

        if (reason == AsyncMqttClientDisconnectReason::MQTT_MALFORMED_CREDENTIALS) {
            DEBUG_MSG(PSTR("[INFO][MQTT] Malformed credentials\n"));
        }

        if (reason == AsyncMqttClientDisconnectReason::MQTT_NOT_AUTHORIZED) {
            DEBUG_MSG(PSTR("[INFO][MQTT] Not authorized\n"));
        }

        #if NETWORK_SSL_ENABLED
            if (reason == AsyncMqttClientDisconnectReason::TLS_BAD_FINGERPRINT) {
                DEBUG_MSG(PSTR("[INFO][MQTT] Bad fingerprint\n"));
            }
        #endif

        _mqttOnDisconnect();
    });

    _mqtt.onMessage([](char * topic, char * payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
        _mqttOnMessage(topic, payload, len);
    });

    _mqtt.onSubscribe([](uint16_t packetId, uint8_t qos) {
        DEBUG_MSG(PSTR("[INFO][MQTT] Subscribe ACK for PID %d\n"), packetId);
    });

    _mqtt.onPublish([](uint16_t packetId) {
        DEBUG_MSG(PSTR("[INFO][MQTT] Publish ACK for PID %d\n"), packetId);
    });

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

void mqttLoop()
{
    if (WiFi.status() != WL_CONNECTED) {
        return;
    }

    _mqttConnect();
}

#endif // MQTT_SUPPORT
