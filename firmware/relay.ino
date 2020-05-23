/*

RELAY MODULE

Copyright (C) 2017 - 2018 FastyBird Ltd. <info@fastybird.com>

*/

#if RELAY_PROVIDER != RELAY_PROVIDER_NONE

#include <EEPROM.h>

typedef struct {
    // Configuration variables
    uint8_t pin;            // GPIO pin for the relay
    uint8_t type;           // RELAY_TYPE_NORMAL, RELAY_TYPE_INVERSE, RELAY_TYPE_LATCHED or RELAY_TYPE_LATCHED_INVERSE
    uint8_t reset_pin;      // GPIO to reset the relay if RELAY_TYPE_LATCHED
    uint8_t channel_index;  // Communication channel number
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

bool _relay_recursive = false;

Ticker _relay_save_ticker;

// -----------------------------------------------------------------------------
// MODULE PRIVATE
// -----------------------------------------------------------------------------

void _relayConfigure()
{
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

void _relayBoot()
{
    _relay_recursive = true;

    uint8_t bit = 1;
    bool trigger_save = false;
    uint32_t stored_mask = 0;

    if (rtcmemStatus()) {
        stored_mask = _relayMaskRtcmem();

    } else {
        stored_mask = EEPROMr.read(EEPROM_RELAY_STATUS);
    }

    DEBUG_MSG(PSTR("[INFO][RELAY] Retrieving mask: %d\n"), stored_mask);

    auto mask = std::bitset<RELAY_SAVE_MASK_MAX>(stored_mask);

    // Walk the relays
    bool status;

    for (uint8_t i = 0; i < _relays.size(); i++) {
        uint8_t boot_mode = getSetting("relayBoot", i, RELAY_BOOT_MODE).toInt();

        DEBUG_MSG(PSTR("[INFO][RELAY] Relay #%d boot mode %d\n"), i, boot_mode);

        status = false;

        switch (boot_mode) {
            case RELAY_BOOT_SAME:
                if (i < 8) {
                    status = mask.test(i);
                }
                break;

            case RELAY_BOOT_TOGGLE:
                if (i < 8) {
                    status = !mask[i];
                    mask.flip(i);
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
        _relayMaskRtcmem(mask.to_ulong());

        EEPROMr.write(EEPROM_RELAY_STATUS, mask.to_ulong());
        eepromCommit();
    }

    _relay_recursive = false;
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

    } else {
        return 0xFF;
    }
}

// -----------------------------------------------------------------------------

#if FASTYBIRD_SUPPORT || (WEB_SUPPORT && WS_SUPPORT)
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
    bool _relayUpdateConfiguration(
        JsonObject& configuration
    ) {
        DEBUG_MSG(PSTR("[INFO][RELAY] Updating module\n"));

        if (relayCount() > 1) {
            if (
                configuration.containsKey("relays_sync")
                && configuration["relays_sync"].as<uint8_t>() >= 0  // Minimum select value
                && configuration["relays_sync"].as<uint8_t>() <= 3  // Maximum select value
                && configuration["relays_sync"].as<uint8_t>() != getSetting("relaysSync").toInt()
            )  {
                setSetting("relaysSync", configuration["relays_sync"].as<uint8_t>());

                return true;
            }

        } else {
            delSetting("relaysSync");
        }

        return false;
    }

// -----------------------------------------------------------------------------

    void _relayReportRelayConfigurationSchema(
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

    void _relayReportRelayConfiguration(
        const uint8_t id,
        JsonObject& configuration
    ) {
        configuration["relay_boot"] = getSetting("relayBoot", id, RELAY_BOOT_MODE).toInt();
        configuration["pulse_mode"] = getSetting("relayPulse", id, RELAY_PULSE_MODE).toInt();
        configuration["pulse_time"] = getSetting("relayTime", id, RELAY_PULSE_TIME).toFloat();
        configuration["on_disconnect"] = getSetting("relayOnDisc", id, 0).toInt();
    }

// -----------------------------------------------------------------------------

    bool _relayRelayConfigure(
        const uint8_t id,
        JsonObject& configuration
    ) {
        DEBUG_MSG(PSTR("[INFO][RELAY] Updating channel: %d\n"), id);

        bool is_updated = false;

        if (configuration.containsKey("relay_boot"))  {
            DEBUG_MSG(PSTR("[INFO][RELAY] Setting: \"relay_boot\" to: %d\n"), configuration["relay_boot"].as<uint8_t>());

            setSetting("relayBoot", id, configuration["relay_boot"].as<uint8_t>());

            is_updated = true;
        }

        if (configuration.containsKey("pulse_mode"))  {
            DEBUG_MSG(PSTR("[INFO][RELAY] Setting: \"pulse_mode\" to: %d\n"), configuration["pulse_mode"].as<uint8_t>());

            setSetting("relayPulseMode", id, configuration["pulse_mode"].as<uint8_t>());

            is_updated = true;
        }

        if (
            configuration.containsKey("pulse_time")
            && configuration["pulse_time"].as<float>() >= 1.0
            && configuration["pulse_time"].as<float>() <= 60.0
        )  {
            DEBUG_MSG(PSTR("[INFO][RELAY] Setting: \"pulse_time\" to: %d\n"), configuration["pulse_time"].as<uint8_t>());

            setSetting("relayPulseTime", id, configuration["pulse_time"].as<float>());

            is_updated = true;
        }

        if (configuration.containsKey("on_disconnect"))  {
            DEBUG_MSG(PSTR("[INFO][RELAY] Setting: \"on_disconnect\" to: %d\n"), configuration["on_disconnect"].as<uint8_t>());

            setSetting("relayOnDisc", id, configuration["on_disconnect"].as<uint8_t>());

            is_updated = true;
        }

        return is_updated;
    }
#endif // FASTYBIRD_SUPPORT || (WEB_SUPPORT && WS_SUPPORT)

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
        JsonArray& channels = container.createNestedArray("channels");

        for (uint8_t i = 0; i < relayCount(); i++) {
            channels.add(_relays[i].target_status);
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

        // Data container
        JsonObject& data = module.createNestedObject("data");

        // Channels statuses
        JsonArray& channels = data.createNestedArray("channels");

        for (uint8_t i = 0; i < relayCount(); i++) {
            channels.add(_relays[i].target_status);
        }

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

        _relayReportRelayConfigurationSchema(channels_configuration_schema);

        // Channels configuration values container
        JsonArray& channels_configuration_values = channels_configuration.createNestedArray("values");

        for (uint8_t i = 0; i < relayCount(); i++) {
            JsonObject& channel_configuration_value = channels_configuration_values.createNestedObject();

            _relayReportRelayConfiguration(i, channel_configuration_value);
            channel_configuration_value["gpio"] = _relays[i].pin;
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
                DEBUG_MSG(PSTR("[INFO][RELAY] Found configuration for module\n"));

                bool is_configuration_updated = false;

                // Extract configuration container
                JsonObject& configuration = module["config"].as<JsonObject&>();

                if (configuration.containsKey("values") && _relayUpdateConfiguration(configuration["values"])) {
                    is_configuration_updated = true;
                }

                if (configuration.containsKey("channels")) {
                    JsonObject& channels_configuration = configuration["channels"].as<JsonObject&>();

                    if (channels_configuration.containsKey("values")) {
                        for (uint8_t i = 0; i < channels_configuration["values"].size(); i++) {
                            if (_relayRelayConfigure(i, channels_configuration["values"][i])) {
                                is_configuration_updated = true;

                                #if FASTYBIRD_SUPPORT
                                    DynamicJsonBuffer jsonBuffer;

                                    JsonObject& configuration = jsonBuffer.createObject();

                                    fastybirdCallReportChannelConfiguration(_relays[i].channel_index, configuration);

                                    fastybird_channel_t channel = fastybirdGetChannel(_relays[i].channel_index);

                                    if (configuration.size() > 0) {
                                        fastybirdApiPropagateChannelConfiguration(channel.name.c_str(), configuration);
                                    }
                                #endif
                            }
                        }
                    }
                }

                if (is_configuration_updated) {
                    wsSend_P(clientId, PSTR("{\"message\": \"relay_updated\"}"));

                    // Reload & cache settings
                    firmwareReload();
                }
            }
        }
    }

// -----------------------------------------------------------------------------

    void _relayWSOnUpdate(
        JsonObject& root
    ) {
        JsonArray& modules = root.containsKey("modules") ? root["modules"] : root.createNestedArray("modules");
        JsonObject& module = modules.createNestedObject();

        module["module"] = "relay";

        // Data container
        JsonObject& data = module.createNestedObject("data");

        // Channels statuses
        JsonArray& channels_statuses = data.createNestedArray("channels");

        for (uint8_t i = 0; i < relayCount(); i++) {
            channels_statuses.add(_relays[i].target_status);
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
                uint32_t value = _relayParsePayload(data["channels"][i].as<char *>());

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
#endif // WEB_SUPPORT && WS_SUPPORT

// -----------------------------------------------------------------------------

#if FASTYBIRD_SUPPORT && FASTYBIRD_MAX_CHANNELS > 0
    void _relayFastyBirdProperyPayload(
        const uint8_t channelIndex,
        const uint8_t propertyIndex,
        const char * payload
    ) {
        for (uint8_t i = 0; i < _relays.size(); i++) {
            if (_relays[i].channel_index == channelIndex) {
                // Toggle relay status
                if (strcmp(payload, FASTYBIRD_SWITCH_PAYLOAD_TOGGLE) == 0) {
                    relayToggle(i);

                // Set relay status
                } else {
                    relayStatus(i, strcmp(payload, FASTYBIRD_SWITCH_PAYLOAD_ON) == 0);
                }
            }
        }
    }

// -----------------------------------------------------------------------------

    void _relayFastyBirdProperyQuery(
        const uint8_t channelIndex,
        const uint8_t propertyIndex
    ) {
        for (uint8_t i = 0; i < _relays.size(); i++) {
            if (_relays[i].channel_index == channelIndex) {
                fastybirdReportChannelPropertyValue(
                    channelIndex,
                    fastybirdFindChannelPropertyIndex(channelIndex, FASTYBIRD_PROPERTY_SWITCH),
                    relayStatus(i) ? FASTYBIRD_SWITCH_PAYLOAD_ON : FASTYBIRD_SWITCH_PAYLOAD_OFF
                );
            }
        }
    }

// -----------------------------------------------------------------------------

    void _relayFastyBirdChannelReportConfigurationSchema(
        const uint8_t channelIndex,
        JsonArray& schema
    ) {
        for (uint8_t i = 0; i < _relays.size(); i++) {
            if (_relays[i].channel_index == channelIndex) {
                _relayReportRelayConfigurationSchema(schema);
            }
        }
    }

// -----------------------------------------------------------------------------

    void _relayFastyBirdChannelReportConfiguration(
        const uint8_t channelIndex,
        JsonObject& configuration
    ) {
        for (uint8_t i = 0; i < _relays.size(); i++) {
            if (_relays[i].channel_index == channelIndex) {
                _relayReportRelayConfiguration(i, configuration);
            }
        }
    }

// -----------------------------------------------------------------------------

    void _relayFastyBirdChannelConfigure(
        const uint8_t channelIndex,
        JsonObject& configuration
    ) {
        for (uint8_t i = 0; i < _relays.size(); i++) {
            if (_relays[i].channel_index == channelIndex) {
                _relayRelayConfigure(i, configuration);

                DEBUG_MSG(PSTR("[INFO][RELAY] Configuration changes were saved\n"));
            }
        }
    }

// -----------------------------------------------------------------------------

    void _relayFastyBirdRegisterRealys()
    {
        bool has_channel = false;

        for (uint8_t i = 0; i < _relays.size(); i++) {
            if (_relays[i].channel_index != INDEX_NONE) {
                has_channel = true;
            }
        }

        if (has_channel) {
            char format[30];

            strcpy(format, FASTYBIRD_SWITCH_PAYLOAD_ON);
            strcat(format, ",");
            strcat(format, FASTYBIRD_SWITCH_PAYLOAD_OFF);
            strcat(format, ",");
            strcat(format, FASTYBIRD_SWITCH_PAYLOAD_TOGGLE);

            // Create relay property structure
            uint8_t propertyIndex = fastybirdRegisterProperty(
                FASTYBIRD_PROPERTY_SWITCH,
                FASTYBIRD_PROPERTY_DATA_TYPE_ENUM,
                "",
                format,
                _relayFastyBirdProperyPayload,
                _relayFastyBirdProperyQuery
            );

            for (uint8_t i = 0; i < _relays.size(); i++) {
                // Register property to channel
                fastybirdMapPropertyToChannel(_relays[i].channel_index, propertyIndex);

                fastybirdReportChannelConfigurationSchemaRegister(_relays[i].channel_index, _relayFastyBirdChannelReportConfigurationSchema);
                fastybirdReportChannelConfigurationRegister(_relays[i].channel_index, _relayFastyBirdChannelReportConfiguration);
                fastybirdOnChannelConfigureRegister(_relays[i].channel_index, _relayFastyBirdChannelConfigure);
            }
        }
    }
#endif // FASTYBIRD_SUPPORT && FASTYBIRD_MAX_CHANNELS > 0

// -----------------------------------------------------------------------------

#if MQTT_SUPPORT
    void _relayMQTTOnConnect()
    {
        // TODO: implement functionality
    }

// -----------------------------------------------------------------------------

    void _relayMQTTOnDisconnect()
    {
        // TODO: implement functionality
    }
#endif // MQTT_SUPPORT

// -----------------------------------------------------------------------------
// RELAY PROVIDERS
// -----------------------------------------------------------------------------

void _relayMaskRtcmem(
    uint32_t mask
) {
    Rtcmem->relay = mask;
}

// -----------------------------------------------------------------------------

uint32_t _relayMaskRtcmem()
{
    return Rtcmem->relay;
}

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
        uint8_t mask = 0;

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

        DEBUG_MSG(PSTR("[INFO][RELAY] #%d set to %s\n"), id, target ? "ON" : "OFF");

        // Call the provider to perform the action
        _relayProviderStatus(id, target);

        // Send to Broker
        #if FASTYBIRD_SUPPORT && FASTYBIRD_MAX_CHANNELS > 0
            fastybirdReportChannelPropertyValue(
                _relays[id].channel_index,
                fastybirdFindChannelPropertyIndex(_relays[id].channel_index, FASTYBIRD_PROPERTY_SWITCH),
                target ? FASTYBIRD_SWITCH_PAYLOAD_ON : FASTYBIRD_SWITCH_PAYLOAD_OFF
            );
        #endif

        if (!_relay_recursive) {
            relayPulse(id);

            // We will trigger a commit only if
            // we care about current relay status on boot
            uint8_t boot_mode = getSetting("relayBoot", id, RELAY_BOOT_MODE).toInt();
            bool save_eeprom = ((RELAY_BOOT_SAME == boot_mode) || (RELAY_BOOT_TOGGLE == boot_mode));

            _relay_save_ticker.once_ms(RELAY_SAVE_DELAY, relaySave, save_eeprom);

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
        DEBUG_MSG(PSTR("[INFO][RELAY] Scheduling relay #%d back in %lums (pulse)\n"), id, ms);

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
            DEBUG_MSG(PSTR("[INFO][RELAY] #%d scheduled change cancelled\n"), id);

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

        DEBUG_MSG(PSTR("[INFO][RELAY] #%d scheduled %s in %u ms\n"),
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
    if (_relay_recursive) {
        return;
    }

    // Flag sync mode
    _relay_recursive = true;

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
    _relay_recursive = false;
}

// -----------------------------------------------------------------------------

uint8_t relayCount()
{
    return _relays.size();
}

// -----------------------------------------------------------------------------

void relaySave(
    const bool eeprom
) {
    auto mask = std::bitset<RELAY_SAVE_MASK_MAX>(0);

    uint8_t count = relayCount();

    if (count > RELAY_SAVE_MASK_MAX) {
        count = RELAY_SAVE_MASK_MAX;
    }

    for (uint8_t i = 0; i < count; ++i) {
        mask.set(i, relayStatus(i));
    }

    const uint32_t mask_value = mask.to_ulong();

    DEBUG_MSG(PSTR("[INFO][RELAY] Setting relay mask: %d\n"), mask);

    // Persist only to rtcmem, unless requested to save to the eeprom
    _relayMaskRtcmem(mask_value);

    // The 'eeprom' flag controls wether we are commiting this change or not.
    // It is useful to set it to 'false' if the relay change triggering the
    // save involves a relay whose boot mode is independent from current mode,
    // thus storing the last relay value is not absolutely necessary.
    // Nevertheless, we store the value in the EEPROM buffer so it will be written
    // on the next commit.
    if (eeprom) {
        // We are actually enqueuing the commit so it will be
        // executed on the main loop, in case this is called from a callback
        eepromCommit();
    }
}

// -----------------------------------------------------------------------------

void relaySave()
{
    relaySave(false);
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

void relaySetup()
{
    // Ad-hoc relays
    #if RELAY1_PIN != GPIO_NONE
        _relays.push_back((relay_t) { RELAY1_PIN, RELAY1_TYPE, RELAY1_RESET_PIN, FASTYBIRD_RELAY1_CHANNEL_INDEX, RELAY1_DELAY_ON, RELAY1_DELAY_OFF });
    #endif
    #if RELAY2_PIN != GPIO_NONE
        _relays.push_back((relay_t) { RELAY2_PIN, RELAY2_TYPE, RELAY2_RESET_PIN, FASTYBIRD_RELAY2_CHANNEL_INDEX, RELAY2_DELAY_ON, RELAY2_DELAY_OFF });
    #endif
    #if RELAY3_PIN != GPIO_NONE
        _relays.push_back((relay_t) { RELAY3_PIN, RELAY3_TYPE, RELAY3_RESET_PIN, FASTYBIRD_RELAY3_CHANNEL_INDEX, RELAY3_DELAY_ON, RELAY3_DELAY_OFF });
    #endif
    #if RELAY4_PIN != GPIO_NONE
        _relays.push_back((relay_t) { RELAY4_PIN, RELAY4_TYPE, RELAY4_RESET_PIN, FASTYBIRD_RELAY4_CHANNEL_INDEX, RELAY4_DELAY_ON, RELAY4_DELAY_OFF });
    #endif
    #if RELAY5_PIN != GPIO_NONE
        _relays.push_back((relay_t) { RELAY5_PIN, RELAY5_TYPE, RELAY5_RESET_PIN, FASTYBIRD_RELAY5_CHANNEL_INDEX, RELAY5_DELAY_ON, RELAY5_DELAY_OFF });
    #endif
    #if RELAY6_PIN != GPIO_NONE
        _relays.push_back((relay_t) { RELAY6_PIN, RELAY6_TYPE, RELAY6_RESET_PIN, FASTYBIRD_RELAY6_CHANNEL_INDEX, RELAY6_DELAY_ON, RELAY6_DELAY_OFF });
    #endif
    #if RELAY7_PIN != GPIO_NONE
        _relays.push_back((relay_t) { RELAY7_PIN, RELAY7_TYPE, RELAY7_RESET_PIN, FASTYBIRD_RELAY7_CHANNEL_INDEX, RELAY7_DELAY_ON, RELAY7_DELAY_OFF });
    #endif
    #if RELAY8_PIN != GPIO_NONE
        _relays.push_back((relay_t) { RELAY8_PIN, RELAY8_TYPE, RELAY8_RESET_PIN, FASTYBIRD_RELAY8_CHANNEL_INDEX, RELAY8_DELAY_ON, RELAY8_DELAY_OFF });
    #endif

    #if BUTTON_SUPPORT
        #if RELAY1_BTN_INDEX != INDEX_NONE
            buttonOnEventRegister(
                [](uint8_t event) {
                    if (event == RELAY1_BTN_EVENT) {
                        relayToggle(0);
                    }
                },
                (uint8_t) RELAY1_BTN_INDEX
            );
        #endif

        #if RELAY2_BTN_INDEX != INDEX_NONE
            buttonOnEventRegister(
                [](uint8_t event) {
                    if (event == RELAY2_BTN_EVENT) {
                        relayToggle(1);
                    }
                },
                (uint8_t) RELAY2_BTN_INDEX
            );
        #endif

        #if RELAY3_BTN_INDEX != INDEX_NONE
            buttonOnEventRegister(
                [](uint8_t event) {
                    if (event == RELAY3_BTN_EVENT) {
                        relayToggle(2);
                    }
                },
                (uint8_t) RELAY3_BTN_INDEX
            );
        #endif

        #if RELAY4_BTN_INDEX != INDEX_NONE
            buttonOnEventRegister(
                [](uint8_t event) {
                    if (event == RELAY4_BTN_EVENT) {
                        relayToggle(3);
                    }
                },
                (uint8_t) RELAY4_BTN_INDEX
            );
        #endif

        #if RELAY5_BTN_INDEX != INDEX_NONE
            buttonOnEventRegister(
                [](uint8_t event) {
                    if (event == RELAY5_BTN_EVENT) {
                        relayToggle(4);
                    }
                },
                (uint8_t) RELAY5_BTN_INDEX
            );
        #endif

        #if RELAY6_BTN_INDEX != INDEX_NONE
            buttonOnEventRegister(
                [](uint8_t event) {
                    if (event == RELAY6_BTN_EVENT) {
                        relayToggle(5);
                    }
                },
                (uint8_t) RELAY6_BTN_INDEX
            );
        #endif

        #if RELAY7_BTN_INDEX != INDEX_NONE
            buttonOnEventRegister(
                [](uint8_t event) {
                    if (event == RELAY7_BTN_EVENT) {
                        relayToggle(6);
                    }
                },
                (uint8_t) RELAY7_BTN_INDEX
            );
        #endif

        #if RELAY8_BTN_INDEX != INDEX_NONE
            buttonOnEventRegister(
                [](uint8_t event) {
                    if (event == RELAY8_BTN_EVENT) {
                        relayToggle(7);
                    }
                },
                (uint8_t) RELAY8_BTN_INDEX
            );
        #endif
    #endif

    // Dummy relays for AI Light, Magic Home LED Controller, H801, Sonoff Dual and Sonoff RF Bridge
    // No delay_on or off for these devices to easily allow having more than
    // 8 channels. This behaviour will be recovered with v2.
    for (uint8_t i = 0; i < DUMMY_RELAY_COUNT; i++) {
        _relays.push_back((relay_t) {GPIO_NONE, RELAY_TYPE_NORMAL, 0, 0, 0, INDEX_NONE});
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
        wsOnUpdateRegister(_relayWSOnUpdate);
        wsOnActionRegister(_relayWSOnAction);
    #endif

    #if FASTYBIRD_SUPPORT
        // Module schema report
        fastybirdReportConfigurationSchemaRegister(_relayReportConfigurationSchema);
        fastybirdReportConfigurationRegister(_relayReportConfiguration);
        fastybirdOnConfigureRegister(_relayUpdateConfiguration);

        #if FASTYBIRD_MAX_CHANNELS > 0
            if (relayCount() > 0) {
                _relayFastyBirdRegisterRealys();

                fastybirdOnConnectRegister([](){
                    for (uint8_t i = 0; i < relayCount(); i++) {
                        fastybirdReportChannelPropertyValue(
                            _relays[i].channel_index,
                            fastybirdFindChannelPropertyIndex(_relays[i].channel_index, FASTYBIRD_PROPERTY_SWITCH),
                            relayStatus(i) ? FASTYBIRD_SWITCH_PAYLOAD_ON : FASTYBIRD_SWITCH_PAYLOAD_OFF
                        );
                    }
                });
            }
        #endif
    #endif

    #if MQTT_SUPPORT
        mqttOnConnectRegister(_relayMQTTOnConnect);
        mqttOnDisconnectRegister(_relayMQTTOnDisconnect);
    #endif

    DEBUG_MSG(PSTR("[INFO][RELAY] Number of relays: %d\n"), relayCount());
}

//------------------------------------------------------------------------------

void relayLoop()
{
    _relayProcess(false);
    _relayProcess(true);
}

#endif // RELAY_PROVIDER != RELAY_PROVIDER_NONE