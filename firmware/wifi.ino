/*

WIFI MODULE

Copyright (C) 2018 FastyBird s.r.o. <code@fastybird.com>

*/

#if WIFI_SUPPORT

uint32_t _wifi_scan_client_id = 0;

Ticker _wifi_defer;

// -----------------------------------------------------------------------------
// MODULE PRIVATE
// -----------------------------------------------------------------------------

void _wifiCheckAP()
{
    if (
        jw.connected()
        && (WiFi.getMode() & WIFI_AP) > 0
        && WiFi.softAPgetStationNum() == 0
    ) {
        jw.enableAP(false);
    }
}

// -----------------------------------------------------------------------------

void _wifiConfigure()
{
    jw.setHostname(getIdentifier().c_str());

    if (getSetting("wifi_password", WIFI_PASSWORD).length() == 0) {
        jw.setSoftAP(getIdentifier().c_str());

    } else {
        jw.setSoftAP(getIdentifier().c_str(), getSetting("wifi_password", WIFI_PASSWORD).c_str());
    }

    jw.setConnectTimeout(WIFI_CONNECT_TIMEOUT);

    wifiReconnectCheck();

    jw.enableAPFallback(WIFI_FALLBACK_APMODE);
    jw.cleanNetworks();

    // If system is flagged unstable we do not init wifi networks
    #if STABILTY_CHECK_ENABLED
        if (!stabiltyCheck()) {
            return;
        }
    #endif

    // Clean settings
    _wifiClean(WIFI_MAX_NETWORKS);

    for (uint8_t i = 0; i < WIFI_MAX_NETWORKS; i++) {
        if (getSetting("wifi_ssid", i, "").length() == 0) {
            break;
        }

        DEBUG_MSG(PSTR("[INFO][WIFI] Wifi configured, trying to connect\n"));

        if (getSetting("wifi_ip", i, "").length() == 0) {
            jw.addNetwork(
                getSetting("wifi_ssid", i, "").c_str(),
                getSetting("wifi_password", i, "").c_str()
            );

        } else {
            jw.addNetwork(
                getSetting("wifi_ssid", i, "").c_str(),
                getSetting("wifi_password", i, "").c_str(),
                getSetting("wifi_ip", i, "").c_str(),
                getSetting("wifi_gw", i, "").c_str(),
                getSetting("wifi_mask", i, "").c_str(),
                getSetting("wifi_dns", i, "").c_str()
            );
        }
    }

    jw.enableScan(getSetting("wifi_scan", WIFI_SCAN_NETWORKS).toInt() == 1);
}

// -----------------------------------------------------------------------------

