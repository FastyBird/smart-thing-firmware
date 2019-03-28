/*

SCHEDULER MODULE

Copyright (C) 2018 FastyBird Ltd. <info@fastybird.com>

*/

#if SCHEDULER_SUPPORT

#include <TimeLib.h>

typedef struct {
    bool        enabled;
    uint8_t     channel;
    uint8_t     action;
    uint8_t     type;
    uint8_t     hour;
    uint8_t     minute;
    bool        utc;
    String      days;
} scheduler_record_t;

std::vector<scheduler_record_t> _scheduler_records;

const char * _scheduler_config_filename = "schedules.conf";

// -----------------------------------------------------------------------------
// MODULE PRIVATE
// -----------------------------------------------------------------------------

String _schReadStoredConfiguration() {
    String stored_content = storageReadConfiguration(_scheduler_config_filename);

    if (strcmp(stored_content.c_str(), "") == 0) {
        stored_content = String("[]");
    }

    return stored_content;
}

// -----------------------------------------------------------------------------

#if WEB_SUPPORT && WS_SUPPORT
    // New WS client is connected
    void _schWSOnConnect(
        JsonObject& root
    ) {
        JsonArray& modules = root.containsKey("modules") ? root["modules"] : root.createNestedArray("modules");
        JsonObject& module = modules.createNestedObject();

        module["module"] = "sch";
        module["visible"] = true;

        // Configuration container
        JsonObject& configuration = module.createNestedObject("config");

        configuration["max"] = SCHEDULER_MAX_SCHEDULES;

        // Data container
        JsonObject& data = module.createNestedObject("data");

        JsonArray &sch = data.createNestedArray("schedules");

        for (unsigned int i = 0; i < _scheduler_records.size(); i++) {
            JsonObject &scheduler = sch.createNestedObject();

            scheduler["enabled"] = _scheduler_records[i].enabled;
            scheduler["channel"] = _scheduler_records[i].channel;
            scheduler["action"] = _scheduler_records[i].action;
            scheduler["type"] = _scheduler_records[i].type;
            scheduler["hour"] = _scheduler_records[i].hour;
            scheduler["minute"] = _scheduler_records[i].minute;
            scheduler["utc"] = _scheduler_records[i].utc;
            scheduler["days"] = _scheduler_records[i].days.c_str();
        }
    }

// -----------------------------------------------------------------------------

    void _schWSOnConfigure(
        uint32_t clientId, 
        JsonObject& module
    ) {
        if (module.containsKey("module") && module["module"] == "sch") {
            if (module.containsKey("config")) {
                JsonObject& configuration = module["config"];

                if (configuration.containsKey("schedules")) {
                    DEBUG_MSG(PSTR("[SCH] Received %d schedules rules\n"), configuration["schedules"].size());

                    DynamicJsonBuffer jsonBuffer;

                    JsonArray& schedules_configuration = jsonBuffer.createArray();

                    for (unsigned int i = 0; i < configuration["schedules"].size(); i++) {
                        if (i >= SCHEDULER_MAX_SCHEDULES) {
                            break;
                        }

                        JsonObject& field = schedules_configuration.createNestedObject();

                        field["enabled"] = configuration["schedules"][i]["enabled"].as<bool>();
                        field["channel"] = configuration["schedules"][i]["channel"].as<unsigned int>();
                        field["action"] = configuration["schedules"][i]["action"].as<unsigned int>();
                        field["type"] = configuration["schedules"][i]["type"].as<unsigned int>();
                        field["hour"] = configuration["schedules"][i]["hour"].as<unsigned int>();
                        field["minute"] = configuration["schedules"][i]["minute"].as<unsigned int>();
                        field["utc"] = configuration["schedules"][i]["utc"].as<bool>();
                        field["days"] = configuration["schedules"][i]["days"].as<char *>();
                    }

                    String output;

                    schedules_configuration.printTo(output);

                    storageWriteConfiguration(_scheduler_config_filename, output);

                    // Reload schedules
                    _schLoadSchedules();
                }

                wsSend_P(clientId, PSTR("{\"message\": \"sch_updated\"}"));

                // Reload & cache settings
                firmwareReload();
            }
        }
    }
