/*

RELAY MODULE

Copyright (C) 2018 FastyBird s.r.o. <code@fastybird.com>

*/

#if RELAY_PROVIDER != RELAY_PROVIDER_NONE

#include <EEPROM.h>

std::vector<relay_t> _relays;

bool _relay_recursive = false;

Ticker _relay_save_ticker;

// -----------------------------------------------------------------------------
// MODULE PRIVATE
// -----------------------------------------------------------------------------

void _relayConfigure()
{
    for (uint8_t i = 0; i < _relays.size(); i++) {
        _relays[i].pulse = getSetting("relay_pulse_mode", i, RELAY_PULSE_MODE).toInt();
        _relays[i].pulse_ms = 1000 * getSetting("relay_pulse_time", i, RELAY_PULSE_TIME).toFloat();

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

void _relay_boot()
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
        uint8_t boot_mode = getSetting("relay_boot_mode", i, RELAY_BOOT_MODE).toInt();

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

    if (strcmp(p, "false") == 0) {
        return 0;

    } else if (strcmp(p, "true") == 0) {
        return 1;

    } else if (strcmp(p, "toggle") == 0) {
        return 2;

    } else {
        return 0xFF;
    }
}

// -----------------------------------------------------------------------------

#if WEB_SUPPORT && WS_SUPPORT
    /**
     * Provide module configuration schema
     */
    void _relayReportConfigurationSchema(
        JsonArray& configuration
    ) {
        if (relayCount() > 1) {
            JsonObject& sync = configuration.createNestedObject();

            sync["name"] = "sync";
            sync["type"] = "select";
            sync["default"] = RELAY_SYNC;

            JsonArray& relays_sync_values = sync.createNestedArray("values");

            JsonObject& relays_sync_value_0 = relays_sync_values.createNestedObject();
            relays_sync_value_0["value"] = 0;
            relays_sync_value_0["name"] = "disabled";

            JsonObject& relays_sync_value_1 = relays_sync_values.createNestedObject();
            relays_sync_value_1["value"] = 1;
            relays_sync_value_1["name"] = "zero_or_one";

            JsonObject& relays_sync_value_2 = relays_sync_values.createNestedObject();
            relays_sync_value_2["value"] = 2;
            relays_sync_value_2["name"] = "only_one";

            JsonObject& relays_sync_value_3 = relays_sync_values.createNestedObject();
            relays_sync_value_3["value"] = 3;
            relays_sync_value_3["name"] = "all_synchronized";
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
            configuration["relays_sync"] = getSetting("relays_sync", RELAY_SYNC).toInt();
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
                configuration.containsKey("sync")
                && configuration["sync"].as<uint8_t>() >= 0  // Minimum select value
                && configuration["sync"].as<uint8_t>() <= 3  // Maximum select value
                && configuration["sync"].as<uint8_t>() != getSetting("relays_sync").toInt()
            )  {
                setSetting("relays_sync", configuration["sync"].as<uint8_t>());

                return true;
            }

        } else {
            delSetting("relays_sync");
        }

        return false;
    }

