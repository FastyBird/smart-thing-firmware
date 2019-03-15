/*

WEB MODULE

Copyright (C) 2018 FastyBird Ltd. <info@fastybird.com>

*/

#if WEB_SUPPORT

#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Hash.h>
#include <FS.h>
#include <AsyncJson.h>
#include <ArduinoJson.h>

#if WEB_EMBEDDED
    #include "static/index.html.gz.h"
#endif

#if NETWORK_ASYNC_TCP_SSL_ENABLED & WEB_SSL_ENABLED
    #include "static/server.cer.h"
    #include "static/server.key.h"
#endif

AsyncWebServer * _web_server;

char _web_last_modified[50];

std::vector<web_on_request_callback_f> _web_on_request_callbacks;

// -----------------------------------------------------------------------------

void _onReset(
    AsyncWebServerRequest * request
) {
    deferredReset(100, CUSTOM_RESET_WEB);

    request->send(200);
}

// -----------------------------------------------------------------------------

void _onDiscover(
    AsyncWebServerRequest * request
) {
    webLog(request);

    AsyncResponseStream *response = request->beginResponseStream("text/json");

    DynamicJsonBuffer jsonBuffer;

    JsonObject &root = jsonBuffer.createObject();

    root["manufacturer"] = FIRMWARE_MANUFACTURER;
    root["version"] = FIRMWARE_VERSION;
    root["hostname"] = getIdentifier();
    root["thing"] = THING_NAME;

    root.printTo(*response);

    request->send(response);
}

// -----------------------------------------------------------------------------

void _onUpgrade(
    AsyncWebServerRequest * request
) {
    webLog(request);

    if (!webAuthenticate(request)) {
        return request->requestAuthentication(getIdentifier().c_str());
    }

    char buffer[10];

    if (!Update.hasError()) {
        sprintf_P(buffer, PSTR("OK"));

    } else {
        sprintf_P(buffer, PSTR("ERROR %d"), Update.getError());
    }

    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", buffer);

    response->addHeader("Connection", "close");
    response->addHeader("X-XSS-Protection", "1; mode=block");
    response->addHeader("X-Content-Type-Options", "nosniff");
    response->addHeader("X-Frame-Options", "deny");

    if (Update.hasError()) {
        eepromRotate(true);

    } else {
        deferredReset(100, CUSTOM_RESET_UPGRADE);
    }

    request->send(response);
}

// -----------------------------------------------------------------------------

