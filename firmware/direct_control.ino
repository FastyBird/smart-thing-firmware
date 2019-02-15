/*

DIRECT CONTROL MODULE

Copyright (C) 2018 FastyBird Ltd. <info@fastybird.com>

*/

#if DIRECT_CONTROL_SUPPORT

typedef struct {
    const char * channelType;
    const char * channelProperty;
    unsigned int channels;
    unsigned int controlsCnt;
} direct_control_channel_t;

std::vector<direct_control_channel_t> _direct_controls_channels;

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

        for (unsigned int i = 0; i < _direct_controls_channels.size(); i++) {
            for (unsigned int channel_id = 0; channel_id < _direct_controls_channels[i].channels; channel_id++) {
                for (unsigned int j = 0; j < DIRECT_CONTROL_MAX_CONTROLS; j++) {
                    if (!hasSetting(directControlCreateSettingsKey("dcControlAction_", _direct_controls_channels[i].channelType, j), channel_id)) {
                        break;
                    }

                    JsonObject& control = controls.createNestedObject();

                    control["control_channel_type"] = _direct_controls_channels[i].channelType;
                    control["control_property"] = _direct_controls_channels[i].channelProperty;
                    control["control_channel"] = channel_id;
                    control["control_action"] = getSetting(directControlCreateSettingsKey("dcControlAction_", _direct_controls_channels[i].channelType, j), channel_id, "");

                    control["listen_topic"] = getSetting(directControlCreateSettingsKey("dcListenTopic_", _direct_controls_channels[i].channelType, j), channel_id, "");
                    control["listen_action"] = getSetting(directControlCreateSettingsKey("dcListenAction_", _direct_controls_channels[i].channelType, j), channel_id, "");

                    control["expression"] = getSetting(directControlCreateSettingsKey("dcExpression_", _direct_controls_channels[i].channelType, j), channel_id, "");

                    control["enabled"] = getSetting(directControlCreateSettingsKey("dcEnabled_", _direct_controls_channels[i].channelType, j), channel_id, "").toInt() == 1 ? true : false;
                }
            }
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

                    // Clear existing controls
                    for (unsigned int i = 0; i < _direct_controls_channels.size(); i++) {
                        for (unsigned int channel_id = 0; channel_id < _direct_controls_channels[i].channels; channel_id++) {
                            for (unsigned int j = 0; j < DIRECT_CONTROL_MAX_CONTROLS; j++) {
                                delSetting(directControlCreateSettingsKey("dcControlProperty_", _direct_controls_channels[i].channelType, j), channel_id);
                                delSetting(directControlCreateSettingsKey("dcControlAction_", _direct_controls_channels[i].channelType, j), channel_id);
                                delSetting(directControlCreateSettingsKey("dcListenTopic_", _direct_controls_channels[i].channelType, j), channel_id);
                                delSetting(directControlCreateSettingsKey("dcListenAction_", _direct_controls_channels[i].channelType, j), channel_id);
                                delSetting(directControlCreateSettingsKey("dcExpression_", _direct_controls_channels[i].channelType, j), channel_id);
                                delSetting(directControlCreateSettingsKey("dcEnabled_", _direct_controls_channels[i].channelType, j), channel_id);
                            }
                        }
                    }

                    for (unsigned int i = 0; i < _direct_controls_channels.size(); i++) {
                        _direct_controls_channels[i].controlsCnt = 0;
                    }

                    JsonArray& controls = configuration["controls"].as<JsonArray&>();
                    unsigned int control_cnt = 0;

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
                            unsigned int channel_id = control["control_channel"].as<unsigned int>();

                            for (unsigned int i = 0; i < _direct_controls_channels.size(); i++) {
                                // Check if channel is registered
                                if (
                                    _direct_controls_channels[i].channelType == control["control_channel_type"]
                                    && channel_id < _direct_controls_channels[i].channels
                                    && _direct_controls_channels[i].controlsCnt < DIRECT_CONTROL_MAX_CONTROLS
                                ) {
                                    DEBUG_MSG(PSTR("[DC] Saving direct control rule\n"));

                                    setSetting(directControlCreateSettingsKey("dcControlProperty_", _direct_controls_channels[i].channelType, control_cnt), channel_id, control["control_property"].as<char*>());
                                    setSetting(directControlCreateSettingsKey("dcControlAction_", _direct_controls_channels[i].channelType, control_cnt), channel_id, control["control_action"].as<char*>());
                                    setSetting(directControlCreateSettingsKey("dcListenTopic_", _direct_controls_channels[i].channelType, control_cnt), channel_id, control["listen_topic"].as<char*>());
                                    setSetting(directControlCreateSettingsKey("dcListenAction_", _direct_controls_channels[i].channelType, control_cnt), channel_id, control["listen_action"].as<char*>());
                                    setSetting(directControlCreateSettingsKey("dcExpression_", _direct_controls_channels[i].channelType, control_cnt), channel_id, control["expression"].as<char*>());
                                    setSetting(directControlCreateSettingsKey("dcEnabled_", _direct_controls_channels[i].channelType, control_cnt), channel_id, control["enabled"].as<bool>() ? 1 : 0);

                                    control_cnt++;

                                    _direct_controls_channels[i].controlsCnt++;
                                }
                            }
                        }
                    }
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

