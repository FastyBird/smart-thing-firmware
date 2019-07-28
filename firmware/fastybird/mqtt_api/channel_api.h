/*

FASTYBIRD CHANNEL MQTT MODULE

Copyright (C) 2018 FastyBird s.r.o. <info@fastybird.com>

*/

#if FASTYBIRD_SUPPORT && MQTT_SUPPORT

// -----------------------------------------------------------------------------
// MODULE PRIVATE
// -----------------------------------------------------------------------------

String _fastybirdMqttApiCreateChannelTopicString(
    const char * thingId,
    const char * channel,
    String topic
) {
    topic.replace("{channel}", channel);

    return fastybirdMqttApiBuildTopicPrefix(thingId) + topic;
}

// -----------------------------------------------------------------------------

String _fastybirdMqttApiCreateChannelTopicString(
    const char * thingId,
    const char * channel,
    String topic,
    String replace,
    String replaceWith
) {
    topic = _fastybirdMqttApiCreateChannelTopicString(thingId, channel, topic);
    topic.replace("{" + replace + "}", replaceWith);

    return topic;
}

// -----------------------------------------------------------------------------

String _fastybirdMqttApiCreateChannelTopicString(
    const char * thingId,
    const char * channel,
    String topic,
    String replace,
    String replaceWith,
    String secondReplace,
    String secondReplaceWith
) {
    topic = _fastybirdMqttApiCreateChannelTopicString(thingId, channel, topic, replace, replaceWith);
    topic.replace("{" + secondReplace + "}", secondReplaceWith);

    return topic;
}

// -----------------------------------------------------------------------------

String _fastybirdMqttApiCreateChannelTopicString(
    const char * thingId,
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

    return fastybirdMqttApiBuildTopicPrefix(thingId) + topic;
}

