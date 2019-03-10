/*

RELAY MODULE

Copyright (C) 2017 - 2018 FastyBird Ltd. <info@fastybird.com>

*/

#if RELAY_PROVIDER != RELAY_PROVIDER_NONE

#include <EEPROM.h>
#include <Ticker.h>
#include <ArduinoJson.h>
#include <vector>
#include <functional>

typedef struct {
    // Configuration variables
    uint8_t pin;            // GPIO pin for the relay
    uint8_t type;           // RELAY_TYPE_NORMAL, RELAY_TYPE_INVERSE, RELAY_TYPE_LATCHED or RELAY_TYPE_LATCHED_INVERSE
    uint8_t reset_pin;      // GPIO to reset the relay if RELAY_TYPE_LATCHED
    uint32_t delay_on;      // Delay to turn relay ON
    uint32_t delay_off;     // Delay to turn relay OFF
    uint8_t pulse;          // RELAY_PULSE_NONE, RELAY_PULSE_OFF or RELAY_PULSE_ON
    uint32_t pulse_ms;      // Pulse length in millis

    // Status variables
    bool current_status;    // Holds the current (physical) status of the relay
    bool target_status;     // Holds the target status
    uint32_t fw_start;      // Flood window start time
    uint8_t fw_count;       // Number of changes within the current flood window
    uint32_t change_time;   // Scheduled time to change
    bool report;            // Whether to report to own topic

    // Helping objects
    Ticker pulseTicker;     // Holds the pulse back timer

} relay_t;

std::vector<relay_t> _relays;

bool _relayRecursive = false;

Ticker _relaySaveTicker;

#if FASTYBIRD_SUPPORT
    uint8_t _relay_fastybird_channel_index = 0xFF;
#endif

// -----------------------------------------------------------------------------
// MODULE PRIVATE
// -----------------------------------------------------------------------------

void _relayConfigure() {
    for (uint8_t i = 0; i < _relays.size(); i++) {
        _relays[i].pulse = getSetting("relayPulse", i, RELAY_PULSE_MODE).toInt();
        _relays[i].pulse_ms = 1000 * getSetting("relayTime", i, RELAY_PULSE_TIME).toFloat();

        if (_relays[i].pin == GPIO_NONE) {
            continue;
        }

        pinMode(_relays[i].pin, OUTPUT);

        if (_relays[i].reset_pin != GPIO_NONE) {
            pinMode(_relays[i].reset_pin, OUTPUT);
        }

        if (_relays[i].type == RELAY_TYPE_INVERSE) {
            // Set to high to block short opening of relay
            digitalWrite(_relays[i].pin, HIGH);
        }
    }
}

// -----------------------------------------------------------------------------

void _relayBoot() {
    _relayRecursive = true;

    uint8_t bit = 1;
    bool trigger_save = false;

    // Get last statuses from EEPROM
    uint8_t mask = EEPROMr.read(EEPROM_RELAY_STATUS);
    DEBUG_MSG(PSTR("[RELAY] Retrieving mask: %d\n"), mask);

    // Walk the relays
    bool status;

    for (uint8_t i = 0; i < _relays.size(); i++) {
        uint8_t boot_mode = getSetting("relayBoot", i, RELAY_BOOT_MODE).toInt();

        DEBUG_MSG(PSTR("[RELAY] Relay #%d boot mode %d\n"), i, boot_mode);

        status = false;

        switch (boot_mode) {
            case RELAY_BOOT_SAME:
                if (i < 8) {
                    status = ((mask & bit) == bit);
                }
                break;

            case RELAY_BOOT_TOGGLE:
                if (i < 8) {
                    status = ((mask & bit) != bit);
                    mask ^= bit;
                    trigger_save = true;
                }
                break;

            case RELAY_BOOT_ON:
                status = true;
                break;

            case RELAY_BOOT_OFF:
            default:
                break;
        }

        _relays[i].current_status = !status;
        _relays[i].target_status = status;

        #if RELAY_PROVIDER == RELAY_PROVIDER_STM
            _relays[i].change_time = millis() + 3000 + 1000 * i;
        #else
            _relays[i].change_time = millis();
        #endif

        bit <<= 1;
    }

    // Save if there is any relay in the RELAY_BOOT_TOGGLE mode
    if (trigger_save) {
        EEPROMr.write(EEPROM_RELAY_STATUS, mask);
        eepromCommit();
    }

    _relayRecursive = false;
}

// -----------------------------------------------------------------------------

uint8_t _relayParsePayload(
    const char * payload
) {
    // Payload could be "OFF", "ON", "TOGGLE"
    // or its number equivalents: 0, 1 or 2

    if (payload[0] == '0') {
        return 0;

    } else if (payload[0] == '1') {
        return 1;
    
    } else if (payload[0] == '2') {
        return 2;
    }

    // trim payload
    char * p = ltrim((char *) payload);

    // to lower
    uint8_t length = strlen(p);

    if (length > 6) {
        length = 6;
    }

    for (uint8_t i = 0; i < length; i++) {
        p[i] = tolower(p[i]);
    }

    if (strcmp(p, FASTYBIRD_SWITCH_PAYLOAD_OFF) == 0) {
        return 0;

    } else if (strcmp(p, FASTYBIRD_SWITCH_PAYLOAD_ON) == 0) {
        return 1;

    } else if (strcmp(p, FASTYBIRD_SWITCH_PAYLOAD_TOGGLE) == 0) {
        return 2;

    } else if (strcmp(p, FASTYBIRD_SWITCH_PAYLOAD_QUERY) == 0) {
        return 3;

    } else {
        return 0xFF;
    }
}

