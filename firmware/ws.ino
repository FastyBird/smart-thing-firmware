/*

WEBSOCKET MODULE

Copyright (C) 2018 FastyBird s.r.o. <info@fastybird.com>

*/

#if WEB_SUPPORT && WS_SUPPORT

#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <Ticker.h>
#include <vector>

#include "libs/WebSocketIncommingBuffer.h"

AsyncWebSocket _ws_client(WEB_API_WS_DATA);

Ticker _web_defer;

std::vector<ws_on_connect_callback_f> _ws_on_connect_callbacks;
std::vector<ws_on_update_callback_f> _ws_on_update_callbacks;
std::vector<ws_on_action_callback_f> _ws_on_action_callbacks;
std::vector<ws_on_configure_callback_f> _ws_on_configure_callbacks;

typedef struct {
    IPAddress ip;
    uint32_t timestamp = 0;
} ws_ticket_t;

ws_ticket_t _ws_ticket[WS_BUFFER_SIZE];

// -----------------------------------------------------------------------------
// MODULE PRIVATE
// -----------------------------------------------------------------------------

#ifndef NOWSAUTH
    void _onAuth(
        AsyncWebServerRequest * request
    ) {
        webLog(request);

        if (!webAuthenticate(request)) {
            return request->requestAuthentication();
        }

        IPAddress ip = request->client()->remoteIP();

        uint32_t now = millis();
        uint16_t index;

        for (index = 0; index < WS_BUFFER_SIZE; index++) {
            if (_ws_ticket[index].ip == ip) {
                break;
            }

            if (_ws_ticket[index].timestamp == 0) {
                break;
            }

            if (now - _ws_ticket[index].timestamp > WS_TIMEOUT) {
                break;
            }
        }

        if (index == WS_BUFFER_SIZE) {
            request->send(429);

        } else {
            _ws_ticket[index].ip = ip;
            _ws_ticket[index].timestamp = now;

            request->send(200, "text/plain", "OK");
        }
    }

// -----------------------------------------------------------------------------

    bool _wsAuth(
        AsyncWebSocketClient * client
    ) {
        IPAddress ip = client->remoteIP();

        uint32_t now = millis();
        uint16_t index = 0;

        for (index = 0; index < WS_BUFFER_SIZE; index++) {
            if (_ws_ticket[index].ip == ip && (now - _ws_ticket[index].timestamp < WS_TIMEOUT)) {
                break;
            }
        }

        if (index == WS_BUFFER_SIZE) {
            DEBUG_MSG(PSTR("[WEBSOCKET] Validation check failed\n"));

            wsSend_P(client->id(), PSTR("{\"message\": \"ws_authentication_failed\", \"level\": \"error\"}"));

            return false;
        }

        return true;
    }
#endif

// -----------------------------------------------------------------------------

