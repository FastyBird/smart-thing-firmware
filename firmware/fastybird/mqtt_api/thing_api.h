/*

FASTYBIRD THING MQTT MODULE

Copyright (C) 2018 FastyBird Ltd. <info@fastybird.com>

*/

#if FASTYBIRD_SUPPORT && MQTT_SUPPORT

// -----------------------------------------------------------------------------
// MODULE PRIVATE
// -----------------------------------------------------------------------------

String _fastybirdMqttApiCreateThingTopicString(
    const char * thingId,
    String topic
) {
    return fastybirdMqttApiBuildTopicPrefix(thingId) + topic;
}

// -----------------------------------------------------------------------------

String _fastybirdMqttApiCreateThingTopicString(
    const char * thingId,
    String topic,
    String replace,
    String replaceWith
) {
    topic.replace("{" + replace + "}", replaceWith);

    return fastybirdMqttApiBuildTopicPrefix(thingId) + topic;
}

// -----------------------------------------------------------------------------

String _fastybirdMqttApiCreatePropertyTopicString(
    const char * thingId,
    String property
) {
    return _fastybirdMqttApiCreateThingTopicString(
        thingId,
        FASTYBIRD_TOPIC_THING_PROPERTY,
        "property",
        property
    );
}

// -----------------------------------------------------------------------------

String _fastybirdMqttApiCreateStatTopicString(
    const char * thingId,
    String stat
) {
    return _fastybirdMqttApiCreateThingTopicString(
        thingId,
        FASTYBIRD_TOPIC_THING_STATS,
        "stats",
        stat
    );
}

// -----------------------------------------------------------------------------

String _fastybirdMqttApiCreateHWTopicString(
    const char * thingId,
    String field
) {
    return _fastybirdMqttApiCreateThingTopicString(
        thingId,
        FASTYBIRD_TOPIC_THING_HW_INFO,
        "hw",
        field
    );
}

// -----------------------------------------------------------------------------

String _fastybirdMqttApiCreateFWTopicString(
    const char * thingId,
    String field
) {
    return _fastybirdMqttApiCreateThingTopicString(
        thingId,
        FASTYBIRD_TOPIC_THING_FW_INFO,
        "fw",
        field
    );
}

// -----------------------------------------------------------------------------
// MODULE INTERNAL API
// -----------------------------------------------------------------------------