void _wifiScan(
    const uint32_t clientId = 0
) {
    DEBUG_MSG(PSTR("[INFO][WIFI] Start scanning\n"));

    #if WEB_SUPPORT && WS_SUPPORT
        DynamicJsonBuffer json_buffer;

        JsonObject& output = json_buffer.createObject();

        output["module"] = "wifi";

        // Data container
        JsonObject& data = output.createNestedObject("data");

        data["status"] = WiFi.getMode() == WIFI_AP ? String("ap") : String("sta");

        // Registered networks
        JsonArray& networks = data.createNestedArray("networks");

        for (uint8_t i = 0; i < WIFI_MAX_NETWORKS; i++) {
            if (!hasSetting("wifi_ssid", i)) {
                break;
            }

            JsonObject& network = networks.createNestedObject();

            network["ssid"] = getSetting("wifi_ssid", i, "");
            network["password"] = getSetting("wifi_password", i, "");
            network["ip"] = getSetting("wifi_ip", i, "");
            network["gw"] = getSetting("wifi_gw", i, "");
            network["mask"] = getSetting("wifi_mask", i, "");
            network["dns"] = getSetting("wifi_dns", i, "");
        }

        // Found networks container
        JsonObject& scanResult = data.createNestedObject("scan");

        JsonArray& foundNetworks = scanResult.createNestedArray("networks");
    #endif

    uint8_t result = WiFi.scanNetworks();

    if (result == WIFI_SCAN_FAILED) {
        DEBUG_MSG(PSTR("[INFO][WIFI] Scan failed\n"));

        #if WEB_SUPPORT && WS_SUPPORT
            scanResult["result"] = String("Failed scan");
        #endif

    } else if (result == 0) {
        DEBUG_MSG(PSTR("[INFO][WIFI] No networks found\n"));

        #if WEB_SUPPORT && WS_SUPPORT
            scanResult["result"] = String("No networks found");
        #endif

    } else {
        DEBUG_MSG(PSTR("[INFO][WIFI] %d networks found:\n"), result);

        // Populate defined networks with scan data
        for (uint8_t i = 0; i < result; ++i) {
            String ssid_scan;
            int32_t rssi_scan;
            uint8_t sec_scan;
            uint8_t* BSSID_scan;
            int32_t chan_scan;
            bool hidden_scan;
            char buffer[128];

            WiFi.getNetworkInfo(i, ssid_scan, sec_scan, rssi_scan, BSSID_scan, chan_scan, hidden_scan);

            snprintf_P(buffer, sizeof(buffer),
                PSTR("BSSID: %02X:%02X:%02X:%02X:%02X:%02X SEC: %s RSSI: %3d CH: %2d SSID: %s"),
                BSSID_scan[1], BSSID_scan[2], BSSID_scan[3], BSSID_scan[4], BSSID_scan[5], BSSID_scan[6],
                (sec_scan != ENC_TYPE_NONE ? "YES" : "NO "),
                rssi_scan,
                chan_scan,
                (char *) ssid_scan.c_str()
            );

            DEBUG_MSG(PSTR("[INFO][WIFI] > %s\n"), buffer);

            #if WEB_SUPPORT && WS_SUPPORT
                if (clientId > 0) {
                    scanResult["result"] = "OK";
                    scanResult["found"] = result;

                    JsonObject& line = foundNetworks.createNestedObject();

                    snprintf_P(buffer, sizeof(buffer),
                        PSTR("%02X:%02X:%02X:%02X:%02X:%02X"),
                        BSSID_scan[1], BSSID_scan[2], BSSID_scan[3], BSSID_scan[4], BSSID_scan[5], BSSID_scan[6]
                    );

                    line["bssid"] = buffer;
                    line["sec"] = (sec_scan != ENC_TYPE_NONE ? true : false);
                    line["rssi"] = rssi_scan;
                    line["ch"] = chan_scan;
                    line["ssid"] = ssid_scan;
                }
            #endif
        }
    }

    #if WEB_SUPPORT && WS_SUPPORT
        if (clientId > 0) {
            String convertedOutput;

            output.printTo(convertedOutput);

            wsSend(clientId, convertedOutput.c_str());
        }
    #endif

    WiFi.scanDelete();
}

// -----------------------------------------------------------------------------

bool _wifiClean(
    const uint8_t num
) {
    bool changed = false;
    uint8_t i = 0;

    // Clean defined settings
    while (i < num) {
        // Skip on first non-defined setting
        if (!hasSetting("wifi_ssid", i)) {
            delSetting("wifi_ssid", i);
            break;
        }

        // Delete empty values
        if (!hasSetting("wifi_password", i)) delSetting("wifi_password", i);
        if (!hasSetting("wifi_ip", i)) delSetting("wifi_ip", i);
        if (!hasSetting("wifi_gw", i)) delSetting("wifi_gw", i);
        if (!hasSetting("wifi_mask", i)) delSetting("wifi_mask", i);
        if (!hasSetting("wifi_dns", i)) delSetting("wifi_dns", i);

        ++i;
    }

    // Delete all other settings
    while (i < WIFI_MAX_NETWORKS) {
        changed = hasSetting("wifi_ssid", i);

        delSetting("wifi_ssid", i);
        delSetting("wifi_pass", i);
        delSetting("wifi_ip", i);
        delSetting("wifi_gw", i);
        delSetting("wifi_mask", i);
        delSetting("wifi_dns", i);

        ++i;
    }

    return changed;
}