// -----------------------------------------------------------------------------
// MODULE API
// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelName(
    const char * thingId,
    fastybird_channel_t channel,
    const char * name
) {
    uint8_t packet_id;

    packet_id = mqttSend(
        _fastybirdMqttApiCreateChannelTopicString(
            thingId,
            _fastybirdMqttApiConvertChannelName(channel.type).c_str(),
            FASTYBIRD_TOPIC_CHANNEL_NAME
        ).c_str(),
        name
    );

    if (packet_id == 0) return false;

    return true;
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelName(
    fastybird_channel_t channel,
    const char * name
) {
    return _fastybirdPropagateChannelName((fastybirdThingSN()).c_str(), channel, name);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelType(
    const char * thingId,
    fastybird_channel_t channel,
    const char * type
) {
    uint8_t packet_id;

    packet_id = mqttSend(
        _fastybirdMqttApiCreateChannelTopicString(
            thingId,
            _fastybirdMqttApiConvertChannelName(channel.type).c_str(),
            FASTYBIRD_TOPIC_CHANNEL_TYPE
        ).c_str(),
        type
    );

    if (packet_id == 0) return false;

    return true;
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelType(
    fastybird_channel_t channel,
    const char * type
) {
    return _fastybirdPropagateChannelType((fastybirdThingSN()).c_str(), channel, type);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelProperties(
    const char * thingId,
    fastybird_channel_t channel,
    std::vector<fastybird_channel_property_t> properties
) {
    if (properties.size() <= 0) {
        return true;
    }

    char formatted_properties[50];

    strcpy(formatted_properties, properties[0].type);

    for (uint8_t i = 1; i < properties.size(); i++) {
        strcat(formatted_properties, ",");
        strcat(formatted_properties, properties[i].type);
    }

    uint8_t packet_id;

    packet_id = mqttSend(
        _fastybirdMqttApiCreateChannelTopicString(
            thingId,
            _fastybirdMqttApiConvertChannelName(channel.type).c_str(),
            FASTYBIRD_TOPIC_CHANNEL_PROPERTIES
        ).c_str(),
        formatted_properties
    );

    if (packet_id == 0) return false;

    String topic;

    for (uint8_t i = 0; i < channel.length; i++) {
        for (uint8_t j = 0; j < properties.size(); j++) {
            fastybird_channel_property_t property = properties[j];

            if (property.settable) {
                if (channel.length > 1) {
                    topic = _fastybirdMqttApiCreateChannelTopicString(
                        thingId,
                        _fastybirdMqttApiConvertChannelName(channel.type).c_str(),
                        i,
                        FASTYBIRD_TOPIC_CHANNEL_PROPERTY_RECEIVE,
                        "property",
                        property.type
                    );

                } else {
                    topic = _fastybirdMqttApiCreateChannelTopicString(
                        thingId,
                        _fastybirdMqttApiConvertChannelName(channel.type).c_str(),
                        FASTYBIRD_TOPIC_CHANNEL_PROPERTY_RECEIVE,
                        "property",
                        property.type
                    );
                }

                packet_id = mqttSubscribe(
                    topic.c_str(),
                    [channel, i, property](const char * topic, const char * payload) {
                        property.payloadCallback(i, payload);
                    }
                );

                if (packet_id == 0) return false;
            }

            if (property.queryable) {
                if (channel.length > 1) {
                    topic = _fastybirdMqttApiCreateChannelTopicString(
                        thingId,
                        _fastybirdMqttApiConvertChannelName(channel.type).c_str(),
                        i,
                        FASTYBIRD_TOPIC_CHANNEL_PROPERTY_QUERY,
                        "property",
                        property.type
                    );

                } else {
                    topic = _fastybirdMqttApiCreateChannelTopicString(
                        thingId,
                        _fastybirdMqttApiConvertChannelName(channel.type).c_str(),
                        FASTYBIRD_TOPIC_CHANNEL_PROPERTY_QUERY,
                        "property",
                        property.type
                    );
                }

                packet_id = mqttSubscribe(
                    topic.c_str(),
                    [channel, i, property](const char * topic, const char * payload) {
                        property.queryCallback(i, payload);
                    }
                );

                if (packet_id == 0) return false;
            }
        }
    }

    return true;
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelProperties(
    fastybird_channel_t channel,
    std::vector<fastybird_channel_property_t> properties
) {
    return _fastybirdPropagateChannelProperties((fastybirdThingSN()).c_str(), channel, properties);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelSize(
    const char * thingId,
    fastybird_channel_t channel,
    uint8_t size
) {
    if (size <= 1) {
        return true;
    }

    uint8_t packet_id;

    char array_size[6];
    sprintf(array_size, "0-%d", (size - 1));

    packet_id = mqttSend(
        _fastybirdMqttApiCreateChannelTopicString(
            thingId,
            _fastybirdMqttApiConvertChannelName(channel.type).c_str(),
            FASTYBIRD_TOPIC_CHANNEL_ARRAY
        ).c_str(),
        array_size
    );

    if (packet_id == 0) return false;

    return true;
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelSize(
    fastybird_channel_t channel,
    uint8_t size
) {
    return _fastybirdPropagateChannelSize((fastybirdThingSN()).c_str(), channel, size);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelPropertyName(
    const char * thingId,
    fastybird_channel_t channel,
    fastybird_channel_property_t property
) {
    uint8_t packet_id;

    packet_id = mqttSend(
        _fastybirdMqttApiCreateChannelTopicString(
            thingId,
            _fastybirdMqttApiConvertChannelName(channel.type).c_str(),
            FASTYBIRD_TOPIC_CHANNEL_PROPERTY_NAME,
            "property",
            property.type
        ).c_str(),
        property.name
    );

    if (packet_id == 0) return false;

    return true;
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelPropertyName(
    fastybird_channel_t channel,
    fastybird_channel_property_t property
) {
    return _fastybirdPropagateChannelPropertyName((fastybirdThingSN()).c_str(), channel, property);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelPropertySettable(
    const char * thingId,
    fastybird_channel_t channel,
    fastybird_channel_property_t property
) {
    uint8_t packet_id;

    packet_id = mqttSend(
        _fastybirdMqttApiCreateChannelTopicString(
            thingId,
            _fastybirdMqttApiConvertChannelName(channel.type).c_str(),
            FASTYBIRD_TOPIC_CHANNEL_PROPERTY_SETTABLE,
            "property",
            property.type
        ).c_str(),
        property.settable ? FASTYBIRD_PROPERTY_IS_SETTABLE : FASTYBIRD_PROPERTY_IS_NOT_SETTABLE
    );

    if (packet_id == 0) return false;

    return true;
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelPropertySettable(
    fastybird_channel_t channel,
    fastybird_channel_property_t property
) {
    return _fastybirdPropagateChannelPropertySettable((fastybirdThingSN()).c_str(), channel, property);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelPropertyQueryable(
    const char * thingId,
    fastybird_channel_t channel,
    fastybird_channel_property_t property
) {
    uint8_t packet_id;

    packet_id = mqttSend(
        _fastybirdMqttApiCreateChannelTopicString(
            thingId,
            _fastybirdMqttApiConvertChannelName(channel.type).c_str(),
            FASTYBIRD_TOPIC_CHANNEL_PROPERTY_QUERYABLE,
            "property",
            property.type
        ).c_str(),
        property.queryable ? FASTYBIRD_PROPERTY_IS_QUERYABLE : FASTYBIRD_PROPERTY_IS_NOT_QUERYABLE
    );

    if (packet_id == 0) return false;

    return true;
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelPropertyQueryable(
    fastybird_channel_t channel,
    fastybird_channel_property_t property
) {
    return _fastybirdPropagateChannelPropertyQueryable((fastybirdThingSN()).c_str(), channel, property);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelPropertyDataType(
    const char * thingId,
    fastybird_channel_t channel,
    fastybird_channel_property_t property
) {
    uint8_t packet_id;

    packet_id = mqttSend(
        _fastybirdMqttApiCreateChannelTopicString(
            thingId,
            _fastybirdMqttApiConvertChannelName(channel.type).c_str(),
            FASTYBIRD_TOPIC_CHANNEL_PROPERTY_DATA_TYPE,
            "property",
            property.type
        ).c_str(),
        property.dataType
    );

    if (packet_id == 0) return false;

    return true;
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelPropertyDataType(
    fastybird_channel_t channel,
    fastybird_channel_property_t property
) {
    return _fastybirdPropagateChannelPropertyDataType((fastybirdThingSN()).c_str(), channel, property);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelPropertyFormat(
    const char * thingId,
    fastybird_channel_t channel,
    fastybird_channel_property_t property
) {
    if (property.format.size() <= 0) {
        return true;
    }

    char payload[30];

    strcpy(payload, property.format[0].c_str());

    for (uint8_t i = 1; i < property.format.size(); i++) {
        strcat(payload, ",");
        strcat(payload, property.format[i].c_str());
    }

    uint8_t packet_id;

    packet_id = mqttSend(
        _fastybirdMqttApiCreateChannelTopicString(
            thingId,
            _fastybirdMqttApiConvertChannelName(channel.type).c_str(),
            FASTYBIRD_TOPIC_CHANNEL_PROPERTY_FORMAT,
            "property",
            property.type
        ).c_str(),
        payload
    );

    if (packet_id == 0) return false;

    return true;
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelPropertyFormat(
    fastybird_channel_t channel,
    fastybird_channel_property_t property
) {
    return _fastybirdPropagateChannelPropertyFormat((fastybirdThingSN()).c_str(), channel, property);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelPropertyMapping(
    const char * thingId,
    fastybird_channel_t channel,
    const char * property,
    fastybird_channel_property_mapping_t mapping
) {
    uint8_t packet_id;

    packet_id = mqttSend(
        _fastybirdMqttApiCreateChannelTopicString(
            thingId,
            _fastybirdMqttApiConvertChannelName(channel.type).c_str(),
            FASTYBIRD_TOPIC_CHANNEL_PROPERTY_MAPPING,
            "property",
            property,
            "mapping",
            mapping.from
        ).c_str(),
        mapping.to
    );

    if (packet_id == 0) return false;

    return true;
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelPropertyMappings(
    const char * thingId,
    fastybird_channel_t channel,
    fastybird_channel_property_t property
) {
    if (property.mappings.size() <= 0) {
        return true;
    }

    for (uint8_t i = 0; i < property.mappings.size(); i++) {
        if (!_fastybirdPropagateChannelPropertyMapping(thingId, channel, property.type, property.mappings[i])) {
            return false;
        }
    }

    return true;
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelPropertyMappings(
    fastybird_channel_t channel,
    fastybird_channel_property_t property
) {
    return _fastybirdPropagateChannelPropertyMappings((fastybirdThingSN()).c_str(), channel, property);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelControlConfiguration(
    const char * thingId,
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
            thingId,
            _fastybirdMqttApiConvertChannelName(channel.type).c_str(),
            FASTYBIRD_TOPIC_CHANNEL_CONTROL
        ).c_str(),
        payload
    );

    if (packet_id == 0) return false;

    String topic;

    for (uint8_t i = 0; i < channel.length; i++) {
        if (channel.isConfigurable) {
            if (channel.length > 1) {
                topic = _fastybirdMqttApiCreateChannelTopicString(
                    thingId,
                    _fastybirdMqttApiConvertChannelName(channel.type).c_str(),
                    i,
                    FASTYBIRD_TOPIC_CHANNEL_CONTROL_RECEIVE,
                    "control",
                    FASTYBIRD_CHANNEL_CONTROL_CONFIGURE
                );

            } else {
                topic = _fastybirdMqttApiCreateChannelTopicString(
                    thingId,
                    _fastybirdMqttApiConvertChannelName(channel.type).c_str(),
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
                        DEBUG_MSG(PSTR("[FASTYBIRD] Sending configuration to channel: %s:%d\n"), channel.type, i);

                        channel.configureCallback(i, root);

                        DEBUG_MSG(PSTR("[FASTYBIRD] Changes were saved\n"));

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

        #if SCHEDULER_SUPPORT
            if (channel.hasScheduler) {
                if (channel.length > 1) {
                    topic = _fastybirdMqttApiCreateChannelTopicString(
                        thingId,
                        _fastybirdMqttApiConvertChannelName(channel.type).c_str(),
                        i,
                        FASTYBIRD_TOPIC_CHANNEL_CONTROL_RECEIVE,
                        "control",
                        FASTYBIRD_CHANNEL_CONTROL_SCHEDULE
                    );

                } else {
                    topic = _fastybirdMqttApiCreateChannelTopicString(
                        thingId,
                        _fastybirdMqttApiConvertChannelName(channel.type).c_str(),
                        FASTYBIRD_TOPIC_CHANNEL_CONTROL_RECEIVE,
                        "control",
                        FASTYBIRD_CHANNEL_CONTROL_SCHEDULE
                    );
                }

                packet_id = mqttSubscribe(
                    topic.c_str(),
                    [channel, i](const char * topic, const char * payload) {
                        DynamicJsonBuffer jsonBuffer;

                        // Parse payload
                        JsonArray& root = jsonBuffer.parseArray(payload);

                        if (root.success()) {
                            DEBUG_MSG(PSTR("[FASTYBIRD] Sending scheduler configuration to channel: %s\n"), channel.type);

                            channel.configureSchedulesCallback(i, root);

                            DEBUG_MSG(PSTR("[FASTYBIRD] Changes were saved\n"));

                            #if WEB_SUPPORT && WS_SUPPORT
                                wsReportConfiguration();
                            #endif

                            // Reload & cache settings
                            firmwareReload();

                        } else {
                            DEBUG_MSG(PSTR("[FASTYBIRD] Error parsing scheduler configuration data\n"));
                        }
                    }
                );

                if (packet_id == 0) return false;
            }
        #endif
    }

    return true;
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelControlConfiguration(
    fastybird_channel_t channel,
    std::vector<String> controls
) {
    return _fastybirdPropagateChannelControlConfiguration((fastybirdThingSN()).c_str(), channel, controls);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelConfigurationSchema(
    const char * thingId,
    fastybird_channel_t channel,
    String payload
) {
    uint8_t packet_id;

    packet_id = mqttSend(
        _fastybirdMqttApiCreateChannelTopicString(
            thingId,
            _fastybirdMqttApiConvertChannelName(channel.type).c_str(),
            FASTYBIRD_TOPIC_CHANNEL_CONTROL_SCHEMA,
            "control",
            FASTYBIRD_CHANNEL_CONTROL_CONFIGURE
        ).c_str(),
        payload.c_str()
    );

    if (packet_id == 0) return false;

    return true;
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelConfigurationSchema(
    fastybird_channel_t channel,
    String payload
) {
    return _fastybirdPropagateChannelConfigurationSchema((fastybirdThingSN()).c_str(), channel, payload);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelConfiguration(
    const char * thingId,
    fastybird_channel_t channel,
    const uint8_t channelId,
    String payload
) {
    String topic;

    if (channel.length > 1) {
        topic = _fastybirdMqttApiCreateChannelTopicString(
            thingId,
            _fastybirdMqttApiConvertChannelName(channel.type).c_str(),
            channelId,
            FASTYBIRD_TOPIC_CHANNEL_CONTROL_DATA,
            "control",
            FASTYBIRD_CHANNEL_CONTROL_CONFIGURE
        );

    } else {
        topic = _fastybirdMqttApiCreateChannelTopicString(
            thingId,
            _fastybirdMqttApiConvertChannelName(channel.type).c_str(),
            FASTYBIRD_TOPIC_CHANNEL_CONTROL_DATA,
            "control",
            FASTYBIRD_CHANNEL_CONTROL_CONFIGURE
        );
    }

    uint8_t packet_id;

    packet_id = mqttSend(
        topic.c_str(),
        payload.c_str()
    );

    if (packet_id == 0) return false;

    return true;
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelConfiguration(
    fastybird_channel_t channel,
    const uint8_t channelId,
    String payload
) {
    return _fastybirdPropagateChannelConfiguration((fastybirdThingSN()).c_str(), channel, channelId, payload);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelSchedulerConfiguration(
    const char * thingId,
    fastybird_channel_t channel,
    const uint8_t channelId,
    String payload
) {
    String topic;

    if (channel.length > 1) {
        topic = _fastybirdMqttApiCreateChannelTopicString(
            thingId,
            _fastybirdMqttApiConvertChannelName(channel.type).c_str(),
            channelId,
            FASTYBIRD_TOPIC_CHANNEL_CONTROL_DATA,
            "control",
            FASTYBIRD_CHANNEL_CONTROL_SCHEDULE
        );

    } else {
        topic = _fastybirdMqttApiCreateChannelTopicString(
            thingId,
            _fastybirdMqttApiConvertChannelName(channel.type).c_str(),
            FASTYBIRD_TOPIC_CHANNEL_CONTROL_DATA,
            "control",
            FASTYBIRD_CHANNEL_CONTROL_SCHEDULE
        );
    }

    uint8_t packet_id;

    packet_id = mqttSend(
        topic.c_str(),
        payload.c_str()
    );

    if (packet_id == 0) return false;

    return true;
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelSchedulerConfiguration(
    fastybird_channel_t channel,
    const uint8_t channelId,
    String payload
) {
    return _fastybirdPropagateChannelSchedulerConfiguration((fastybirdThingSN()).c_str(), channel, channelId, payload);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelValue(
    const char * thingId,
    fastybird_channel_t channel,
    fastybird_channel_property_t property,
    const uint8_t channelId,
    const char * payload
) {
    String topic;

    if (channel.length > 1) {
        topic = _fastybirdMqttApiCreateChannelTopicString(
            thingId,
            _fastybirdMqttApiConvertChannelName(channel.type).c_str(),
            channelId,
            FASTYBIRD_TOPIC_CHANNEL_PROPERTY,
            "property",
            property.type
        );

    } else {
        topic = _fastybirdMqttApiCreateChannelTopicString(
            thingId,
            _fastybirdMqttApiConvertChannelName(channel.type).c_str(),
            FASTYBIRD_TOPIC_CHANNEL_PROPERTY,
            "property",
            property.type
        );
    }

    uint8_t packet_id;

    packet_id = mqttSend(
        topic.c_str(),
        payload
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
    return _fastybirdPropagateChannelValue((fastybirdThingSN()).c_str(), channel, property, channelId, payload);
}

#endif
