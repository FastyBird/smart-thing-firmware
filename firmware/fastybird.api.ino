/*

FASTYBIRD MQTT API MODULE

Copyright (C) 2018 FastyBird s.r.o. <code@fastybird.com>

*/

#if MQTT_SUPPORT

std::vector<String> _fastybird_topic_parts;

// -----------------------------------------------------------------------------
// MODULE: SUB-MODULE PRIVATE
// -----------------------------------------------------------------------------

String _fastybirdMqttApiCreateTopic(
    String topic
) {
    // Replace identifier
    String topic_prefix = FASTYBIRD_MQTT_BASE_TOPIC;

    // Check if topic prefix is ending with "/"
    if (!topic_prefix.endsWith("/")) {
        // ...if not, add it
        topic_prefix = topic_prefix + "/";
    }

    return topic_prefix + topic;
}

// -----------------------------------------------------------------------------

String _fastybirdMqttApiCreateDeviceTopic(
    const char * deviceId
) {
    String topic = _fastybirdMqttApiCreateTopic(String(deviceId));

    // Check if topic prefix is ending with "/"
    if (!topic.endsWith("/")) {
        // ...if not, add it
        topic = topic + "/";
    }

    // All topic parts have to lowercased
    topic.toLowerCase();

    return topic;
}

// -----------------------------------------------------------------------------

String _fastybirdMqttApiCreateDeviceTopicString(
    const char * deviceId,
    String topic
) {
    return _fastybirdMqttApiCreateDeviceTopic(deviceId) + topic;
}

// -----------------------------------------------------------------------------

String _fastybirdMqttApiCreateDeviceTopicString(
    const char * deviceId,
    String topic,
    String replace,
    String replaceWith
) {
    topic = _fastybirdMqttApiCreateDeviceTopicString(deviceId, topic);
    topic.replace("{" + replace + "}", replaceWith);

    return topic;
}

// -----------------------------------------------------------------------------

String _fastybirdMqttApiCreateDeviceTopicString(
    const char * deviceId,
    String topic,
    String replace,
    String replaceWith,
    String secondReplace,
    String secondReplaceWith
) {
    topic = _fastybirdMqttApiCreateDeviceTopicString(deviceId, topic, replace, replaceWith);
    topic.replace("{" + secondReplace + "}", secondReplaceWith);

    return topic;
}

// -----------------------------------------------------------------------------

String _fastybirdMqttApiCreateStateTopicString(
    const char * deviceId
) {
    return _fastybirdMqttApiCreateDeviceTopicString(
        deviceId,
        FASTYBIRD_TOPIC_DEVICE_STATE
    );
}

// -----------------------------------------------------------------------------

String _fastybirdMqttApiCreateChannelTopicString(
    const char * deviceId,
    const char * channel,
    String topic
) {
    topic = _fastybirdMqttApiCreateDeviceTopic(deviceId) + topic;
    topic.replace("{channel}", channel);

    return topic;
}

// -----------------------------------------------------------------------------

String _fastybirdMqttApiCreateChannelTopicString(
    const char * deviceId,
    const char * channel,
    String topic,
    String replace,
    String replaceWith
) {
    topic = _fastybirdMqttApiCreateChannelTopicString(deviceId, channel, topic);
    topic.replace("{" + replace + "}", replaceWith);

    return topic;
}

// -----------------------------------------------------------------------------

String _fastybirdMqttApiCreateChannelTopicString(
    const char * deviceId,
    const char * channel,
    String topic,
    String replace,
    String replaceWith,
    String secondReplace,
    String secondReplaceWith
) {
    topic = _fastybirdMqttApiCreateChannelTopicString(deviceId, channel, topic, replace, replaceWith);
    topic.replace("{" + secondReplace + "}", secondReplaceWith);

    return topic;
}

// -----------------------------------------------------------------------------

void _fastybirdMqttApiTopicMatchCallback(
    const char * match,
    const uint32_t length,
    const MatchState & topic_match
) {
    // Add string to vector
    _fastybird_topic_parts.push_back(String(match).substring(0, length));
}

// -----------------------------------------------------------------------------