// -----------------------------------------------------------------------------

#if DEBUG_SUPPORT
    void _wifiDebugCallback(
        justwifi_messages_t code,
        char * parameter
    ) {
        if (code == MESSAGE_SCANNING) {
            DEBUG_MSG(PSTR("[INFO][WIFI] Scanning\n"));
        }

        if (code == MESSAGE_SCAN_FAILED) {
            DEBUG_MSG(PSTR("[INFO][WIFI] Scan failed\n"));
        }

        if (code == MESSAGE_NO_NETWORKS) {
            DEBUG_MSG(PSTR("[INFO][WIFI] No networks found\n"));
        }

        if (code == MESSAGE_NO_KNOWN_NETWORKS) {
            DEBUG_MSG(PSTR("[INFO][WIFI] No known networks found\n"));
        }

        if (code == MESSAGE_FOUND_NETWORK) {
            DEBUG_MSG(PSTR("[INFO][WIFI] %s\n"), parameter);
        }

        if (code == MESSAGE_CONNECTING) {
            DEBUG_MSG(PSTR("[INFO][WIFI] Connecting to %s\n"), parameter);
        }

        if (code == MESSAGE_CONNECT_WAITING) {
            // too much noise
        }

        if (code == MESSAGE_CONNECT_FAILED) {
            DEBUG_MSG(PSTR("[INFO][WIFI] Could not connect to %s\n"), parameter);
        }

        if (code == MESSAGE_CONNECTED) {
            wifiStatus();
        }

        if (code == MESSAGE_ACCESSPOINT_CREATED) {
            wifiStatus();
        }

        if (code == MESSAGE_DISCONNECTED) {
            DEBUG_MSG(PSTR("[INFO][WIFI] Disconnected\n"));
        }

        if (code == MESSAGE_ACCESSPOINT_CREATING) {
            DEBUG_MSG(PSTR("[INFO][WIFI] Creating access point\n"));
        }

        if (code == MESSAGE_ACCESSPOINT_FAILED) {
            DEBUG_MSG(PSTR("[INFO][WIFI] Could not create access point\n"));
        }
    }
#endif // DEBUG_SUPPORT

// -----------------------------------------------------------------------------

#if WEB_SUPPORT
    void _wifiOnReconnect(
        AsyncWebServerRequest * request
    ) {
        webLog(request);

        if (!webAuthenticate(request)) {
            return request->requestAuthentication(getIdentifier().c_str());
        }

        request->send(201);

        DEBUG_MSG(PSTR("[INFO][WIFI] Requested reconnect action\n"));

        #if WEB_SUPPORT && WS_SUPPORT
            // Send notification to all clients
            wsSend_P(PSTR("{\"doAction\": \"reload\", \"reason\": \"reconnect\"}"));
        #endif
        
        _wifi_defer.once_ms(250, wifiDisconnect);
    }

// -----------------------------------------------------------------------------

    void _wifiOnConfigure(
        AsyncWebServerRequest * request
    ) {
        webLog(request);

        if (!webAuthenticate(request)) {
            return request->requestAuthentication(getIdentifier().c_str());
        }

        request->send(201);

        // TODO: finish remote wifi configuration
    }
#endif // WEB_SUPPORT

// -----------------------------------------------------------------------------

#if WEB_SUPPORT && WS_SUPPORT
    /**
     * Provide module configuration schema
     */
    void _wifiReportConfigurationSchema(
        JsonArray& configuration
    ) {
        // Configuration field
        JsonObject& scan = configuration.createNestedObject();

        scan["name"] = "scan";
        scan["type"] = "boolean";
        scan["default"] = WIFI_SCAN_NETWORKS == 1 ? true : false;
    }