void _onUpgradeData(
    AsyncWebServerRequest * request,
    String filename,
    size_t index,
    uint8_t * data,
    size_t len,
    bool final
) {
    if (!index) {
        // Disabling EEPROM rotation to prevent writing to EEPROM after the upgrade
        eepromRotate(false);

        DEBUG_MSG(PSTR("[UPGRADE] Start: %s\n"), filename.c_str());

        Update.runAsync(true);

        if (!Update.begin((ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000)) {
            #ifdef DEBUG_PORT
                Update.printError(DEBUG_PORT);
            #endif
        }
    }

    if (!Update.hasError()) {
        if (Update.write(data, len) != len) {
            #ifdef DEBUG_PORT
                Update.printError(DEBUG_PORT);
            #endif
        }
    }

    if (final) {
        if (Update.end(true)){
            DEBUG_MSG(PSTR("[UPGRADE] Success:  %u bytes\n"), index + len);

        } else {
            #ifdef DEBUG_PORT
                Update.printError(DEBUG_PORT);
            #endif
        }

    } else {
        DEBUG_MSG(PSTR("[UPGRADE] Progress: %u bytes\r"), index + len);
    }
}

// -----------------------------------------------------------------------------

#if WEB_EMBEDDED
    void _onHome(
        AsyncWebServerRequest *request
    ) {
        webLog(request);

        if (request->header("If-Modified-Since").equals(_web_last_modified)) {
            request->send(304);

        } else {
            #if ASYNC_TCP_SSL_ENABLED
                // Chunked response, we calculate the chunks based on free heap (in multiples of 32)
                // This is necessary when a TLS connection is open since it sucks too much memory
                DEBUG_MSG_P(PSTR("[MAIN] Free heap: %d bytes\n"), getFreeHeap());

                size_t max = (getFreeHeap() / 3) & 0xFFE0;

                AsyncWebServerResponse *response = request->beginChunkedResponse("text/html", [max](uint8_t *buffer, size_t maxLen, size_t index) -> size_t {
                    // Get the chunk based on the index and maxLen
                    size_t len = webui_image_len - index;

                    if (len > maxLen) {
                        len = maxLen;
                    }

                    if (len > max) {
                        len = max;
                    }

                    if (len > 0) {
                        memcpy_P(buffer, webui_image + index, len);
                    }

                    DEBUG_MSG_P(PSTR("[WEB] Sending %d%%%% (max chunk size: %4d)\r"), int(100 * index / webui_image_len), max);

                    if (len == 0) {
                        DEBUG_MSG_P(PSTR("\n"));
                    }

                    // Return the actual length of the chunk (0 for end of file)
                    return len;
                });

            #else
                AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", webui_image, webui_image_len);
            #endif

            response->addHeader("Content-Encoding", "gzip");
            response->addHeader("Last-Modified", _web_last_modified);
            response->addHeader("X-XSS-Protection", "1; mode=block");
            response->addHeader("X-Content-Type-Options", "nosniff");
            response->addHeader("X-Frame-Options", "deny");

            request->send(response);
        }
    }
#endif

// -----------------------------------------------------------------------------

void _onRequest(
    AsyncWebServerRequest * request
){
    // Send request to subscribers
    for (uint8_t i = 0; i < _web_on_request_callbacks.size(); i++) {
        bool response = (_web_on_request_callbacks[i])(request);

        if (response) {
            return;
        }
    }

    // No subscriber handled the request, return a 404
    request->send(404);
}

// -----------------------------------------------------------------------------

#if NETWORK_ASYNC_TCP_SSL_ENABLED & WEB_SSL_ENABLED
    int _onCertificate(
        void * arg,
        const char * filename,
        uint8_t ** buf
    ) {
        #if WEB_EMBEDDED
            if (strcmp(filename, "server.cer") == 0) {
                uint8_t * nbuf = (uint8_t*) malloc(server_cer_len);
                memcpy_P(nbuf, server_cer, server_cer_len);
                *buf = nbuf;
                DEBUG_MSG(PSTR("[WEB] SSL File: %s - OK\n"), filename);

                return server_cer_len;
            }

            if (strcmp(filename, "server.key") == 0) {
                uint8_t * nbuf = (uint8_t*) malloc(server_key_len);
                memcpy_P(nbuf, server_key, server_key_len);
                *buf = nbuf;
                DEBUG_MSG(PSTR("[WEB] SSL File: %s - OK\n"), filename);

                return server_key_len;
            }

            DEBUG_MSG(PSTR("[WEB] SSL File: %s - ERROR\n"), filename);
            *buf = 0;

            return 0;
        #else
            File file = SPIFFS.open(filename, "r");

            if (file) {
                size_t size = file.size();
                uint8_t * nbuf = (uint8_t*) malloc(size);

                if (nbuf) {
                    size = file.read(nbuf, size);
                    file.close();
                    *buf = nbuf;
                    DEBUG_MSG(PSTR("[WEB] SSL File: %s - OK\n"), filename);

                    return size;
                }

                file.close();
            }

            DEBUG_MSG(PSTR("[WEB] SSL File: %s - ERROR\n"), filename);
            *buf = 0;

            return 0;
        #endif
    }
#endif

// -----------------------------------------------------------------------------
// MODULE API
// -----------------------------------------------------------------------------

AsyncWebServer * webServer() {
    return _web_server;
}

// -----------------------------------------------------------------------------

bool webAuthenticate(
    AsyncWebServerRequest * request
) {
    #if USE_PASSWORD
        String password = getAdminPass();

        char httpPassword[password.length() + 1];

        password.toCharArray(httpPassword, password.length() + 1);

        return request->authenticate(WEB_USERNAME, httpPassword);
    #else
        return true;
    #endif
}

// -----------------------------------------------------------------------------

void webOnRequestRegister(
    web_on_request_callback_f callback
) {
    _web_on_request_callbacks.push_back(callback);
}

// -----------------------------------------------------------------------------

uint8_t webPort() {
    #if NETWORK_ASYNC_TCP_SSL_ENABLED & WEB_SSL_ENABLED
        return 443;
    #else
        return getSetting("webPort", WEB_PORT).toInt();
    #endif
}

// -----------------------------------------------------------------------------

void webLog(
    AsyncWebServerRequest * request
) {
    DEBUG_MSG(PSTR("[WEBSERVER] Request: %s %s\n"), request->methodToString(), request->url().c_str());
}

// -----------------------------------------------------------------------------
// MODULE CORE
// -----------------------------------------------------------------------------

void webSetup() {
    // Cache the Last-Modifier header value
    snprintf_P(_web_last_modified, sizeof(_web_last_modified), PSTR("%s %s GMT"), __DATE__, __TIME__);

    // Create server
    uint8_t port = webPort();

    _web_server = new AsyncWebServer(port);

    // Rewrites
    _web_server->rewrite("/", "/index.html");

    // Serve home (basic authentication protection)
    #if WEB_EMBEDDED
        _web_server->on("/index.html", HTTP_GET, _onHome);
    #endif

    // Other entry points
    _web_server->on("/control/reset", HTTP_GET, _onReset);
    _web_server->on("/control/upgrade", HTTP_POST, _onUpgrade, _onUpgradeData);
    _web_server->on("/discover", HTTP_GET, _onDiscover);

    // Serve static files
    #if SPIFFS_SUPPORT
        _web_server->serveStatic("/", SPIFFS, "/")
            .setLastModified(_web_last_modified)
            .setFilter([](AsyncWebServerRequest * request) -> bool {
                webLog(request);
                return true;
            });
    #endif

    // Handle other requests, including 404
    _web_server->onNotFound(_onRequest);

    // Run server
    #if NETWORK_ASYNC_TCP_SSL_ENABLED & WEB_SSL_ENABLED
        _web_server->onSslFileRequest(_onCertificate, NULL);
        _web_server->beginSecure("server.cer", "server.key", NULL);
    #else
        _web_server->begin();
    #endif

    DEBUG_MSG(PSTR("[WEBSERVER] Webserver running on port %u\n"), port);
}

#endif // WEB_SUPPORT