void _fastybirdMqttApiMqttOnConnect()
{
    DEBUG_MSG(PSTR("[INFO][FASTYBIRD][API] MQTT connected event\n"));

    fastybirdResetDeviceInitialization();

    fastybird_device_t device = fastybirdGetDevice(FASTYBIRD_MAIN_DEVICE_INDEX);

    String topic;

    // Broker broadcast topic
    topic = _fastybirdMqttApiCreateTopic(FASTYBIRD_TOPIC_BROADCAST);

    mqttSubscribe(topic.c_str());

    // Device property set topic
    topic = _fastybirdMqttApiCreateDeviceTopicString(
        device.name,
        FASTYBIRD_TOPIC_DEVICE_PROPERTY_SET,
        "property",
        "+"
    );

    mqttSubscribe(topic.c_str());

    // Device property query topic
    topic = _fastybirdMqttApiCreateDeviceTopicString(
        device.name,
        FASTYBIRD_TOPIC_DEVICE_PROPERTY_QUERY,
        "property",
        "+"
    );

    mqttSubscribe(topic.c_str());

    // Device control topic
    topic = _fastybirdMqttApiCreateDeviceTopicString(
        device.name,
        FASTYBIRD_TOPIC_DEVICE_CONTROL_SET,
        "control",
        "+"
    );

    mqttSubscribe(topic.c_str());

    #if FASTYBIRD_MAX_CHANNELS > 0
        // Channel property set topic
        topic = _fastybirdMqttApiCreateChannelTopicString(
            device.name,
            "+",
            FASTYBIRD_TOPIC_CHANNEL_PROPERTY_SET,
            "property",
            "+"
        );

        mqttSubscribe(topic.c_str());

        // Channel property query topic
        topic = _fastybirdMqttApiCreateChannelTopicString(
            device.name,
            "+",
            FASTYBIRD_TOPIC_CHANNEL_PROPERTY_QUERY,
            "property",
            "+"
        );

        mqttSubscribe(topic.c_str());

        // Channel control topic
        topic = _fastybirdMqttApiCreateChannelTopicString(
            device.name,
            "+",
            FASTYBIRD_TOPIC_CHANNEL_CONTROL_SET,
            "control",
            "+"
        );

        mqttSubscribe(topic.c_str());
    #endif
}

// -----------------------------------------------------------------------------

void _fastybirdMqttApiMqttOnDisconnect()
{
    DEBUG_MSG(PSTR("[INFO][FASTYBIRD][API] MQTT disconnected event\n"));

    fastybirdResetDeviceInitialization();
}

// -----------------------------------------------------------------------------