// -----------------------------------------------------------------------------

    void _relayReportRelayConfigurationSchema(
        JsonArray& container
    ) {
        JsonObject& relay_boot_mode = container.createNestedObject();

        relay_boot_mode["name"] = "boot_mode";
        relay_boot_mode["type"] = "select";
        relay_boot_mode["default"] = RELAY_BOOT_MODE;

        JsonArray& relay_boot_mode_values = relay_boot_mode.createNestedArray("values");

        JsonObject& relay_boot_mode_value_0 = relay_boot_mode_values.createNestedObject();
        relay_boot_mode_value_0["value"] = 0;
        relay_boot_mode_value_0["name"] = "always_off";

        JsonObject& relay_boot_mode_value_1 = relay_boot_mode_values.createNestedObject();
        relay_boot_mode_value_1["value"] = 1;
        relay_boot_mode_value_1["name"] = "always_on";

        JsonObject& relay_boot_mode_value_2 = relay_boot_mode_values.createNestedObject();
        relay_boot_mode_value_2["value"] = 2;
        relay_boot_mode_value_2["name"] = "same_before";

        JsonObject& relay_boot_mode_value_3 = relay_boot_mode_values.createNestedObject();
        relay_boot_mode_value_3["value"] = 3;
        relay_boot_mode_value_3["name"] = "toggle_before";

        // -------------------------------------------------------------------------

        JsonObject& relay_pulse_mode = container.createNestedObject();

        relay_pulse_mode["name"] = "pulse_mode";
        relay_pulse_mode["type"] = "select";
        relay_pulse_mode["default"] = RELAY_PULSE_MODE;

        JsonArray& relay_pulse_mode_values = relay_pulse_mode.createNestedArray("values");

        JsonObject& relay_pulse_mode_value_0 = relay_pulse_mode_values.createNestedObject();
        relay_pulse_mode_value_0["value"] = 0;
        relay_pulse_mode_value_0["name"] = "disabled";

        JsonObject& relay_pulse_mode_value_1 = relay_pulse_mode_values.createNestedObject();
        relay_pulse_mode_value_1["value"] = 1;
        relay_pulse_mode_value_1["name"] = "normally_off";

        JsonObject& relay_pulse_mode_value_2 = relay_pulse_mode_values.createNestedObject();
        relay_pulse_mode_value_2["value"] = 2;
        relay_pulse_mode_value_2["name"] = "normally_on";

        // -------------------------------------------------------------------------

        JsonObject& relay_pulse_time = container.createNestedObject();

        relay_pulse_time["name"] = "pulse_time";
        relay_pulse_time["type"] = "number";
        relay_pulse_time["default"] = RELAY_PULSE_TIME;
        relay_pulse_time["min"] = 1;
        relay_pulse_time["max"] = 60;
        relay_pulse_time["step"] = 0.1;

        // -------------------------------------------------------------------------

        JsonObject& relay_on_disconnect = container.createNestedObject();

        relay_on_disconnect["name"] = "on_disconnect";
        relay_on_disconnect["type"] = "select";
        relay_on_disconnect["default"] = RELAY_PULSE_MODE;

        JsonArray& relay_on_disconnect_values = relay_on_disconnect.createNestedArray("values");

        JsonObject& relay_on_disconnect_value_0 = relay_on_disconnect_values.createNestedObject();
        relay_on_disconnect_value_0["value"] = 0;
        relay_on_disconnect_value_0["name"] = "no_change";

        JsonObject& relay_on_disconnect_value_1 = relay_on_disconnect_values.createNestedObject();
        relay_on_disconnect_value_1["value"] = 1;
        relay_on_disconnect_value_1["name"] = "turn_off";

        JsonObject& relay_on_disconnect_value_2 = relay_on_disconnect_values.createNestedObject();
        relay_on_disconnect_value_2["value"] = 2;
        relay_on_disconnect_value_2["name"] = "turn_on";
    }

// -----------------------------------------------------------------------------

    void _relayReportRelayConfiguration(
        const uint8_t id,
        JsonObject& configuration
    ) {
        configuration["boot_mode"] = getSetting("relay_boot_mode", id, RELAY_BOOT_MODE).toInt();
        configuration["pulse_mode"] = getSetting("relay_pulse_mode", id, RELAY_PULSE_MODE).toInt();
        configuration["pulse_time"] = getSetting("relay_pulse_time", id, RELAY_PULSE_TIME).toFloat();
        configuration["on_disconnect"] = getSetting("relay_on_disconnect", id, 0).toInt();
    }

