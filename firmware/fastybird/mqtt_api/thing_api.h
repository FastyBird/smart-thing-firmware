/*

FASTYBIRD THING MQTT MODULE

Copyright (C) 2018 FastyBird s.r.o. <info@fastybird.com>

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
        _fastybirdMqttApiCreateThingTopicString(thingId, FASTYBIRD_TOPIC_THING_PROPERTIES_STRUCTURE).c_str(),
        payload
    );

    if (packet_id == 0) return false;

    return true;
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateThingPropertiesStructure(
    std::vector<String> properties
) {
    return _fastybirdPropagateThingPropertiesStructure((fastybirdThingSN()).c_str(), properties);
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
    return _fastybirdPropagateThingProperty((fastybirdThingSN()).c_str(), property, payload);
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
    return _fastybirdPropagateThingName((fastybirdThingSN()).c_str(), name);
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
    return _fastybirdPropagateThingHardwareField((fastybirdThingSN()).c_str(), field, payload);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateThingHardwareVersion(
    const char * thingId,
    const char * name
) {
    return _fastybirdPropagateThingHardwareField(thingId, "version", name);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateThingHardwareVersion(
    const char * name
) {
    return _fastybirdPropagateThingHardwareVersion((fastybirdThingSN()).c_str(), name);
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
    return _fastybirdPropagateThingHardwareModelName((fastybirdThingSN()).c_str(), model);
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
    return _fastybirdPropagateThingHardwareManufacturer((fastybirdThingSN()).c_str(), manufacturer);
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
    return _fastybirdPropagateThingFirmwareField((fastybirdThingSN()).c_str(), field, payload);
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
    return _fastybirdPropagateThingFirmwareName((fastybirdThingSN()).c_str(), name);
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
    return _fastybirdPropagateThingFirmwareManufacturer((fastybirdThingSN()).c_str(), manufacturer);
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
    return _fastybirdPropagateThingFirmwareVersion((fastybirdThingSN()).c_str(), version);
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
            strcpy(payload, channels[i].type.c_str());

            if (channels[i].length > 1) {
                strcat(payload, "[]");
            }

            break;
        }
    }

    for (uint8_t i = (start_index + 1); i < channels.size(); i++) {
        if (channels[i].length > 0) {
            strcat(payload, ",");
            strcat(payload, channels[i].type.c_str());

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
    return _fastybirdPropagateThingChannels((fastybirdThingSN()).c_str(), channels);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateThingControlConfiguration(
    const char * thingId,
    std::vector<String> controls
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

    for (uint8_t i = 0; i < _fastybird_on_control_callbacks.size(); i++) {
        std::function<void(const char *)> controll_callback = _fastybird_on_control_callbacks[i].callback;

        packet_id = mqttSubscribe(
            _fastybirdMqttApiCreateThingTopicString(
                thingId,
                FASTYBIRD_TOPIC_THING_CONTROL_RECEIVE,
                "control",
                _fastybird_on_control_callbacks[i].controlName
            ).c_str(),
            [controll_callback](const char * topic, const char * payload) {
                controll_callback(payload);
            }
        );
        
        if (packet_id == 0) return false;
    }

    return true;
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateThingControlConfiguration(
    std::vector<String> controls
) {
    return _fastybirdPropagateThingControlConfiguration(
        (fastybirdThingSN()).c_str(),
        controls
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
                "config"
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
    return _fastybirdPropagateThingConfigurationSchema((fastybirdThingSN()).c_str(), schema);
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
                "config"
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
    return _fastybirdPropagateThingConfiguration((fastybirdThingSN()).c_str(), configuration);
}

#endif