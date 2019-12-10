/*

FASTYBIRD DEVICE MQTT MODULE

Copyright (C) 2018 FastyBird s.r.o. <info@fastybird.com>

*/

#if FASTYBIRD_SUPPORT && MQTT_SUPPORT

// -----------------------------------------------------------------------------
// MODULE PRIVATE
// -----------------------------------------------------------------------------

String _fastybirdMqttApiCreateDeviceTopicString(
    const char * deviceId,
    String topic
) {
    return fastybirdMqttApiBuildTopicPrefix(deviceId) + topic;
}

// -----------------------------------------------------------------------------

String _fastybirdMqttApiCreateDeviceTopicString(
    const char * deviceId,
    String topic,
    String replace,
    String replaceWith
) {
    topic.replace("{" + replace + "}", replaceWith);

    return fastybirdMqttApiBuildTopicPrefix(deviceId) + topic;
}

// -----------------------------------------------------------------------------

String _fastybirdMqttApiCreatePropertyTopicString(
    const char * deviceId,
    String property
) {
    return _fastybirdMqttApiCreateDeviceTopicString(
        deviceId,
        FASTYBIRD_TOPIC_DEVICE_PROPERTY,
        "property",
        property
    );
}

// -----------------------------------------------------------------------------

String _fastybirdMqttApiCreateHWTopicString(
    const char * deviceId,
    String field
) {
    return _fastybirdMqttApiCreateDeviceTopicString(
        deviceId,
        FASTYBIRD_TOPIC_DEVICE_HW_INFO,
        "hw",
        field
    );
}

// -----------------------------------------------------------------------------

String _fastybirdMqttApiCreateFWTopicString(
    const char * deviceId,
    String field
) {
    return _fastybirdMqttApiCreateDeviceTopicString(
        deviceId,
        FASTYBIRD_TOPIC_DEVICE_FW_INFO,
        "fw",
        field
    );
}

// -----------------------------------------------------------------------------
// MODULE INTERNAL API
// -----------------------------------------------------------------------------

