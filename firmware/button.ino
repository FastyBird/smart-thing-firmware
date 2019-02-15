/*

BUTTON MODULE

Copyright (C) 2018 FastyBird Ltd. <info@fastybird.com>

*/

#if BUTTON_SUPPORT

#include <vector>

typedef struct {
    DebounceEvent * button;
    std::vector<button_on_event_callback_f> callbacks;
} button_t;

std::vector<button_t> _buttons;

// -----------------------------------------------------------------------------
// MODULE PRIVATE
// -----------------------------------------------------------------------------

uint8_t _buttonMapEvent(
    uint8_t event,
    uint8_t count,
    uint16_t length
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
    delay["min"] = 0;
    delay["max"] = 1000;
    delay["step"] = 100;
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
void _buttonUpdateConfiguration(
    JsonObject& configuration
) {
    if (configuration.containsKey("btn_delay"))  {
        setSetting("btnDelay", configuration["btn_delay"].as<unsigned int>());
    }
}

// -----------------------------------------------------------------------------

#if FASTYBIRD_SUPPORT
    fastybird_channel_property_t _buttonFastybirdGetChannelStatePropertyStructure() {
        fastybird_channel_property_t property = {
            FASTYBIRD_PROPERTY_STATE,
            "Button state",
            false,
            FASTYBIRD_PROPERTY_DATA_TYPE_ENUM,
        };

        property.format.push_back(String(BUTTON_EVENT_PRESSED).c_str());
        property.format.push_back(String(BUTTON_EVENT_CLICK).c_str());
        property.format.push_back(String(BUTTON_EVENT_DBLCLICK).c_str());
        property.format.push_back(String(BUTTON_EVENT_TRIPLECLICK).c_str());
        property.format.push_back(String(BUTTON_EVENT_LNGCLICK).c_str());
        property.format.push_back(String(BUTTON_EVENT_LNGLNGCLICK).c_str());

        property.mappings.push_back({
            "1",
            FASTYBIRD_BTN_PAYLOAD_PRESS
        });

        property.mappings.push_back({
            "2",
            FASTYBIRD_BTN_PAYLOAD_CLICK
        });

        property.mappings.push_back({
            "3",
            FASTYBIRD_BTN_PAYLOAD_DBL_CLICK
        });

        property.mappings.push_back({
            "6",
            FASTYBIRD_BTN_PAYLOAD_TRIPLE_CLICK
        });

        property.mappings.push_back({
            "4",
            FASTYBIRD_BTN_PAYLOAD_LNG_CLICK
        });

        property.mappings.push_back({
            "5",
            FASTYBIRD_BTN_PAYLOAD_LNG_LNG_CLICK
        });

        return property;
    }

// -----------------------------------------------------------------------------

    fastybird_channel_t _buttonFastybirdGetChannelStructure() {
        fastybird_channel_t channel = {
            "Buttons",
            FASTYBIRD_CHANNEL_TYPE_BUTTON,
            buttonCount(),
            false,
            false,
            false
        };

        channel.properties.push_back(_buttonFastybirdGetChannelStatePropertyStructure());

        return channel;
    }
#endif

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
        uint32_t clientId, 
        JsonObject& module
    ) {
        if (module.containsKey("module") && module["module"] == "btn") {
            if (module.containsKey("config")) {
                // Extract configuration container
                JsonObject& configuration = module["config"].as<JsonObject&>();

                if (configuration.containsKey("values")) {
                    // Update module
                    _buttonUpdateConfiguration(configuration["values"]);

                    wsSend_P(clientId, PSTR("{\"message\": \"btn_updated\"}"));

                    // Reload & cache settings
                    firmwareReload();
                }
            }
        }
    }
#endif

// -----------------------------------------------------------------------------

void _buttonEvent(
    unsigned int id,
    unsigned int event
) {
    if (id >= _buttons.size() || event == 0) {
        return;
    }

    // Button event was fired
    for (unsigned int i = 0; i < _buttons[id].callbacks.size(); i++) {
        _buttons[id].callbacks[i](event);
    }

    #if FASTYBIRD_SUPPORT
        char payload[2];
        itoa(event, payload, 10);

        fastybirdReportChannelValue(
            _buttonFastybirdGetChannelStructure(),
            _buttonFastybirdGetChannelStatePropertyStructure(),
            id,
            payload
        );
    #endif
}

