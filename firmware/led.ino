/*

LED MODULE

Copyright (C) 2018 FastyBird Ltd. <info@fastybird.com>

*/

#if LED_SUPPORT

typedef struct {
    uint8_t pin;
    bool reverse;
    uint8_t mode;
} led_t;

std::vector<led_t> _leds;

// -----------------------------------------------------------------------------
// MODULE PRIVATE
// -----------------------------------------------------------------------------

/**
 * Get status of given LED
 */
bool _ledStatus(
    const uint8_t id
) {
    if (id >= ledCount()) {
        return false;
    }

    bool status = digitalRead(_leds[id].pin);

    return _leds[id].reverse ? !status : status;
}

// -----------------------------------------------------------------------------

/**
 * Set status of given LED
 */
bool _ledStatus(
    const uint8_t id,
    const bool status
) {
    if (id >=ledCount()) {
        return false;
    }

    digitalWrite(_leds[id].pin, _leds[id].reverse ? !status : status);

    return status;
}

// -----------------------------------------------------------------------------

/**
 * Toggle status of given LED
 */
bool _ledToggle(
    const uint8_t id
) {
    if (id >= ledCount()) {
        return false;
    }

    return _ledStatus(id, !_ledStatus(id));
}

// -----------------------------------------------------------------------------

/**
 * Get mode of given LED
 */
uint8_t _ledMode(
    const uint8_t id
) {
    if (id >= ledCount()) {
        return false;
    }

    return _leds[id].mode;
}

// -----------------------------------------------------------------------------

/**
 * Set mode of given LED
 */
void _ledMode(
    const uint8_t id,
    const uint8_t mode
) {
    if (id >= ledCount()) {
        return;
    }

    _leds[id].mode = mode;
}

// -----------------------------------------------------------------------------

void _ledBlink(
    const uint8_t id,
    const uint32_t delayOff,
    const uint32_t delayOn
) {
    if (id >= ledCount()) {
        return;
    }

    static uint32_t next = millis();

    if (next < millis()) {
        next += (_ledToggle(id) ? delayOn : delayOff);
    }
}

// -----------------------------------------------------------------------------

/**
 * Initialize all configured LEDs
 */
void _ledInitialize() {
    for (uint8_t i = 0; i < ledCount(); i++) {
        _ledMode(i, getSetting("ledMode", i, _ledMode(i)).toInt());
    }
}

// -----------------------------------------------------------------------------

void _ledReportChannelConfigurationSchema(
    JsonArray& container
) {
    JsonObject& mode = container.createNestedObject();

    mode["name"] = "led_mode";
    mode["type"] = "select";
    #if WIFI_SUPPORT
    mode["default"] = LED_MODE_WIFI;
    #else
    mode["default"] = LED_MODE_OFF;
    #endif

    JsonArray& modeValues = mode.createNestedArray("values");

    JsonObject& value0 = modeValues.createNestedObject();
    value0["value"] = LED_MODE_MQTT;
    value0["name"] = "mqtt_managed";

    #if WIFI_SUPPORT
        JsonObject& value1 = modeValues.createNestedObject();
        value1["value"] = LED_MODE_WIFI;
        value1["name"] = "wifi_status";
    #endif

    JsonObject& value2 = modeValues.createNestedObject();
    value2["value"] = LED_MODE_ON;
    value2["name"] = "always_on";

    JsonObject& value3 = modeValues.createNestedObject();
    value3["value"] = LED_MODE_OFF;
    value3["name"] = "always_off";
}

// -----------------------------------------------------------------------------

String _ledReportChannelConfigurationSchema() {
    DynamicJsonBuffer jsonBuffer;
    JsonArray& schema = jsonBuffer.createArray();

    _ledReportChannelConfigurationSchema(schema);

    String output;

    schema.printTo(output);

    return output;
}

// -----------------------------------------------------------------------------

void _ledReportChannelConfiguration(
    const uint8_t id,
    JsonObject& configuration
) {
    configuration["led_mode"] = _ledMode(id);
}

// -----------------------------------------------------------------------------

String _ledReportChannelConfiguration(
    const uint8_t id
) {
    DynamicJsonBuffer jsonBuffer;
    JsonObject& configuration = jsonBuffer.createObject();

    _ledReportChannelConfiguration(id, configuration);

    String output;

    configuration.printTo(output);

    return output;
}

// -----------------------------------------------------------------------------

// Update module channel configuration via WS or MQTT etc.
void _ledConfigureChannel(
    const uint8_t id,
    JsonObject& configuration
) {
    if (configuration.containsKey("led_mode"))  {
        setSetting("ledMode", id, configuration["led_mode"].as<uint8_t>());

        _ledMode(id, configuration["led_mode"].as<uint8_t>());
    }
}

// -----------------------------------------------------------------------------