String directControlCreateSettingsKey(
    const char * prefix,
    const char * channelType,
    unsigned int channel
) {
    char buffer[100];
    
    strcpy(buffer, prefix);
    strcat(buffer, channelType);
    strcat(buffer, "_%d");

    sprintf(buffer, buffer, channel);

    return String(buffer);
}

// -----------------------------------------------------------------------------

void directControlReportChannelConfiguration(
    unsigned int id,
    const char * channelType,
    JsonArray& directControls
) {
    for (unsigned int i = 0; i < DIRECT_CONTROL_MAX_CONTROLS; i++) {
        if (!hasSetting(directControlCreateSettingsKey("dcControlAction_", channelType, i), id)) {
            break;
        }

        JsonObject& direct_control = directControls.createNestedObject();

        direct_control["control_property"] = getSetting(directControlCreateSettingsKey("dcControlProperty_", channelType, i), id, "");
        direct_control["control_action"] = getSetting(directControlCreateSettingsKey("dcControlAction_", channelType, i), id, "");

        direct_control["listen_topic"] = getSetting(directControlCreateSettingsKey("dcListenTopic_", channelType, i), id, "");
        direct_control["listen_action"] = getSetting(directControlCreateSettingsKey("dcListenAction_", channelType, i), id, "");

        direct_control["expression"] = getSetting(directControlCreateSettingsKey("dcExpression_", channelType, i), id, "");

        direct_control["enabled"] = getSetting(directControlCreateSettingsKey("dcEnabled_", channelType, i), id, "").toInt() ? true : false;
    }
}

// -----------------------------------------------------------------------------

void directControlConfigureChannelConfiguration(
    unsigned int id,
    const char * channelType,
    JsonArray& configuration
) {
    // Clear existing direct controls
    for (unsigned int i = 0; i < DIRECT_CONTROL_MAX_CONTROLS; i++) {
        delSetting(directControlCreateSettingsKey("dcControlProperty_", channelType, i), id);
        delSetting(directControlCreateSettingsKey("dcControlAction_", channelType, i), id);
        delSetting(directControlCreateSettingsKey("dcListenTopic_", channelType, i), id);
        delSetting(directControlCreateSettingsKey("dcListenAction_", channelType, i), id);
        delSetting(directControlCreateSettingsKey("dcExpression_", channelType, i), id);
    }
    
    unsigned int i = 0;

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

            setSetting(directControlCreateSettingsKey("dcControlProperty_", channelType, i), id, control["control_property"].as<char*>());
            setSetting(directControlCreateSettingsKey("dcControlAction_", channelType, i), id, control["control_action"].as<char*>());

            setSetting(directControlCreateSettingsKey("dcListenTopic_", channelType, i), id, control["listen_topic"].as<char*>());
            setSetting(directControlCreateSettingsKey("dcListenAction_", channelType, i), id, control["listen_action"].as<char*>());

            setSetting(directControlCreateSettingsKey("dcExpression_", channelType, i), id, control["expression"].as<char*>());

            setSetting(directControlCreateSettingsKey("dcEnabled_", channelType, i), id, control["enabled"].as<bool>() ? 1 : 0);

            i++;
        }
    }
}

// -----------------------------------------------------------------------------

void directControlRegisterChannel(
    const char * channelType,
    const char * channelProperty,
    unsigned int channels
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
