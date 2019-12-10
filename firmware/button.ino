/*

BUTTON MODULE

Copyright (C) 2018 FastyBird s.r.o. <info@fastybird.com>

*/

#if BUTTON_SUPPORT

#include <vector>

typedef struct {
    DebounceEvent * button;
    uint8_t channel;
    std::vector<button_on_event_callback_f> callbacks;
} button_t;

std::vector<button_t> _buttons;

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
            if (length > BUTTON_LNGLNGCLICK_DELAY) {
                return BUTTON_EVENT_LNGLNGCLICK;

            } else if (length > BUTTON_LNGCLICK_DELAY) {
                return BUTTON_EVENT_LNGCLICK;
            }

            return BUTTON_EVENT_CLICK;

        } else if (count == 2) {
            return BUTTON_EVENT_DBLCLICK;

        } else if (count == 3) {
            return BUTTON_EVENT_TRIPLECLICK;
        }
    }

    return BUTTON_EVENT_NONE;
}

// -----------------------------------------------------------------------------

#if FASTYBIRD_SUPPORT || (WEB_SUPPORT && WS_SUPPORT)
    /**
     * Provide module configuration schema
     */
    void _buttonReportConfigurationSchema(
        JsonArray& configuration
    ) {
        // Configuration field
        JsonObject& delay = configuration.createNestedObject();

        delay["name"] = "btn_delay";
        delay["type"] = "number";
        delay["min"] = BUTTON_DEBOUNCE_DBLCLICK_MIN;
        delay["max"] = BUTTON_DEBOUNCE_DBLCLICK_MAX;
        delay["step"] = BUTTON_DEBOUNCE_DBLCLICK_STEP;
        delay["default"] = BUTTON_DBLCLICK_DELAY;
    }

// -----------------------------------------------------------------------------

    /**
     * Report module configuration
     */
    void _buttonReportConfiguration(
        JsonObject& configuration
    ) {
        configuration["btn_delay"] = getSetting("btnDelay", BUTTON_DBLCLICK_DELAY).toInt();
    }

// -----------------------------------------------------------------------------

    /**
     * Update module configuration via WS or MQTT etc.
     */
    bool _buttonUpdateConfiguration(
        JsonObject& configuration
    ) {
        DEBUG_MSG(PSTR("[BUTTON] Updating module\n"));

        bool is_updated = false;

        if (
            configuration.containsKey("btn_delay")
            && configuration["btn_delay"].as<uint16_t>() >= BUTTON_DEBOUNCE_DBLCLICK_MIN
            && configuration["btn_delay"].as<uint16_t>() <= BUTTON_DEBOUNCE_DBLCLICK_MAX
            && configuration["btn_delay"].as<uint16_t>() != getSetting("btnDelay").toInt()
        )  {
            DEBUG_MSG(PSTR("[BUTTON] Setting: \"btn_delay\" to: %d\n"), configuration["btn_delay"].as<uint16_t>());

            setSetting("btnDelay", configuration["btn_delay"].as<uint16_t>());

            is_updated = true;
        }

        return is_updated;
    }
#endif // FASTYBIRD_SUPPORT || (WEB_SUPPORT && WS_SUPPORT)

// -----------------------------------------------------------------------------

#if FASTYBIRD_SUPPORT
    void _buttonFastybirdRegister() {
        // Create button property structure
        fastybird_channel_property_t property = {
            FASTYBIRD_PROPERTY_BUTTON,
            FASTYBIRD_PROPERTY_BUTTON,
            false,
            false,
            FASTYBIRD_PROPERTY_DATA_TYPE_ENUM,
            "",
        };

        char format[100];

        strcpy(format, FASTYBIRD_BTN_PAYLOAD_PRESS);
        strcat(format, ",");
        strcat(format, FASTYBIRD_BTN_PAYLOAD_CLICK);
        strcat(format, ",");
        strcat(format, FASTYBIRD_BTN_PAYLOAD_DBL_CLICK);
        strcat(format, ",");
        strcat(format, FASTYBIRD_BTN_PAYLOAD_TRIPLE_CLICK);
        strcat(format, ",");
        strcat(format, FASTYBIRD_BTN_PAYLOAD_LNG_CLICK);
        strcat(format, ",");
        strcat(format, FASTYBIRD_BTN_PAYLOAD_LNG_LNG_CLICK);

        property.format = String(format);

        // Register channel property into fastybird
        uint8_t propertyIndex = fastybirdRegisterChannelProperty(property);

        // Process all buttons
        for (uint8_t i = 0; i < _buttons.size(); i++) {
            if (_buttons[i].channel != INDEX_NONE) {
                fastybirdRegisterPropertyToChannel(_buttons[i].channel, propertyIndex);
            }
        }
    }
#endif // FASTYBIRD_SUPPORT

// -----------------------------------------------------------------------------