// -----------------------------------------------------------------------------

    bool _relayRelayConfigure(
        const uint8_t id,
        JsonObject& configuration
    ) {
        DEBUG_MSG(PSTR("[INFO][RELAY] Updating channel: %d\n"), id);

        bool is_updated = false;

        if (configuration.containsKey("boot_mode"))  {
            DEBUG_MSG(PSTR("[INFO][RELAY] Setting: \"relay_boot\" to: %d\n"), configuration["boot_mode"].as<uint8_t>());

            setSetting("relay_boot_mode", id, configuration["boot_mode"].as<uint8_t>());

            is_updated = true;
        }

        if (configuration.containsKey("pulse_mode"))  {
            DEBUG_MSG(PSTR("[INFO][RELAY] Setting: \"pulse_mode\" to: %d\n"), configuration["pulse_mode"].as<uint8_t>());

            setSetting("relay_pulse_mode", id, configuration["pulse_mode"].as<uint8_t>());

            is_updated = true;
        }

        if (
            configuration.containsKey("pulse_time")
            && configuration["pulse_time"].as<float>() >= 1.0
            && configuration["pulse_time"].as<float>() <= 60.0
        )  {
            DEBUG_MSG(PSTR("[INFO][RELAY] Setting: \"pulse_time\" to: %d\n"), configuration["pulse_time"].as<uint8_t>());

            setSetting("relay_pulse_time", id, configuration["pulse_time"].as<float>());

            is_updated = true;
        }

        if (configuration.containsKey("on_disconnect"))  {
            DEBUG_MSG(PSTR("[INFO][RELAY] Setting: \"on_disconnect\" to: %d\n"), configuration["on_disconnect"].as<uint8_t>());

            setSetting("relay_on_disconnect", id, configuration["on_disconnect"].as<uint8_t>());

            is_updated = true;
        }

        return is_updated;
    }
#endif // WEB_SUPPORT && WS_SUPPORT

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
        const uint32_t clientId,
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
        SERIAL_PORT.flush();
        SERIAL_PORT.write(0xA0);
        SERIAL_PORT.write(0x04);
        SERIAL_PORT.write(mask);
        SERIAL_PORT.write(0xA1);
        SERIAL_PORT.flush();
    #endif

    #if RELAY_PROVIDER == RELAY_PROVIDER_STM
        SERIAL_PORT.flush();
        SERIAL_PORT.write(0xA0);
        SERIAL_PORT.write(id + 1);
        SERIAL_PORT.write(status);
        SERIAL_PORT.write(0xA1 + status + id);
        SERIAL_PORT.flush();
    #endif

    #if RELAY_PROVIDER == RELAY_PROVIDER_RELAY
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

        // Relay event was fired
        for (uint8_t i = 0; i < _relays[id].callbacks.size(); i++) {
            _relays[id].callbacks[i](target);
        }

        if (!_relay_recursive) {
            relay_pulse_mode(id);

            // We will trigger a commit only if
            // we care about current relay status on boot
            uint8_t boot_mode = getSetting("relay_boot_mode", id, RELAY_BOOT_MODE).toInt();
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

void relay_pulse_mode(
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
        _relays[id].pulse = getSetting("relay_pulse_mode", id, RELAY_PULSE_MODE).toInt();
        _relays[id].pulse_ms = 1000 * getSetting("relay_pulse_time", id, RELAY_PULSE_MODE).toFloat();
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
        relay_pulse_mode(id);

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
        EEPROMr.write(EEPROM_RELAY_STATUS, mask.to_ulong());
        eepromCommit();

        DEBUG_MSG(PSTR("[INFO][RELAY] Comminting relay mask: %d\n"), mask);
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

void relayOnEventRegister(
    relay_on_event_callback_t callback,
    const uint8_t id
) {
    if (id >= _relays.size()) {
        return;
    }

    _relays[id].callbacks.push_back(callback);
}

// -----------------------------------------------------------------------------
// MODULE CORE
// -----------------------------------------------------------------------------

void relaySetup()
{
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

    // Dummy relays for Sonoff Dual and Sonoff RF Bridge
    // No delay_on or off for these devices to easily allow having more than
    // 8 channels. This behaviour will be recovered with v2.
    for (uint8_t i = 0; i < DUMMY_RELAY_COUNT; i++) {
        _relays.push_back((relay_t) {GPIO_NONE, RELAY_TYPE_NORMAL, 0, 0, 0, INDEX_NONE});
    }

    _relayConfigure();
    _relay_boot();

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