// -----------------------------------------------------------------------------

/**
 * Provide module configuration schema
 */
void _relayReportConfigurationSchema(
    JsonArray& configuration
) {
    if (relayCount() > 1) {
        JsonObject& sync = configuration.createNestedObject();

        sync["name"] = "relays_sync";
        sync["type"] = "select";
        sync["default"] = RELAY_SYNC;

        JsonArray& relaysSyncValues = sync.createNestedArray("values");

        JsonObject& relaysSyncValue0 = relaysSyncValues.createNestedObject();
        relaysSyncValue0["value"] = 0;
        relaysSyncValue0["name"] = "disabled";

        JsonObject& relaysSyncValue1 = relaysSyncValues.createNestedObject();
        relaysSyncValue1["value"] = 1;
        relaysSyncValue1["name"] = "zero_or_one";

        JsonObject& relaysSyncValue2 = relaysSyncValues.createNestedObject();
        relaysSyncValue2["value"] = 2;
        relaysSyncValue2["name"] = "only_one";

        JsonObject& relaysSyncValue3 = relaysSyncValues.createNestedObject();
        relaysSyncValue3["value"] = 3;
        relaysSyncValue3["name"] = "all_synchronized";
    }
}

// -----------------------------------------------------------------------------

/**
 * Report module configuration
 */
void _relayReportConfiguration(
    JsonObject& configuration
) {
    if (relayCount() > 1) {
        configuration["relays_sync"] = getSetting("relaysSync", RELAY_SYNC).toInt();
    }
}

// -----------------------------------------------------------------------------

/**
 * Update module configuration via WS or MQTT etc.
 */
void _relayUpdateConfiguration(
    JsonObject& configuration
) {
    DEBUG_MSG(PSTR("[RELAY] Updating module\n"));

    if (relayCount() > 1) {
        if (configuration.containsKey("relays_sync"))  {
            setSetting("relaysSync", configuration["relays_sync"].as<uint8_t>());
        }

    } else {
        delSetting("relaysSync");
    }
}

// -----------------------------------------------------------------------------

void _relayReportChannelConfigurationSchema(
    JsonArray& container
) {
    JsonObject& relayMode = container.createNestedObject();

    relayMode["name"] = "relay_boot";
    relayMode["type"] = "select";
    relayMode["default"] = RELAY_BOOT_MODE;

    JsonArray& relayModeValues = relayMode.createNestedArray("values");

    JsonObject& relayModeValue0 = relayModeValues.createNestedObject();
    relayModeValue0["value"] = 0;
    relayModeValue0["name"] = "always_off";

    JsonObject& relayModeValue1 = relayModeValues.createNestedObject();
    relayModeValue1["value"] = 1;
    relayModeValue1["name"] = "always_on";

    JsonObject& relayModeValue2 = relayModeValues.createNestedObject();
    relayModeValue2["value"] = 2;
    relayModeValue2["name"] = "same_before";

    JsonObject& relayModeValue3 = relayModeValues.createNestedObject();
    relayModeValue3["value"] = 3;
    relayModeValue3["name"] = "toggle_before";

    // -------------------------------------------------------------------------

    JsonObject& relayPulseMode = container.createNestedObject();

    relayPulseMode["name"] = "pulse_mode";
    relayPulseMode["type"] = "select";
    relayPulseMode["default"] = RELAY_PULSE_MODE;

    JsonArray& relayPulseModeValues = relayPulseMode.createNestedArray("values");

    JsonObject& relayPulseModeValue0 = relayPulseModeValues.createNestedObject();
    relayPulseModeValue0["value"] = 0;
    relayPulseModeValue0["name"] = "disabled";

    JsonObject& relayPulseModeValue1 = relayPulseModeValues.createNestedObject();
    relayPulseModeValue1["value"] = 1;
    relayPulseModeValue1["name"] = "normally_off";

    JsonObject& relayPulseModeValue2 = relayPulseModeValues.createNestedObject();
    relayPulseModeValue2["value"] = 2;
    relayPulseModeValue2["name"] = "normally_on";

    // -------------------------------------------------------------------------

    JsonObject& relayPulseTime = container.createNestedObject();

    relayPulseTime["name"] = "pulse_time";
    relayPulseTime["type"] = "number";
    relayPulseTime["default"] = RELAY_PULSE_TIME;
    relayPulseTime["min"] = 1;
    relayPulseTime["max"] = 60;
    relayPulseTime["step"] = 0.1;

    // -------------------------------------------------------------------------

    JsonObject& onDisconnect = container.createNestedObject();

    onDisconnect["name"] = "on_disconnect";
    onDisconnect["type"] = "select";
    onDisconnect["default"] = RELAY_PULSE_MODE;

    JsonArray& onDisconnectValues = onDisconnect.createNestedArray("values");

    JsonObject& onDisconnectValue0 = onDisconnectValues.createNestedObject();
    onDisconnectValue0["value"] = 0;
    onDisconnectValue0["name"] = "no_change";

    JsonObject& onDisconnectValue1 = onDisconnectValues.createNestedObject();
    onDisconnectValue1["value"] = 1;
    onDisconnectValue1["name"] = "turn_off";

    JsonObject& onDisconnectValue3 = onDisconnectValues.createNestedObject();
    onDisconnectValue3["value"] = 2;
    onDisconnectValue3["name"] = "turn_on";
}