void _fastybirdMqttApiMqttOnMessage(
    const char * topic,
    const char * payload
) {
    DEBUG_MSG(PSTR("[INFO][FASTYBIRD][API] MQTT message event\n"));

    if (!fastybirdIsDeviceInitialzed()) {
        DEBUG_MSG(PSTR("[INFO][FASTYBIRD][API] Skipping - Device is not initialized yet\n"));

        return;
    }

    char topic_string[strlen(topic) + 2];
    strcpy(topic_string, topic);

    MatchState topic_match (topic_string);

    // Reset topic parts storage
    _fastybird_topic_parts.clear();

    // Find how many parts are in topic
    uint8_t parts_count = topic_match.MatchCount("[^/]+");

    _fastybird_topic_parts.reserve(parts_count);

    // Pull the words out (the callback will put them in the storage)
    topic_match.GlobalMatch ("[^/]+", _fastybirdMqttApiTopicMatchCallback);

    // Broadcast topic
    if (
        parts_count == FASTYBIRD_TOPIC_PART_COUNT_BROADCAST
        && _fastybird_topic_parts[FASTYBIRD_TOPIC_POSITION_BROADCAST_PREFIX].equals(FASTYBIRD_TOPIC_PART_BROADCAST)
    ) {
        DEBUG_MSG(PSTR("[INFO][FASTYBIRD][API] Broadcast topic\n"));

        // TODO: Implement handlign broadcast messages

        return;
    }

    // Device topis
    if (parts_count > FASTYBIRD_TOPIC_POSITION_DEVICE_NAME) {
        String deviceName = _fastybird_topic_parts[FASTYBIRD_TOPIC_POSITION_DEVICE_NAME];

        uint8_t deviceIndex = fastybirdFindDeviceIndex(deviceName.c_str());

        if (deviceIndex == INDEX_NONE) {
            return;
        }

        fastybird_device_t device = fastybirdGetDevice(deviceIndex);

        // Device property topic
        if (
            parts_count == FASTYBIRD_TOPIC_PART_COUNT_DEVICE_CONTROL
            && _fastybird_topic_parts[FASTYBIRD_TOPIC_POSITION_DEVICE_PROPERTY_PREFIX].equals(FASTYBIRD_TOPIC_PART_PROPERTY)
            && (
                _fastybird_topic_parts[FASTYBIRD_TOPIC_POSITION_DEVICE_PROPERTY_ACTION].equals(FASTYBIRD_TOPIC_PART_SET)
                || _fastybird_topic_parts[FASTYBIRD_TOPIC_POSITION_DEVICE_PROPERTY_ACTION].equals(FASTYBIRD_TOPIC_PART_QUERY)
            )
        ) {
            DEBUG_MSG(PSTR("[INFO][FASTYBIRD][API] Device property topic\n"));

            String propertyName = _fastybird_topic_parts[FASTYBIRD_TOPIC_POSITION_DEVICE_PROPERTY_NAME];

            uint8_t propertyIndex = fastybirdFindDevicePropertyIndex(deviceIndex, propertyName.c_str());

            if (propertyIndex == INDEX_NONE) {
                DEBUG_MSG(PSTR("[WARN][FASTYBIRD][API] Device property: %s was not found\n"), propertyName.c_str());

                return;
            }

            fastybird_property_t property = fastybirdGetProperty(propertyIndex);

            if (
                _fastybird_topic_parts[FASTYBIRD_TOPIC_POSITION_DEVICE_PROPERTY_ACTION].equals(FASTYBIRD_TOPIC_PART_SET)
                && property.settable
            ) {
                DEBUG_MSG(PSTR("[INFO][FASTYBIRD][API] Calling device property topic setter\n"));

                property.set_callback(deviceIndex, propertyIndex, payload);

                return;
            }
            
            if (
                _fastybird_topic_parts[FASTYBIRD_TOPIC_POSITION_DEVICE_PROPERTY_ACTION].equals(FASTYBIRD_TOPIC_PART_QUERY)
                && property.queryable
            ) {
                DEBUG_MSG(PSTR("[INFO][FASTYBIRD][API] Calling device property topic getter\n"));

                property.query_callback(deviceIndex, propertyIndex);

                return;
            }

            return;
        }

        // Control device topic
        if (
            parts_count == FASTYBIRD_TOPIC_PART_COUNT_DEVICE_CONTROL
            && _fastybird_topic_parts[FASTYBIRD_TOPIC_POSITION_DEVICE_CONTROL_PREFIX].equals(FASTYBIRD_TOPIC_PART_CONTROL)
            && _fastybird_topic_parts[FASTYBIRD_TOPIC_POSITION_DEVICE_CONTROL_ACTION].equals(FASTYBIRD_TOPIC_PART_SET)
        ) {
            DEBUG_MSG(PSTR("[INFO][FASTYBIRD][API] Device control topic\n"));

            String controlName = _fastybird_topic_parts[FASTYBIRD_TOPIC_POSITION_DEVICE_CONTROL_NAME];

            uint8_t controlIndex = fastybirdFindDeviceControlIndex(deviceIndex, controlName.c_str());

            if (controlIndex == INDEX_NONE) {
                return;
            }

            fastybird_control_t control = fastybirdGetControl(controlIndex);

            control.call_callback(controlIndex, payload);

            return;
        }

        // Channel topic
        if (
            parts_count > FASTYBIRD_TOPIC_POSITION_CHANNEL_NAME
            && _fastybird_topic_parts[FASTYBIRD_TOPIC_POSITION_CHANNEL_PREFIX].equals(FASTYBIRD_TOPIC_PART_CHANNEL)
        ) {
            DEBUG_MSG(PSTR("[INFO][FASTYBIRD][API] Device channel topic\n"));

            String channelName = _fastybird_topic_parts[FASTYBIRD_TOPIC_POSITION_CHANNEL_NAME];

            uint8_t channelIndex = fastybirdFindChannelIndex(deviceIndex, channelName.c_str());

            if (channelIndex == INDEX_NONE) {
                return;
            }

            fastybird_channel_t channel = fastybirdGetChannel(channelIndex);

            // Channel property topic
            if (
                parts_count == FASTYBIRD_TOPIC_PART_COUNT_CHANNEL_PROPERTY
                && _fastybird_topic_parts[FASTYBIRD_TOPIC_POSITION_CHANNEL_PROPERTY_PREFIX].equals(FASTYBIRD_TOPIC_PART_PROPERTY)
                && (
                    _fastybird_topic_parts[FASTYBIRD_TOPIC_POSITION_CHANNEL_PROPERTY_ACTION].equals(FASTYBIRD_TOPIC_PART_SET)
                    || _fastybird_topic_parts[FASTYBIRD_TOPIC_POSITION_CHANNEL_PROPERTY_ACTION].equals(FASTYBIRD_TOPIC_PART_QUERY)
                )
            ) {
                DEBUG_MSG(PSTR("[INFO][FASTYBIRD][API] Channel property topic\n"));

                String propertyName = _fastybird_topic_parts[FASTYBIRD_TOPIC_POSITION_CHANNEL_PROPERTY_NAME];

                uint8_t propertyIndex = fastybirdFindChannelPropertyIndex(deviceIndex, channelIndex, propertyName.c_str());

                if (propertyIndex == INDEX_NONE) {
                    DEBUG_MSG(PSTR("[WARN][FASTYBIRD][API] Channel property: %s was not found\n"), propertyName.c_str());

                    return;
                }

                fastybird_property_t property = fastybirdGetProperty(propertyIndex);

                if (
                    _fastybird_topic_parts[FASTYBIRD_TOPIC_POSITION_CHANNEL_PROPERTY_ACTION].equals(FASTYBIRD_TOPIC_PART_SET)
                    && property.settable
                ) {
                    DEBUG_MSG(PSTR("[INFO][FASTYBIRD][API] Calling channel property topic setter\n"));

                    property.set_callback(channelIndex, propertyIndex, payload);

                    return;
                }
                
                if (
                    _fastybird_topic_parts[FASTYBIRD_TOPIC_POSITION_CHANNEL_PROPERTY_ACTION].equals(FASTYBIRD_TOPIC_PART_QUERY)
                    && property.queryable
                ) {
                    DEBUG_MSG(PSTR("[INFO][FASTYBIRD][API] Calling channel property topic getter\n"));

                    property.query_callback(channelIndex, propertyIndex);

                    return;
                }

                return;
            }

            // Channel control topic
            if (
                parts_count == FASTYBIRD_TOPIC_PART_COUNT_CHANNEL_CONTROL
                && _fastybird_topic_parts[FASTYBIRD_TOPIC_POSITION_CHANNEL_CONTROL_PREFIX].equals(FASTYBIRD_TOPIC_PART_CONTROL)
                && _fastybird_topic_parts[FASTYBIRD_TOPIC_POSITION_CHANNEL_CONTROL_ACTION].equals(FASTYBIRD_TOPIC_PART_SET)
            ) {
                DEBUG_MSG(PSTR("[INFO][FASTYBIRD][API] Channel control topic\n"));

                String controlName = _fastybird_topic_parts[FASTYBIRD_TOPIC_POSITION_CHANNEL_CONTROL_NAME];

                uint8_t controlIndex = fastybirdFindChannelControlIndex(deviceIndex, channelIndex, controlName.c_str());

                if (controlIndex == INDEX_NONE) {
                    return;
                }

                fastybird_control_t control = fastybirdGetControl(controlIndex);

                control.call_callback(controlIndex, payload);

                return;
            }
        }
    }
}

