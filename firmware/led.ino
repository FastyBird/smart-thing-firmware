/*

LED MODULE

Copyright (C) 2018 FastyBird s.r.o. <code@fastybird.com>

*/

#if LED_SUPPORT

uint8_t _led_mode = STATUS_LED_MODE;

// -----------------------------------------------------------------------------
// MODULE PRIVATE
// -----------------------------------------------------------------------------

/**
 * Get status of given LED
 */
bool _ledStatus()
{
    bool status = digitalRead(STATUS_LED_PIN);

    return STATUS_LED_PIN_INVERSE ? !status : status;
}

// -----------------------------------------------------------------------------

/**
 * Set status of given LED
 */
bool _ledStatus(
    const bool status
) {
    digitalWrite(STATUS_LED_PIN, STATUS_LED_PIN_INVERSE ? !status : status);

    return status;
}

// -----------------------------------------------------------------------------

/**
 * Toggle status of given LED
 */
bool _ledToggle()
{
    return _ledStatus(!_ledStatus());
}

// -----------------------------------------------------------------------------

void _ledBlink(
    const uint32_t delayOff,
    const uint32_t delayOn
) {
    static uint32_t next = millis();

    if (next < millis()) {
        next += (_ledToggle() ? delayOn : delayOff);
    }
}


// -----------------------------------------------------------------------------

#if WEB_SUPPORT && WS_SUPPORT
    /**
     * Provide module configuration schema
     */
    void _ledReportConfigurationSchema(
        JsonArray& configuration
    ) {
        JsonObject& mode = configuration.createNestedObject();

        mode["name"] = "led_mode";
        mode["type"] = "select";
        #if WIFI_SUPPORT
        mode["default"] = LED_MODE_WIFI;
        #else
        mode["default"] = LED_MODE_OFF;
        #endif

        JsonArray& modeValues = mode.createNestedArray("values");

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

    /**
     * Report module configuration
     */
    void _ledReportConfiguration(
        JsonObject& configuration
    ) {
        configuration["led_mode"] = _led_mode;
    }

// -----------------------------------------------------------------------------

    /**
     * Update module configuration via WS or MQTT etc.
     */
    bool _ledUpdateConfiguration(
        JsonObject& configuration
    ) {
        DEBUG_MSG(PSTR("[INFO][LED] Updating module\n"));

        bool is_updated = false;

        if (
            configuration.containsKey("led_mode")
            && (
                configuration["led_mode"].as<uint8_t>() == LED_MODE_WIFI
                || configuration["led_mode"].as<uint8_t>() == LED_MODE_ON
                || configuration["led_mode"].as<uint8_t>() == LED_MODE_OFF
            )
            && configuration["led_mode"].as<uint8_t>() != _led_mode
        )  {
            DEBUG_MSG(PSTR("[INFO][LED] Setting: \"led_mode\" to: %d\n"), configuration["led_mode"].as<uint8_t>());

            setSetting("ledMode", configuration["led_mode"].as<uint8_t>());

            _led_mode = configuration["led_mode"].as<uint8_t>();

            is_updated = true;
        }

        return is_updated;
    }

#endif // WEB_SUPPORT && WS_SUPPORT

// -----------------------------------------------------------------------------

#if WEB_SUPPORT && WS_SUPPORT
    // New WS client is connected
    void _ledWSOnConnect(
        JsonObject& root
    ) {
        DynamicJsonBuffer jsonBuffer;

        JsonArray& modules = root.containsKey("modules") ? root["modules"] : root.createNestedArray("modules");
        JsonObject& module = modules.createNestedObject();

        module["module"] = "led";
        module["visible"] = true;

        // Configuration container
        JsonObject& configuration = module.createNestedObject("config");

        // Configuration values container
        JsonObject& configuration_values = configuration.createNestedObject("values");

        _ledReportConfiguration(configuration_values);

        // Configuration schema container
        JsonArray& configuration_schema = configuration.createNestedArray("schema");

        _ledReportConfigurationSchema(configuration_schema);
    }

// -----------------------------------------------------------------------------

    // WS client requested configuration update
    void _ledWSOnConfigure(
        const uint32_t clientId, 
        JsonObject& module
    ) {
        if (module.containsKey("module") && module["module"] == "led") {
            if (module.containsKey("config")) {
                // Extract configuration container
                JsonObject& configuration = module["config"].as<JsonObject&>();

                if (
                    configuration.containsKey("values")
                    && _ledUpdateConfiguration(configuration["values"])
                ) {
                    wsSend_P(clientId, PSTR("{\"message\": \"led_updated\"}"));

                    // Reload & cache settings
                    firmwareReload();
                }
            }
        }
    }
#endif

// -----------------------------------------------------------------------------

/**
 * Initialize all configured LEDs
 */
void _ledInitialize()
{
    _led_mode = getSetting("ledMode", STATUS_LED_MODE).toInt();
}

// -----------------------------------------------------------------------------
// MODULE CORE
// -----------------------------------------------------------------------------

void ledSetup()
{
    if (!hasSetting("ledMode")) {
        setSetting("ledMode", STATUS_LED_MODE);
    }

    pinMode(STATUS_LED_PIN, OUTPUT);

    _ledStatus(false);

    _ledInitialize();

    #if WEB_SUPPORT && WS_SUPPORT
        wsOnConnectRegister(_ledWSOnConnect);
        wsOnConfigureRegister(_ledWSOnConfigure);
    #endif

    // Register firmware callbacks
    firmwareRegisterLoop(ledLoop);
    firmwareRegisterReload(_ledInitialize);
}

// -----------------------------------------------------------------------------

void ledLoop()
{
    #if WIFI_SUPPORT
        if (_led_mode == LED_MODE_WIFI) {
            if (wifiState() & WIFI_STATE_STA) {
                _ledBlink(4900, 100);

            } else if (wifiState() & WIFI_STATE_AP) {
                _ledBlink(900, 100);

            } else {
                _ledBlink(500, 500);
            }
        }
    #endif

    if (_led_mode == LED_MODE_ON) {
        _ledStatus(true);
    }

    if (_led_mode == LED_MODE_OFF) {
        _ledStatus(false);
    }
}

#endif // LED_SUPPORT