// -----------------------------------------------------------------------------

    /**
     * Report module configuration
     */
    void _wifiReportConfiguration(
        JsonObject& configuration
    ) {
        configuration["max"] = WIFI_MAX_NETWORKS;
        configuration["scan"] = getSetting("wifi_scan", WIFI_SCAN_NETWORKS).toInt() == 1;
    }

// -----------------------------------------------------------------------------

    /**
     * Update module configuration via WS or MQTT etc.
     */
    bool _wifiUpdateConfiguration(
        JsonObject& configuration
    ) {
        DEBUG_MSG(PSTR("[INFO][WIFI] Updating module\n"));

        bool is_updated = false;

        if (
            configuration.containsKey("scan")
            && configuration["scan"].as<bool>() != ((getSetting("wifi_scan").toInt() == 1))
        )  {
            DEBUG_MSG(PSTR("[INFO][WIFI] Setting: \"wifi_scan\" to: %d\n"), configuration["scan"].as<bool>() ? 1 : 0);

            setSetting("wifi_scan", configuration["scan"].as<bool>() ? 1 : 0);

            is_updated = true;
        }

        return is_updated;
    }

// -----------------------------------------------------------------------------

    // WS client is connected - send info about module
    void _wifiWSOnClientConnect(
        JsonObject& root
    ) {
        JsonArray& modules = root.containsKey("modules") ? root["modules"] : root.createNestedArray("modules");
        JsonObject& module = modules.createNestedObject();
        
        module["module"] = "wifi";
        module["visible"] = true;

        // Data container
        JsonObject& data = module.createNestedObject("data");

        data["status"] = WiFi.getMode() == WIFI_AP ? String("ap") : String("sta");

        // Registered networks
        JsonArray& networks = data.createNestedArray("networks");

        for (uint8_t i = 0; i < WIFI_MAX_NETWORKS; i++) {
            if (!hasSetting("wifi_ssid", i)) {
                break;
            }

            JsonObject& network = networks.createNestedObject();

            network["ssid"] = getSetting("wifi_ssid", i, "");
            network["password"] = getSetting("wifi_password", i, "");
            network["ip"] = getSetting("wifi_ip", i, "");
            network["gw"] = getSetting("wifi_gw", i, "");
            network["mask"] = getSetting("wifi_mask", i, "");
            network["dns"] = getSetting("wifi_dns", i, "");
        }

        // Configuration container
        JsonObject& configuration = module.createNestedObject("config");

        // Configuration values container
        JsonObject& configuration_values = configuration.createNestedObject("values");

        _wifiReportConfiguration(configuration_values);

        // Configuration schema container
        JsonArray& configuration_schema = configuration.createNestedArray("schema");

        _wifiReportConfigurationSchema(configuration_schema);
    }

// -----------------------------------------------------------------------------

    // WS client send configuration request
    void _wifiWSOnConfigure(
        const uint32_t clientId,
        JsonObject& module
    ) {
        if (module.containsKey("module") && module["module"] == "wifi") {
            if (module.containsKey("config")) {
                JsonObject& configuration = module["config"].as<JsonObject&>();

                if (
                    configuration.containsKey("values")
                    && _wifiUpdateConfiguration(configuration["values"])
                ) {
                    wsSend_P(clientId, PSTR("{\"message\": \"wifi_updated\"}"));

                    // Reload & cache settings
                    firmwareReload();
                }

                if (configuration.containsKey("networks")) {
                    uint8_t i = 0;

                    // Delete all other configuration
                    while (i < WIFI_MAX_NETWORKS) {
                        delSetting("wifi_ssid", i);
                        delSetting("wifi_password", i);
                        delSetting("wifi_ip", i);
                        delSetting("wifi_gw", i);
                        delSetting("wifi_mask", i);
                        delSetting("wifi_dns", i);

                        ++i;
                    }

                    for (uint8_t i = 0; i < configuration["networks"].size(); i++) {
                        setSetting("wifi_ssid", i, configuration["networks"][i]["ssid"].as<char *>());
                        setSetting("wifi_password", i, configuration["networks"][i]["password"].as<char *>());
                        setSetting("wifi_ip", i, configuration["networks"][i]["ip"].as<char *>());
                        setSetting("wifi_gw", i, configuration["networks"][i]["gw"].as<char *>());
                        setSetting("wifi_mask", i, configuration["networks"][i]["mask"].as<char *>());
                        setSetting("wifi_dns", i, configuration["networks"][i]["dns"].as<char *>());
                    }

                    wsSend_P(clientId, PSTR("{\"message\": \"wifi_updated\"}"));

                    // Send notification to all clients
                    wsSend_P(clientId, PSTR("{\"doAction\": \"reload\", \"reason\": \"wifi_reconfigure\"}"));

                    // Reconfigure wifi
                    _wifiConfigure();

                    _wifi_defer.once_ms(250, wifiDisconnect);
                }
            }
        }
    }

