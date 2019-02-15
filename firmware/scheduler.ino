/*

SCHEDULER MODULE

Copyright (C) 2018 FastyBird Ltd. <info@fastybird.com>

*/

#if SCHEDULER_SUPPORT && (RELAY_PROVIDER != RELAY_PROVIDER_NONE || LIGHT_PROVIDER != LIGHT_PROVIDER_NONE)

#include <TimeLib.h>

void schReportChannelConfiguration(
    unsigned int id,
    const char * channelType,
    JsonArray& schedules
) {
    for (unsigned int i = 0; i < SCHEDULER_MAX_SCHEDULES; i++) {
        if (!hasSetting("schChannel", i) || getSetting("schChannel", i, 0).toInt() != id) {
            break;
        }

        JsonObject& schedule = schedules.createNestedObject();

        schedule["enabled"] = getSetting("schEnabled", i, 1).toInt() == 1;
        schedule["property"] = FASTYBIRD_PROPERTY_STATE;
        schedule["action"] = getSetting("schAction", i, 0).toInt();
        schedule["hour"] = getSetting("schHour", i, 0).toInt();
        schedule["minute"] = getSetting("schMinute", i, 0).toInt();
        schedule["utc"] = getSetting("schUTC", i, 0).toInt() == 1;
        schedule["days"] = getSetting("schWDs", i, "");
    }
}

// -----------------------------------------------------------------------------

void schConfigureChannelConfiguration(
    unsigned int id,
    const char * channelType,
    JsonArray& configuration
) {
    // Clear existing direct controls
    for (unsigned int i = 0; i < SCHEDULER_MAX_SCHEDULES; i++) {
        if (!hasSetting("schChannel", i) || getSetting("schChannel", i, 0).toInt() != id) {
            break;
        }

        delSetting("schEnabled", i);
        delSetting("schChannel", i);
        delSetting("schAction", i);
        delSetting("schType", i);
        delSetting("schHour", i);
        delSetting("schMinute", i);
        delSetting("schUTC", i);
        delSetting("schWDs", i);
    }
        
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

            setSetting("schEnabled", i, schedule["enabled"].as<unsigned int>());
            setSetting("schChannel", i, id);
            setSetting("schAction", i, schedule["action"].as<char*>());
            setSetting("schType", i, SCHEDULER_TYPE_SWITCH);
            setSetting("schHour", i, schedule["hour"].as<unsigned int>());
            setSetting("schMinute", i, schedule["minute"].as<unsigned int>());
            setSetting("schUTC", i, schedule["utc"].as<bool>());
            setSetting("schWDs", i, schedule["days"].as<char*>());

            i++;
        }
    }
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

        for (byte i = 0; i < SCHEDULER_MAX_SCHEDULES; i++) {
            if (!hasSetting("schChannel", i)) {
                break;
            }

            JsonObject &scheduler = sch.createNestedObject();

            scheduler["enabled"] = getSetting("schEnabled", i, 1).toInt() == 1;
            scheduler["channel"] = getSetting("schChannel", i, 0).toInt();
            scheduler["action"] = getSetting("schAction", i, 0).toInt();
            scheduler["type"] = getSetting("schType", i, 0).toInt();
            scheduler["hour"] = getSetting("schHour", i, 0).toInt();
            scheduler["minute"] = getSetting("schMinute", i, 0).toInt();
            scheduler["utc"] = getSetting("schUTC", i, 0).toInt() == 1;
            scheduler["days"] = getSetting("schWDs", i, "");
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
                    // Clear existing schedules
                    for (byte i = 0; i < SCHEDULER_MAX_SCHEDULES; i++) {
                        delSetting("schEnabled", i);
                        delSetting("schChannel", i);
                        delSetting("schAction", i);
                        delSetting("schType", i);
                        delSetting("schHour", i);
                        delSetting("schMinute", i);
                        delSetting("schUTC", i);
                        delSetting("schDays", i);
                    }

                    for (unsigned int i = 0; i < configuration["schedules"].size(); i++) {
                        if (i >= SCHEDULER_MAX_SCHEDULES) {
                            break;
                        }

                        setSetting("schEnabled", i, configuration["schedules"][i]["enabled"].as<unsigned int>());
                        setSetting("schChannel", i, configuration["schedules"][i]["channel"].as<unsigned int>());
                        setSetting("schAction", i, configuration["schedules"][i]["action"].as<unsigned int>());
                        setSetting("schType", i, configuration["schedules"][i]["type"].as<unsigned int>());
                        setSetting("schHour", i, configuration["schedules"][i]["hour"].as<unsigned int>());
                        setSetting("schMinute", i, configuration["schedules"][i]["minute"].as<unsigned int>());
                        setSetting("schUTC", i, configuration["schedules"][i]["utc"].as<bool>());
                        setSetting("schWDs", i, configuration["schedules"][i]["days"].as<char*>());
                    }
                }

                wsSend_P(clientId, PSTR("{\"message\": \"sch_updated\"}"));

                // Reload & cache settings
                firmwareReload();
            }
        }
    }
