/*

NTP MODULE

Copyright (C) 2018 FastyBird Ltd. <info@fastybird.com>

*/

#if NTP_SUPPORT

#include <TimeLib.h>
#include <NtpClientLib.h>
#include <WiFiClient.h>
#include <Ticker.h>

unsigned long _ntp_start = 0;

// -----------------------------------------------------------------------------
// MODULE PRIVATE
// -----------------------------------------------------------------------------

/**
 * Provide module configuration schema
 */
void _ntpReportConfigurationSchema(
    JsonArray& configuration
) {
    // Configuration field
    JsonObject& server = configuration.createNestedObject();

    server["name"] = "ntp_server";
    server["type"] = "text";
    server["default"] = NTP_SERVER;

    // Configuration field
    JsonObject& offset = configuration.createNestedObject();

    offset["name"] = "ntp_offset";
    offset["type"] = "text";
    offset["default"] = NTP_TIME_OFFSET;

    // Configuration field
    JsonObject& dst = configuration.createNestedObject();

    dst["name"] = "ntp_dst";
    dst["type"] = "boolean";
    dst["default"] = NTP_DAY_LIGHT;

    // Configuration field
    JsonObject& region = configuration.createNestedObject();

    region["name"] = "ntp_region";
    region["type"] = "select";
    region["default"] = NTP_DST_REGION;

    JsonArray& regionValues = region.createNestedArray("values");

    JsonObject& regionValue0 = regionValues.createNestedObject();
    regionValue0["value"] = 0;
    regionValue0["name"] = "europe";

    JsonObject& regionValue1 = regionValues.createNestedObject();
    regionValue1["value"] = 1;
    regionValue1["name"] = "usa";
}

// -----------------------------------------------------------------------------

/**
 * Provide module current configuration
 */
void _ntpReportConfiguration(
    JsonObject& configuration
) {
    configuration["ntp_server"] = getSetting("ntpServer", NTP_SERVER);
    configuration["ntp_offset"] = getSetting("ntpOffset", NTP_TIME_OFFSET).toInt();
    configuration["ntp_dst"] = getSetting("ntpDST", NTP_DAY_LIGHT).toInt() == 1;
    configuration["ntp_region"] = getSetting("ntpRegion", NTP_DST_REGION).toInt();
}

// -----------------------------------------------------------------------------

/**
 * Update module configuration via WS or MQTT etc.
 */
void _ntpUpdateConfiguration(
    JsonObject& configuration
) {
    if (configuration.containsKey("ntp_server"))  {
        setSetting("ntpServer", configuration["ntp_server"].as<char *>());
    }

    if (configuration.containsKey("ntp_offset"))  {
        setSetting("ntpOffset", configuration["ntp_offset"].as<uint8_t>());
    }

    if (configuration.containsKey("ntp_dst"))  {
        setSetting("ntpDST", configuration["ntp_dst"].as<uint8_t>());
    }

    if (configuration.containsKey("ntp_region"))  {
        setSetting("ntpRegion", configuration["ntp_region"].as<uint8_t>());
    }
}

// -----------------------------------------------------------------------------

#if WEB_SUPPORT && WS_SUPPORT
    // New WS client is connected
    void _ntpWSOnConnect(
        JsonObject& root
    ) {
        JsonArray& modules = root.containsKey("modules") ? root["modules"] : root.createNestedArray("modules");
        JsonObject& module = modules.createNestedObject();

        module["module"] = "ntp";
        module["visible"] = true;

        // Configuration container
        JsonObject& configuration = module.createNestedObject("config");

        // Configuration values container
        JsonObject& configuration_values = configuration.createNestedObject("values");

        _ntpReportConfiguration(configuration_values);

        // Configuration schema container
        JsonArray& configuration_schema = configuration.createNestedArray("schema");

        _ntpReportConfigurationSchema(configuration_schema);

        // Data container
        JsonObject& data = module.createNestedObject("data");

        data["now"] = ntpSynced() ? ((char *) ntpDateTimeAtom().c_str()) : NULL;
        data["status"] = ntpSynced() ? timeStatus() == timeSet : false;
    }

// -----------------------------------------------------------------------------

    // WS client requested configuration update
    void _ntpWSOnConfigure(
        const uint32_t clientId,
        JsonObject& module
    ) {
        if (module.containsKey("module") && module["module"] == "ntp") {
            if (module.containsKey("config")) {
                // Extract configuration container
                JsonObject& configuration = module["config"].as<JsonObject&>();

                if (configuration.containsKey("values")) {
                    // Update module
                    _ntpUpdateConfiguration(configuration["values"]);

                    wsSend_P(clientId, PSTR("{\"message\": \"ntp_updated\"}"));

                    // Reload & cache settings
                    firmwareReload();
                }
            }
        }
    }

// -----------------------------------------------------------------------------

    void _ntpWSOnUpdate(
        JsonObject& root
    ) {
        JsonArray& modules = root.containsKey("modules") ? root["modules"] : root.createNestedArray("modules");
        JsonObject& module = modules.createNestedObject();

        module["module"] = "ntp";

        // Data container
        JsonObject& data = module.createNestedObject("data");

        data["now"] = ntpSynced() ? ((char *) ntpDateTimeAtom().c_str()) : NULL;
        data["status"] = ntpSynced() ? timeStatus() == timeSet : false;
    }
#endif

// -----------------------------------------------------------------------------

void _ntpSystemOnHeartbeat() {
    if (ntpSynced()) {
        DEBUG_MSG(PSTR("[NTP] Time: %s\n"), (char *) ntpDateTime().c_str());

    } else {
        DEBUG_MSG(PSTR("[NTP] Time: NOT SYNCED\n"));
    }
}