// -----------------------------------------------------------------------------
// MODULE API
// -----------------------------------------------------------------------------

void buttonOnEventRegister(
    button_on_event_callback_f callback,
    unsigned int id
) {
    if (id >= buttonCount()) {
        return;
    }

    _buttons[id].callbacks.push_back(callback);
}

// -----------------------------------------------------------------------------

int buttonCount() {
    return _buttons.size();
}

// -----------------------------------------------------------------------------
// MODULE CORE
// -----------------------------------------------------------------------------

void buttonSetup() {
    unsigned long btn_delay = BUTTON_DBLCLICK_DELAY;

    #if BUTTON1_PIN != GPIO_NONE
    {
        _buttons.push_back({new DebounceEvent(BUTTON1_PIN, BUTTON1_MODE, BUTTON_DEBOUNCE_DELAY, btn_delay)});
    }
    #endif

    #if BUTTON2_PIN != GPIO_NONE
    {
        _buttons.push_back({new DebounceEvent(BUTTON2_PIN, BUTTON2_MODE, BUTTON_DEBOUNCE_DELAY, btn_delay)});
    }
    #endif

    #if BUTTON3_PIN != GPIO_NONE
    {
        _buttons.push_back({new DebounceEvent(BUTTON3_PIN, BUTTON3_MODE, BUTTON_DEBOUNCE_DELAY, btn_delay)});
    }
    #endif

    #if BUTTON4_PIN != GPIO_NONE
    {
        _buttons.push_back({new DebounceEvent(BUTTON4_PIN, BUTTON4_MODE, BUTTON_DEBOUNCE_DELAY, btn_delay)});
    }
    #endif

    #if BUTTON5_PIN != GPIO_NONE
    {
        _buttons.push_back({new DebounceEvent(BUTTON5_PIN, BUTTON5_MODE, BUTTON_DEBOUNCE_DELAY, btn_delay)});
    }
    #endif

    #if BUTTON6_PIN != GPIO_NONE
    {
        _buttons.push_back({new DebounceEvent(BUTTON6_PIN, BUTTON6_MODE, BUTTON_DEBOUNCE_DELAY, btn_delay)});
    }
    #endif

    #if BUTTON7_PIN != GPIO_NONE
    {
        _buttons.push_back({new DebounceEvent(BUTTON7_PIN, BUTTON7_MODE, BUTTON_DEBOUNCE_DELAY, btn_delay)});
    }
    #endif

    #if BUTTON8_PIN != GPIO_NONE
    {
        _buttons.push_back({new DebounceEvent(BUTTON8_PIN, BUTTON8_MODE, BUTTON_DEBOUNCE_DELAY, btn_delay)});
    }
    #endif

    DEBUG_MSG(PSTR("[BUTTON] Number of buttons: %u\n"), buttonCount());

    #if WEB_SUPPORT && WS_SUPPORT
        wsOnConnectRegister(_buttonWSOnConnect);
        wsOnConfigureRegister(_buttonWSOnConfigure);
    #endif

    #if FASTYBIRD_SUPPORT
        fastybirdReportConfigurationSchemaRegister(_buttonReportConfigurationSchema);
        fastybirdReportConfigurationRegister(_buttonReportConfiguration);
        fastybirdOnConfigureRegister(_buttonUpdateConfiguration);

        if (buttonCount() > 0) {
            fastybirdRegisterChannel(_buttonFastybirdGetChannelStructure());
        }
    #endif

    // Register loop
    firmwareRegisterLoop(buttonLoop);
}

// -----------------------------------------------------------------------------

void buttonLoop() {
    for (unsigned int i = 0; i < _buttons.size(); i++) {
        if (unsigned int event = _buttons[i].button->loop()) {
            unsigned int count = _buttons[i].button->getEventCount();
            unsigned long length = _buttons[i].button->getEventLength();

            unsigned int mapped = _buttonMapEvent(event, count, length);

            _buttonEvent(i, mapped);
        }
    }
}

#endif // BUTTON_SUPPORT