// -----------------------------------------------------------------------------
// MODULE: SUB-MODULE API
// -----------------------------------------------------------------------------

bool fastybirdApiIsReady()
{
    return mqttConnected();
}

// -----------------------------------------------------------------------------

void fastybirdApiSetup()
{
    fastybird_device_t device = fastybirdGetDevice(FASTYBIRD_MAIN_DEVICE_INDEX);

    mqttOnConnectRegister(_fastybirdMqttApiMqttOnConnect);
    mqttOnDisconnectRegister(_fastybirdMqttApiMqttOnDisconnect);
    mqttOnMessageRegister(_fastybirdMqttApiMqttOnMessage);

    char will_topic[100];

    strcpy(will_topic, _fastybirdMqttApiCreateStateTopicString(device.name).c_str());

    mqttSetWill(
        will_topic,
        FASTYBIRD_STATUS_LOST
    );
}

// -----------------------------------------------------------------------------
// DATA PROPAGATION
// -----------------------------------------------------------------------------

bool fastybirdApiReportDeviceState(
    const char * deviceId,
    const char * payload
) {
    uint8_t packet_id;

    packet_id = mqttSend(
        _fastybirdMqttApiCreateStateTopicString(
            deviceId
        ).c_str(),
        payload,
        true
    );

    if (packet_id == 0) return false;

    return true;
}