// -----------------------------------------------------------------------------

String _relayReportChannelConfigurationSchema() {
    DynamicJsonBuffer jsonBuffer;
    JsonArray& schema = jsonBuffer.createArray();

    _relayReportChannelConfigurationSchema(schema);

    String output;

    schema.printTo(output);

    return output;
}

// -----------------------------------------------------------------------------

void _relayReportChannelConfiguration(
    const uint8_t id,
    JsonObject& configuration
) {
    configuration["relay_boot"] = getSetting("relayBoot", id, RELAY_BOOT_MODE).toInt();
    configuration["pulse_mode"] = getSetting("relayPulse", id, RELAY_PULSE_MODE).toInt();
    configuration["pulse_time"] = getSetting("relayTime", id, RELAY_PULSE_TIME).toFloat();
    configuration["on_disconnect"] = getSetting("relayOnDisc", id, 0).toInt();
}

// -----------------------------------------------------------------------------

String _relayReportChannelConfiguration(
    const uint8_t id
) {
    DynamicJsonBuffer jsonBuffer;
    JsonObject& configuration = jsonBuffer.createObject();

    _relayReportChannelConfiguration(id, configuration);

    String output;

    configuration.printTo(output);

    return output;
}

// -----------------------------------------------------------------------------

bool _relayConfigureChannel(
    const uint8_t id,
    JsonObject& configuration
) {
    DEBUG_MSG(PSTR("[RELAY] Updating channel: %d\n"), id);

    if (configuration.containsKey("relay_boot"))  {
        DEBUG_MSG(PSTR("[RELAY] Setting: \"relay_boot\" to: %d\n"), configuration["relay_boot"].as<uint8_t>());

        setSetting("relayBoot", id, configuration["relay_boot"].as<uint8_t>());
    }

    if (configuration.containsKey("pulse_mode"))  {
        DEBUG_MSG(PSTR("[RELAY] Setting: \"pulse_mode\" to: %d\n"), configuration["pulse_mode"].as<uint8_t>());

        setSetting("relayPulseMode", id, configuration["pulse_mode"].as<uint8_t>());
    }

    if (configuration.containsKey("pulse_time"))  {
        if (configuration["pulse_time"].as<float>() >= 1.0 && configuration["pulse_time"].as<float>() <= 60.0) {
            DEBUG_MSG(PSTR("[RELAY] Setting: \"pulse_time\" to: %d\n"), configuration["pulse_time"].as<uint8_t>());

            setSetting("relayPulseTime", id, configuration["pulse_time"].as<float>());
        }
    }

    if (configuration.containsKey("on_disconnect"))  {
        DEBUG_MSG(PSTR("[RELAY] Setting: \"on_disconnect\" to: %d\n"), configuration["on_disconnect"].as<uint8_t>());

        setSetting("relayOnDisc", id, configuration["on_disconnect"].as<uint8_t>());
    }
}

// -----------------------------------------------------------------------------

#if WEB_SUPPORT && WS_SUPPORT
    // Send module status to WS clients
    void _relayWebSocketUpdate(
        JsonObject& root
    ) {
        root["module"] = "relay";

        // Container
        JsonObject& container = root.createNestedObject("data");

        // Relays status container
        JsonArray& relay = container.createNestedArray("status");

        for (uint8_t i = 0; i < relayCount(); i++) {
            relay.add(_relays[i].target_status ? 1 : 0);
        }
    }

// -----------------------------------------------------------------------------

    // WS client is connected
    void _relayWSOnConnect(
        JsonObject& root
    ) {
        if (relayCount() == 0) {
            return;
        }

        DynamicJsonBuffer jsonBuffer;

        JsonArray& modules = root.containsKey("modules") ? root["modules"] : root.createNestedArray("modules");
        JsonObject& module = modules.createNestedObject();

        module["module"] = "relay";
        module["visible"] = true;

        // Configuration container
        JsonObject& configuration = module.createNestedObject("config");

        // MODULE CONFIGURATION

        // Configuration schema container
        JsonArray& configuration_schema = configuration.createNestedArray("schema");

        _relayReportConfigurationSchema(configuration_schema);

        // Configuration values container
        JsonObject& configuration_values = configuration.createNestedObject("values");

        _relayReportConfiguration(configuration_values);

        // CHANNELS CONFIGURATION

        // Channels configuration container
        JsonObject& channels_configuration = configuration.createNestedObject("channels");

        // Channels configuration schema container
        JsonArray& channels_configuration_schema = channels_configuration.createNestedArray("schema");

        _relayReportChannelConfigurationSchema(channels_configuration_schema);

        // Channels configuration values container
        JsonArray& channels_configuration_values = channels_configuration.createNestedArray("values");

        for (uint8_t i = 0; i < relayCount(); i++) {
            JsonObject& channel_configuration_values = channels_configuration_values.createNestedObject();

            channel_configuration_values["gpio"] = _relays[i].pin;
            channel_configuration_values["type"] = _relays[i].type;
            channel_configuration_values["reset"] = _relays[i].reset_pin;
            channel_configuration_values["relay_boot"] = getSetting("relayBoot", i, RELAY_BOOT_MODE).toInt();
            channel_configuration_values["on_disconnect"] = getSetting("relayOnDisc", i, 0).toInt();
            channel_configuration_values["pulse_mode"] = _relays[i].pulse;
            channel_configuration_values["pulse_time"] = _relays[i].pulse_ms / 1000.0;
        }

        // Data container
        JsonObject& data = module.createNestedObject("data");

        // Channels statuses
        JsonArray& channels_statuses = data.createNestedArray("channels");

        for (uint8_t i = 0; i < relayCount(); i++) {
            channels_statuses.add(_relays[i].target_status);
        }
    }