// -----------------------------------------------------------------------------

    // WS client send module action request
    void _wifiWSOnAction(
        const uint32_t clientId,
        const char * action,
        JsonObject& data
    ) {
        if (strcmp(action, "scan") == 0) {
            _wifi_scan_client_id = clientId;
        }
    }
#endif

// -----------------------------------------------------------------------------
// MODULE API
// -----------------------------------------------------------------------------

String getIP()
{
    if (WiFi.getMode() == WIFI_AP) {
        return WiFi.softAPIP().toString();
    }

    return WiFi.localIP().toString();
}

// -----------------------------------------------------------------------------

String getNetwork()
{
    if (WiFi.getMode() == WIFI_AP) {
        return jw.getAPSSID();
    }

    return WiFi.SSID();
}

// -----------------------------------------------------------------------------

bool wifiIsConnected()
{
    return jw.connected();
}

// -----------------------------------------------------------------------------

void wifiDisconnect()
{
    jw.disconnect();
}

// -----------------------------------------------------------------------------

void wifiStartAP(
    const bool only
) {
    if (only) {
        jw.enableSTA(false);
        jw.disconnect();
        jw.resetReconnectTimeout();
    }

    jw.enableAP(true);
}

// -----------------------------------------------------------------------------

void wifiStartAP()
{
    wifiStartAP(true);
}

// -----------------------------------------------------------------------------

void wifiReconnectCheck()
{
    bool connected = false;

    #if WS_SUPPORT
        if (wsConnected()) {
            connected = true;
        }
    #endif

    jw.setReconnectTimeout(connected ? 0 : WIFI_RECONNECT_INTERVAL);
}

// -----------------------------------------------------------------------------

void wifiStatus()
{
    if (WiFi.getMode() == WIFI_AP_STA) {
        DEBUG_MSG(PSTR("[INFO][WIFI] MODE AP + STA --------------------------------\n"));

    } else if (WiFi.getMode() == WIFI_AP) {
        DEBUG_MSG(PSTR("[INFO][WIFI] MODE AP --------------------------------------\n"));

    } else if (WiFi.getMode() == WIFI_STA) {
        DEBUG_MSG(PSTR("[INFO][WIFI] MODE STA -------------------------------------\n"));

    } else {
        DEBUG_MSG(PSTR("[INFO][WIFI] MODE OFF -------------------------------------\n"));
        DEBUG_MSG(PSTR("[INFO][WIFI] No connection\n"));
    }

    if ((WiFi.getMode() & WIFI_AP) == WIFI_AP) {
        DEBUG_MSG(PSTR("[INFO][WIFI] SSID  %s\n"), jw.getAPSSID().c_str());
        DEBUG_MSG(PSTR("[INFO][WIFI] IP    %s\n"), WiFi.softAPIP().toString().c_str());
        DEBUG_MSG(PSTR("[INFO][WIFI] MAC   %s\n"), WiFi.softAPmacAddress().c_str());

    } else if ((WiFi.getMode() & WIFI_STA) == WIFI_STA) {
        uint8_t * bssid = WiFi.BSSID();

        DEBUG_MSG(PSTR("[INFO][WIFI] SSID  %s\n"), WiFi.SSID().c_str());
        DEBUG_MSG(PSTR("[INFO][WIFI] IP    %s\n"), WiFi.localIP().toString().c_str());
        DEBUG_MSG(PSTR("[INFO][WIFI] MAC   %s\n"), WiFi.macAddress().c_str());
        DEBUG_MSG(PSTR("[INFO][WIFI] GW    %s\n"), WiFi.gatewayIP().toString().c_str());
        DEBUG_MSG(PSTR("[INFO][WIFI] DNS   %s\n"), WiFi.dnsIP().toString().c_str());
        DEBUG_MSG(PSTR("[INFO][WIFI] MASK  %s\n"), WiFi.subnetMask().toString().c_str());
        DEBUG_MSG(PSTR("[INFO][WIFI] BSSID %02X:%02X:%02X:%02X:%02X:%02X\n"),
            bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5], bssid[6]
        );
        DEBUG_MSG(PSTR("[INFO][WIFI] CH    %d\n"), WiFi.channel());
        DEBUG_MSG(PSTR("[INFO][WIFI] RSSI  %d\n"), WiFi.RSSI());
    }

    DEBUG_MSG(PSTR("[INFO][WIFI] ----------------------------------------------\n"));
}