// -----------------------------------------------------------------------------

bool fastybirdApiReportDevicePropertyValue(
    const char * deviceId,
    const char * propertyId,
    const char * payload
) {
    uint8_t packet_id;

    packet_id = mqttSend(
        _fastybirdMqttApiCreateDeviceTopicString(
            deviceId,
            FASTYBIRD_TOPIC_DEVICE_PROPERTY,
            "property",
            propertyId
        ).c_str(),
        payload,
        true
    );

    if (packet_id == 0) return false;

    return true;
}

// -----------------------------------------------------------------------------

bool fastybirdApiReportChannelPropertyValue(
    const char * deviceId,
    const char * channelId,
    const char * propertyId,
    const char * payload
) {
    uint8_t packet_id;

    packet_id = mqttSend(
        _fastybirdMqttApiCreateChannelTopicString(
            deviceId,
            channelId,
            FASTYBIRD_TOPIC_CHANNEL_PROPERTY,
            "property",
            propertyId
        ).c_str(),
        payload,
        true
    );

    if (packet_id == 0) return false;

    return true;
}

// -----------------------------------------------------------------------------
// DEVICE PROPAGATION
// -----------------------------------------------------------------------------

bool fastybirdApiPropagateDeviceName(
    const char * deviceId,
    const char * name
) {
    uint8_t packet_id;

    packet_id = mqttSend(
        _fastybirdMqttApiCreateDeviceTopicString(
            deviceId,
            FASTYBIRD_TOPIC_DEVICE_NAME
        ).c_str(),
        name,
        false
    );

    if (packet_id == 0) return false;

    return true;
}

// -----------------------------------------------------------------------------

bool fastybirdApiPropagateDeviceHardwareField(
    const char * deviceId,
    const char * field,
    const char * payload
) {
    uint8_t packet_id;

    packet_id = mqttSend(
        _fastybirdMqttApiCreateDeviceTopicString(
            deviceId,
            FASTYBIRD_TOPIC_DEVICE_HW_INFO,
            "hw",
            field
        ).c_str(),
        payload,
        false
    );

    if (packet_id == 0) return false;

    return true;
}
// -----------------------------------------------------------------------------

bool fastybirdApiPropagateDeviceFirmwareField(
    const char * deviceId,
    const char * field,
    const char * payload
) {
    uint8_t packet_id;

    packet_id = mqttSend(
        _fastybirdMqttApiCreateDeviceTopicString(
            deviceId,
            FASTYBIRD_TOPIC_DEVICE_FW_INFO,
            "fw",
            field
        ).c_str(),
        payload,
        false
    );

    if (packet_id == 0) return false;

    return true;
}

// -----------------------------------------------------------------------------

bool fastybirdApiPropagateDevicePropertiesNames(
    const char * deviceId,
    std::vector<String> properties
) {
    uint8_t packet_id;

    String topic = _fastybirdMqttApiCreateDeviceTopicString(
        deviceId,
        FASTYBIRD_TOPIC_DEVICE_PROPERTIES
    );

    if (properties.size() > 0) {
        char payload[100];

        strcpy(payload, properties[0].c_str());

        for (uint8_t i = 1; i < properties.size(); i++) {
            strcat(payload, ",");
            strcat(payload, properties[i].c_str());
        }

        packet_id = mqttSend(
            topic.c_str(),
            payload,
            false
        );

        if (packet_id == 0) return false;

    } else {
        packet_id = mqttSend(
            topic.c_str(),
            "",
            false
        );

        if (packet_id == 0) return false;
    }

    return true;
}