bool _fastybirdPropagateDevicePropertiesStructure(
    const char * deviceId,
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
        _fastybirdMqttApiCreateDeviceTopicString(deviceId, FASTYBIRD_TOPIC_DEVICE_PROPERTIES_STRUCTURE).c_str(),
        payload,
        false
    );

    if (packet_id == 0) return false;

    return true;
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateDevicePropertiesStructure(
    std::vector<String> properties
) {
    return _fastybirdPropagateDevicePropertiesStructure((fastybirdDeviceSN()).c_str(), properties);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateDeviceProperty(
    const char * deviceId,
    const char * property,
    const char * payload
) {
    uint8_t packet_id;

    bool retain = false;

    packet_id = mqttSend(
        _fastybirdMqttApiCreatePropertyTopicString(deviceId, property).c_str(),
        payload,
        true
    );

    if (packet_id == 0) return false;

    return true;
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateDeviceProperty(
    const char * property,
    const char * payload
) {
    return _fastybirdPropagateDeviceProperty((fastybirdDeviceSN()).c_str(), property, payload);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateDeviceName(
    const char * deviceId,
    const char * name
) {
    uint8_t packet_id;

    packet_id = mqttSend(
        _fastybirdMqttApiCreateDeviceTopicString(deviceId, FASTYBIRD_TOPIC_DEVICE_NAME).c_str(),
        name,
        false
    );

    if (packet_id == 0) return false;

    return true;
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateDeviceName(
    const char * name
) {
    return _fastybirdPropagateDeviceName((fastybirdDeviceSN()).c_str(), name);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateDeviceHardwareField(
    const char * deviceId,
    const char * field,
    const char * payload
) {
    uint8_t packet_id;

    packet_id = mqttSend(
        _fastybirdMqttApiCreateHWTopicString(deviceId, field).c_str(),
        payload,
        false
    );

    if (packet_id == 0) return false;

    return true;
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateDeviceHardwareField(
    const char * field,
    const char * payload
) {
    return _fastybirdPropagateDeviceHardwareField((fastybirdDeviceSN()).c_str(), field, payload);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateDeviceHardwareVersion(
    const char * deviceId,
    const char * name
) {
    return _fastybirdPropagateDeviceHardwareField(deviceId, "version", name);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateDeviceHardwareVersion(
    const char * name
) {
    return _fastybirdPropagateDeviceHardwareVersion((fastybirdDeviceSN()).c_str(), name);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateDeviceHardwareModelName(
    const char * deviceId,
    const char * model
) {
    return _fastybirdPropagateDeviceHardwareField(deviceId, "model", model);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateDeviceHardwareModelName(
    const char * model
) {
    return _fastybirdPropagateDeviceHardwareModelName((fastybirdDeviceSN()).c_str(), model);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateDeviceHardwareManufacturer(
    const char * deviceId,
    const char * manufacturer
) {
    return _fastybirdPropagateDeviceHardwareField(deviceId, "manufacturer", manufacturer);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateDeviceHardwareManufacturer(
    const char * manufacturer
) {
    return _fastybirdPropagateDeviceHardwareManufacturer((fastybirdDeviceSN()).c_str(), manufacturer);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateDeviceFirmwareField(
    const char * deviceId,
    const char * field,
    const char * payload
) {
    uint8_t packet_id;

    packet_id = mqttSend(
        _fastybirdMqttApiCreateFWTopicString(deviceId, field).c_str(),
        payload,
        false
    );

    if (packet_id == 0) return false;

    return true;
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateDeviceFirmwareField(
    const char * field,
    const char * payload
) {
    return _fastybirdPropagateDeviceFirmwareField((fastybirdDeviceSN()).c_str(), field, payload);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateDeviceFirmwareName(
    const char * deviceId,
    const char * name
) {
    return _fastybirdPropagateDeviceFirmwareField(deviceId, "name", name);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateDeviceFirmwareName(
    const char * name
) {
    return _fastybirdPropagateDeviceFirmwareName((fastybirdDeviceSN()).c_str(), name);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateDeviceFirmwareManufacturer(
    const char * deviceId,
    const char * manufacturer
) {
    return _fastybirdPropagateDeviceFirmwareField(deviceId, "manufacturer", manufacturer);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateDeviceFirmwareManufacturer(
    const char * manufacturer
) {
    return _fastybirdPropagateDeviceFirmwareManufacturer((fastybirdDeviceSN()).c_str(), manufacturer);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateDeviceFirmwareVersion(
    const char * deviceId,
    const char * version
) {
    return _fastybirdPropagateDeviceFirmwareField(deviceId, "version", version);
}


// -----------------------------------------------------------------------------

bool _fastybirdPropagateDeviceFirmwareVersion(
    const char * version
) {
    return _fastybirdPropagateDeviceFirmwareVersion((fastybirdDeviceSN()).c_str(), version);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateDeviceChannels(
    const char * deviceId,
    std::vector<fastybird_channel_t> channels
) {
    char payload[80];

    if (channels.size() > 0) {
        uint8_t start_index = 0;

        for (uint8_t i = 0; i < channels.size(); i++) {
            start_index = i;

            if (channels[i].length > 0) {
                strcpy(payload, channels[i].name.c_str());

                if (channels[i].length > 1) {
                    strcat(payload, "[]");
                }

                break;
            }
        }

        for (uint8_t i = (start_index + 1); i < channels.size(); i++) {
            if (channels[i].length > 0) {
                strcat(payload, ",");
                strcat(payload, channels[i].name.c_str());

                if (channels[i].length > 1) {
                    strcat(payload, "[]");
                }
            }
        }
    }

    uint8_t packet_id;

    packet_id = mqttSend(
        _fastybirdMqttApiCreateDeviceTopicString(deviceId, FASTYBIRD_TOPIC_DEVICE_CHANNELS).c_str(),
        payload,
        false
    );

    if (packet_id == 0) {
        return false;
    }

    return true;
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateDeviceChannels(
    std::vector<fastybird_channel_t> channels
) {
    return _fastybirdPropagateDeviceChannels((fastybirdDeviceSN()).c_str(), channels);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateDeviceControlConfiguration(
    const char * deviceId,
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
        _fastybirdMqttApiCreateDeviceTopicString(deviceId, FASTYBIRD_TOPIC_DEVICE_CONTROL).c_str(),
        payload,
        false
    );

    if (packet_id == 0) return false;

    for (uint8_t i = 0; i < _fastybird_on_control_callbacks.size(); i++) {
        std::function<void(const char *)> controll_callback = _fastybird_on_control_callbacks[i].callback;

        packet_id = mqttSubscribe(
            _fastybirdMqttApiCreateDeviceTopicString(
                deviceId,
                FASTYBIRD_TOPIC_DEVICE_CONTROL_RECEIVE,
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

bool _fastybirdPropagateDeviceControlConfiguration(
    std::vector<String> controls
) {
    return _fastybirdPropagateDeviceControlConfiguration(
        (fastybirdDeviceSN()).c_str(),
        controls
    );
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateDeviceConfigurationSchema(
    const char * deviceId,
    JsonArray& schema
) {
    uint8_t packet_id;

    if (schema.size() > 0) {
        String output;

        schema.printTo(output);

        packet_id = mqttSend(
            _fastybirdMqttApiCreateDeviceTopicString(
                deviceId,
                FASTYBIRD_TOPIC_DEVICE_CONTROL_SCHEMA,
                "control",
                FASTYBIRD_DEVICE_CONTROL_CONFIGURE
            ).c_str(),
            output.c_str(),
            false
        );

        if (packet_id == 0) return false;
    }

    return true;
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateDeviceConfigurationSchema(
    JsonArray& schema
) {
    return _fastybirdPropagateDeviceConfigurationSchema((fastybirdDeviceSN()).c_str(), schema);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateDeviceConfiguration(
    const char * deviceId,
    JsonObject& configuration
) {
    if (configuration.size() > 0) {
        String output;

        configuration.printTo(output);

        uint8_t packet_id;

        packet_id = mqttSend(
            _fastybirdMqttApiCreateDeviceTopicString(
                deviceId,
                FASTYBIRD_TOPIC_DEVICE_CONTROL_DATA,
                "control",
                FASTYBIRD_DEVICE_CONTROL_CONFIGURE
            ).c_str(),
            output.c_str(),
            false
        );

        if (packet_id == 0) return false;
    }

    return true;
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateDeviceConfiguration(
    JsonObject& configuration
) {
    return _fastybirdPropagateDeviceConfiguration((fastybirdDeviceSN()).c_str(), configuration);
}

#endif