void _wsParse(
    AsyncWebSocketClient * client,
    uint8_t * payload,
    size_t length
) {
    // Get client ID
    uint32_t client_id = client->id();

    // Parse JSON input
    DynamicJsonBuffer jsonBuffer;

    JsonObject& root = jsonBuffer.parseObject((char *) payload);

    if (!root.success()) {
        DEBUG_MSG(PSTR("[WEBSOCKET] Error parsing data\n"));

        wsSend_P(client_id, PSTR("{\"message\": \"parsing_error\", \"level\": \"error\"}"));

        return;
    }

    // Check actions -----------------------------------------------------------

    if (root.containsKey("action")) {
        const char * action = root["action"];

        DEBUG_MSG(PSTR("[WEBSOCKET] Requested action: %s\n"), action);

        if (strcmp(action, "reboot") == 0) {
            // Send notification to all clients
            wsSend_P(PSTR("{\"doAction\": \"reload\", \"reason\": \"reset\"}"));

            deferredReset(250, CUSTOM_RESET_WEB);
            return;

        } else if (strcmp(action, "reconnect") == 0) {
            #if WIFI_SUPPORT
                // Send notification to all clients
                wsSend_P(PSTR("{\"doAction\": \"reload\", \"reason\": \"reconnect\"}"));

                _web_defer.once_ms(250, wifiDisconnect);
            #endif
            return;

        } else if (strcmp(action, "configure") == 0) {
            if (root.containsKey("config")) {
                JsonObject& configuration = root["config"];

                if (configuration.success()) {
                    // Multiple module configuration
                    if (root["config"].is<JsonArray>()) {
                        DEBUG_MSG(PSTR("[WEBSOCKET] Parsing multi modules configuration\n"));

                        for (uint8_t i = 0; i < root["config"].size(); i++) {
                            for (uint8_t j = 0; j < _ws_on_configure_callbacks.size(); j++) {
                                (_ws_on_configure_callbacks[j])(client_id, root["config"][i]);
                            }
                        }

                    // Single module configuration
                    } else {
                        DEBUG_MSG(PSTR("[WEBSOCKET] Parsing single module configuration\n"));

                        for (uint8_t i = 0; i < _ws_on_configure_callbacks.size(); i++) {
                            (_ws_on_configure_callbacks[i])(client_id, configuration);
                        }
                    }

                    wsSend_P(client_id, PSTR("{\"message\": \"changes_saved\"}"));

                    wsSendStatusToClients();

                    #if FASTYBIRD_SUPPORT
                        fastybirdReportConfiguration();
                    #endif
                }
            }

        } else {
            if (root.containsKey("data")) {
                JsonObject& data = root["data"];

                if (data.success()) {
                    // Callbacks
                    for (uint8_t i = 0; i < _ws_on_action_callbacks.size(); i++) {
                        (_ws_on_action_callbacks[i])(client_id, action, data);
                    }
                }

            } else {
                JsonObject& data = jsonBuffer.createObject();

                // Callbacks
                for (uint8_t i = 0; i < _ws_on_action_callbacks.size(); i++) {
                    (_ws_on_action_callbacks[i])(client_id, action, data);
                }
            }
        }
    }
}

// -----------------------------------------------------------------------------

void _wsEvent(
    AsyncWebSocket * server,
    AsyncWebSocketClient * client,
    AwsEventType type,
    void * arg,
    uint8_t * data,
    size_t len
) {
    if (type == WS_EVT_CONNECT) {
        #ifndef NOWSAUTH
            if (!_wsAuth(client)) {
                return;
            }
        #endif

        IPAddress ip = client->remoteIP();

        DEBUG_MSG(
            PSTR("[WEBSOCKET] #%u connected, ip: %d.%d.%d.%d, url: %s\n"),
            client->id(),
            ip[0],
            ip[1],
            ip[2],
            ip[3],
            server->url()
        );

        wsSendStatusToClient(client->id());

        client->_tempObject = new WebSocketIncommingBuffer(&_wsParse, true);

        #if WIFI_SUPPORT
            wifiReconnectCheck();
        #endif

    } else if (type == WS_EVT_DISCONNECT) {
        DEBUG_MSG(PSTR("[WEBSOCKET] #%u disconnected\n"), client->id());

        if (client->_tempObject) {
            delete (WebSocketIncommingBuffer *) client->_tempObject;
        }

        #if WIFI_SUPPORT
            wifiReconnectCheck();
        #endif

    } else if (type == WS_EVT_ERROR) {
        DEBUG_MSG(PSTR("[WEBSOCKET] #%u error(%u): %s\n"), client->id(), *((uint16_t*) arg), (char *) data);

    } else if (type == WS_EVT_PONG) {
        DEBUG_MSG(PSTR("[WEBSOCKET] #%u pong(%u): %s\n"), client->id(), len, len ? (char *) data : "");

    } else if (type == WS_EVT_DATA) {
        WebSocketIncommingBuffer *buffer = (WebSocketIncommingBuffer *) client->_tempObject;
        AwsFrameInfo * info = (AwsFrameInfo *) arg;
        buffer->data_event(client, info, data, len);
    }
}

// -----------------------------------------------------------------------------
// MODULE API
// -----------------------------------------------------------------------------