// -----------------------------------------------------------------------------

bool fastybirdApiPropagateDeviceControlsNames(
    const char * deviceId,
    std::vector<String> controls
) {
    uint8_t packet_id;

    String topic = _fastybirdMqttApiCreateDeviceTopicString(
        deviceId,
        FASTYBIRD_TOPIC_DEVICE_CONTROLS
    );

    if (controls.size() > 0) {
        char payload[100];

        strcpy(payload, controls[0].c_str());

        for (uint8_t i = 1; i < controls.size(); i++) {
            strcat(payload, ",");
            strcat(payload, controls[i].c_str());
        }

        packet_id = mqttSend(
            topic.c_str(),
            payload,
            false
        );

        if (packet_id == 0) return false;

    } else {
        packet_id = mqttSend(
            topic.c_str(),
            "",
            false
        );

        if (packet_id == 0) return false;
    }

    return true;
}

// -----------------------------------------------------------------------------

bool fastybirdApiPropagateDeviceChannelsNames(
    const char * deviceId,
    std::vector<String> channels
) {
    uint8_t packet_id;

    String topic = _fastybirdMqttApiCreateDeviceTopicString(
        deviceId,
        FASTYBIRD_TOPIC_DEVICE_CHANNELS
    );

    if (channels.size() > 0) {
        char payload[300];

        strcpy(payload, channels[0].c_str());

        for (uint8_t i = 1; i < channels.size(); i++) {
            strcat(payload, ",");
            strcat(payload, channels[i].c_str());
        }

        packet_id = mqttSend(
            topic.c_str(),
            payload,
            false
        );

        if (packet_id == 0) return false;

    } else {
        packet_id = mqttSend(
            topic.c_str(),
            "",
            false
        );

        if (packet_id == 0) return false;
    }

    return true;
}

// -----------------------------------------------------------------------------
// DEVICE CHANNEL PROPAGATION
// -----------------------------------------------------------------------------

bool fastybirdApiPropagateChannelName(
    const char * deviceId,
    const char * channelId,
    const char * name
) {
    uint8_t packet_id;

    packet_id = mqttSend(
        _fastybirdMqttApiCreateChannelTopicString(
            deviceId,
            channelId,
            FASTYBIRD_TOPIC_CHANNEL_NAME
        ).c_str(),
        name,
        false
    );

    if (packet_id == 0) return false;

    return true;
}

// -----------------------------------------------------------------------------

bool fastybirdApiPropagateChannelPropertiesNames(
    const char * deviceId,
    const char * channelId,
    std::vector<String> properties
) {
    uint8_t packet_id;

    String topic = _fastybirdMqttApiCreateChannelTopicString(
        deviceId,
        channelId,
        FASTYBIRD_TOPIC_CHANNEL_PROPERTIES
    );

    if (properties.size() > 0) {
        char payload[100];

        strcpy(payload, properties[0].c_str());

        for (uint8_t i = 1; i < properties.size(); i++) {
            strcat(payload, ",");
            strcat(payload, properties[i].c_str());
        }

        packet_id = mqttSend(
            topic.c_str(),
            payload,
            false
        );

        if (packet_id == 0) return false;

    } else {
        packet_id = mqttSend(
            topic.c_str(),
            "",
            false
        );

        if (packet_id == 0) return false;
    }

    return true;
}

// -----------------------------------------------------------------------------

bool fastybirdApiPropagateChannelControlsNames(
    const char * deviceId,
    const char * channelId,
    std::vector<String> controls
) {
    uint8_t packet_id;

    String topic = _fastybirdMqttApiCreateChannelTopicString(
        deviceId,
        channelId,
        FASTYBIRD_TOPIC_CHANNEL_CONTROLS
    );

    if (controls.size() > 0) {
        char payload[100];

        strcpy(payload, controls[0].c_str());

        for (uint8_t i = 1; i < controls.size(); i++) {
            strcat(payload, ",");
            strcat(payload, controls[i].c_str());
        }

        packet_id = mqttSend(
            topic.c_str(),
            payload,
            false
        );

        if (packet_id == 0) return false;

    } else {
        packet_id = mqttSend(
            topic.c_str(),
            "",
            false
        );

        if (packet_id == 0) return false;
    }

    return true;
}

