/*

WIFI MODULE

Copyright (C) 2018 FastyBird Ltd. <info@fastybird.com>

*/

#if WIFI_SUPPORT

#include <JustWifi.h>
#include <Ticker.h>

uint32_t _wifi_scan_client_id = 0;

// -----------------------------------------------------------------------------
// MODULE PRIVATE
// -----------------------------------------------------------------------------

void _wifiCheckAP() {
    if (
        jw.connected()
        && (WiFi.getMode() & WIFI_AP) > 0
        && WiFi.softAPgetStationNum() == 0
    ) {
        jw.enableAP(false);
    }
}

// -----------------------------------------------------------------------------

void _wifiConfigure() {
    jw.setHostname(getIdentifier().c_str());
    #if USE_PASSWORD
        jw.setSoftAP(getIdentifier().c_str(), getAdminPass().c_str());
    #else
        jw.setSoftAP(getIdentifier().c_str());
    #endif
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

    for (unsigned int i = 0; i < WIFI_MAX_NETWORKS; i++) {
        if (getSetting("ssid", i, "").length() == 0) {
            break;
        }

        DEBUG_MSG(PSTR("[WIFI] Wifi configured, trying to connect\n"));

        if (getSetting("ip", i, "").length() == 0) {
            jw.addNetwork(
                getSetting("ssid", i, "").c_str(),
                getSetting("pass", i, "").c_str()
            );

        } else {
            jw.addNetwork(
                getSetting("ssid", i, "").c_str(),
                getSetting("pass", i, "").c_str(),
                getSetting("ip", i, "").c_str(),
                getSetting("gw", i, "").c_str(),
                getSetting("mask", i, "").c_str(),
                getSetting("dns", i, "").c_str()
            );
        }
    }

    jw.enableScan(getSetting("wifiScan", WIFI_SCAN_NETWORKS).toInt() == 1);
}

// -----------------------------------------------------------------------------

