/*

NOFUSS MODULE

Copyright (C) 2018 FastyBird Ltd. <info@fastybird.com>

*/

#if NOFUSS_SUPPORT

#include "NoFUSSClient.h"

uint32_t _nofussLastCheck = 0;
uint32_t _nofussInterval = 0;
bool _nofussEnabled = false;

// -----------------------------------------------------------------------------
// MODULE PRIVATE
// -----------------------------------------------------------------------------

#if WEB_SUPPORT
    void _nofussWSOnConnect(
        JsonObject& root
    ) {
        JsonArray& modules = root.containsKey("modules") ? root["modules"] : root.createNestedArray("modules");
        JsonObject& module = modules.createNestedObject();

        module["module"] = "nofuss";
        module["visible"] = true;

        // Configuration container
        JsonObject& configuration = module.createNestedObject("config");

        // Configuration values container
        JsonObject& configuration_values = configuration.createNestedObject("values");

        configuration_values["enabled"] = getSetting("nofussEnabled", NOFUSS_ENABLED).toInt() == 1;
        configuration_values["server"] = getSetting("nofussServer", NOFUSS_SERVER);

        // Configuration schema container
        JsonArray& configuration_schema = configuration.createNestedArray("schema");

        JsonObject& enabled = configuration_schema.createNestedObject();

        enabled["name"] = "enabled";
        enabled["type"] = "boolean";
        enabled["default"] = false;

        JsonObject& server = configuration_schema.createNestedObject();

        server["name"] = "server";
        server["type"] = "text";
        server["default"] = "";
    }
#endif

// -----------------------------------------------------------------------------

void _nofussConfigure() {
    String nofussServer = getSetting("nofussServer", NOFUSS_SERVER);

    if (nofussServer.length() == 0) {
        setSetting("nofussEnabled", 0);
        _nofussEnabled = false;

    } else {
        _nofussEnabled = getSetting("nofussEnabled", NOFUSS_ENABLED).toInt() == 1;
    }

    _nofussInterval = getSetting("nofussInterval", NOFUSS_INTERVAL).toInt();
    _nofussLastCheck = 0;

    if (!_nofussEnabled) {
        DEBUG_MSG(PSTR("[NOFUSS] Disabled\n"));

    } else {
        char buffer[20];

        snprintf_P(buffer, sizeof(buffer), PSTR("%s-%s"), FIRMWARE_MANUFACTURER, THING);

        NoFUSSClient.setServer(nofussServer);
        NoFUSSClient.setDevice(buffer);
        NoFUSSClient.setVersion(FIRMWARE_VERSION);

        DEBUG_MSG(PSTR("[NOFUSS] Server : %s\n"), nofussServer.c_str());
        DEBUG_MSG(PSTR("[NOFUSS] Thing: %s\n"), buffer);
        DEBUG_MSG(PSTR("[NOFUSS] Version: %s\n"), FIRMWARE_VERSION);
        DEBUG_MSG(PSTR("[NOFUSS] Enabled\n"));
    }
}

// -----------------------------------------------------------------------------

void nofussRun() {
    NoFUSSClient.handle();
    _nofussLastCheck = millis();
}

// -----------------------------------------------------------------------------
// MODULE CORE
// -----------------------------------------------------------------------------

void nofussSetup() {
    _nofussConfigure();

    NoFUSSClient.onMessage([](nofuss_t code) {
        if (code == NOFUSS_START) {
        	DEBUG_MSG(PSTR("[NoFUSS] Start\n"));
        }

        if (code == NOFUSS_UPTODATE) {
        	DEBUG_MSG(PSTR("[NoFUSS] Already in the last version\n"));
        }

        if (code == NOFUSS_NO_RESPONSE_ERROR) {
        	DEBUG_MSG(PSTR("[NoFUSS] Wrong server response: %d %s\n"), NoFUSSClient.getErrorNumber(), (char *) NoFUSSClient.getErrorString().c_str());
        }

        if (code == NOFUSS_PARSE_ERROR) {
        	DEBUG_MSG(PSTR("[NoFUSS] Error parsing server response\n"));
        }

        if (code == NOFUSS_UPDATING) {
        	DEBUG_MSG(PSTR("[NoFUSS] Updating\n"));
    	    DEBUG_MSG(PSTR("         New version: %s\n"), (char *) NoFUSSClient.getNewVersion().c_str());
        	DEBUG_MSG(PSTR("         Firmware: %s\n"), (char *) NoFUSSClient.getNewFirmware().c_str());
        	DEBUG_MSG(PSTR("         File System: %s\n"), (char *) NoFUSSClient.getNewFileSystem().c_str());
            #if WEB_SUPPORT && WS_SUPPORT
                wsSend_P(PSTR("{\"message\": 1}"));
            #endif

            // Disabling EEPROM rotation to prevent writing to EEPROM after the upgrade
            eepromRotate(false);
        }

        if (code == NOFUSS_FILESYSTEM_UPDATE_ERROR) {
        	DEBUG_MSG(PSTR("[NoFUSS] File System Update Error: %s\n"), (char *) NoFUSSClient.getErrorString().c_str());
        }

        if (code == NOFUSS_FILESYSTEM_UPDATED) {
        	DEBUG_MSG(PSTR("[NoFUSS] File System Updated\n"));
        }

        if (code == NOFUSS_FIRMWARE_UPDATE_ERROR) {
            DEBUG_MSG(PSTR("[NoFUSS] Firmware Update Error: %s\n"), (char *) NoFUSSClient.getErrorString().c_str());
        }

        if (code == NOFUSS_FIRMWARE_UPDATED) {
        	DEBUG_MSG(PSTR("[NoFUSS] Firmware Updated\n"));
        }

        if (code == NOFUSS_RESET) {
        	DEBUG_MSG(PSTR("[NoFUSS] Resetting board\n"));
            #if WEB_SUPPORT
                wsSend_P(PSTR("{\"action\": \"reload\"}"));
            #endif
            niceDelay(100);
        }

        if (code == NOFUSS_END) {
            DEBUG_MSG(PSTR("[NoFUSS] End\n"));
            eepromRotate(true);
        }

    });

    #if WEB_SUPPORT && WS_SUPPORT
        wsOnConnectRegister(_nofussWSOnConnect);
    #endif

    // Main callbacks
    firmwareRegisterLoop(nofussLoop);
    firmwareRegisterReload(_nofussConfigure);
}

// -----------------------------------------------------------------------------

void nofussLoop() {
    if (!_nofussEnabled) {
        return;
    }

    #if WIFI_SUPPORT
        if (!wifiIsConnected()) {
            return;
        }
    #endif

    if ((_nofussLastCheck > 0) && ((millis() - _nofussLastCheck) < _nofussInterval)) {
        return;
    }

    nofussRun();
}

#endif // NOFUSS_SUPPORT