// -----------------------------------------------------------------------------

uint8_t wifiState()
{
    uint8_t state = 0;

    if (jw.connected()) {
        state += WIFI_STATE_STA;
    }

    if (jw.connectable()) {
        state += WIFI_STATE_AP;
    }

    return state;
}

// -----------------------------------------------------------------------------

void wifiRegister(
    wifi_callback_t callback
) {
    jw.subscribe(callback);
}

// -----------------------------------------------------------------------------
// MODULE CORE
// -----------------------------------------------------------------------------

void wifiSetup()
{
    WiFi.setSleepMode(WIFI_SLEEP_MODE);

    _wifiConfigure();

    #if DEBUG_SUPPORT
        wifiRegister(_wifiDebugCallback);
    #endif

    #if WEB_SUPPORT
        webEventsRegister([](AsyncWebServer * server) {
            webServer()->on(WEB_API_NETWORK_RECONNECT, HTTP_PUT, _wifiOnReconnect);
            webServer()->on(WEB_API_NETWORK_CONFIGURATION, HTTP_POST, _wifiOnConfigure);
        });

        #if WS_SUPPORT
            wsOnConnectRegister(_wifiWSOnClientConnect);
            wsOnConfigureRegister(_wifiWSOnConfigure);
            wsOnActionRegister(_wifiWSOnAction);
        #endif
    #endif

    #if BUTTON_SUPPORT && WIFI_AP_BTN_INDEX != INDEX_NONE
        buttonOnEventRegister(
            [](uint8_t event) {
                if (event == WIFI_AP_BTN_EVENT) {
                    wifiStartAP();
                }
            },
            (uint8_t) WIFI_AP_BTN_INDEX
        );
    #endif

    #if FASTYBIRD_SUPPORT
        fastybirdOnControlRegister(
            [](const uint8_t controlIndex, const char * payload) {
                DEBUG_MSG(PSTR("[INFO][WIFI] Requested reconnect action\n"));

                #if WEB_SUPPORT && WS_SUPPORT
                    // Send notification to all clients
                    wsSend_P(PSTR("{\"doAction\": \"reload\", \"reason\": \"reconnect\"}"));
                #endif
                
                _wifi_defer.once_ms(250, wifiDisconnect);
            },
            FASTYBIRD_DEVICE_CONTROL_RECONNECT
        );
    #endif

    // Register loop
    firmwareRegisterLoop(wifiLoop);
}

// -----------------------------------------------------------------------------

void wifiLoop()
{
    jw.loop();

    // Request for available networks scan
    if (_wifi_scan_client_id > 0) {
        _wifiScan(_wifi_scan_client_id);
        _wifi_scan_client_id = 0;
    }
}

#endif // WIFI_SUPPORT