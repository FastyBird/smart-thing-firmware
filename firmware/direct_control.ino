/*

DIRECT CONTROL MODULE

Copyright (C) 2018 FastyBird Ltd. <info@fastybird.com>

*/

#if DIRECT_CONTROL_SUPPORT

typedef struct {
    const char * channelType;
    const char * channelProperty;
    uint8_t channels;
    uint8_t controlsCnt;
} direct_control_channel_t;

std::vector<direct_control_channel_t> _direct_controls_channels;

const char * _direct_control_config_filename = "dc.conf";

// -----------------------------------------------------------------------------
// MODULE PRIVATE
// -----------------------------------------------------------------------------

#if WEB_SUPPORT && WS_SUPPORT
    // New WS client is connected
    void _directControlWSOnConnect(
        JsonObject& root
    ) {
        JsonArray& modules = root.containsKey("modules") ? root["modules"] : root.createNestedArray("modules");
        JsonObject& module = modules.createNestedObject();

        module["module"] = "direct_control";
        module["visible"] = true;

        // Configuration container
        JsonObject& configuration = module.createNestedObject("config");

        configuration["max"] = DIRECT_CONTROL_MAX_CONTROLS;

        // Data container
        JsonObject& data = module.createNestedObject("data");

        JsonArray &controls = data.createNestedArray("controls");

        DynamicJsonBuffer jsonBuffer;

        JsonArray& dc_configuration = jsonBuffer.parseArray(storageReadConfiguration(_direct_control_config_filename));

        for (JsonObject& stored_control : dc_configuration) {
            JsonObject& control = controls.createNestedObject();

            control["expression"] = stored_control["expression"].as<char*>();
            control["enabled"] = stored_control["enabled"].as<bool>();

            control["control_channel"] = stored_control["control_channel"].as<char*>();
            control["control_channel_type"] = stored_control["control_channel_type"].as<char*>();
            control["control_property"] = stored_control["control_property"].as<char*>();
            control["control_action"] = stored_control["control_action"].as<char*>();

            control["listen_topic"] = stored_control["listen_topic"].as<char*>();
            control["listen_action"] = stored_control["listen_action"].as<char*>();
        }
    }

// -----------------------------------------------------------------------------

    void _directControlWSOnConfigure(
        uint32_t clientId, 
        JsonObject& module
    ) {
        if (module.containsKey("module") && module["module"] == "direct_control") {
            if (module.containsKey("config")) {
                JsonObject& configuration = module["config"];

                if (configuration.containsKey("controls")) {
                    DEBUG_MSG(PSTR("[DC] Received %d direct control rules\n"), configuration["controls"].size());

                    DynamicJsonBuffer jsonBuffer;

                    JsonArray& dc_configuration = jsonBuffer.createArray();

                    for (uint8_t i = 0; i < _direct_controls_channels.size(); i++) {
                        _direct_controls_channels[i].controlsCnt = 0;
                    }

                    JsonArray& controls = configuration["controls"].as<JsonArray&>();

                    uint8_t control_cnt = 0;

                    for (JsonObject& control : controls) {
                        if (
                            control.containsKey("control_channel_type")
                            && control.containsKey("control_property")
                            && control.containsKey("control_channel")
                            && control.containsKey("control_action")
                            && control.containsKey("listen_topic")
                            && control.containsKey("listen_action")
                            && control.containsKey("expression")
                        )  {
                            uint8_t channel_id = control["control_channel"].as<uint8_t>();

                            for (uint8_t i = 0; i < _direct_controls_channels.size(); i++) {
                                // Check if channel is registered
                                if (
                                    _direct_controls_channels[i].channelType == control["control_channel_type"]
                                    && channel_id < _direct_controls_channels[i].channels
                                    && _direct_controls_channels[i].controlsCnt < DIRECT_CONTROL_MAX_CONTROLS
                                ) {
                                    DEBUG_MSG(PSTR("[DC] Saving direct control rule\n"));

                                    JsonObject& field = dc_configuration.createNestedObject();

                                    field["expression"] = control["expression"].as<char*>();
                                    field["enabled"] = control["enabled"].as<char*>();

                                    field["control_channel"] = control["control_channel"].as<char*>();
                                    field["control_channel_type"] = control["control_channel_type"].as<char*>();
                                    field["control_property"] = control["control_property"].as<char*>();
                                    field["control_action"] = control["control_action"].as<char*>();

                                    field["listen_topic"] = control["listen_topic"].as<char*>();
                                    field["listen_action"] = control["listen_action"].as<char*>();

                                    control_cnt++;

                                    _direct_controls_channels[i].controlsCnt++;
                                }
                            }
                        }
                    }

                    String output;

                    dc_configuration.printTo(output);

                    storageWriteConfiguration(_direct_control_config_filename, output);
                }

                wsSend_P(clientId, PSTR("{\"message\": \"direct_control_updated\"}"));

                // Reload & cache settings
                firmwareReload();
            }
        }
    }
