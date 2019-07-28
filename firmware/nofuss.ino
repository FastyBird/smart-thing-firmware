/*

NOFUSS MODULE

Copyright (C) 2018 FastyBird s.r.o. <info@fastybird.com>

*/

#if NOFUSS_SUPPORT

#include "NoFUSSClient.h"

uint32_t _nofussLastCheck = 0;
uint32_t _nofussInterval = 0;
bool _nofussEnabled = false;

// -----------------------------------------------------------------------------
// MODULE PRIVATE
// -----------------------------------------------------------------------------

#if FASTYBIRD_SUPPORT || (WEB_SUPPORT && WS_SUPPORT)
    /**
     * Provide module configuration schema
     */
    void _buttonReportConfigurationSchema(
        JsonArray& configuration
    ) {
        // Configuration field
        JsonObject& enabled = configuration.createNestedObject();

        enabled["name"] = "nofuss_enabled";
        enabled["type"] = "boolean";
        enabled["default"] = false;

        JsonObject& server = configuration.createNestedObject();

        server["name"] = "nofuss_server";
        server["type"] = "text";
        server["default"] = "";
    }

// -----------------------------------------------------------------------------

    /**
     * Report module configuration
     */
    void _nofussReportConfiguration(
        JsonObject& configuration
    ) {
        configuration["nofuss_enabled"] = getSetting("nofussEnabled", NOFUSS_ENABLED).toInt() == 1;
        configuration["nofuss_server"] = getSetting("nofussServer", NOFUSS_SERVER);
    }

// -----------------------------------------------------------------------------

    /**
     * Update module configuration via WS or MQTT etc.
     */
    bool _nofussUpdateConfiguration(
        JsonObject& configuration
    ) {
        DEBUG_MSG(PSTR("[NOFUSS] Updating module\n"));

        bool is_updated = false;

        if (
            configuration.containsKey("nofuss_enabled")
            && configuration["nofuss_enabled"].as<bool>() != (getSetting("nofussEnabled").toInt() == 1)
        )  {
            DEBUG_MSG(PSTR("[NOFUSS] Setting: \"nofuss_enabled\" to: %d\n"), (configuration["nofuss_enabled"].as<bool>() ? 1 : 0));

            setSetting("nofussEnabled", configuration["nofuss_enabled"].as<bool>() ? 1 : 0);

            is_updated = true;
        }
        
        if (
            configuration.containsKey("nofuss_server")
            && configuration["nofuss_server"].as<char *>() != getSetting("nofussServer").c_str()
        )  {
            DEBUG_MSG(PSTR("[NOFUSS] Setting: \"nofuss_server\" to: %s\n"), configuration["nofuss_server"].as<char *>());

            setSetting("nofussServer", configuration["nofuss_server"].as<char *>());

            is_updated = true;
        }

        return is_updated;
    }
#endif // FASTYBIRD_SUPPORT || (WEB_SUPPORT && WS_SUPPORT)

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

        _nofussReportConfiguration(configuration_values);

        // Configuration schema container
        JsonArray& configuration_schema = configuration.createNestedArray("schema");

        _nofussReportConfigurationSchema(configuration_schema);
    }

// -----------------------------------------------------------------------------

    // WS client requested configuration update
    void _nofussWSOnConfigure(
        const uint32_t clientId, 
        JsonObject& module
    ) {
        if (module.containsKey("module") && module["module"] == "nofuss") {
            if (module.containsKey("config")) {
                // Extract configuration container
                JsonObject& configuration = module["config"].as<JsonObject&>();

                if (
                    configuration.containsKey("values")
                    && _nofussUpdateConfiguration(configuration["values"])
                ) {
                    wsSend_P(clientId, PSTR("{\"message\": \"nofuss_updated\"}"));

                    // Reload & cache settings
                    firmwareReload();
                }
            }
        }
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
        	DEBUG_MSG(PSTR(
                "[NoFUSS] Wrong server response: %d %s\n"),
                NoFUSSClient.getErrorNumber(),
                (char *) NoFUSSClient.getErrorString().c_str()
            );
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
                wsSend_P(PSTR("{\"message\": \"updating_nofuss\"}"));
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

            #if WEB_SUPPORT && WS_SUPPORT
                // Send notification to all clients
                wsSend_P(PSTR("{\"doAction\": \"reload\", \"reason\": \"upgrade\", \"module\": \"nofuss\"}"));
            #endif

            deferredReset(250, CUSTOM_UPGRADE_NOFUSS);
        }

        if (code == NOFUSS_END) {
            DEBUG_MSG(PSTR("[NoFUSS] End\n"));

            eepromRotate(true);
        }

    });

    #if WEB_SUPPORT && WS_SUPPORT
        wsOnConnectRegister(_nofussWSOnConnect);
        wsOnConfigureRegister(_nofussWSOnConfigure);
    #endif

    #if FASTYBIRD_SUPPORT
        // Module schema report
        fastybirdReportConfigurationSchemaRegister(_nofussReportConfigurationSchema);
        fastybirdReportConfigurationRegister(_nofussReportConfiguration);
        fastybirdOnConfigureRegister(_nofussUpdateConfiguration);
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