// -----------------------------------------------------------------------------

    // WS client requested configuration
    void _relayWSOnConfigure(
        const uint32_t clientId,
        JsonObject& module
    ) {
        if (module.containsKey("module") && module["module"] == "relay") {
            if (module.containsKey("config")) {
                DEBUG_MSG(PSTR("[RELAY] Found configuration for module\n"));

                // Extract configuration container
                JsonObject& configuration = module["config"].as<JsonObject&>();

                if (configuration.containsKey("values")) {
                    _relayUpdateConfiguration(configuration["values"]);
                }

                if (configuration.containsKey("channels")) {
                    JsonObject& channels_configuration = configuration["channels"].as<JsonObject&>();

                    if (channels_configuration.containsKey("values")) {
                        for (uint8_t i = 0; i < channels_configuration["values"].size(); i++) {
                            _relayConfigureChannel(i, channels_configuration["values"][i]);
                        }
                    }
                }

                // Send message
                wsSend_P(clientId, PSTR("{\"message\": \"relay_updated\"}"));

                // Reload & cache settings
                firmwareReload();
            }
        }
    }

// -----------------------------------------------------------------------------

    // WS client called action
    void _relayWSOnAction(
        const uint32_t client_id,
        const char * action,
        JsonObject& data
    ) {
        if (strcmp(action, "switch") == 0 && data.containsKey("channels")) {
            for (uint8_t i = 0; i < data["channels"].size(); i++) {
                // Parse value
                uint32_t value = _relayParsePayload(data["channels"][i].as<char*>());

                // Action to perform
                if (value == 0) {
                    relayStatus(i, false);

                } else if (value == 1) {
                    relayStatus(i, true);

                } else if (value == 2) {
                    relayToggle(i);

                } else if (value == 3) {
                    wsSend(_relayWebSocketUpdate);
                }
            }
        }
    }
#endif

// -----------------------------------------------------------------------------

#if FASTYBIRD_SUPPORT
    fastybird_channel_property_t _relayFastybirdGetChannelStatePropertyStructure() {
        fastybird_channel_property_t property = {
            FASTYBIRD_PROPERTY_STATE,
            "Switch relay state",
            true,
            FASTYBIRD_PROPERTY_DATA_TYPE_ENUM,
        };

        property.format.push_back(FASTYBIRD_SWITCH_PAYLOAD_ON);
        property.format.push_back(FASTYBIRD_SWITCH_PAYLOAD_OFF);
        property.format.push_back(FASTYBIRD_SWITCH_PAYLOAD_TOGGLE);
        property.format.push_back(FASTYBIRD_SWITCH_PAYLOAD_QUERY);

        property.mappings.push_back({
            FASTYBIRD_SWITCH_PAYLOAD_ON,
            FASTYBIRD_SWITCH_PAYLOAD_ON
        });

        property.mappings.push_back({
            FASTYBIRD_SWITCH_PAYLOAD_OFF,
            FASTYBIRD_SWITCH_PAYLOAD_OFF
        });

        property.mappings.push_back({
            FASTYBIRD_SWITCH_PAYLOAD_TOGGLE,
            FASTYBIRD_SWITCH_PAYLOAD_TOGGLE
        });

        property.mappings.push_back({
            FASTYBIRD_SWITCH_PAYLOAD_QUERY,
            FASTYBIRD_SWITCH_PAYLOAD_QUERY
        });

        property.payloadCallback = ([](uint8_t id, const char * payload) {
            // Action to perform
            if (strcmp(payload, FASTYBIRD_SWITCH_PAYLOAD_TOGGLE) == 0) {
                relayToggle(id);

            } else {
                relayStatus(id, strcmp(payload, FASTYBIRD_SWITCH_PAYLOAD_ON) == 0);
            }
        });

        return property;
    }

// -----------------------------------------------------------------------------

    fastybird_channel_t _relayFastybirdGetChannelStructure() {
        fastybird_channel_t channel = {
            "Switch relays",
            FASTYBIRD_CHANNEL_TYPE_SWITCH,
            relayCount(),
            true,
            true,
            true
        };

        channel.properties.push_back(_relayFastybirdGetChannelStatePropertyStructure());

        channel.configurationSchema = (_relayReportChannelConfigurationSchema());

        channel.configureCallback = ([](uint8_t id, JsonObject& configuration){
            _relayConfigureChannel(id, configuration);

            fastybirdReportChannelConfiguration(
                _relay_fastybird_channel_index,
                id,
                _relayReportChannelConfiguration(id)
            );

            DEBUG_MSG(PSTR("[RELAY] Configuration changes were saved\n"));
        });

        #if DIRECT_CONTROL_SUPPORT
            channel.configureDirectControlsCallback = ([](uint8_t id, JsonArray& configuration){
                directControlConfigureChannelConfiguration(id, FASTYBIRD_CHANNEL_TYPE_SWITCH, configuration);

                String output;
                DynamicJsonBuffer jsonBuffer;

                JsonArray& direct_controls = jsonBuffer.createArray();

                directControlReportChannelConfiguration(id, FASTYBIRD_CHANNEL_TYPE_SWITCH, direct_controls);
                
                direct_controls.printTo(output);

                fastybirdReportChannelDirectControl(
                    _relay_fastybird_channel_index,
                    id,
                    output
                );

                DEBUG_MSG(PSTR("[RELAY] Direct controls configuration changes were saved\n"));
            });
        #else
            channel.hasDirectControl = false;
        #endif

        #if SCHEDULER_SUPPORT
            channel.configureSchedulesCallback = ([](uint8_t id, JsonArray& configuration){
                schConfigureChannelConfiguration(id, FASTYBIRD_CHANNEL_TYPE_SWITCH, configuration);

                String output;
                DynamicJsonBuffer jsonBuffer;

                JsonArray& schedules = jsonBuffer.createArray();

                schReportChannelConfiguration(id, FASTYBIRD_CHANNEL_TYPE_SWITCH, schedules);

                schedules.printTo(output);

                fastybirdReportChannelScheduler(
                    _relay_fastybird_channel_index,
                    id,
                    output
                );

                DEBUG_MSG(PSTR("[RELAY] Scheduler configuration changes were saved\n"));
            });
        #else
            channel.hasScheduler = false;
        #endif

        return channel;
    }