#endif

// -----------------------------------------------------------------------------
// MODULE API
// -----------------------------------------------------------------------------

void directControlReportChannelConfiguration(
    uint8_t id,
    const char * channelType,
    JsonArray& directControls
) {
    uint8_t counter = 0;

    DynamicJsonBuffer jsonBuffer;

    JsonArray& dc_configuration = jsonBuffer.parseArray(storageReadConfiguration(_direct_control_config_filename));

    for (JsonObject& stored_control : dc_configuration) {
        if (id == stored_control["control_channel"].as<unsigned int>() && strcmp(channelType, stored_control["control_channel_type"].as<char*>()) == 0) {
            JsonObject& direct_control = directControls.createNestedObject();

            direct_control["expression"] = stored_control["expression"].as<char*>();
            direct_control["enabled"] = stored_control["enabled"].as<bool>();

            direct_control["control_channel"] = stored_control["control_channel"].as<char*>();
            direct_control["control_channel_type"] = stored_control["control_channel_type"].as<char*>();
            direct_control["control_property"] = stored_control["control_property"].as<char*>();
            direct_control["control_action"] = stored_control["control_action"].as<char*>();

            direct_control["listen_topic"] = stored_control["listen_topic"].as<char*>();
            direct_control["listen_action"] = stored_control["listen_action"].as<char*>();
        }

        counter++;

        if (counter >= DIRECT_CONTROL_MAX_CONTROLS) {
            break;
        }
    }
}

// -----------------------------------------------------------------------------

void directControlConfigureChannelConfiguration(
    uint8_t id,
    const char * channelType,
    JsonArray& configuration
) {
    uint8_t i = 0;

    DynamicJsonBuffer jsonBuffer;

    JsonArray& dc_configuration = jsonBuffer.createArray();

    // Store new direct controls configuration
    for (JsonObject& control : configuration) {
        if (
            control.containsKey("control_property")
            && control.containsKey("control_action")
            && control.containsKey("listen_topic")
            && control.containsKey("expression")
        )  {
            if (i >= DIRECT_CONTROL_MAX_CONTROLS) {
                break;
            }

            JsonObject& field = dc_configuration.createNestedObject();

            field["expression"] = control["expression"].as<char*>();
            field["enabled"] = control["enabled"].as<bool>();

            field["control_channel"] = id;
            field["control_channel_type"] = channelType;
            field["control_property"] = control["control_property"].as<char*>();
            field["control_action"] = control["control_action"].as<char*>();

            field["listen_topic"] = control["listen_topic"].as<char*>();
            field["listen_action"] = control["listen_action"].as<char*>();

            i++;
        }
    }

    String output;

    dc_configuration.printTo(output);

    storageWriteConfiguration(_direct_control_config_filename, output);
}

// -----------------------------------------------------------------------------

void directControlRegisterChannel(
    const char * channelType,
    const char * channelProperty,
    uint8_t channels
) {
    direct_control_channel_t channel = {
        channelType,
        channelProperty,
        channels,
        0
    };

    _direct_controls_channels.push_back(channel);
}

// -----------------------------------------------------------------------------
// MODULE CORE
// -----------------------------------------------------------------------------

void directControlSetup() {
    // Update websocket clients
    #if WEB_SUPPORT && WS_SUPPORT
        wsOnConnectRegister(_directControlWSOnConnect);
        wsOnConfigureRegister(_directControlWSOnConfigure);
    #endif
}

#endif
