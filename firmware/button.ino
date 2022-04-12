/*

BUTTON MODULE

Copyright (C) 2018 FastyBird s.r.o. <code@fastybird.com>

*/

#if BUTTON_SUPPORT

std::vector<button_t> _btn_buttons;

uint32_t _btn_delay = BUTTON_DBL_CLICK_DELAY;

// -----------------------------------------------------------------------------
// MODULE PRIVATE
// -----------------------------------------------------------------------------

uint8_t _buttonMapEvent(
    const uint8_t event,
    const uint8_t count,
    const uint16_t length
) {
    if (event == EVENT_PRESSED) {
        return BUTTON_EVENT_PRESSED;

    } else if (event == EVENT_CHANGED) {
        return BUTTON_EVENT_CLICK;

    } else if (event == EVENT_RELEASED) {
        if (count == 1) {
            if (length > BUTTON_LNG_LNG_CLICK_DELAY) {
                return BUTTON_EVENT_LNG_LNG_CLICK;

            } else if (length > BUTTON_LNG_CLICK_DELAY) {
                return BUTTON_EVENT_LNG_CLICK;
            }

            return BUTTON_EVENT_CLICK;

        } else if (count == 2) {
            return BUTTON_EVENT_DBL_CLICK;

        } else if (count == 3) {
            return BUTTON_EVENT_TRIPLE_CLICK;
        }
    }

    return BUTTON_EVENT_NONE;
}

// -----------------------------------------------------------------------------

#if WEB_SUPPORT && WS_SUPPORT
    /**
     * Provide module configuration schema
     */
    void _buttonReportConfigurationSchema(
        JsonArray& configuration
    ) {
        // Configuration field
        JsonObject& delay = configuration.createNestedObject();

        delay["name"] = "delay";
        delay["type"] = "number";
        delay["min"] = BUTTON_DEBOUNCE_DBL_CLICK_MIN;
        delay["max"] = BUTTON_DEBOUNCE_DBL_CLICK_MAX;
        delay["step"] = BUTTON_DEBOUNCE_DBL_CLICK_STEP;
        delay["default"] = BUTTON_DBL_CLICK_DELAY;
    }

// -----------------------------------------------------------------------------

    /**
     * Report module configuration
     */
    void _buttonReportConfiguration(
        JsonObject& configuration
    ) {
        configuration["delay"] = _btn_delay;
    }

// -----------------------------------------------------------------------------

    /**
     * Update module configuration via WS or MQTT etc.
     */
    bool _buttonUpdateConfiguration(
        JsonObject& configuration
    ) {
        DEBUG_MSG(PSTR("[INFO][BUTTON] Updating module\n"));

        bool is_updated = false;

        if (
            configuration.containsKey("delay")
            && configuration["delay"].as<uint32_t>() >= BUTTON_DEBOUNCE_DBL_CLICK_MIN
            && configuration["delay"].as<uint32_t>() <= BUTTON_DEBOUNCE_DBL_CLICK_MAX
            && configuration["delay"].as<uint32_t>() != _btn_delay
        )  {
            DEBUG_MSG(PSTR("[INFO][BUTTON] Setting: \"btn_delay\" to: %d\n"), configuration["delay"].as<uint32_t>());

            setSetting("btn_delay", configuration["delay"].as<uint32_t>());

            _btn_delay = configuration["delay"].as<uint32_t>();

            is_updated = true;
        }

        return is_updated;
    }

// -----------------------------------------------------------------------------

    // New WS client is connected
    void _buttonWSOnConnect(
        JsonObject& root
    ) {
        JsonArray& modules = root.containsKey("modules") ? root["modules"] : root.createNestedArray("modules");
        JsonObject& module = modules.createNestedObject();

        module["module"] = "btn";
        module["visible"] = true;

        // Configuration container
        JsonObject& configuration = module.createNestedObject("config");

        // Configuration values container
        JsonObject& configuration_values = configuration.createNestedObject("values");

        _buttonReportConfiguration(configuration_values);

        // Configuration schema container
        JsonArray& configuration_schema = configuration.createNestedArray("schema");

        _buttonReportConfigurationSchema(configuration_schema);
    }