// -----------------------------------------------------------------------------

    bool _relayFastybirdReportChannelsConfiguration() {
        for (uint8_t i = 0; i < relayCount(); i++) {
            if (
                !fastybirdReportChannelConfiguration(
                    _relay_fastybird_channel_index,
                    i,
                    _relayReportChannelConfiguration(i)
                )
            ) {
                return false;
            }
        }

        return true;
    }

// -----------------------------------------------------------------------------

    #if DIRECT_CONTROL_SUPPORT
        bool _relayFastybirdReportChannelsDirectControl() {
            String output;
            DynamicJsonBuffer jsonBuffer;

            for (uint8_t i = 0; i < relayCount(); i++) {
                JsonArray& direct_controls = jsonBuffer.createArray();

                directControlReportChannelConfiguration(i, FASTYBIRD_CHANNEL_TYPE_SWITCH, direct_controls);
                
                direct_controls.printTo(output);

                if (
                    !fastybirdReportChannelDirectControl(
                        _relay_fastybird_channel_index,
                        i,
                        output
                    )
                ) {
                    return false;
                }
            }

            return true;
        }
    #endif

    #if SCHEDULER_SUPPORT
        bool _relayFastybirdReportChannelsScheduler() {
            String output;
            DynamicJsonBuffer jsonBuffer;

            for (uint8_t i = 0; i < relayCount(); i++) {
                JsonArray& schedules = jsonBuffer.createArray();
                
                schReportChannelConfiguration(i, FASTYBIRD_CHANNEL_TYPE_SWITCH, schedules);

                schedules.printTo(output);

                if (
                    !fastybirdReportChannelScheduler(
                        _relay_fastybird_channel_index,
                        i,
                        output
                    )
                ) {
                    return false;
                }
            }

            return true;
        }
    #endif
#endif

// -----------------------------------------------------------------------------
// RELAY PROVIDERS
// -----------------------------------------------------------------------------

void _relayProviderStatus(
    const uint8_t id,
    bool status
) {
    // Check relay ID
    if (id >= _relays.size()) {
        return;
    }

    // Store new current status
    _relays[id].current_status = status;

    #if RELAY_PROVIDER == RELAY_PROVIDER_RFBRIDGE
        rfbStatus(id, status);
    #endif

    #if RELAY_PROVIDER == RELAY_PROVIDER_DUAL
        // Calculate mask
        uint8_t mask=0;

        for (uint8_t i = 0; i < _relays.size(); i++) {
            if (_relays[i].current_status) {
                mask = mask + (1 << i);
            }
        }

        // Send it to F330
        Serial.flush();
        Serial.write(0xA0);
        Serial.write(0x04);
        Serial.write(mask);
        Serial.write(0xA1);
        Serial.flush();
    #endif

    #if RELAY_PROVIDER == RELAY_PROVIDER_STM
        Serial.flush();
        Serial.write(0xA0);
        Serial.write(id + 1);
        Serial.write(status);
        Serial.write(0xA1 + status + id);
        Serial.flush();
    #endif

    #if RELAY_PROVIDER == RELAY_PROVIDER_LIGHT
        // Real relays
        uint8_t physical = _relays.size() - DUMMY_RELAY_COUNT;

        // Support for a mixed of dummy and real relays
        // Reference: https://github.com/xoseperez/espurna/issues/1305
        if (id >= physical) {
            // If the number of dummy relays matches the number of light channels
            // assume each relay controls one channel.
            // If the number of dummy relays is the number of channels plus 1
            // assume the first one controls all the channels and
            // the rest one channel each.
            // Otherwise every dummy relay controls all channels.
            if (DUMMY_RELAY_COUNT == lightChannels()) {
                lightState(id-physical, status);
                lightState(true);

            } else if (DUMMY_RELAY_COUNT == (lightChannels() + 1u)) {
                if (id == physical) {
                    lightState(status);

                } else {
                    lightState(id - 1 - physical, status);
                }

            } else {
                lightState(status);
            }

            lightUpdate(true, true);

            return;
        }
    #endif

    #if (RELAY_PROVIDER == RELAY_PROVIDER_RELAY) || (RELAY_PROVIDER == RELAY_PROVIDER_LIGHT)
        // If this is a light, all dummy relays have already been processed above
        // we reach here if the user has toggled a physical relay
        if (_relays[id].type == RELAY_TYPE_NORMAL) {
            digitalWrite(_relays[id].pin, status);

        } else if (_relays[id].type == RELAY_TYPE_INVERSE) {
            digitalWrite(_relays[id].pin, !status);

        } else if (_relays[id].type == RELAY_TYPE_LATCHED || _relays[id].type == RELAY_TYPE_LATCHED_INVERSE) {
            bool pulse = RELAY_TYPE_LATCHED ? HIGH : LOW;

            digitalWrite(_relays[id].pin, !pulse);

            if (GPIO_NONE != _relays[id].reset_pin) {
                digitalWrite(_relays[id].reset_pin, !pulse);
            }

            if (status || (GPIO_NONE == _relays[id].reset_pin)) {
                digitalWrite(_relays[id].pin, pulse);

            } else {
                digitalWrite(_relays[id].reset_pin, pulse);
            }

            niceDelay(RELAY_LATCHING_PULSE);

            digitalWrite(_relays[id].pin, !pulse);

            if (GPIO_NONE != _relays[id].reset_pin) {
                digitalWrite(_relays[id].reset_pin, !pulse);
            }
        }
    #endif
}