#endif // WEB_SUPPORT && WS_SUPPORT

// -----------------------------------------------------------------------------

void _schConfigure() {
    bool delete_flag = false;

    for (unsigned int i = 0; i < SCHEDULER_MAX_SCHEDULES; i++) {
        int sch_channel = getSetting("schChannel", i, 0xFF).toInt();

        if (sch_channel == 0xFF) {
            delete_flag = true;
        }

        if (delete_flag) {
            delSetting("schEnabled", i);
            delSetting("schChannel", i);
            delSetting("schAction", i);
            delSetting("schHour", i);
            delSetting("schMinute", i);
            delSetting("schWDs", i);
            delSetting("schType", i);
            delSetting("schUTC", i);

        } else {
            #if DEBUG_SUPPORT
                bool sch_enabled = getSetting("schEnabled", i, 1).toInt() == 1;

                int sch_action = getSetting("schAction", i, 0).toInt();

                int sch_hour = getSetting("schHour", i, 0).toInt();
                int sch_minute = getSetting("schMinute", i, 0).toInt();

                bool sch_utc = getSetting("schUTC", i, 0).toInt() == 1;

                String sch_weekdays = getSetting("schWDs", i, "");

                unsigned char sch_type = getSetting("schType", i, SCHEDULER_TYPE_SWITCH).toInt();

                DEBUG_MSG(
                    PSTR("[SCH] Schedule #%d: %s #%d to %d at %02d:%02d %s on %s%s\n"),
                    i, SCHEDULER_TYPE_SWITCH == sch_type ? "switch" : "channel", sch_channel,
                    sch_action, sch_hour, sch_minute, sch_utc ? "UTC" : "local time",
                    (char *) sch_weekdays.c_str(),
                    sch_enabled ? "" : " (disabled)"
                );
            #endif // DEBUG_SUPPORT
        }
    }
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
    for (unsigned int i = 0; i < SCHEDULER_MAX_SCHEDULES; i++) {
        int sch_channel = getSetting("schChannel", i, 0xFF).toInt();

        if (sch_channel == 0xFF) {
            break;
        }

        // Skip disabled schedules
        if (getSetting("schEnabled", i, 1).toInt() == 0) {
            continue;
        }

        // Get the datetime used for the calculation
        bool sch_utc = getSetting("schUTC", i, 0).toInt() == 1;

        time_t t = sch_utc ? utc_time : local_time;

        String sch_weekdays = getSetting("schWDs", i, "");

        if (_schIsThisWeekday(t, sch_weekdays)) {
            int sch_hour = getSetting("schHour", i, 0).toInt();
            int sch_minute = getSetting("schMinute", i, 0).toInt();

            int minutes_to_trigger = _schMinutesLeft(t, sch_hour, sch_minute);

            if (minutes_to_trigger == 0) {
                unsigned char sch_type = getSetting("schType", i, SCHEDULER_TYPE_SWITCH).toInt();

                #if RELAY_PROVIDER != RELAY_PROVIDER_NONE
                    if (sch_type == SCHEDULER_TYPE_SWITCH) {
                        int sch_action = getSetting("schAction", i, 0).toInt();

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
                        int sch_brightness = getSetting("schAction", i, -1).toInt();

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