#if WEB_SUPPORT && WS_SUPPORT
    // New WS client is connected
    void _ledWSOnConnect(
        JsonObject& root
    ) {
        if (ledCount() == 0) {
            return;
        }

        DynamicJsonBuffer jsonBuffer;

        JsonArray& modules = root.containsKey("modules") ? root["modules"] : root.createNestedArray("modules");
        JsonObject& module = modules.createNestedObject();

        module["module"] = "led";
        module["visible"] = true;

        // Configuration container
        JsonObject& configuration = module.createNestedObject("config");

        // Channels configuration
        JsonObject& channels_configuration = configuration.createNestedObject("channels");

        JsonArray& channels_configuration_schema = channels_configuration.createNestedArray("schema");

        _ledReportChannelConfigurationSchema(channels_configuration_schema);

        JsonArray& channels_configuration_values = channels_configuration.createNestedArray("values");

        for (uint8_t i = 0; i < ledCount(); i++) {
            JsonObject& channel_configuration_values = channels_configuration_values.createNestedObject();

            _ledReportChannelConfiguration(i, channel_configuration_values);
        }

        // Data container
        JsonObject& data = module.createNestedObject("data");

        // Channels statuses
        JsonArray& channels_statuses = data.createNestedArray("channels");

        for (uint8_t i = 0; i < ledCount(); i++) {
            channels_statuses.add(_ledStatus(i));
        }
    }

// -----------------------------------------------------------------------------

    // WS client requested configuration update
    void _ledWSOnConfigure(
        const uint32_t clientId,
        JsonObject& root
    ) {
        if (ledCount() == 0) {
            delSetting("ledMode");

            return;
        }

        if (root.containsKey("module") && root["module"] == "led") {
            if (root.containsKey("config")) {
                JsonObject& configuration = root["config"];

                if (configuration.containsKey("channels")) {
                    for (uint8_t i = 0; i < ledCount(); i++) {
                        _ledConfigureChannel(i, configuration["channels"][i]);
                    }
                }

                // Send message
                wsSend_P(clientId, PSTR("{\"message\": \"led_updated\"}"));

                // Reload & cache settings
                firmwareReload();
            }
        }
    }
#endif

// -----------------------------------------------------------------------------
// MODULE API
// -----------------------------------------------------------------------------

uint8_t ledCount() {
    return _leds.size();
}

// -----------------------------------------------------------------------------
// MODULE CORE
// -----------------------------------------------------------------------------

void ledSetup() {
    #if LED1_PIN != GPIO_NONE
        _leds.push_back((led_t) { LED1_PIN, LED1_PIN_INVERSE, LED1_MODE });
    #endif

    #if LED2_PIN != GPIO_NONE
        _leds.push_back((led_t) { LED2_PIN, LED2_PIN_INVERSE, LED2_MODE });
    #endif

    #if LED3_PIN != GPIO_NONE
        _leds.push_back((led_t) { LED3_PIN, LED3_PIN_INVERSE, LED3_MODE });
    #endif

    #if LED4_PIN != GPIO_NONE
        _leds.push_back((led_t) { LED4_PIN, LED4_PIN_INVERSE, LED4_MODE });
    #endif

    #if LED5_PIN != GPIO_NONE
        _leds.push_back((led_t) { LED5_PIN, LED5_PIN_INVERSE, LED5_MODE });
    #endif

    #if LED6_PIN != GPIO_NONE
        _leds.push_back((led_t) { LED6_PIN, LED6_PIN_INVERSE, LED6_MODE });
    #endif

    #if LED7_PIN != GPIO_NONE
        _leds.push_back((led_t) { LED7_PIN, LED7_PIN_INVERSE, LED7_MODE });
    #endif

    #if LED8_PIN != GPIO_NONE
        _leds.push_back((led_t) { LED8_PIN, LED8_PIN_INVERSE, LED8_MODE });
    #endif

    for (uint8_t i = 0; i < ledCount(); i++) {
        pinMode(_leds[i].pin, OUTPUT);

        _ledStatus(i, false);
    }

    _ledInitialize();

    #if WEB_SUPPORT && WS_SUPPORT
        wsOnConnectRegister(_ledWSOnConnect);
        wsOnConfigureRegister(_ledWSOnConfigure);
    #endif

    #if FASTYBIRD_SUPPORT && LED1_PIN != GPIO_NONE
        fastybirdOnControlRegister(
            [](const char * payload) {
                DEBUG_MSG(PSTR("[LED] Controling status LED\n"));

                if (strcmp(payload, FASTYBIRD_LED_PAYLOAD_ON) == 0) {
                    _ledMode(0, LED_MODE_ON);

                } else if (strcmp(payload, FASTYBIRD_LED_PAYLOAD_OFF) == 0) {
                    _ledMode(0, LED_MODE_OFF);

                } else if (strcmp(payload, FASTYBIRD_LED_PAYLOAD_RESTORE) == 0) {
                    _ledMode(0, LED_MODE_WIFI);
                }  
            },
            "status-led"
        );
    #endif

    DEBUG_MSG(PSTR("[LED] Number of leds: %d\n"), ledCount());

    // Register firmware callbacks
    firmwareRegisterLoop(ledLoop);
    firmwareRegisterReload(_ledInitialize);
}

// -----------------------------------------------------------------------------

void ledLoop() {
    for (uint8_t i = 0; i < ledCount(); i++) {
        #if WIFI_SUPPORT
            if (_ledMode(i) == LED_MODE_WIFI) {
                if (wifiState() & WIFI_STATE_STA) {
                    _ledBlink(i, 4900, 100);

                } else if (wifiState() & WIFI_STATE_AP) {
                    _ledBlink(i, 900, 100);

                } else {
                    _ledBlink(i, 500, 500);
                }
            }
        #endif

        if (_ledMode(i) == LED_MODE_ON) {
            _ledStatus(i, true);
        }

        if (_ledMode(i) == LED_MODE_OFF) {
            _ledStatus(i, false);
        }
    }
}

#endif // LED_SUPPORT