// -----------------------------------------------------------------------------

/**
 * Walks the relay vector processing only those relays
 * that have to change to the requested mode
 * @bool mode Requested mode
 */
void _relayProcess(
    const bool mode
) {
    uint32_t current_time = millis();

    for (uint8_t id = 0; id < _relays.size(); id++) {
        bool target = _relays[id].target_status;

        // Only process the relays we have to change
        if (target == _relays[id].current_status) {
            continue;
        }

        // Only process the relays we have to change to the requested mode
        if (target != mode) {
            continue;
        }

        // Only process if the change_time has arrived
        if (current_time < _relays[id].change_time) {
            continue;
        }

        DEBUG_MSG(PSTR("[RELAY] #%d set to %s\n"), id, target ? "ON" : "OFF");

        // Call the provider to perform the action
        _relayProviderStatus(id, target);

        // Send to Broker
        #if FASTYBIRD_SUPPORT
            fastybirdReportChannelValue(
                _relay_fastybird_channel_index,
                id,
                target ? FASTYBIRD_SWITCH_PAYLOAD_ON : FASTYBIRD_SWITCH_PAYLOAD_OFF
            );
        #endif

        if (!_relayRecursive) {
            relayPulse(id);

            // We will trigger a commit only if
            // we care about current relay status on boot
            uint8_t boot_mode = getSetting("relayBoot", id, RELAY_BOOT_MODE).toInt();
            bool do_commit = ((RELAY_BOOT_SAME == boot_mode) || (RELAY_BOOT_TOGGLE == boot_mode));

            _relaySaveTicker.once_ms(RELAY_SAVE_DELAY, relaySave, do_commit);

            #if WEB_SUPPORT && WS_SUPPORT
                wsSend(_relayWebSocketUpdate);
            #endif
        }

        _relays[id].report = false;
    }
}

// -----------------------------------------------------------------------------
// MODULE API
// -----------------------------------------------------------------------------

void relayPulse(
    const uint8_t id
) {
    _relays[id].pulseTicker.detach();

    byte mode = _relays[id].pulse;

    if (mode == RELAY_PULSE_NONE) {
        return;
    }

    uint32_t ms = _relays[id].pulse_ms;

    if (ms == 0) {
        return;
    }

    bool status = relayStatus(id);
    bool pulseStatus = (mode == RELAY_PULSE_ON);

    if (pulseStatus != status) {
        DEBUG_MSG(PSTR("[RELAY] Scheduling relay #%d back in %lums (pulse)\n"), id, ms);

        _relays[id].pulseTicker.once_ms(ms, relayToggle, id);

        // Reconfigure after dynamic pulse
        _relays[id].pulse = getSetting("relayPulse", id, RELAY_PULSE_MODE).toInt();
        _relays[id].pulse_ms = 1000 * getSetting("relayTime", id, RELAY_PULSE_MODE).toFloat();
    }
}

// -----------------------------------------------------------------------------

bool relayStatus(
    const uint8_t id,
    const bool status,
    const bool report
) {
    if (id >= _relays.size()) {
        return false;
    }

    bool changed = false;

    if (_relays[id].current_status == status) {
        if (_relays[id].target_status != status) {
            DEBUG_MSG(PSTR("[RELAY] #%d scheduled change cancelled\n"), id);

            _relays[id].target_status = status;
            _relays[id].report = false;

            changed = true;
        }

        // For RFBridge, keep sending the message even if the status is already the required
        #if RELAY_PROVIDER == RELAY_PROVIDER_RFBRIDGE
            rfbStatus(id, status);
        #endif

        // Update the pulse counter if the relay is already in the non-normal state (#454)
        relayPulse(id);

    } else {
        uint32_t current_time = millis();
        uint32_t fw_end = _relays[id].fw_start + 1000 * RELAY_FLOOD_WINDOW;
        uint32_t delay = status ? _relays[id].delay_on : _relays[id].delay_off;

        _relays[id].fw_count++;
        _relays[id].change_time = current_time + delay;

        // If current_time is off-limits the floodWindow...
        if (current_time < _relays[id].fw_start || fw_end <= current_time) {
            // We reset the floodWindow
            _relays[id].fw_start = current_time;
            _relays[id].fw_count = 1;

        // If current_time is in the floodWindow and there have been too many requests...
        } else if (_relays[id].fw_count >= RELAY_FLOOD_CHANGES) {
            // We schedule the changes to the end of the floodWindow
            // unless it's already delayed beyond that point
            if (fw_end - delay > current_time) {
                _relays[id].change_time = fw_end;
            }
        }

        _relays[id].target_status = status;

        if (report) {
            _relays[id].report = true;
        }

        relaySync(id);

        DEBUG_MSG(PSTR("[RELAY] #%d scheduled %s in %u ms\n"),
                id, status ? "ON" : "OFF",
                (_relays[id].change_time - current_time));

        changed = true;
    }

    return changed;
}

