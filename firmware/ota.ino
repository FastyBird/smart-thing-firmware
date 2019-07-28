/*

OTA MODULE

Copyright (C) 2018 FastyBird s.r.o. <info@fastybird.com>

*/

#include "ArduinoOTA.h"

// -----------------------------------------------------------------------------
// MODULE PRIVATE
// -----------------------------------------------------------------------------

void _otaConfigure() {
    ArduinoOTA.setPort(OTA_PORT);
    ArduinoOTA.setHostname(getIdentifier().c_str());
    ArduinoOTA.setPassword(getSetting("adminPass", ADMIN_PASSWORD).c_str());
}

// -----------------------------------------------------------------------------

void _otaLoop() {
    ArduinoOTA.handle();
}

// -----------------------------------------------------------------------------
// MODULE API
// -----------------------------------------------------------------------------

void otaSetup() {
    _otaConfigure();

    // Register loop
    firmwareRegisterLoop(_otaLoop);

    // -------------------------------------------------------------------------

    ArduinoOTA.onStart([]() {
        DEBUG_MSG(PSTR("[OTA] Start\n"));

        #if WEB_SUPPORT
            wsSend_P(PSTR("{\"message\": \"ota_started\"}"));
        #endif
    });

    ArduinoOTA.onEnd([]() {
        DEBUG_MSG(PSTR("\n"));
        DEBUG_MSG(PSTR("[OTA] Done, restarting...\n"));

        #if WEB_SUPPORT && WS_SUPPORT
            // Send notification to all clients
            wsSend_P(PSTR("{\"message\": \"ota_finished\"}"));
            wsSend_P(PSTR("{\"doAction\": \"reload\", \"reason\": \"upgrade\", \"module\": \"ota\"}"));
        #endif

        deferredReset(250, CUSTOM_UPGRADE_OTA);
    });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        DEBUG_MSG(PSTR("[OTA] Progress: %u%%\r"), (progress / (total / 100)));
    });

    ArduinoOTA.onError([](ota_error_t error) {
        #if DEBUG_SUPPORT
            DEBUG_MSG(PSTR("\n[OTA] Error #%u: "), error);

            if (error == OTA_AUTH_ERROR) {
                DEBUG_MSG(PSTR("Auth Failed\n"));

            } else if (error == OTA_BEGIN_ERROR) {
                DEBUG_MSG(PSTR("Begin Failed\n"));

            } else if (error == OTA_CONNECT_ERROR) {
                DEBUG_MSG(PSTR("Connect Failed\n"));

            } else if (error == OTA_RECEIVE_ERROR) {
                DEBUG_MSG(PSTR("Receive Failed\n"));

            } else if (error == OTA_END_ERROR) {
                DEBUG_MSG(PSTR("End Failed\n"));
            }
        #endif
    });

    ArduinoOTA.begin();
}
