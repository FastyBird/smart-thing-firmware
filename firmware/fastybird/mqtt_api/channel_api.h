/*

FASTYBIRD CHANNEL MQTT MODULE

Copyright (C) 2018 FastyBird s.r.o. <info@fastybird.com>

*/

#if FASTYBIRD_SUPPORT && MQTT_SUPPORT

// -----------------------------------------------------------------------------
// MODULE PRIVATE
// -----------------------------------------------------------------------------

String _fastybirdMqttApiCreateChannelTopicString(
    const char * deviceId,
    const char * channel,
    String topic
) {
    topic.replace("{channel}", channel);

    return fastybirdMqttApiBuildTopicPrefix(deviceId) + topic;
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

String _fastybirdMqttApiCreateChannelTopicString(
    const char * deviceId,
    const char * channel,
    int channelNo,
    String topic,
    String replace,
    String replaceWith
) {
    // Create channel name with replacable suffix: somechannel_%d
    char channel_with_suffix[strlen(channel)+ 5];
    strcpy(channel_with_suffix, channel);
    strcat(channel_with_suffix, FASTYBIRD_CHANNEL_ARRAY_SUFFIX);

    char channel_topic[strlen(channel_with_suffix) + 3];
    sprintf(channel_topic, channel_with_suffix, channelNo);

    topic.replace("{channel}", channel_topic);
    topic.replace("{" + replace + "}", replaceWith);

    return fastybirdMqttApiBuildTopicPrefix(deviceId) + topic;
}

// -----------------------------------------------------------------------------
// MODULE API
// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelName(
    const char * deviceId,
    fastybird_channel_t channel
) {
    uint8_t packet_id;

    packet_id = mqttSend(
        _fastybirdMqttApiCreateChannelTopicString(
            deviceId,
            channel.name.c_str(),
            FASTYBIRD_TOPIC_CHANNEL_NAME
        ).c_str(),
        channel.name.c_str(),
        false
    );

    if (packet_id == 0) return false;

    return true;
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelName(
    fastybird_channel_t channel
) {
    return _fastybirdPropagateChannelName((fastybirdDeviceSN()).c_str(), channel);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelProperties(
    const char * deviceId,
    fastybird_channel_t channel,
    std::vector<String> properties
) {
    if (properties.size() <= 0) {
        return true;
    }

    char formatted_properties[100];

    strcpy(formatted_properties, properties[0].c_str());

    for (uint8_t i = 1; i < properties.size(); i++) {
        strcat(formatted_properties, ",");
        strcat(formatted_properties, properties[i].c_str());
    }

    uint8_t packet_id;

    packet_id = mqttSend(
        _fastybirdMqttApiCreateChannelTopicString(
            deviceId,
            channel.name.c_str(),
            FASTYBIRD_TOPIC_CHANNEL_PROPERTIES
        ).c_str(),
        formatted_properties,
        false
    );

    if (packet_id == 0) return false;

    return true;
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelProperties(
    fastybird_channel_t channel,
    std::vector<String> properties
) {
    return _fastybirdPropagateChannelProperties((fastybirdDeviceSN()).c_str(), channel, properties);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelSize(
    const char * deviceId,
    fastybird_channel_t channel
) {
    if (channel.length <= 1) {
        return true;
    }

    uint8_t packet_id;

    char array_size[6];
    sprintf(array_size, "0-%d", (channel.length - 1));

    packet_id = mqttSend(
        _fastybirdMqttApiCreateChannelTopicString(
            deviceId,
            channel.name.c_str(),
            FASTYBIRD_TOPIC_CHANNEL_ARRAY
        ).c_str(),
        array_size,
        false
    );

    if (packet_id == 0) return false;

    return true;
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelSize(
    fastybird_channel_t channel
) {
    return _fastybirdPropagateChannelSize((fastybirdDeviceSN()).c_str(), channel);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelPropertyName(
    const char * deviceId,
    fastybird_channel_t channel,
    fastybird_channel_property_t property
) {
    uint8_t packet_id;

    packet_id = mqttSend(
        _fastybirdMqttApiCreateChannelTopicString(
            deviceId,
            channel.name.c_str(),
            FASTYBIRD_TOPIC_CHANNEL_PROPERTY_NAME,
            "property",
            property.name.c_str()
        ).c_str(),
        property.name.c_str(),
        false
    );

    if (packet_id == 0) return false;

    return true;
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelPropertyName(
    fastybird_channel_t channel,
    fastybird_channel_property_t property
) {
    return _fastybirdPropagateChannelPropertyName((fastybirdDeviceSN()).c_str(), channel, property);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelPropertyType(
    const char * deviceId,
    fastybird_channel_t channel,
    fastybird_channel_property_t property
) {
    uint8_t packet_id;

    packet_id = mqttSend(
        _fastybirdMqttApiCreateChannelTopicString(
            deviceId,
            channel.name.c_str(),
            FASTYBIRD_TOPIC_CHANNEL_PROPERTY_TYPE,
            "property",
            property.name.c_str()
        ).c_str(),
        property.type.c_str(),
        false
    );

    if (packet_id == 0) return false;

    return true;
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelPropertyType(
    fastybird_channel_t channel,
    fastybird_channel_property_t property
) {
    return _fastybirdPropagateChannelPropertyType((fastybirdDeviceSN()).c_str(), channel, property);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelPropertySettable(
    const char * deviceId,
    fastybird_channel_t channel,
    fastybird_channel_property_t property
) {
    uint8_t packet_id;

    packet_id = mqttSend(
        _fastybirdMqttApiCreateChannelTopicString(
            deviceId,
            channel.name.c_str(),
            FASTYBIRD_TOPIC_CHANNEL_PROPERTY_SETTABLE,
            "property",
            property.name.c_str()
        ).c_str(),
        property.settable ? FASTYBIRD_PROPERTY_IS_SETTABLE : FASTYBIRD_PROPERTY_IS_NOT_SETTABLE,
        false
    );

    if (packet_id == 0) return false;

    String topic;

    for (uint8_t i = 0; i < channel.length; i++) {
        if (property.settable) {
            if (channel.length > 1) {
                topic = _fastybirdMqttApiCreateChannelTopicString(
                    deviceId,
                    channel.name.c_str(),
                    i,
                    FASTYBIRD_TOPIC_CHANNEL_PROPERTY_RECEIVE,
                    "property",
                    property.name.c_str()
                );

            } else {
                topic = _fastybirdMqttApiCreateChannelTopicString(
                    deviceId,
                    channel.name.c_str(),
                    FASTYBIRD_TOPIC_CHANNEL_PROPERTY_RECEIVE,
                    "property",
                    property.name.c_str()
                );
            }

            packet_id = mqttSubscribe(
                topic.c_str(),
                [i, property](const char * topic, const char * payload) {
                    property.payloadCallback((i + 1), payload);
                }
            );

            if (packet_id == 0) return false;
        }
    }

    return true;
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelPropertySettable(
    fastybird_channel_t channel,
    fastybird_channel_property_t property
) {
    return _fastybirdPropagateChannelPropertySettable((fastybirdDeviceSN()).c_str(), channel, property);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelPropertyQueryable(
    const char * deviceId,
    fastybird_channel_t channel,
    fastybird_channel_property_t property
) {
    uint8_t packet_id;

    packet_id = mqttSend(
        _fastybirdMqttApiCreateChannelTopicString(
            deviceId,
            channel.name.c_str(),
            FASTYBIRD_TOPIC_CHANNEL_PROPERTY_QUERYABLE,
            "property",
            property.name.c_str()
        ).c_str(),
        property.queryable ? FASTYBIRD_PROPERTY_IS_QUERYABLE : FASTYBIRD_PROPERTY_IS_NOT_QUERYABLE,
        false
    );

    if (packet_id == 0) return false;

    String topic;

    for (uint8_t i = 0; i < channel.length; i++) {
        if (property.queryable) {
            if (channel.length > 1) {
                topic = _fastybirdMqttApiCreateChannelTopicString(
                    deviceId,
                    channel.name.c_str(),
                    i,
                    FASTYBIRD_TOPIC_CHANNEL_PROPERTY_QUERY,
                    "property",
                    property.name.c_str()
                );

            } else {
                topic = _fastybirdMqttApiCreateChannelTopicString(
                    deviceId,
                    channel.name.c_str(),
                    FASTYBIRD_TOPIC_CHANNEL_PROPERTY_QUERY,
                    "property",
                    property.name.c_str()
                );
            }

            packet_id = mqttSubscribe(
                topic.c_str(),
                [i, property](const char * topic, const char * payload) {
                    property.queryCallback((i + 1), payload);
                }
            );

            if (packet_id == 0) return false;
        }
    }

    return true;
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelPropertyQueryable(
    fastybird_channel_t channel,
    fastybird_channel_property_t property
) {
    return _fastybirdPropagateChannelPropertyQueryable((fastybirdDeviceSN()).c_str(), channel, property);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelPropertyDataType(
    const char * deviceId,
    fastybird_channel_t channel,
    fastybird_channel_property_t property
) {
    uint8_t packet_id;

    packet_id = mqttSend(
        _fastybirdMqttApiCreateChannelTopicString(
            deviceId,
            channel.name.c_str(),
            FASTYBIRD_TOPIC_CHANNEL_PROPERTY_DATA_TYPE,
            "property",
            property.name.c_str()
        ).c_str(),
        property.dataType.c_str(),
        false
    );

    if (packet_id == 0) return false;

    return true;
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelPropertyDataType(
    fastybird_channel_t channel,
    fastybird_channel_property_t property
) {
    return _fastybirdPropagateChannelPropertyDataType((fastybirdDeviceSN()).c_str(), channel, property);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelPropertyUnit(
    const char * deviceId,
    fastybird_channel_t channel,
    fastybird_channel_property_t property
) {
    if (strcmp(property.units.c_str(), "") == 0) {
        return true;
    }

    uint8_t packet_id;

    packet_id = mqttSend(
        _fastybirdMqttApiCreateChannelTopicString(
            deviceId,
            channel.name.c_str(),
            FASTYBIRD_TOPIC_CHANNEL_PROPERTY_UNIT,
            "property",
            property.name.c_str()
        ).c_str(),
        property.units.c_str(),
        false
    );

    if (packet_id == 0) return false;

    return true;
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelPropertyUnit(
    fastybird_channel_t channel,
    fastybird_channel_property_t property
) {
    return _fastybirdPropagateChannelPropertyUnit((fastybirdDeviceSN()).c_str(), channel, property);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelPropertyFormat(
    const char * deviceId,
    fastybird_channel_t channel,
    fastybird_channel_property_t property
) {
    if (strcmp(property.format.c_str(), "") == 0) {
        return true;
    }

    uint8_t packet_id;

    packet_id = mqttSend(
        _fastybirdMqttApiCreateChannelTopicString(
            deviceId,
            channel.name.c_str(),
            FASTYBIRD_TOPIC_CHANNEL_PROPERTY_FORMAT,
            "property",
            property.name.c_str()
        ).c_str(),
        property.format.c_str(),
        false
    );

    if (packet_id == 0) return false;

    return true;
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelPropertyFormat(
    fastybird_channel_t channel,
    fastybird_channel_property_t property
) {
    return _fastybirdPropagateChannelPropertyFormat((fastybirdDeviceSN()).c_str(), channel, property);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelControlConfiguration(
    const char * deviceId,
    fastybird_channel_t channel,
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
        _fastybirdMqttApiCreateChannelTopicString(
            deviceId,
            channel.name.c_str(),
            FASTYBIRD_TOPIC_CHANNEL_CONTROL
        ).c_str(),
        payload,
        false
    );

    if (packet_id == 0) return false;

    String topic;

    for (uint8_t i = 0; i < channel.length; i++) {
        if (channel.configureCallback.size()) {
            if (channel.length > 1) {
                topic = _fastybirdMqttApiCreateChannelTopicString(
                    deviceId,
                    channel.name.c_str(),
                    i,
                    FASTYBIRD_TOPIC_CHANNEL_CONTROL_RECEIVE,
                    "control",
                    FASTYBIRD_CHANNEL_CONTROL_CONFIGURE
                );

            } else {
                topic = _fastybirdMqttApiCreateChannelTopicString(
                    deviceId,
                    channel.name.c_str(),
                    FASTYBIRD_TOPIC_CHANNEL_CONTROL_RECEIVE,
                    "control",
                    FASTYBIRD_CHANNEL_CONTROL_CONFIGURE
                );
            }

            packet_id = mqttSubscribe(
                topic.c_str(),
                [channel, i](const char * topic, const char * payload) {
                    DynamicJsonBuffer jsonBuffer;

                    // Parse payload
                    JsonObject& root = jsonBuffer.parseObject(payload);

                    if (root.success()) {
                        for(uint8_t j = 0; j < channel.configureCallback.size(); j++) {
                            DEBUG_MSG(PSTR("[FASTYBIRD] Sending configuration to channel: %s:%d\n"), channel.name.c_str(), j);

                            channel.configureCallback[j](root);
                        }

                        DEBUG_MSG(PSTR("[FASTYBIRD] Changes were saved\n"));

                        fastybirdReportChannelConfiguration(i);

                        #if WEB_SUPPORT && WS_SUPPORT
                            wsReportConfiguration();
                        #endif

                        // Reload & cache settings
                        firmwareReload();

                    } else {
                        DEBUG_MSG(PSTR("[FASTYBIRD] Error parsing settings data\n"));
                    }
                }
            );

            if (packet_id == 0) return false;
        }
    }

    return true;
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelControlConfiguration(
    fastybird_channel_t channel,
    std::vector<String> controls
) {
    return _fastybirdPropagateChannelControlConfiguration((fastybirdDeviceSN()).c_str(), channel, controls);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelConfigurationSchema(
    const char * deviceId,
    fastybird_channel_t channel,
    JsonArray& schema
) {
    uint8_t packet_id;

    if (schema.size() > 0) {
        String output;

        schema.printTo(output);

        packet_id = mqttSend(
            _fastybirdMqttApiCreateChannelTopicString(
                deviceId,
                channel.name.c_str(),
                FASTYBIRD_TOPIC_CHANNEL_CONTROL_SCHEMA,
                "control",
                FASTYBIRD_CHANNEL_CONTROL_CONFIGURE
            ).c_str(),
            output.c_str(),
            false
        );

        if (packet_id == 0) return false;
    }

    return true;
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelConfigurationSchema(
    fastybird_channel_t channel,
    JsonArray& schema
) {
    return _fastybirdPropagateChannelConfigurationSchema((fastybirdDeviceSN()).c_str(), channel, schema);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelConfiguration(
    const char * deviceId,
    fastybird_channel_t channel,
    const uint8_t channelId,
    JsonObject& configuration
) {
    if (configuration.size() > 0) {
        String output;

        configuration.printTo(output);

        String topic;

        if (channel.length > 1) {
            topic = _fastybirdMqttApiCreateChannelTopicString(
                deviceId,
                channel.name.c_str(),
                channelId,
                FASTYBIRD_TOPIC_CHANNEL_CONTROL_DATA,
                "control",
                FASTYBIRD_CHANNEL_CONTROL_CONFIGURE
            );

        } else {
            topic = _fastybirdMqttApiCreateChannelTopicString(
                deviceId,
                channel.name.c_str(),
                FASTYBIRD_TOPIC_CHANNEL_CONTROL_DATA,
                "control",
                FASTYBIRD_CHANNEL_CONTROL_CONFIGURE
            );
        }

        uint8_t packet_id;

        packet_id = mqttSend(
            topic.c_str(),
            output.c_str(),
            false
        );

        if (packet_id == 0) return false;
    }

    return true;
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelConfiguration(
    fastybird_channel_t channel,
    const uint8_t channelId,
    JsonObject& configuration
) {
    return _fastybirdPropagateChannelConfiguration((fastybirdDeviceSN()).c_str(), channel, channelId, configuration);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelValue(
    const char * deviceId,
    fastybird_channel_t channel,
    fastybird_channel_property_t property,
    const uint8_t channelId,
    const char * payload
) {
    String topic;

    topic = _fastybirdMqttApiCreateChannelTopicString(
        deviceId,
        channel.name.c_str(),
        channelId,
        FASTYBIRD_TOPIC_CHANNEL_PROPERTY,
        "property",
        property.name.c_str()
    );

    uint8_t packet_id;

    packet_id = mqttSend(
        topic.c_str(),
        payload,
        false
    );

    if (packet_id == 0) return false;

    return true;
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelValue(
    fastybird_channel_t channel,
    fastybird_channel_property_t property,
    const uint8_t channelId,
    const char * payload
) {
    return _fastybirdPropagateChannelValue((fastybirdDeviceSN()).c_str(), channel, property, channelId, payload);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelValue(
    const char * deviceId,
    fastybird_channel_t channel,
    fastybird_channel_property_t property,
    const char * payload
) {
    String topic;

    topic = _fastybirdMqttApiCreateChannelTopicString(
        deviceId,
        channel.name.c_str(),
        FASTYBIRD_TOPIC_CHANNEL_PROPERTY,
        "property",
        property.name.c_str()
    );

    uint8_t packet_id;

    packet_id = mqttSend(
        topic.c_str(),
        payload,
        true
    );

    if (packet_id == 0) return false;

    return true;
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelValue(
    fastybird_channel_t channel,
    fastybird_channel_property_t property,
    const char * payload
) {
    return _fastybirdPropagateChannelValue((fastybirdDeviceSN()).c_str(), channel, property, payload);
}

#endif