#endif // WEB_SUPPORT && WS_SUPPORT

// -----------------------------------------------------------------------------

void _schLoadSchedules() {
    DynamicJsonBuffer jsonBuffer;

    JsonArray& scheduler_configuration = jsonBuffer.parseArray(_schReadStoredConfiguration().c_str());

    std::vector<scheduler_record_t> stored_scheduler_records;

    for (JsonObject& stored_configuration : scheduler_configuration) {
        stored_scheduler_records.push_back((scheduler_record_t) {
            stored_configuration["enabled"].as<bool>(),
            stored_configuration["channel"].as<unsigned int>(),
            stored_configuration["action"].as<unsigned int>(),
            stored_configuration["type"].as<unsigned int>(),
            stored_configuration["hour"].as<unsigned int>(),
            stored_configuration["minute"].as<unsigned int>(),
            stored_configuration["utc"].as<bool>(),
            String(stored_configuration["days"].as<char *>())
        });
    }

    _scheduler_records = stored_scheduler_records;
}

// -----------------------------------------------------------------------------

void _schConfigure() {
    _schLoadSchedules();

    #if DEBUG_SUPPORT
        for (unsigned int i = 0; i < _scheduler_records.size(); i++) {
            bool sch_enabled = _scheduler_records[i].enabled;
            uint8_t sch_channel = _scheduler_records[i].channel;
            uint8_t sch_type = _scheduler_records[i].type;
            uint8_t sch_action = _scheduler_records[i].action;

            uint8_t sch_hour = _scheduler_records[i].hour;
            uint8_t sch_minute = _scheduler_records[i].minute;

            DEBUG_MSG(
                PSTR("[SCH] Schedule #%d: %s #%d to %d at %02d:%02d %s on %s%s\n"),
                i, SCHEDULER_TYPE_SWITCH == sch_type ? "switch" : "channel", sch_channel,
                sch_action, sch_hour, sch_minute, _scheduler_records[i].utc ? "UTC" : "local time",
                (char *) _scheduler_records[i].days.c_str(),
                sch_enabled ? "" : " (disabled)"
            );
        }
    #endif // DEBUG_SUPPORT
}

// -----------------------------------------------------------------------------

bool _schIsThisWeekday(
    time_t t,
    String weekdays
) {
    // Convert from Sunday to Monday as day 1
    int w = weekday(t) - 1;

    if (0 == w) {
        w = 7;
    }

    char pch;
    char * p = (char *) weekdays.c_str();

    unsigned char position = 0;

    while ((pch = p[position++])) {
        if ((pch - '0') == w) {
            return true;
        }
    }

    return false;
}

// -----------------------------------------------------------------------------

int _schMinutesLeft(
    time_t t,
    unsigned char schedule_hour,
    unsigned char schedule_minute
) {
    unsigned char now_hour = hour(t);
    unsigned char now_minute = minute(t);
    
    return (schedule_hour - now_hour) * 60 + schedule_minute - now_minute;
}

// -----------------------------------------------------------------------------