// -----------------------------------------------------------------------------

    // WS client requested configuration update
    void _buttonWSOnConfigure(
        const uint32_t clientId, 
        JsonObject& module
    ) {
        if (module.containsKey("module") && module["module"] == "btn") {
            if (module.containsKey("config")) {
                // Extract configuration container
                JsonObject& configuration = module["config"].as<JsonObject&>();

                if (
                    configuration.containsKey("values")
                    && _buttonUpdateConfiguration(configuration["values"])
                ) {
                    wsSend_P(clientId, PSTR("{\"message\": \"btn_updated\"}"));

                    // Reload & cache settings
                    firmwareReload();
                }
            }
        }
    }
#endif // WEB_SUPPORT && WS_SUPPORT

// -----------------------------------------------------------------------------

void _buttonEvent(
    const uint8_t id,
    const uint8_t event
) {
    if (id >= _btn_buttons.size() || event == 0) {
        return;
    }

    // Button event was fired
    for (uint8_t i = 0; i < _btn_buttons[id].callbacks.size(); i++) {
        _btn_buttons[id].callbacks[i](event);
    }
}

// -----------------------------------------------------------------------------
// MODULE API
// -----------------------------------------------------------------------------

void buttonOnEventRegister(
    button_on_event_callback_t callback,
    const uint8_t id
) {
    if (id >= _btn_buttons.size()) {
        return;
    }

    _btn_buttons[id].callbacks.push_back(callback);
}

// -----------------------------------------------------------------------------
// MODULE CORE
// -----------------------------------------------------------------------------

void buttonSetup()
{
    _btn_delay = getSetting("btn_delay", BUTTON_DBL_CLICK_DELAY).toInt();

    #if BUTTON1_PIN != GPIO_NONE
    {
        _btn_buttons.push_back({new DebounceEvent(BUTTON1_PIN, BUTTON1_MODE, BUTTON_DEBOUNCE_DELAY, _btn_delay)});
    }
    #endif

    #if BUTTON2_PIN != GPIO_NONE
    {
        _btn_buttons.push_back({new DebounceEvent(BUTTON2_PIN, BUTTON2_MODE, BUTTON_DEBOUNCE_DELAY, _btn_delay)});
    }
    #endif

    #if BUTTON3_PIN != GPIO_NONE
    {
        _btn_buttons.push_back({new DebounceEvent(BUTTON3_PIN, BUTTON3_MODE, BUTTON_DEBOUNCE_DELAY, _btn_delay)});
    }
    #endif

    #if BUTTON4_PIN != GPIO_NONE
    {
        _btn_buttons.push_back({new DebounceEvent(BUTTON4_PIN, BUTTON4_MODE, BUTTON_DEBOUNCE_DELAY, _btn_delay)});
    }
    #endif

    #if BUTTON5_PIN != GPIO_NONE
    {
        _btn_buttons.push_back({new DebounceEvent(BUTTON5_PIN, BUTTON5_MODE, BUTTON_DEBOUNCE_DELAY, _btn_delay)});
    }
    #endif

    #if BUTTON6_PIN != GPIO_NONE
    {
        _btn_buttons.push_back({new DebounceEvent(BUTTON6_PIN, BUTTON6_MODE, BUTTON_DEBOUNCE_DELAY, _btn_delay)});
    }
    #endif

    #if BUTTON7_PIN != GPIO_NONE
    {
        _btn_buttons.push_back({new DebounceEvent(BUTTON7_PIN, BUTTON7_MODE, BUTTON_DEBOUNCE_DELAY, _btn_delay)});
    }
    #endif

    #if BUTTON8_PIN != GPIO_NONE
    {
        _btn_buttons.push_back({new DebounceEvent(BUTTON8_PIN, BUTTON8_MODE, BUTTON_DEBOUNCE_DELAY, _btn_delay)});
    }
    #endif

    DEBUG_MSG(PSTR("[INFO][BUTTON] Number of buttons: %u\n"), _btn_buttons.size());

    #if WEB_SUPPORT && WS_SUPPORT
        wsOnConnectRegister(_buttonWSOnConnect);
        wsOnConfigureRegister(_buttonWSOnConfigure);
    #endif

    // Register loop
    firmwareRegisterLoop(buttonLoop);
}

// -----------------------------------------------------------------------------

void buttonLoop()
{
    for (uint8_t i = 0; i < _btn_buttons.size(); i++) {
        if (uint8_t event = _btn_buttons[i].button->loop()) {
            uint8_t count = _btn_buttons[i].button->getEventCount();
            uint32_t length = _btn_buttons[i].button->getEventLength();

            uint8_t mapped = _buttonMapEvent(event, count, length);

            _buttonEvent(i, mapped);
        }
    }
}

#endif // BUTTON_SUPPORT