// -----------------------------------------------------------------------------

void relaySync(
    const uint8_t id
) {
    // No sync if none or only one relay
    if (_relays.size() < 2) {
        return;
    }

    // Do not go on if we are comming from a previous sync
    if (_relayRecursive) {
        return;
    }

    // Flag sync mode
    _relayRecursive = true;

    byte relaySync = getSetting("relaySync", RELAY_SYNC).toInt();
    bool status = _relays[id].target_status;

    // If RELAY_SYNC_SAME all relays should have the same state
    if (relaySync == RELAY_SYNC_SAME) {
        for (uint8_t i = 0; i<_relays.size(); i++) {
            if (i != id) {
                relayStatus(i, status);
            }
        }

    // If NONE_OR_ONE or ONE and setting ON we should set OFF all the others
    } else if (status) {
        if (relaySync != RELAY_SYNC_ANY) {
            for (uint8_t i = 0; i<_relays.size(); i++) {
                if (i != id) {
                    relayStatus(i, false);
                }
            }
        }

    // If ONLY_ONE and setting OFF we should set ON the other one
    } else {
        if (relaySync == RELAY_SYNC_ONE) {
            uint8_t i = (id + 1) % _relays.size();
            relayStatus(i, true);
        }
    }

    // Unflag sync mode
    _relayRecursive = false;
}

// -----------------------------------------------------------------------------

uint8_t relayCount() {
    return _relays.size();
}

// -----------------------------------------------------------------------------

void relaySave(
    const bool doCommit
) {
    // Relay status is stored in a single byte
    // This means that, atm,
    // we are only storing the status of the first 8 relays.
    uint8_t bit = 1;
    uint8_t mask = 0;
    uint8_t count = _relays.size();

    if (count > 8) {
        count = 8;
    }

    for (uint8_t i = 0; i < count; i++) {
        if (relayStatus(i)) {
            mask += bit;
        }

        bit += bit;
    }

    EEPROMr.write(EEPROM_RELAY_STATUS, mask);

    DEBUG_MSG(PSTR("[RELAY] Setting relay mask: %d\n"), mask);

    // The 'doCommit' flag controls wether we are commiting this change or not.
    // It is useful to set it to 'false' if the relay change triggering the
    // save involves a relay whose boot mode is independent from current mode,
    // thus storing the last relay value is not absolutely necessary.
    // Nevertheless, we store the value in the EEPROM buffer so it will be written
    // on the next commit.
    if (doCommit) {
        // We are actually enqueuing the commit so it will be
        // executed on the main loop, in case this is called from a callback
        eepromCommit();
    }
}

// -----------------------------------------------------------------------------

void relaySave() {
    relaySave(true);
}

// -----------------------------------------------------------------------------

void relayToggle(
    const uint8_t id,
    const bool report
) {
    if (id >= _relays.size()) {
        return;
    }

    relayStatus(id, !relayStatus(id), report);
}

// -----------------------------------------------------------------------------

void relayToggle(
    const uint8_t id
) {
    relayToggle(id, true);
}

// -----------------------------------------------------------------------------

bool relayStatus(
    const uint8_t id,
    const bool status
) {
    return relayStatus(id, status, true);
}

// -----------------------------------------------------------------------------

bool relayStatus(
    const uint8_t id
) {
    // Check relay ID
    if (id >= _relays.size()) {
        return false;
    }

    // Get status from storage
    return _relays[id].current_status;
}

// -----------------------------------------------------------------------------
// MODULE CORE
// -----------------------------------------------------------------------------