// -----------------------------------------------------------------------------
// DEVICE CHANNEL PROPERTY PROPAGATION
// -----------------------------------------------------------------------------

bool fastybirdApiPropagateChannelPropertyName(
    const char * deviceId,
    const char * channelId,
    const char * propertyId,
    const char * name
) {
    uint8_t packet_id;

    packet_id = mqttSend(
        _fastybirdMqttApiCreateChannelTopicString(
            deviceId,
            channelId,
            FASTYBIRD_TOPIC_CHANNEL_PROPERTY_NAME,
            "property",
            propertyId
        ).c_str(),
        name,
        false
    );

    if (packet_id == 0) return false;

    return true;
}

// -----------------------------------------------------------------------------

bool fastybirdApiPropagateChannelPropertySettable(
    const char * deviceId,
    const char * channelId,
    const char * propertyId,
    bool settable
) {
    uint8_t packet_id;

    packet_id = mqttSend(
        _fastybirdMqttApiCreateChannelTopicString(
            deviceId,
            channelId,
            FASTYBIRD_TOPIC_CHANNEL_PROPERTY_SETTABLE,
            "property",
            propertyId
        ).c_str(),
        settable ? FASTYBIRD_PROPERTY_IS_SETTABLE : FASTYBIRD_PROPERTY_IS_NOT_SETTABLE,
        false
    );

    if (packet_id == 0) return false;

    return true;
}

// -----------------------------------------------------------------------------

bool fastybirdApiPropagateChannelPropertyQueryable(
    const char * deviceId,
    const char * channelId,
    const char * propertyId,
    bool queryable
) {
    uint8_t packet_id;

    packet_id = mqttSend(
        _fastybirdMqttApiCreateChannelTopicString(
            deviceId,
            channelId,
            FASTYBIRD_TOPIC_CHANNEL_PROPERTY_QUERYABLE,
            "property",
            propertyId
        ).c_str(),
        queryable ? FASTYBIRD_PROPERTY_IS_QUERYABLE : FASTYBIRD_PROPERTY_IS_NOT_QUERYABLE,
        false
    );

    if (packet_id == 0) return false;

    return true;
}

// -----------------------------------------------------------------------------

bool fastybirdApiPropagateChannelPropertyDataType(
    const char * deviceId,
    const char * channelId,
    const char * propertyId,
    const char * dataType
) {
    uint8_t packet_id;

    packet_id = mqttSend(
        _fastybirdMqttApiCreateChannelTopicString(
            deviceId,
            channelId,
            FASTYBIRD_TOPIC_CHANNEL_PROPERTY_DATA_TYPE,
            "property",
            propertyId
        ).c_str(),
        dataType,
        false
    );

    if (packet_id == 0) return false;

    return true;
}

// -----------------------------------------------------------------------------

bool fastybirdApiPropagateChannelPropertyUnit(
    const char * deviceId,
    const char * channelId,
    const char * propertyId,
    const char * unit
) {
    uint8_t packet_id;

    packet_id = mqttSend(
        _fastybirdMqttApiCreateChannelTopicString(
            deviceId,
            channelId,
            FASTYBIRD_TOPIC_CHANNEL_PROPERTY_UNIT,
            "property",
            propertyId
        ).c_str(),
        unit,
        false
    );

    if (packet_id == 0) return false;

    return true;
}

// -----------------------------------------------------------------------------

bool fastybirdApiPropagateChannelPropertyFormat(
    const char * deviceId,
    const char * channelId,
    const char * propertyId,
    const char * format
) {
    uint8_t packet_id;

    packet_id = mqttSend(
        _fastybirdMqttApiCreateChannelTopicString(
            deviceId,
            channelId,
            FASTYBIRD_TOPIC_CHANNEL_PROPERTY_FORMAT,
            "property",
            propertyId
        ).c_str(),
        format,
        false
    );

    if (packet_id == 0) return false;

    return true;
}

#endif