void _schCheck() {
    time_t local_time = now();
    time_t utc_time = ntpLocal2UTC(local_time);

    // Check schedules
    for (unsigned int i = 0; i < _scheduler_records.size(); i++) {

        // Skip disabled schedules
        if (_scheduler_records[i].enabled == false) {
            continue;
        }

        time_t t = _scheduler_records[i].utc ? utc_time : local_time;

        if (_schIsThisWeekday(t, _scheduler_records[i].days)) {
            int minutes_to_trigger = _schMinutesLeft(t, _scheduler_records[i].hour, _scheduler_records[i].minute);

            if (minutes_to_trigger == 0) {
                uint8_t sch_type = _scheduler_records[i].type;

                #if RELAY_PROVIDER != RELAY_PROVIDER_NONE
                    if (sch_type == SCHEDULER_TYPE_SWITCH) {
                        uint8_t sch_action = _scheduler_records[i].action;

                        DEBUG_MSG(PSTR("[SCH] Switching switch %d to %d\n"), sch_channel, sch_action);

                        if (sch_action == 2) {
                            relayToggle(sch_channel);

                        } else {
                            relayStatus(sch_channel, sch_action);
                        }
                    }
                #endif

                #if LIGHT_PROVIDER != LIGHT_PROVIDER_NONE
                    if (sch_type == SCHEDULER_TYPE_DIM) {
                        int sch_brightness = _scheduler_records[i].action;

                        DEBUG_MSG(PSTR("[SCH] Set channel %d value to %d\n"), sch_channel, sch_brightness);

                        lightChannel(sch_channel, sch_brightness);
                        lightUpdate(true, true);
                    }
                #endif

                DEBUG_MSG(PSTR("[SCH] Schedule #%d TRIGGERED!!\n"), i);

            // Show minutes to trigger every 15 minutes
            // or every minute if less than 15 minutes to scheduled time.
            // This only works for schedules on this same day.
            // For instance, if your scheduler is set for 00:01 you will only
            // get one notification before the trigger (at 00:00)
            } else if (minutes_to_trigger > 0) {
                #if DEBUG_SUPPORT
                    if ((minutes_to_trigger % 15 == 0) || (minutes_to_trigger < 15)) {
                        DEBUG_MSG(
                            PSTR("[SCH] %d minutes to trigger schedule #%d\n"),
                            minutes_to_trigger, i
                        );
                    }
                #endif
            }
        }
    }
}

// -----------------------------------------------------------------------------
// MODULE API
// -----------------------------------------------------------------------------

void schReportChannelConfiguration(
    unsigned int id,
    const char * channelType,
    JsonArray& schedules
) {
    for (unsigned int i = 0; i < _scheduler_records.size(); i++) {
        JsonObject& schedule = schedules.createNestedObject();

        schedule["enabled"] = _scheduler_records[i].enabled;
        schedule["property"] = FASTYBIRD_PROPERTY_STATE;
        schedule["action"] = _scheduler_records[i].action;
        schedule["hour"] = _scheduler_records[i].hour;
        schedule["minute"] = _scheduler_records[i].minute;
        schedule["utc"] = _scheduler_records[i].utc;
        schedule["days"] = _scheduler_records[i].days.c_str();
    }
}

// -----------------------------------------------------------------------------

void schConfigureChannelConfiguration(
    unsigned int id,
    const char * channelType,
    JsonArray& configuration
) {
    DynamicJsonBuffer jsonBuffer;

    JsonArray& schedules_configuration = jsonBuffer.createArray();

    unsigned int i = 0;

    // Store new schedules configuration
    for (JsonObject& schedule : configuration) {
        if (
            schedule.containsKey("action")
            && schedule.containsKey("hour")
            && schedule.containsKey("minute")
            && schedule.containsKey("utc")
            && schedule.containsKey("days")
        )  {
            if (i >= SCHEDULER_MAX_SCHEDULES) {
                break;
            }

            JsonObject& field = schedules_configuration.createNestedObject();

            field["enabled"] = schedule["enabled"].as<bool>();
            field["channel"] = id;
            field["action"] = schedule["action"].as<unsigned int>();
            field["type"] = SCHEDULER_TYPE_SWITCH;
            field["hour"] = schedule["hour"].as<unsigned int>();
            field["minute"] = schedule["minute"].as<unsigned int>();
            field["utc"] = schedule["utc"].as<bool>();
            field["days"] = schedule["action"].as<char *>();

            i++;
        }
    }
}

// -----------------------------------------------------------------------------
// MODULE CORE
// -----------------------------------------------------------------------------

void schSetup() {
    _schConfigure();

    // Update websocket clients
    #if WEB_SUPPORT && WS_SUPPORT
        wsOnConnectRegister(_schWSOnConnect);
        wsOnConfigureRegister(_schWSOnConfigure);
    #endif

    // Main callbacks
    firmwareRegisterLoop(schLoop);
    firmwareRegisterReload(_schConfigure);
}

// -----------------------------------------------------------------------------

void schLoop() {
    // Check time has been sync'ed
    if (!ntpSynced()) {
        return;
    }

    // Check schedules every minute at hh:mm:00
    static unsigned long last_minute = 60;
    unsigned char current_minute = minute();

    if (current_minute != last_minute) {
        last_minute = current_minute;

        _schCheck();
    }
}
#endif