void relaySetup() {
    // Ad-hoc relays
    #if RELAY1_PIN != GPIO_NONE
        _relays.push_back((relay_t) { RELAY1_PIN, RELAY1_TYPE, RELAY1_RESET_PIN, RELAY1_DELAY_ON, RELAY1_DELAY_OFF });
    #endif
    #if RELAY2_PIN != GPIO_NONE
        _relays.push_back((relay_t) { RELAY2_PIN, RELAY2_TYPE, RELAY2_RESET_PIN, RELAY2_DELAY_ON, RELAY2_DELAY_OFF });
    #endif
    #if RELAY3_PIN != GPIO_NONE
        _relays.push_back((relay_t) { RELAY3_PIN, RELAY3_TYPE, RELAY3_RESET_PIN, RELAY3_DELAY_ON, RELAY3_DELAY_OFF });
    #endif
    #if RELAY4_PIN != GPIO_NONE
        _relays.push_back((relay_t) { RELAY4_PIN, RELAY4_TYPE, RELAY4_RESET_PIN, RELAY4_DELAY_ON, RELAY4_DELAY_OFF });
    #endif
    #if RELAY5_PIN != GPIO_NONE
        _relays.push_back((relay_t) { RELAY5_PIN, RELAY5_TYPE, RELAY5_RESET_PIN, RELAY5_DELAY_ON, RELAY5_DELAY_OFF });
    #endif
    #if RELAY6_PIN != GPIO_NONE
        _relays.push_back((relay_t) { RELAY6_PIN, RELAY6_TYPE, RELAY6_RESET_PIN, RELAY6_DELAY_ON, RELAY6_DELAY_OFF });
    #endif
    #if RELAY7_PIN != GPIO_NONE
        _relays.push_back((relay_t) { RELAY7_PIN, RELAY7_TYPE, RELAY7_RESET_PIN, RELAY7_DELAY_ON, RELAY7_DELAY_OFF });
    #endif
    #if RELAY8_PIN != GPIO_NONE
        _relays.push_back((relay_t) { RELAY8_PIN, RELAY8_TYPE, RELAY8_RESET_PIN, RELAY8_DELAY_ON, RELAY8_DELAY_OFF });
    #endif

    #if BUTTON_SUPPORT
        #if RELAY1_BTN > 0
            buttonOnEventRegister(
                [](uint8_t event) {
                    if (event == RELAY_SWITCH_BTN_EVENT) {
                        relayToggle(0);
                    }
                },
                (RELAY1_BTN - 1)
            );
        #endif

        #if RELAY2_BTN > 0
            buttonOnEventRegister(
                [](uint8_t event) {
                    if (event == RELAY_SWITCH_BTN_EVENT) {
                        relayToggle(1);
                    }
                },
                (RELAY2_BTN - 1)
            );
        #endif

        #if RELAY3_BTN > 0
            buttonOnEventRegister(
                [](uint8_t event) {
                    if (event == RELAY_SWITCH_BTN_EVENT) {
                        relayToggle(2);
                    }
                },
                (RELAY3_BTN - 1)
            );
        #endif

        #if RELAY4_BTN > 0
            buttonOnEventRegister(
                [](uint8_t event) {
                    if (event == RELAY_SWITCH_BTN_EVENT) {
                        relayToggle(3);
                    }
                },
                (RELAY4_BTN - 1)
            );
        #endif

        #if RELAY5_BTN > 0
            buttonOnEventRegister(
                [](uint8_t event) {
                    if (event == RELAY_SWITCH_BTN_EVENT) {
                        relayToggle(4);
                    }
                },
                (RELAY5_BTN - 1)
            );
        #endif

        #if RELAY6_BTN > 0
            buttonOnEventRegister(
                [](uint8_t event) {
                    if (event == RELAY_SWITCH_BTN_EVENT) {
                        relayToggle(5);
                    }
                },
                (RELAY6_BTN - 1)
            );
        #endif

        #if RELAY7_BTN > 0
            buttonOnEventRegister(
                [](uint8_t event) {
                    if (event == RELAY_SWITCH_BTN_EVENT) {
                        relayToggle(6);
                    }
                },
                (RELAY7_BTN - 1)
            );
        #endif

        #if RELAY8_BTN > 0
            buttonOnEventRegister(
                [](uint8_t event) {
                    if (event == RELAY_SWITCH_BTN_EVENT) {
                        relayToggle(7);
                    }
                },
                (RELAY8_BTN - 1)
            );
        #endif
    #endif

    // Dummy relays for AI Light, Magic Home LED Controller, H801, Sonoff Dual and Sonoff RF Bridge
    // No delay_on or off for these devices to easily allow having more than
    // 8 channels. This behaviour will be recovered with v2.
    for (uint8_t i = 0; i < DUMMY_RELAY_COUNT; i++) {
        _relays.push_back((relay_t) {GPIO_NONE, RELAY_TYPE_NORMAL, 0, 0, 0});
    }

    _relayConfigure();
    _relayBoot();

    relayLoop();

    // Main callbacks
    firmwareRegisterLoop(relayLoop);
    firmwareRegisterReload(_relayConfigure);

    #if WEB_SUPPORT && WS_SUPPORT
        wsOnConnectRegister(_relayWSOnConnect);
        wsOnConfigureRegister(_relayWSOnConfigure);
        wsOnActionRegister(_relayWSOnAction);
    #endif

    #if FASTYBIRD_SUPPORT
        fastybirdReportConfigurationSchemaRegister(_relayReportConfigurationSchema);
        fastybirdReportConfigurationRegister(_relayReportConfiguration);
        fastybirdOnConfigureRegister(_relayUpdateConfiguration);

        if (relayCount() > 0) {
            _relay_fastybird_channel_index = fastybirdRegisterChannel(_relayFastybirdGetChannelStructure());

            fastybirdChannelsReportConfigurationRegister(_relayFastybirdReportChannelsConfiguration);
            
            #if DIRECT_CONTROL_SUPPORT
               fastybirdChannelsReportDirectControlsRegister(_relayFastybirdReportChannelsDirectControl);
            #endif

            #if SCHEDULER_SUPPORT
               fastybirdChannelsReportSchedulerRegister(_relayFastybirdReportChannelsScheduler);
            #endif
        }
    #endif

    #if DIRECT_CONTROL_SUPPORT
        directControlRegisterChannel(FASTYBIRD_CHANNEL_TYPE_SWITCH, FASTYBIRD_PROPERTY_STATE, _relays.size());
    #endif

    DEBUG_MSG(PSTR("[RELAY] Number of relays: %d\n"), _relays.size());
}

//------------------------------------------------------------------------------

void relayLoop() {
    _relayProcess(false);
    _relayProcess(true);
}

#endif // RELAY_PROVIDER != RELAY_PROVIDER_NONE