void wsSendStatusToClient(
    uint32_t clientId
) {
    for (uint8_t i = 0; i < _ws_on_connect_callbacks.size(); i++) {
        wsSend(clientId, _ws_on_connect_callbacks[i]);
    }
}

// -----------------------------------------------------------------------------

void wsSendStatusToClients() {
    for (uint8_t i = 0; i < _ws_on_connect_callbacks.size(); i++) {
        wsSend(_ws_on_connect_callbacks[i]);
    }
}

// -----------------------------------------------------------------------------

void wsReportConfiguration() {
    wsSendStatusToClients();
}

// -----------------------------------------------------------------------------

bool wsConnected() {
    return (_ws_client.count() > 0);
}

// -----------------------------------------------------------------------------

void wsOnConnectRegister(
    ws_on_connect_callback_f callback
) {
    _ws_on_connect_callbacks.push_back(callback);
}

// -----------------------------------------------------------------------------

void wsOnUpdateRegister(
    ws_on_update_callback_f callback
) {
    _ws_on_update_callbacks.push_back(callback);
}

// -----------------------------------------------------------------------------

void wsOnConfigureRegister(
    ws_on_configure_callback_f callback
) {
    _ws_on_configure_callbacks.push_back(callback);
}

// -----------------------------------------------------------------------------

void wsOnActionRegister(
    ws_on_action_callback_f callback
) {
    _ws_on_action_callbacks.push_back(callback);
}

// -----------------------------------------------------------------------------

void wsSend(
    ws_on_connect_callback_f callback
) {
    DynamicJsonBuffer jsonBuffer;

    JsonObject& root = jsonBuffer.createObject();

    callback(root);

    String output;

    root.printTo(output);

    jsonBuffer.clear();

    _ws_client.textAll((char *) output.c_str());
}

// -----------------------------------------------------------------------------

void wsSend(
    uint32_t clientId,
    ws_on_connect_callback_f callback
) {
    DynamicJsonBuffer jsonBuffer;

    JsonObject& root = jsonBuffer.createObject();

    callback(root);

    String output;

    root.printTo(output);

    jsonBuffer.clear();

    _ws_client.text(clientId, (char *) output.c_str());
}

// -----------------------------------------------------------------------------

void wsSend(
    const char * payload
) {
    _ws_client.textAll(payload);
}

// -----------------------------------------------------------------------------

void wsSend(
    uint32_t clientId,
    const char * payload
) {
    _ws_client.text(clientId, payload);
}

// -----------------------------------------------------------------------------

void wsSend(
    JsonObject& payload
) {
    if (payload.size() > 0) {
        String output;

        payload.printTo(output);

        _ws_client.textAll((char *) output.c_str());
    }
}

// -----------------------------------------------------------------------------

void wsSend_P(
    PGM_P payload
) {
    char buffer[strlen_P(payload)];

    strcpy_P(buffer, payload);

    wsSend(buffer);
}

// -----------------------------------------------------------------------------

void wsSend_P(
    uint32_t clientId,
    PGM_P payload
) {
    char buffer[strlen_P(payload)];

    strcpy_P(buffer, payload);

    wsSend(clientId, buffer);
}

// -----------------------------------------------------------------------------
// MODULE CORE
// -----------------------------------------------------------------------------

void wsSetup() {
    _ws_client.onEvent(_wsEvent);

    webServer()->addHandler(&_ws_client);

    #ifndef NOWSAUTH
        webServer()->on(WEB_API_WS_AUTH, HTTP_PUT, _onAuth);
    #endif

    firmwareRegisterLoop(wsLoop);
}

// -----------------------------------------------------------------------------

void wsLoop() {
    static uint32_t last = 0;

    if (!wsConnected()) {
        return;
    }

    if (millis() - last > WS_UPDATE_INTERVAL) {
        last = millis();

        for (uint8_t i = 0; i < _ws_on_update_callbacks.size(); i++) {
            wsSend(_ws_on_update_callbacks[i]);
        }
    }
}

#endif