void _wifiScan(uint32_t clientId = 0) {
    DEBUG_MSG(PSTR("[WIFI] Start scanning\n"));

    #if WEB_SUPPORT
        DynamicJsonBuffer jsonBuffer;

        JsonObject& output = jsonBuffer.createObject();

        output["module"] = "wifi";

        // Data container
        JsonObject& data = output.createNestedObject("data");

        // Found networks container
        JsonObject& scanResult = data.createNestedObject("scan");

        JsonArray& networks = scanResult.createNestedArray("networks");
    #endif

    unsigned int result = WiFi.scanNetworks();

    if (result == WIFI_SCAN_FAILED) {
        DEBUG_MSG(PSTR("[WIFI] Scan failed\n"));

        #if WEB_SUPPORT
            scanResult["result"] = String("Failed scan");
        #endif

    } else if (result == 0) {
        DEBUG_MSG(PSTR("[WIFI] No networks found\n"));

        #if WEB_SUPPORT
            scanResult["result"] = String("No networks found");
        #endif

    } else {
        DEBUG_MSG(PSTR("[WIFI] %d networks found:\n"), result);

        // Populate defined networks with scan data
        for (unsigned int i = 0; i < result; ++i) {
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

            DEBUG_MSG(PSTR("[WIFI] > %s\n"), buffer);

            #if WEB_SUPPORT && WS_SUPPORT
                if (clientId > 0) {
                    scanResult["result"] = String("OK");

                    JsonObject& line = networks.createNestedObject();

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

bool _wifiClean(unsigned int num) {
    bool changed = false;
    int i = 0;

    // Clean defined settings
    while (i < num) {
        // Skip on first non-defined setting
        if (!hasSetting("ssid", i)) {
            delSetting("ssid", i);
            break;
        }

        // Delete empty values
        if (!hasSetting("pass", i)) delSetting("pass", i);
        if (!hasSetting("ip", i)) delSetting("ip", i);
        if (!hasSetting("gw", i)) delSetting("gw", i);
        if (!hasSetting("mask", i)) delSetting("mask", i);
        if (!hasSetting("dns", i)) delSetting("dns", i);

        ++i;
    }

    // Delete all other settings
    while (i < WIFI_MAX_NETWORKS) {
        changed = hasSetting("ssid", i);

        delSetting("ssid", i);
        delSetting("pass", i);
        delSetting("ip", i);
        delSetting("gw", i);
        delSetting("mask", i);
        delSetting("dns", i);

        ++i;
    }

    return changed;
}

// -----------------------------------------------------------------------------

// Inject hardcoded networks
void _wifiInject() {
    if (strlen(WIFI1_SSID)) {
        if (!hasSetting("ssid", 0)) {
            setSetting("ssid", 0, WIFI1_SSID);
            setSetting("pass", 0, WIFI1_PASS);
            setSetting("ip", 0, WIFI1_IP);
            setSetting("gw", 0, WIFI1_GW);
            setSetting("mask", 0, WIFI1_MASK);
            setSetting("dns", 0, WIFI1_DNS);
        }

        if (strlen(WIFI2_SSID)) {
            if (!hasSetting("ssid", 1)) {
                setSetting("ssid", 1, WIFI2_SSID);
                setSetting("pass", 1, WIFI2_PASS);
                setSetting("ip", 1, WIFI2_IP);
                setSetting("gw", 1, WIFI2_GW);
                setSetting("mask", 1, WIFI2_MASK);
                setSetting("dns", 1, WIFI2_DNS);
            }
        }
    }
}

// -----------------------------------------------------------------------------

#if DEBUG_SUPPORT
    void _wifiDebugCallback(justwifi_messages_t code, char * parameter) {
        if (code == MESSAGE_SCANNING) {
            DEBUG_MSG(PSTR("[WIFI] Scanning\n"));
        }

        if (code == MESSAGE_SCAN_FAILED) {
            DEBUG_MSG(PSTR("[WIFI] Scan failed\n"));
        }

        if (code == MESSAGE_NO_NETWORKS) {
            DEBUG_MSG(PSTR("[WIFI] No networks found\n"));
        }

        if (code == MESSAGE_NO_KNOWN_NETWORKS) {
            DEBUG_MSG(PSTR("[WIFI] No known networks found\n"));
        }

        if (code == MESSAGE_FOUND_NETWORK) {
            DEBUG_MSG(PSTR("[WIFI] %s\n"), parameter);
        }

        if (code == MESSAGE_CONNECTING) {
            DEBUG_MSG(PSTR("[WIFI] Connecting to %s\n"), parameter);
        }

        if (code == MESSAGE_CONNECT_WAITING) {
            // too much noise
        }

        if (code == MESSAGE_CONNECT_FAILED) {
            DEBUG_MSG(PSTR("[WIFI] Could not connect to %s\n"), parameter);
        }

        if (code == MESSAGE_CONNECTED) {
            wifiStatus();
        }

        if (code == MESSAGE_ACCESSPOINT_CREATED) {
            wifiStatus();
        }

        if (code == MESSAGE_DISCONNECTED) {
            DEBUG_MSG(PSTR("[WIFI] Disconnected\n"));
        }

        if (code == MESSAGE_ACCESSPOINT_CREATING) {
            DEBUG_MSG(PSTR("[WIFI] Creating access point\n"));
        }

        if (code == MESSAGE_ACCESSPOINT_FAILED) {
            DEBUG_MSG(PSTR("[WIFI] Could not create access point\n"));
        }
    }
#endif // DEBUG_SUPPORT

// -----------------------------------------------------------------------------

#if WEB_SUPPORT
    // WS client is connected - send info about module
    void _wifiWSOnClientConnect(JsonObject& root) {
        JsonArray& modules = root.containsKey("modules") ? root["modules"] : root.createNestedArray("modules");
        JsonObject& module = modules.createNestedObject();
        
        module["module"] = "wifi";
        module["visible"] = true;

        // Configuration container
        JsonObject& configuration = module.createNestedObject("config");

        configuration["max"] = WIFI_MAX_NETWORKS;
        configuration["scan"] = getSetting("wifiScan", WIFI_SCAN_NETWORKS).toInt() == 1;

        // Data container
        JsonObject& data = module.createNestedObject("data");

        data["status"] = WiFi.getMode() == WIFI_AP ? String("ap") : String("sta");

        // Registered networks
        JsonArray& networks = data.createNestedArray("networks");

        for (unsigned int i = 0; i < WIFI_MAX_NETWORKS; i++) {
            if (!hasSetting("ssid", i)) {
                break;
            }

            JsonObject& network = networks.createNestedObject();

            network["ssid"] = getSetting("ssid", i, "");
            network["pass"] = getSetting("pass", i, "");
            network["ip"] = getSetting("ip", i, "");
            network["gw"] = getSetting("gw", i, "");
            network["mask"] = getSetting("mask", i, "");
            network["dns"] = getSetting("dns", i, "");
        }
    }

// -----------------------------------------------------------------------------

    // WS client send configuration request
    void _wifiWSOnConfigure(
        uint32_t clientId,
        JsonObject& module
    ) {
        if (module.containsKey("module") && module["module"] == "wifi") {
            if (module.containsKey("config")) {
                JsonObject& configuration = module["config"].as<JsonObject&>();

                if (configuration.containsKey("scan")) {
                    setSetting("wifiScan", configuration["scan"].as<bool>() ? 1 : 0);
                }

                if (configuration.containsKey("networks")) {
                    unsigned int i = 0;

                    // Delete all other configuration
                    while (i < WIFI_MAX_NETWORKS) {
                        delSetting("ssid", i);
                        delSetting("pass", i);
                        delSetting("ip", i);
                        delSetting("gw", i);
                        delSetting("mask", i);
                        delSetting("dns", i);

                        ++i;
                    }

                    for (unsigned int i = 0; i < configuration["networks"].size(); i++) {
                        setSetting("ssid", i, configuration["networks"][i]["ssid"].as<char *>());
                        setSetting("pass", i, configuration["networks"][i]["pass"].as<char *>());
                        setSetting("ip", i, configuration["networks"][i]["ip"].as<char *>());
                        setSetting("gw", i, configuration["networks"][i]["gw"].as<char *>());
                        setSetting("mask", i, configuration["networks"][i]["mask"].as<char *>());
                        setSetting("dns", i, configuration["networks"][i]["dns"].as<char *>());
                    }

                    // Send message
                    wsSend_P(clientId, PSTR("{\"message\": \"wifi_updated\"}"));

                    // Reconfigure wifi
                    _wifiConfigure();

                    wifiDisconnect();
                }
            }
        }
    }

// -----------------------------------------------------------------------------

    // WS client send module action request
    void _wifiWSOnAction(
        uint32_t clientId,
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

String getIP() {
    if (WiFi.getMode() == WIFI_AP) {
        return WiFi.softAPIP().toString();
    }

    return WiFi.localIP().toString();
}

// -----------------------------------------------------------------------------

String getNetwork() {
    if (WiFi.getMode() == WIFI_AP) {
        return jw.getAPSSID();
    }

    return WiFi.SSID();
}

// -----------------------------------------------------------------------------

bool wifiIsConnected() {
    return jw.connected();
}

// -----------------------------------------------------------------------------

void wifiDisconnect() {
    jw.disconnect();
}

// -----------------------------------------------------------------------------

void wifiStartAP(bool only) {
    if (only) {
        jw.enableSTA(false);
        jw.disconnect();
        jw.resetReconnectTimeout();
    }

    jw.enableAP(true);
}

// -----------------------------------------------------------------------------

void wifiStartAP() {
    wifiStartAP(true);
}

// -----------------------------------------------------------------------------

void wifiReconnectCheck() {
    bool connected = false;

    #if WS_SUPPORT
        if (wsConnected()) {
            connected = true;
        }
    #endif

    jw.setReconnectTimeout(connected ? 0 : WIFI_RECONNECT_INTERVAL);
}

// -----------------------------------------------------------------------------

void wifiStatus() {
    if (WiFi.getMode() == WIFI_AP_STA) {
        DEBUG_MSG(PSTR("[WIFI] MODE AP + STA --------------------------------\n"));

    } else if (WiFi.getMode() == WIFI_AP) {
        DEBUG_MSG(PSTR("[WIFI] MODE AP --------------------------------------\n"));

    } else if (WiFi.getMode() == WIFI_STA) {
        DEBUG_MSG(PSTR("[WIFI] MODE STA -------------------------------------\n"));

    } else {
        DEBUG_MSG(PSTR("[WIFI] MODE OFF -------------------------------------\n"));
        DEBUG_MSG(PSTR("[WIFI] No connection\n"));
    }

    if ((WiFi.getMode() & WIFI_AP) == WIFI_AP) {
        DEBUG_MSG(PSTR("[WIFI] SSID  %s\n"), jw.getAPSSID().c_str());
        DEBUG_MSG(PSTR("[WIFI] IP    %s\n"), WiFi.softAPIP().toString().c_str());
        DEBUG_MSG(PSTR("[WIFI] MAC   %s\n"), WiFi.softAPmacAddress().c_str());

    } else if ((WiFi.getMode() & WIFI_STA) == WIFI_STA) {
        uint8_t * bssid = WiFi.BSSID();

        DEBUG_MSG(PSTR("[WIFI] SSID  %s\n"), WiFi.SSID().c_str());
        DEBUG_MSG(PSTR("[WIFI] IP    %s\n"), WiFi.localIP().toString().c_str());
        DEBUG_MSG(PSTR("[WIFI] MAC   %s\n"), WiFi.macAddress().c_str());
        DEBUG_MSG(PSTR("[WIFI] GW    %s\n"), WiFi.gatewayIP().toString().c_str());
        DEBUG_MSG(PSTR("[WIFI] DNS   %s\n"), WiFi.dnsIP().toString().c_str());
        DEBUG_MSG(PSTR("[WIFI] MASK  %s\n"), WiFi.subnetMask().toString().c_str());
        DEBUG_MSG(PSTR("[WIFI] BSSID %02X:%02X:%02X:%02X:%02X:%02X\n"),
            bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5], bssid[6]
        );
        DEBUG_MSG(PSTR("[WIFI] CH    %d\n"), WiFi.channel());
        DEBUG_MSG(PSTR("[WIFI] RSSI  %d\n"), WiFi.RSSI());
    }

    DEBUG_MSG(PSTR("[WIFI] ----------------------------------------------\n"));
}

// -----------------------------------------------------------------------------

uint8_t wifiState() {
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

void wifiRegister(wifi_callback_f callback) {
    jw.subscribe(callback);
}

// -----------------------------------------------------------------------------
// MODULE CORE
// -----------------------------------------------------------------------------

void wifiSetup() {
    WiFi.setSleepMode(WIFI_SLEEP_MODE);

    _wifiInject();
    _wifiConfigure();

    #if DEBUG_SUPPORT
        wifiRegister(_wifiDebugCallback);
    #endif

    #if WS_SUPPORT
        wsOnConnectRegister(_wifiWSOnClientConnect);
        wsOnConfigureRegister(_wifiWSOnConfigure);
        wsOnActionRegister(_wifiWSOnAction);
    #endif

    #if BUTTON_SUPPORT && WIFI_AP_BTN > 0
        buttonOnEventRegister(
            [](unsigned int event) {
                if (event == WIFI_AP_BTN_EVENT) {
                    wifiStartAP();
                }
            },
            (uint8_t) (WIFI_AP_BTN - 1)
        );
    #endif

    // Register loop
    firmwareRegisterLoop(wifiLoop);
}

// -----------------------------------------------------------------------------

void wifiLoop() {
    jw.loop();

    // Request for available networks scan
    if (_wifi_scan_client_id > 0) {
        _wifiScan(_wifi_scan_client_id);
        _wifi_scan_client_id = 0;
    }
}

#endif // WIFI_SUPPORT