bool _fastybirdPropagateThingPropertiesStructure(
    const char * thingId,
    std::vector<String> properties
) {
    if (properties.size() <= 0) {
        return true;
    }

    char payload[80];

    strcpy(payload, properties[0].c_str());

    for (uint8_t i = 1; i < properties.size(); i++) {
        strcat(payload, ",");
        strcat(payload, properties[i].c_str());
    }

    uint8_t packet_id;

    packet_id = mqttSend(
        _fastybirdMqttApiCreateThingTopicString(thingId, FASTYBIRD_TOPIC_THING_PROPERTIES).c_str(),
        payload
    );

    if (packet_id == 0) return false;

    return true;
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateThingPropertiesStructure(
    std::vector<String> properties
) {
    return _fastybirdPropagateThingPropertiesStructure(_fastybird_mqtt_thing_id, properties);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateThingProperty(
    const char * thingId,
    const char * property,
    const char * payload
) {
    uint8_t packet_id;

    packet_id = mqttSend(
        _fastybirdMqttApiCreatePropertyTopicString(thingId, property).c_str(),
        payload
    );

    if (packet_id == 0) return false;

    return true;
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateThingProperty(
    const char * property,
    const char * payload
) {
    return _fastybirdPropagateThingProperty(_fastybird_mqtt_thing_id, property, payload);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateThingName(
    const char * thingId,
    const char * name
) {
    uint8_t packet_id;

    packet_id = mqttSend(
        _fastybirdMqttApiCreateThingTopicString(thingId, FASTYBIRD_TOPIC_THING_NAME).c_str(),
        name
    );

    if (packet_id == 0) return false;

    return true;
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateThingName(
    const char * name
) {
    return _fastybirdPropagateThingName(_fastybird_mqtt_thing_id, name);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateThingHardwareField(
    const char * thingId,
    const char * field,
    const char * payload
) {
    uint8_t packet_id;

    packet_id = mqttSend(
        _fastybirdMqttApiCreateHWTopicString(thingId, field).c_str(),
        payload
    );

    if (packet_id == 0) return false;

    return true;
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateThingHardwareField(
    const char * field,
    const char * payload
) {
    return _fastybirdPropagateThingHardwareField(_fastybird_mqtt_thing_id, field, payload);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateThingHardwareName(
    const char * thingId,
    const char * name
) {
    return _fastybirdPropagateThingHardwareField(thingId, "name", name);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateThingHardwareName(
    const char * name
) {
    return _fastybirdPropagateThingHardwareName(_fastybird_mqtt_thing_id, name);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateThingHardwareModelName(
    const char * thingId,
    const char * model
) {
    return _fastybirdPropagateThingHardwareField(thingId, "model", model);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateThingHardwareModelName(
    const char * model
) {
    return _fastybirdPropagateThingHardwareModelName(_fastybird_mqtt_thing_id, model);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateThingHardwareManufacturer(
    const char * thingId,
    const char * manufacturer
) {
    return _fastybirdPropagateThingHardwareField(thingId, "manufacturer", manufacturer);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateThingHardwareManufacturer(
    const char * manufacturer
) {
    return _fastybirdPropagateThingHardwareManufacturer(_fastybird_mqtt_thing_id, manufacturer);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateThingHardwareVersion(
    const char * thingId,
    const char * version
) {
    return _fastybirdPropagateThingHardwareField(thingId, "version", version);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateThingHardwareVersion(
    const char * version
) {
    return _fastybirdPropagateThingHardwareVersion(_fastybird_mqtt_thing_id, version);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateThingFirmwareField(
    const char * thingId,
    const char * field,
    const char * payload
) {
    uint8_t packet_id;

    packet_id = mqttSend(
        _fastybirdMqttApiCreateFWTopicString(thingId, field).c_str(),
        payload
    );

    if (packet_id == 0) return false;

    return true;
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateThingFirmwareField(
    const char * field,
    const char * payload
) {
    return _fastybirdPropagateThingFirmwareField(_fastybird_mqtt_thing_id, field, payload);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateThingFirmwareName(
    const char * thingId,
    const char * name
) {
    return _fastybirdPropagateThingFirmwareField(thingId, "name", name);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateThingFirmwareName(
    const char * name
) {
    return _fastybirdPropagateThingFirmwareName(_fastybird_mqtt_thing_id, name);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateThingFirmwareManufacturer(
    const char * thingId,
    const char * manufacturer
) {
    return _fastybirdPropagateThingFirmwareField(thingId, "manufacturer", manufacturer);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateThingFirmwareManufacturer(
    const char * manufacturer
) {
    return _fastybirdPropagateThingFirmwareManufacturer(_fastybird_mqtt_thing_id, manufacturer);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateThingFirmwareVersion(
    const char * thingId,
    const char * version
) {
    return _fastybirdPropagateThingFirmwareField(thingId, "version", version);
}


// -----------------------------------------------------------------------------

bool _fastybirdPropagateThingFirmwareVersion(
    const char * version
) {
    return _fastybirdPropagateThingFirmwareVersion(_fastybird_mqtt_thing_id, version);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateThingChannels(
    const char * thingId,
    std::vector<fastybird_channel_t> channels
) {
    if (channels.size() <= 0) {
        return true;
    }

    char payload[80];

    uint8_t start_index = 0;

    for (uint8_t i = 0; i < channels.size(); i++) {
        start_index = i;

        if (channels[i].length > 0) {
            strcpy(payload, channels[i].type);

            if (channels[i].length > 1) {
                strcat(payload, "[]");
            }

            break;
        }
    }

    for (uint8_t i = (start_index + 1); i < channels.size(); i++) {
        if (channels[i].length > 0) {
            strcat(payload, ",");
            strcat(payload, channels[i].type);

            if (channels[i].length > 1) {
                strcat(payload, "[]");
            }
        }
    }

    uint8_t packet_id;

    packet_id = mqttSend(
        _fastybirdMqttApiCreateThingTopicString(thingId, FASTYBIRD_TOPIC_THING_CHANNELS).c_str(),
        payload
    );

    if (packet_id == 0) {
        return false;
    }

    return true;
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateThingChannels(
    std::vector<fastybird_channel_t> channels
) {
    return _fastybirdPropagateThingChannels(_fastybird_mqtt_thing_id, channels);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateThingStatsStructure(
    const char * thingId,
    std::vector<String> stats
) {
    if (stats.size() <= 0) {
        return true;
    }

    char payload[80];

    strcpy(payload, stats[0].c_str());

    for (uint8_t i = 1; i < stats.size(); i++) {
        strcat(payload, ",");
        strcat(payload, stats[i].c_str());
    }

    uint8_t packet_id;

    packet_id = mqttSend(
        _fastybirdMqttApiCreateThingTopicString(thingId, FASTYBIRD_TOPIC_THING_STATS_STRUCTURE).c_str(),
        payload
    );

    if (packet_id == 0) {
        return false;
    }

    return true;
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateThingStatsStructure(
    std::vector<String> stats
) {
    return _fastybirdPropagateThingStatsStructure(_fastybird_mqtt_thing_id, stats);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateThingStat(
    const char * thingId,
    const char * stat,
    const char * payload
) {
    uint8_t packet_id;

    packet_id = mqttSend(
        _fastybirdMqttApiCreateStatTopicString(thingId, stat).c_str(),
        payload
    );

    if (packet_id == 0) return false;

    return true;
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateThingStat(
    const char * stat,
    const char * payload
) {
    return _fastybirdPropagateThingStat(_fastybird_mqtt_thing_id, stat, payload);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateThingControlConfiguration(
    const char * thingId,
    std::vector<String> controls,
    std::function<void(JsonObject&)> configureCallback,
    std::function<void()> resetCallback,
    std::function<void()> reconnectCallback,
    std::function<void()> factoryResetCallback
) {
    if (controls.size() <= 0) {
        return true;
    }

    char payload[80];

    strcpy(payload, controls[0].c_str());

    for (uint8_t i = 1; i < controls.size(); i++) {
        strcat(payload, ",");
        strcat(payload, controls[i].c_str());
    }

    uint8_t packet_id;

    packet_id = mqttSend(
        _fastybirdMqttApiCreateThingTopicString(thingId, FASTYBIRD_TOPIC_THING_CONTROL).c_str(),
        payload
    );

    if (packet_id == 0) return false;

    #if FASTYBIRD_ENABLE_CONFIGURATION
        packet_id = mqttSubscribe(
            _fastybirdMqttApiCreateThingTopicString(
                thingId,
                FASTYBIRD_TOPIC_THING_CONTROL_RECEIVE,
                "control",
                FASTYBIRD_THING_CONTROL_VALUE_CONFIGURATION
            ).c_str(),
            [configureCallback](const char * topic, const char * payload) {
                DynamicJsonBuffer jsonBuffer;

                // Parse payload
                JsonObject& root = jsonBuffer.parseObject(payload);

                if (root.success()) {
                    configureCallback(root);

                } else {
                    DEBUG_MSG(PSTR("[FASTYBIRD] Configuration payload is not in valid JSON format\n"));
                }
            }
        );
        
        if (packet_id == 0) return false;
    #endif

    #if FASTYBIRD_ENABLE_RESET
        packet_id = mqttSubscribe(
            _fastybirdMqttApiCreateThingTopicString(
                thingId,
                FASTYBIRD_TOPIC_THING_CONTROL_RECEIVE,
                "control",
                FASTYBIRD_THING_CONTROL_VALUE_RESET
            ).c_str(),
            [resetCallback](const char * topic, const char * payload) {
                resetCallback();
            }
        );

        if (packet_id == 0) return false;
    #endif

    #if FASTYBIRD_ENABLE_RECONNECT
        packet_id = mqttSubscribe(
            _fastybirdMqttApiCreateThingTopicString(
                thingId,
                FASTYBIRD_TOPIC_THING_CONTROL_RECEIVE,
                "control",
                FASTYBIRD_THING_CONTROL_VALUE_RECONNECT
            ).c_str(),
            [reconnectCallback](const char * topic, const char * payload) {
                reconnectCallback();
            }
        );

        if (packet_id == 0) return false;
    #endif

    #if FASTYBIRD_ENABLE_FACTORY_RESET
        packet_id = mqttSubscribe(
            _fastybirdMqttApiCreateThingTopicString(
                thingId,
                FASTYBIRD_TOPIC_THING_CONTROL_RECEIVE,
                "control",
                FASTYBIRD_THING_CONTROL_VALUE_FACTORY_RESET
            ).c_str(),
            [factoryResetCallback](const char * topic, const char * payload) {
                factoryResetCallback();
            }
        );

        if (packet_id == 0) return false;
    #endif

    return true;
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateThingControlConfiguration(
    std::vector<String> controls,
    std::function<void(JsonObject&)> configureCallback,
    std::function<void()> resetCallback,
    std::function<void()> reconnectCallback,
    std::function<void()> factoryResetCallback
) {
    return _fastybirdPropagateThingControlConfiguration(
        _fastybird_mqtt_thing_id,
        controls,
        configureCallback,
        resetCallback,
        reconnectCallback,
        factoryResetCallback
    );
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateThingConfigurationSchema(
    const char * thingId,
    JsonArray& schema
) {
    uint8_t packet_id;

    if (schema.size() > 0) {
        String output;

        schema.printTo(output);

        packet_id = mqttSend(
            _fastybirdMqttApiCreateThingTopicString(
                thingId,
                FASTYBIRD_TOPIC_THING_CONTROL_SCHEMA,
                "control",
                FASTYBIRD_THING_CONTROL_VALUE_CONFIGURATION
            ).c_str(),
            output.c_str()
        );

        if (packet_id == 0) return false;
    }

    return true;
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateThingConfigurationSchema(
    JsonArray& schema
) {
    return _fastybirdPropagateThingConfigurationSchema(_fastybird_mqtt_thing_id, schema);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateThingConfiguration(
    const char * thingId,
    JsonObject& configuration
) {
    if (configuration.size() > 0) {
        String output;

        configuration.printTo(output);

        uint8_t packet_id;

        packet_id = mqttSend(
            _fastybirdMqttApiCreateThingTopicString(
                thingId,
                FASTYBIRD_TOPIC_THING_CONTROL_DATA,
                "control",
                FASTYBIRD_THING_CONTROL_VALUE_CONFIGURATION
            ).c_str(),
            output.c_str()
        );

        if (packet_id == 0) return false;
    }

    return true;
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateThingConfiguration(
    JsonObject& configuration
) {
    return _fastybirdPropagateThingConfiguration(_fastybird_mqtt_thing_id, configuration);
}

#endif