#if WEB_SUPPORT && WS_SUPPORT
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
    if (id >= _buttons.size() || event == 0) {
        return;
    }

    // Button event was fired
    for (uint8_t i = 0; i < _buttons[id].callbacks.size(); i++) {
        _buttons[id].callbacks[i](event);
    }

    #if FASTYBIRD_SUPPORT
        switch (event) {
            case BUTTON_EVENT_PRESSED:
                fastybirdReportChannelPropertyValue(
                    _buttons[id].channel,
                    FASTYBIRD_PROPERTY_BUTTON,
                    FASTYBIRD_BTN_PAYLOAD_PRESS
                );
                break;

            case BUTTON_EVENT_CLICK:
                fastybirdReportChannelPropertyValue(
                    _buttons[id].channel,
                    FASTYBIRD_PROPERTY_BUTTON,
                    FASTYBIRD_BTN_PAYLOAD_CLICK
                );
                break;

            case BUTTON_EVENT_DBLCLICK:
                fastybirdReportChannelPropertyValue(
                    _buttons[id].channel,
                    FASTYBIRD_PROPERTY_BUTTON,
                    FASTYBIRD_BTN_PAYLOAD_DBL_CLICK
                );
                break;

            case BUTTON_EVENT_TRIPLECLICK:
                fastybirdReportChannelPropertyValue(
                    _buttons[id].channel,
                    FASTYBIRD_PROPERTY_BUTTON,
                    FASTYBIRD_BTN_PAYLOAD_TRIPLE_CLICK
                );
                break;

            case BUTTON_EVENT_LNGCLICK:
                fastybirdReportChannelPropertyValue(
                    _buttons[id].channel,
                    FASTYBIRD_PROPERTY_BUTTON,
                    FASTYBIRD_BTN_PAYLOAD_LNG_CLICK
                );
                break;

            case BUTTON_EVENT_LNGLNGCLICK:
                fastybirdReportChannelPropertyValue(
                    _buttons[id].channel,
                    FASTYBIRD_PROPERTY_BUTTON,
                    FASTYBIRD_BTN_PAYLOAD_LNG_LNG_CLICK
                );
                break;

            default:
                break;
        }
    #endif
}

// -----------------------------------------------------------------------------
// MODULE API
// -----------------------------------------------------------------------------

void buttonOnEventRegister(
    button_on_event_callback_f callback,
    const uint8_t id
) {
    if (id >= _buttons.size()) {
        return;
    }

    _buttons[id].callbacks.push_back(callback);
}

// -----------------------------------------------------------------------------
// MODULE CORE
// -----------------------------------------------------------------------------

void buttonSetup() {
    uint32_t btn_delay = getSetting("btnDelay", BUTTON_DBLCLICK_DELAY).toInt();

    #if BUTTON1_PIN != GPIO_NONE
    {
        _buttons.push_back({new DebounceEvent(BUTTON1_PIN, BUTTON1_MODE, BUTTON_DEBOUNCE_DELAY, btn_delay), BUTTON1_CHANNEL});
    }
    #endif

    #if BUTTON2_PIN != GPIO_NONE
    {
        _buttons.push_back({new DebounceEvent(BUTTON2_PIN, BUTTON2_MODE, BUTTON_DEBOUNCE_DELAY, btn_delay), BUTTON2_CHANNEL});
    }
    #endif

    #if BUTTON3_PIN != GPIO_NONE
    {
        _buttons.push_back({new DebounceEvent(BUTTON3_PIN, BUTTON3_MODE, BUTTON_DEBOUNCE_DELAY, btn_delay), BUTTON3_CHANNEL});
    }
    #endif

    #if BUTTON4_PIN != GPIO_NONE
    {
        _buttons.push_back({new DebounceEvent(BUTTON4_PIN, BUTTON4_MODE, BUTTON_DEBOUNCE_DELAY, btn_delay), BUTTON4_CHANNEL});
    }
    #endif

    #if BUTTON5_PIN != GPIO_NONE
    {
        _buttons.push_back({new DebounceEvent(BUTTON5_PIN, BUTTON5_MODE, BUTTON_DEBOUNCE_DELAY, btn_delay), BUTTON5_CHANNEL});
    }
    #endif

    #if BUTTON6_PIN != GPIO_NONE
    {
        _buttons.push_back({new DebounceEvent(BUTTON6_PIN, BUTTON6_MODE, BUTTON_DEBOUNCE_DELAY, btn_delay), BUTTON6_CHANNEL});
    }
    #endif

    #if BUTTON7_PIN != GPIO_NONE
    {
        _buttons.push_back({new DebounceEvent(BUTTON7_PIN, BUTTON7_MODE, BUTTON_DEBOUNCE_DELAY, btn_delay), BUTTON7_CHANNEL});
    }
    #endif

    #if BUTTON8_PIN != GPIO_NONE
    {
        _buttons.push_back({new DebounceEvent(BUTTON8_PIN, BUTTON8_MODE, BUTTON_DEBOUNCE_DELAY, btn_delay), BUTTON8_CHANNEL});
    }
    #endif

    DEBUG_MSG(PSTR("[BUTTON] Number of buttons: %u\n"), _buttons.size());

    #if WEB_SUPPORT && WS_SUPPORT
        wsOnConnectRegister(_buttonWSOnConnect);
        wsOnConfigureRegister(_buttonWSOnConfigure);
    #endif

    #if FASTYBIRD_SUPPORT
        // Module schema report
        fastybirdReportConfigurationSchemaRegister(_buttonReportConfigurationSchema);
        fastybirdReportConfigurationRegister(_buttonReportConfiguration);
        fastybirdOnConfigureRegister(_buttonUpdateConfiguration);

        // Channels registration
        if (_buttons.size() > 0) {
            _buttonFastybirdRegister();
        }
    #endif

    // Register loop
    firmwareRegisterLoop(buttonLoop);
}

// -----------------------------------------------------------------------------

void buttonLoop() {
    for (uint8_t i = 0; i < _buttons.size(); i++) {
        if (uint8_t event = _buttons[i].button->loop()) {
            uint8_t count = _buttons[i].button->getEventCount();
            uint32_t length = _buttons[i].button->getEventLength();

            uint8_t mapped = _buttonMapEvent(event, count, length);

            _buttonEvent(i, mapped);
        }
    }
}

#endif // BUTTON_SUPPORT