// -----------------------------------------------------------------------------

void _ntpStart() {
    _ntp_start = 0;

    NTP.begin(getSetting("ntpServer", NTP_SERVER));
    NTP.setInterval(NTP_SYNC_INTERVAL, NTP_UPDATE_INTERVAL);
    NTP.setNTPTimeout(NTP_TIMEOUT);

    _ntpConfigure();
}

// -----------------------------------------------------------------------------

void _ntpConfigure() {
    int offset = getSetting("ntpOffset", NTP_TIME_OFFSET).toInt();
    int sign = offset > 0 ? 1 : -1;

    offset = abs(offset);

    int tz_hours = sign * (offset / 60);
    int tz_minutes = sign * (offset % 60);

    if (NTP.getTimeZone() != tz_hours || NTP.getTimeZoneMinutes() != tz_minutes) {
        NTP.setTimeZone(tz_hours, tz_minutes);
        _ntpUpdate();
    }

    bool daylight = getSetting("ntpDST", NTP_DAY_LIGHT).toInt() == 1;

    if (NTP.getDayLight() != daylight) {
        NTP.setDayLight(daylight);
        _ntpUpdate();
    }

    String server = getSetting("ntpServer", NTP_SERVER);

    if (!NTP.getNtpServerName().equals(server)) {
        NTP.setNtpServerName(server);
    }

    uint8_t dst_region = getSetting("ntpRegion", NTP_DST_REGION).toInt();

    NTP.setDSTZone(dst_region);
}

// -----------------------------------------------------------------------------

void _ntpUpdate() {
    #if WEB_SUPPORT && WS_SUPPORT
        wsSend(_ntpWSOnConnect);
    #endif

    if (ntpSynced()) {
        time_t t = now();

        DEBUG_MSG(PSTR("[NTP] UTC Time  : %s\n"), (char *) ntpDateTime(ntpLocal2UTC(t)).c_str());
        DEBUG_MSG(PSTR("[NTP] Local Time: %s\n"), (char *) ntpDateTime(t).c_str());
    }
}

// -----------------------------------------------------------------------------
// MODULE API
// -----------------------------------------------------------------------------

bool ntpSynced() {
    #if NTP_WAIT_FOR_SYNC
        return NTP.getLastNTPSync() > 0;
    #else
        return true;
    #endif
}

// -----------------------------------------------------------------------------

String ntpDateTimeAtom() {
    char buffer[28];

    time_t t = now();
    time_t utc = ntpLocal2UTC(t);

    snprintf_P(buffer, sizeof(buffer),
        PSTR("%04d-%02d-%02dT%02d:%02d:%02d+00:00"),
        year(utc), month(utc), day(utc), hour(utc), minute(utc), second(utc)
    );

    return String(buffer);
}

// -----------------------------------------------------------------------------

String ntpDateTime(
    time_t t
) {
    char buffer[20];

    snprintf_P(buffer, sizeof(buffer),
        PSTR("%04d-%02d-%02d %02d:%02d:%02d"),
        year(t), month(t), day(t), hour(t), minute(t), second(t)
    );

    return String(buffer);
}

// -----------------------------------------------------------------------------

String ntpDateTime() {
    if (ntpSynced()) {
        return ntpDateTime(now());
    }

    return String();
}

// -----------------------------------------------------------------------------

time_t ntpLocal2UTC(
    time_t local
) {
    int offset = getSetting("ntpOffset", NTP_TIME_OFFSET).toInt();

    if (NTP.isSummerTime()) {
        offset += 60;
    }

    return local - offset * 60;
}

// -----------------------------------------------------------------------------
// MODULE CORE
// -----------------------------------------------------------------------------

void ntpSetup() {
    NTP.onNTPSyncEvent([](NTPSyncEvent_t error) {
        if (error) {
            #if WEB_SUPPORT && WS_SUPPORT
                wsSend_P(PSTR("{\"ntpStatus\": false}"));
            #endif

            if (error == noResponse) {
                DEBUG_MSG(PSTR("[NTP] Error: NTP server not reachable\n"));

            } else if (error == invalidAddress) {
                DEBUG_MSG(PSTR("[NTP] Error: Invalid NTP server address\n"));
            }

        } else {
            _ntpUpdate();
        }
    });

    #if WIFI_SUPPORT
        wifiRegister([](justwifi_messages_t code, char * parameter) {
            if (code == MESSAGE_CONNECTED) {
                _ntp_start = millis() + NTP_START_DELAY;
            }
        });
    #endif

    #if WEB_SUPPORT && WS_SUPPORT
        wsOnConnectRegister(_ntpWSOnConnect);
        wsOnUpdateRegister(_ntpWSOnUpdate);
        wsOnConfigureRegister(_ntpWSOnConfigure);
    #endif

    #if FASTYBIRD_SUPPORT
        fastybirdReportConfigurationSchemaRegister(_ntpReportConfigurationSchema);
        fastybirdReportConfigurationRegister(_ntpReportConfiguration);
        fastybirdOnConfigureRegister(_ntpUpdateConfiguration);
    #endif

    systemOnHeartbeatRegister(_ntpSystemOnHeartbeat);

    // Firmware callbacks
    firmwareRegisterLoop(ntpLoop);
    firmwareRegisterReload(_ntpConfigure);
}

// -----------------------------------------------------------------------------

void ntpLoop() {
    if (_ntp_start > 0 && _ntp_start < millis()) {
        _ntpStart();
    }

    now();
}

#endif // NTP_SUPPORT
