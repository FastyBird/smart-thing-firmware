/*

FASTYBIRD MQTT API MODULE

Copyright (C) 2018 FastyBird Ltd. <info@fastybird.com>

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

bool _fastybirdMqttApiIsSameTopic(
    const char * compareTopic,
    const char * topic
) {
    return (String(topic)).endsWith(String(compareTopic));
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
    topic.replace("{" + replace + "}", replaceWith);

    return _fastybirdMqttApiCreateDeviceTopic(deviceId) + topic;
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

String _fastybirdMqttApiCreateChannelTopicString(
    const char * deviceId,
    const char * channel,
    String topic
) {
    topic.replace("{channel}", channel);

    return _fastybirdMqttApiCreateDeviceTopic(deviceId) + topic;
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

void _fastybirdMqttApiPropertyTopicMatchCallback(
    const char * match,
    const unsigned int length,
    const MatchState & topic_match
) {
    // Add string to vector
    _fastybird_topic_parts.push_back(String(match).substring(0, length));
}

// -----------------------------------------------------------------------------

#if FASTYBIRD_MAX_CHANNELS > 0
    void _fastybirdMqttApiMqttHandleChannelProperty(
        String channelName,
        String propertyName,
        String action,
        const char * payload
    ) {
        // Find channel index by name
        uint8_t channelIndex = fastybirdFindChannelIndex(channelName);

        // Channel was found
        if (channelIndex != INDEX_NONE) {
            DEBUG_MSG(PSTR("[INFO][FASTYBIRD][API] Channel was found\n"));

            // Find channel property index by name
            uint8_t propertyIndex = fastybirdFindChannelPropertyIndex(channelIndex, propertyName);

            if (propertyIndex != INDEX_NONE) {
                DEBUG_MSG(PSTR("[INFO][FASTYBIRD][API] Channel property was found\n"));

                fastybird_property_t property = fastybirdGetProperty(propertyIndex);

                // Combination of channel & property was found

                if (
                    action.equals(FASTYBIRD_TOPIC_PART_SET)
                    && property.settable
                ) {
                    property.payload_callback(channelIndex, propertyIndex, payload);

                } else if (
                    action.equals(FASTYBIRD_TOPIC_PART_QUERY)
                    && property.queryable
                ) {
                    property.query_callback(channelIndex, propertyIndex);
                }

            } else {
                DEBUG_MSG(PSTR("[ERR][FASTYBIRD][API] Channel property was not found\n"));
            }

        } else {
            DEBUG_MSG(PSTR("[ERR][FASTYBIRD][API] Channel was not found\n"));
        }
    }
#endif

// -----------------------------------------------------------------------------

void _fastybirdMqttApiMqttOnConnect()
{
    DEBUG_MSG(PSTR("[INFO][FASTYBIRD][API] MQTT connected event\n"));

    fastybirdResetDeviceInitialization();

    String topic;

    // Initialize device topic
    topic = _fastybirdMqttApiCreateTopic(FASTYBIRD_TOPIC_BROADCAST);

    mqttSubscribe(topic.c_str());

    // Control device topic
    topic = _fastybirdMqttApiCreateDeviceTopicString(
        fastybirdDeviceIdentifier().c_str(),
        FASTYBIRD_TOPIC_DEVICE_CONTROL_RECEIVE,
        "control",
        "+"
    );

    mqttSubscribe(topic.c_str());

    #if FASTYBIRD_MAX_CHANNELS > 0
        // Control channel property request topic
        topic = _fastybirdMqttApiCreateChannelTopicString(
            fastybirdDeviceIdentifier().c_str(),
            "+",
            FASTYBIRD_TOPIC_CHANNEL_PROPERTY_RECEIVE,
            "property",
            "+"
        );

        mqttSubscribe(topic.c_str());

        // Control channel property query topic
        topic = _fastybirdMqttApiCreateChannelTopicString(
            fastybirdDeviceIdentifier().c_str(),
            "+",
            FASTYBIRD_TOPIC_CHANNEL_PROPERTY_QUERY,
            "property",
            "+"
        );

        mqttSubscribe(topic.c_str());
    #endif

    #if FASTYBIRD_MAX_CHANNELS > 0
        // Control channel topic
        topic = _fastybirdMqttApiCreateChannelTopicString(
            fastybirdDeviceIdentifier().c_str(),
            "+",
            FASTYBIRD_TOPIC_CHANNEL_CONTROL_RECEIVE,
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
    topic_match.GlobalMatch ("[^/]+", _fastybirdMqttApiPropertyTopicMatchCallback);

    // Broadcast topic
    if (
        parts_count == FASTYBIRD_TOPIC_PART_COUNT_BROADCAST
        && _fastybird_topic_parts[FASTYBIRD_TOPIC_POSITION_BROADCAST_PREFIX].equals(FASTYBIRD_TOPIC_PART_BROADCAST)
    ) {
        DEBUG_MSG(PSTR("[INFO][FASTYBIRD][API] Broadcast topic\n"));

        if (_fastybird_topic_parts[FASTYBIRD_TOPIC_POSITION_BROADCAST_ACTION].equals(FASTYBIRD_TOPIC_PART_INIT)) {
            fastybirdResetDeviceInitialization();
        }

        return;
    }

    // Check for device info in topic
    if (
        parts_count > FASTYBIRD_TOPIC_POSITION_DEVICE
        && fastybirdDeviceIdentifier().equals(_fastybird_topic_parts[FASTYBIRD_TOPIC_POSITION_DEVICE])
    ) {
        // Control device topic
        if (
            parts_count == FASTYBIRD_TOPIC_PART_COUNT_DEVICE_CONTROL
            && _fastybird_topic_parts[FASTYBIRD_TOPIC_POSITION_DEVICE_CONTROL_PREFIX].equals(FASTYBIRD_TOPIC_PART_CONTROL)
            && _fastybird_topic_parts[FASTYBIRD_TOPIC_POSITION_DEVICE_CONTROL_ACTION].equals(FASTYBIRD_TOPIC_PART_SET)
        ) {
            DEBUG_MSG(PSTR("[INFO][FASTYBIRD][API] Device control topic\n"));

            fastybirdCallOnControlRegister(_fastybird_topic_parts[FASTYBIRD_TOPIC_POSITION_DEVICE_CONTROL_NAME], payload);

        #if FASTYBIRD_MAX_CHANNELS > 0
            // Control channel topic
            } else if (
                parts_count == FASTYBIRD_TOPIC_PART_COUNT_CHANNEL_CONTROL
                && _fastybird_topic_parts[FASTYBIRD_TOPIC_POSITION_CHANNEL_PREFIX].equals(FASTYBIRD_TOPIC_PART_CHANNEL)
                && _fastybird_topic_parts[FASTYBIRD_TOPIC_POSITION_CHANNEL_CONTROL_PREFIX].equals(FASTYBIRD_TOPIC_PART_CONTROL)
                && _fastybird_topic_parts[FASTYBIRD_TOPIC_POSITION_CHANNEL_CONTROL_ACTION].equals(FASTYBIRD_TOPIC_PART_SET)
            ) {
                DEBUG_MSG(PSTR("[INFO][FASTYBIRD][API] Channel control topic\n"));

                // Configure channel
                if (_fastybird_topic_parts[FASTYBIRD_TOPIC_POSITION_CHANNEL_CONTROL_NAME].equals(FASTYBIRD_CHANNEL_CONTROL_CONFIGURE)) {
                    DynamicJsonBuffer jsonBuffer;

                    // Parse payload
                    JsonObject& root = jsonBuffer.parseObject(payload);

                    if (root.success()) {
                        // Find channel index by name
                        uint8_t channelIndex = fastybirdFindChannelIndex(_fastybird_topic_parts[FASTYBIRD_TOPIC_POSITION_CHANNEL_NAME]);

                        // Channel was found
                        if (channelIndex != INDEX_NONE) {
                            fastybird_channel_t channel = fastybirdGetChannel(channelIndex);

                            DEBUG_MSG(PSTR("[INFO][FASTYBIRD][API] Sending configuration to channel: %s\n"), channel.name.c_str());

                            fastybirdCallOnChannelConfigure(channelIndex, root);

                            DEBUG_MSG(PSTR("[INFO][FASTYBIRD][API] Changes were saved\n"));

                            DynamicJsonBuffer jsonBuffer;

                            JsonObject& configuration = jsonBuffer.createObject();

                            fastybirdCallReportChannelConfiguration(channelIndex, configuration);

                            if (configuration.size() > 0) {
                                fastybirdApiPropagateChannelConfiguration(channel.name.c_str(), configuration);
                            }

                            #if WEB_SUPPORT && WS_SUPPORT
                                wsReportConfiguration();
                            #endif

                            // Reload & cache settings
                            firmwareReload();

                        } else {
                            DEBUG_MSG(PSTR("[ERR][FASTYBIRD][API] Channel to update was not found\n"));
                        }

                    } else {
                        DEBUG_MSG(PSTR("[ERR][FASTYBIRD][API] Parsing configuration data failed\n"));
                    }
                }

            // Set or Query channel property topic
            } else if (
                parts_count == FASTYBIRD_TOPIC_PART_COUNT_CHANNEL_PROPERTY
                && _fastybird_topic_parts[FASTYBIRD_TOPIC_POSITION_CHANNEL_PREFIX].equals(FASTYBIRD_TOPIC_PART_CHANNEL)
                && _fastybird_topic_parts[FASTYBIRD_TOPIC_POSITION_CHANNEL_PROPERTY_PREFIX].equals(FASTYBIRD_TOPIC_PART_PROPERTY)
                && (
                    _fastybird_topic_parts[FASTYBIRD_TOPIC_POSITION_CHANNEL_PROPERTY_ACTION].equals(FASTYBIRD_TOPIC_PART_SET)
                    || _fastybird_topic_parts[FASTYBIRD_TOPIC_POSITION_CHANNEL_PROPERTY_ACTION].equals(FASTYBIRD_TOPIC_PART_QUERY)
                )
            ) {
                DEBUG_MSG(PSTR("[INFO][FASTYBIRD][API] Channel property topic\n"));

                _fastybirdMqttApiMqttHandleChannelProperty(
                    _fastybird_topic_parts[FASTYBIRD_TOPIC_POSITION_CHANNEL_NAME],
                    _fastybird_topic_parts[FASTYBIRD_TOPIC_POSITION_CHANNEL_PROPERTY_NAME],
                    _fastybird_topic_parts[FASTYBIRD_TOPIC_POSITION_CHANNEL_PROPERTY_ACTION],
                    payload
                );
        #endif
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
    mqttOnConnectRegister(_fastybirdMqttApiMqttOnConnect);
    mqttOnDisconnectRegister(_fastybirdMqttApiMqttOnDisconnect);
    mqttOnMessageRegister(_fastybirdMqttApiMqttOnMessage);

    char will_topic[100];
    
    strcpy(will_topic, _fastybirdMqttApiCreateStateTopicString(fastybirdDeviceIdentifier().c_str()).c_str());

    mqttSetWill(
        will_topic,
        FASTYBIRD_STATUS_LOST
    );
}

// -----------------------------------------------------------------------------

void fastybirdApiReset()
{
    if (mqttConnected()) {
        // Unsubscribe from all device topics
        mqttUnsubscribe((_fastybirdMqttApiCreateDeviceTopic(fastybirdDeviceIdentifier().c_str()) + "#").c_str());

        String topic;

        // Initialize device topic
        topic = _fastybirdMqttApiCreateTopic(FASTYBIRD_TOPIC_BROADCAST);

        mqttUnsubscribe(topic.c_str());

        // Control device topic
        topic = _fastybirdMqttApiCreateDeviceTopicString(
            fastybirdDeviceIdentifier().c_str(),
            FASTYBIRD_TOPIC_DEVICE_CONTROL_RECEIVE,
            "control",
            "+"
        );

        mqttUnsubscribe(topic.c_str());

        #if FASTYBIRD_MAX_CHANNELS > 0
            // Control channel property request topic
            topic = _fastybirdMqttApiCreateChannelTopicString(
                fastybirdDeviceIdentifier().c_str(),
                "+",
                FASTYBIRD_TOPIC_CHANNEL_PROPERTY_RECEIVE,
                "property",
                "+"
            );

            mqttUnsubscribe(topic.c_str());

            // Control channel property query topic
            topic = _fastybirdMqttApiCreateChannelTopicString(
                fastybirdDeviceIdentifier().c_str(),
                "+",
                FASTYBIRD_TOPIC_CHANNEL_PROPERTY_QUERY,
                "property",
                "+"
            );

            mqttUnsubscribe(topic.c_str());
        #endif

        #if FASTYBIRD_MAX_CHANNELS > 0
            // Control channel topic
            topic = _fastybirdMqttApiCreateChannelTopicString(
                fastybirdDeviceIdentifier().c_str(),
                "+",
                FASTYBIRD_TOPIC_CHANNEL_CONTROL_RECEIVE,
                "control",
                "+"
            );

            mqttUnsubscribe(topic.c_str());
        #endif
    }
}

// -----------------------------------------------------------------------------

void fastybirdApiOnHeartbeat()
{
    mqttSend(
        _fastybirdMqttApiCreatePropertyTopicString(fastybirdDeviceIdentifier().c_str(), FASTYBIRD_PROPERTY_FREE_HEAP).c_str(),
        String(getFreeHeap()).c_str()
    );

    mqttSend(
        _fastybirdMqttApiCreatePropertyTopicString(fastybirdDeviceIdentifier().c_str(), FASTYBIRD_PROPERTY_UPTIME).c_str(),
        String(getUptime()).c_str()
    );

    #if WIFI_SUPPORT
        mqttSend(
            _fastybirdMqttApiCreatePropertyTopicString(fastybirdDeviceIdentifier().c_str(), FASTYBIRD_PROPERTY_RSSI).c_str(),
            String(WiFi.RSSI()).c_str()
        );
        mqttSend(
            _fastybirdMqttApiCreatePropertyTopicString(fastybirdDeviceIdentifier().c_str(), FASTYBIRD_PROPERTY_SSID).c_str(),
            getNetwork().c_str()
        );
    #endif

    mqttSend(
        _fastybirdMqttApiCreatePropertyTopicString(fastybirdDeviceIdentifier().c_str(), FASTYBIRD_PROPERTY_CPU_LOAD).c_str(),
        String(systemLoadAverage()).c_str()
    );

    #if ADC_MODE_VALUE == ADC_VCC
        mqttSend(
            _fastybirdMqttApiCreatePropertyTopicString(fastybirdDeviceIdentifier().c_str(), FASTYBIRD_PROPERTY_VCC).c_str(),
            String(ESP.getVcc()).c_str()
        );
    #endif
}

// -----------------------------------------------------------------------------

bool fastybirdApiPropagateDeviceState(
    const char * deviceId,
    const char * payload
) {
    uint8_t packet_id;

    bool retain = false;

    packet_id = mqttSend(
        _fastybirdMqttApiCreateStateTopicString(deviceId).c_str(),
        payload,
        true
    );

    if (packet_id == 0) return false;

    return true;
}

// -----------------------------------------------------------------------------

bool fastybirdApiPropagateDeviceState(
    const char * payload
) {
    return fastybirdApiPropagateDeviceState(fastybirdDeviceIdentifier().c_str(), payload);
}

// -----------------------------------------------------------------------------

bool fastybirdApiPropagateDevicePropertiesNames(
    const char * deviceId,
    std::vector<String> properties
) {
    uint8_t packet_id;

    String topic = _fastybirdMqttApiCreateDeviceTopicString(deviceId, FASTYBIRD_TOPIC_DEVICE_PROPERTIES_STRUCTURE);

    if (properties.size() > 0) {
        char payload[80];

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

bool fastybirdApiPropagateDevicePropertiesNames(
    std::vector<String> properties
) {
    return fastybirdApiPropagateDevicePropertiesNames(fastybirdDeviceIdentifier().c_str(), properties);
}

// -----------------------------------------------------------------------------

bool fastybirdApiPropagateDevicePropertyValue(
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

bool fastybirdApiPropagateDevicePropertyValue(
    const char * property,
    const char * payload
) {
    return fastybirdApiPropagateDevicePropertyValue(fastybirdDeviceIdentifier().c_str(), property, payload);
}

// -----------------------------------------------------------------------------

bool fastybirdApiPropagateDeviceName(
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

bool fastybirdApiPropagateDeviceName(
    const char * name
) {
    return fastybirdApiPropagateDeviceName(fastybirdDeviceIdentifier().c_str(), name);
}

// -----------------------------------------------------------------------------

bool fastybirdApiPropagateDeviceParent(
    const char * deviceId,
    const char * parent
) {
    uint8_t packet_id;

    packet_id = mqttSend(
        _fastybirdMqttApiCreateDeviceTopicString(deviceId, FASTYBIRD_TOPIC_DEVICE_PARENT).c_str(),
        parent,
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
        _fastybirdMqttApiCreateHWTopicString(deviceId, field).c_str(),
        payload,
        false
    );

    if (packet_id == 0) return false;

    return true;
}

// -----------------------------------------------------------------------------

bool fastybirdApiPropagateDeviceHardwareField(
    const char * field,
    const char * payload
) {
    return fastybirdApiPropagateDeviceHardwareField(fastybirdDeviceIdentifier().c_str(), field, payload);
}

// -----------------------------------------------------------------------------

bool fastybirdApiPropagateDeviceHardwareVersion(
    const char * deviceId,
    const char * name
) {
    return fastybirdApiPropagateDeviceHardwareField(deviceId, "version", name);
}

// -----------------------------------------------------------------------------

bool fastybirdApiPropagateDeviceHardwareVersion(
    const char * name
) {
    return fastybirdApiPropagateDeviceHardwareVersion(fastybirdDeviceIdentifier().c_str(), name);
}

// -----------------------------------------------------------------------------

bool fastybirdApiPropagateDeviceHardwareModelName(
    const char * deviceId,
    const char * model
) {
    return fastybirdApiPropagateDeviceHardwareField(deviceId, "model", model);
}

// -----------------------------------------------------------------------------

bool fastybirdApiPropagateDeviceHardwareModelName(
    const char * model
) {
    return fastybirdApiPropagateDeviceHardwareModelName(fastybirdDeviceIdentifier().c_str(), model);
}

// -----------------------------------------------------------------------------

bool fastybirdApiPropagateDeviceHardwareManufacturer(
    const char * deviceId,
    const char * manufacturer
) {
    return fastybirdApiPropagateDeviceHardwareField(deviceId, "manufacturer", manufacturer);
}

// -----------------------------------------------------------------------------

bool fastybirdApiPropagateDeviceHardwareManufacturer(
    const char * manufacturer
) {
    return fastybirdApiPropagateDeviceHardwareManufacturer(fastybirdDeviceIdentifier().c_str(), manufacturer);
}

// -----------------------------------------------------------------------------

bool fastybirdApiPropagateDeviceHardwareSerialNumber(
    const char * deviceId,
    const char * serialNumber
) {
    return fastybirdApiPropagateDeviceHardwareField(deviceId, "serial-number", serialNumber);
}

// -----------------------------------------------------------------------------

bool fastybirdApiPropagateDeviceHardwareSerialNumber(
    const char * serialNumber
) {
    return fastybirdApiPropagateDeviceHardwareSerialNumber(fastybirdDeviceIdentifier().c_str(), serialNumber);
}

// -----------------------------------------------------------------------------

bool fastybirdApiPropagateDeviceFirmwareField(
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

bool fastybirdApiPropagateDeviceFirmwareField(
    const char * field,
    const char * payload
) {
    return fastybirdApiPropagateDeviceFirmwareField(fastybirdDeviceIdentifier().c_str(), field, payload);
}

// -----------------------------------------------------------------------------

bool fastybirdApiPropagateDeviceFirmwareName(
    const char * deviceId,
    const char * name
) {
    return fastybirdApiPropagateDeviceFirmwareField(deviceId, "name", name);
}

// -----------------------------------------------------------------------------

bool fastybirdApiPropagateDeviceFirmwareName(
    const char * name
) {
    return fastybirdApiPropagateDeviceFirmwareName(fastybirdDeviceIdentifier().c_str(), name);
}

// -----------------------------------------------------------------------------

bool fastybirdApiPropagateDeviceFirmwareManufacturer(
    const char * deviceId,
    const char * manufacturer
) {
    return fastybirdApiPropagateDeviceFirmwareField(deviceId, "manufacturer", manufacturer);
}

// -----------------------------------------------------------------------------

bool fastybirdApiPropagateDeviceFirmwareManufacturer(
    const char * manufacturer
) {
    return fastybirdApiPropagateDeviceFirmwareManufacturer(fastybirdDeviceIdentifier().c_str(), manufacturer);
}

// -----------------------------------------------------------------------------

bool fastybirdApiPropagateDeviceFirmwareVersion(
    const char * deviceId,
    const char * version
) {
    return fastybirdApiPropagateDeviceFirmwareField(deviceId, "version", version);
}


// -----------------------------------------------------------------------------

bool fastybirdApiPropagateDeviceFirmwareVersion(
    const char * version
) {
    return fastybirdApiPropagateDeviceFirmwareVersion(fastybirdDeviceIdentifier().c_str(), version);
}

// -----------------------------------------------------------------------------

bool fastybirdApiPropagateDeviceChannelsNames(
    const char * deviceId,
    std::vector<String> channels
) {
    char payload[80];

    uint8_t packet_id;

    if (channels.size() > 0) {
        char formatted_channels[300];

        strcpy(formatted_channels, channels[0].c_str());

        for (uint8_t i = 1; i < channels.size(); i++) {
            strcat(formatted_channels, ",");
            strcat(formatted_channels, channels[i].c_str());
        }

        packet_id = mqttSend(
            _fastybirdMqttApiCreateDeviceTopicString(deviceId, FASTYBIRD_TOPIC_DEVICE_CHANNELS).c_str(),
            formatted_channels,
            false
        );

    } else {
        packet_id = mqttSend(
            _fastybirdMqttApiCreateDeviceTopicString(deviceId, FASTYBIRD_TOPIC_DEVICE_CHANNELS).c_str(),
            "",
            false
        );
    }

    if (packet_id == 0) {
        return false;
    }

    return true;
}

// -----------------------------------------------------------------------------

bool fastybirdApiPropagateDeviceChannelsNames(
    std::vector<String> channels
) {
    return fastybirdApiPropagateDeviceChannelsNames(fastybirdDeviceIdentifier().c_str(), channels);
}

// -----------------------------------------------------------------------------

bool fastybirdApiPropagateDeviceControlNames(
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

    return true;
}

// -----------------------------------------------------------------------------

bool fastybirdApiPropagateDeviceControlNames(
    std::vector<String> controls
) {
    return fastybirdApiPropagateDeviceControlNames(
        fastybirdDeviceIdentifier().c_str(),
        controls
    );
}

// -----------------------------------------------------------------------------

bool fastybirdApiPropagateDeviceConfigurationSchema(
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

bool fastybirdApiPropagateDeviceConfigurationSchema(
    JsonArray& schema
) {
    return fastybirdApiPropagateDeviceConfigurationSchema(fastybirdDeviceIdentifier().c_str(), schema);
}

// -----------------------------------------------------------------------------

bool fastybirdApiPropagateDeviceConfiguration(
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

bool fastybirdApiPropagateDeviceConfiguration(
    JsonObject& configuration
) {
    return fastybirdApiPropagateDeviceConfiguration(fastybirdDeviceIdentifier().c_str(), configuration);
}

// -----------------------------------------------------------------------------

bool fastybirdApiPropagateChannelName(
    const char * deviceId,
    const char * channel,
    const char * name
) {
    uint8_t packet_id;

    packet_id = mqttSend(
        _fastybirdMqttApiCreateChannelTopicString(
            deviceId,
            channel,
            FASTYBIRD_TOPIC_CHANNEL_NAME
        ).c_str(),
        name,
        false
    );

    if (packet_id == 0) return false;

    return true;
}

// -----------------------------------------------------------------------------

bool fastybirdApiPropagateChannelName(
    const char * channel,
    const char * name
) {
    return fastybirdApiPropagateChannelName(fastybirdDeviceIdentifier().c_str(), channel, name);
}

// -----------------------------------------------------------------------------

bool fastybirdApiPropagateChannelPropertiesNames(
    const char * deviceId,
    const char * channel,
    std::vector<String> properties
) {
    uint8_t packet_id;

    String topic = _fastybirdMqttApiCreateChannelTopicString(
        deviceId,
        channel,
        FASTYBIRD_TOPIC_CHANNEL_PROPERTIES
    );

    if (properties.size() > 0) {
        char formatted_properties[100];

        strcpy(formatted_properties, properties[0].c_str());

        for (uint8_t i = 1; i < properties.size(); i++) {
            strcat(formatted_properties, ",");
            strcat(formatted_properties, properties[i].c_str());
        }

        packet_id = mqttSend(
            topic.c_str(),
            formatted_properties,
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

bool fastybirdApiPropagateChannelPropertiesNames(
    const char * channel,
    std::vector<String> properties
) {
    return fastybirdApiPropagateChannelPropertiesNames(fastybirdDeviceIdentifier().c_str(), channel, properties);
}

// -----------------------------------------------------------------------------

bool fastybirdApiPropagateChannelPropertyName(
    const char * deviceId,
    const char * channel,
    const char * property,
    const char * name
) {
    uint8_t packet_id;

    packet_id = mqttSend(
        _fastybirdMqttApiCreateChannelTopicString(
            deviceId,
            channel,
            FASTYBIRD_TOPIC_CHANNEL_PROPERTY_NAME,
            "property",
            property
        ).c_str(),
        name,
        false
    );

    if (packet_id == 0) return false;

    return true;
}

// -----------------------------------------------------------------------------

bool fastybirdApiPropagateChannelPropertyName(
    const char * channel,
    const char * property,
    const char * name
) {
    return fastybirdApiPropagateChannelPropertyName(fastybirdDeviceIdentifier().c_str(), channel, property, name);
}

// -----------------------------------------------------------------------------

bool fastybirdApiPropagateChannelPropertySettable(
    const char * deviceId,
    const char * channel,
    const char * property,
    bool settable
) {
    uint8_t packet_id;

    packet_id = mqttSend(
        _fastybirdMqttApiCreateChannelTopicString(
            deviceId,
            channel,
            FASTYBIRD_TOPIC_CHANNEL_PROPERTY_SETTABLE,
            "property",
            property
        ).c_str(),
        settable ? FASTYBIRD_PROPERTY_IS_SETTABLE : FASTYBIRD_PROPERTY_IS_NOT_SETTABLE,
        false
    );

    if (packet_id == 0) return false;

    return true;
}

// -----------------------------------------------------------------------------

bool fastybirdApiPropagateChannelPropertySettable(
    const char * channel,
    const char * property,
    bool settable
) {
    return fastybirdApiPropagateChannelPropertySettable(fastybirdDeviceIdentifier().c_str(), channel, property, settable);
}

// -----------------------------------------------------------------------------

bool fastybirdApiPropagateChannelPropertyQueryable(
    const char * deviceId,
    const char * channel,
    const char * property,
    bool queryable
) {
    uint8_t packet_id;

    packet_id = mqttSend(
        _fastybirdMqttApiCreateChannelTopicString(
            deviceId,
            channel,
            FASTYBIRD_TOPIC_CHANNEL_PROPERTY_QUERYABLE,
            "property",
            property
        ).c_str(),
        queryable ? FASTYBIRD_PROPERTY_IS_QUERYABLE : FASTYBIRD_PROPERTY_IS_NOT_QUERYABLE,
        false
    );

    if (packet_id == 0) return false;

    return true;
}

// -----------------------------------------------------------------------------

bool fastybirdApiPropagateChannelPropertyQueryable(
    const char * channel,
    const char * property,
    bool queryable
) {
    return fastybirdApiPropagateChannelPropertyQueryable(fastybirdDeviceIdentifier().c_str(), channel, property, queryable);
}

// -----------------------------------------------------------------------------

bool fastybirdApiPropagateChannelPropertyDataType(
    const char * deviceId,
    const char * channel,
    const char * property,
    const char * dataType
) {
    uint8_t packet_id;

    packet_id = mqttSend(
        _fastybirdMqttApiCreateChannelTopicString(
            deviceId,
            channel,
            FASTYBIRD_TOPIC_CHANNEL_PROPERTY_DATA_TYPE,
            "property",
            property
        ).c_str(),
        dataType,
        false
    );

    if (packet_id == 0) return false;

    return true;
}

// -----------------------------------------------------------------------------

bool fastybirdApiPropagateChannelPropertyDataType(
    const char * channel,
    const char * property,
    const char * dataType
) {
    return fastybirdApiPropagateChannelPropertyDataType(fastybirdDeviceIdentifier().c_str(), channel, property, dataType);
}

// -----------------------------------------------------------------------------

bool fastybirdApiPropagateChannelPropertyUnit(
    const char * deviceId,
    const char * channel,
    const char * property,
    const char * unit
) {
    uint8_t packet_id;

    packet_id = mqttSend(
        _fastybirdMqttApiCreateChannelTopicString(
            deviceId,
            channel,
            FASTYBIRD_TOPIC_CHANNEL_PROPERTY_UNIT,
            "property",
            property
        ).c_str(),
        unit,
        false
    );

    if (packet_id == 0) return false;

    return true;
}

// -----------------------------------------------------------------------------

bool fastybirdApiPropagateChannelPropertyUnit(
    const char * channel,
    const char * property,
    const char * unit
) {
    return fastybirdApiPropagateChannelPropertyUnit(fastybirdDeviceIdentifier().c_str(), channel, property, unit);
}

// -----------------------------------------------------------------------------

bool fastybirdApiPropagateChannelPropertyFormat(
    const char * deviceId,
    const char * channel,
    const char * property,
    const char * format
) {
    uint8_t packet_id;

    packet_id = mqttSend(
        _fastybirdMqttApiCreateChannelTopicString(
            deviceId,
            channel,
            FASTYBIRD_TOPIC_CHANNEL_PROPERTY_FORMAT,
            "property",
            property
        ).c_str(),
        format,
        false
    );

    if (packet_id == 0) return false;

    return true;
}

// -----------------------------------------------------------------------------

bool fastybirdApiPropagateChannelPropertyFormat(
    const char * channel,
    const char * property,
    const char * format
) {
    return fastybirdApiPropagateChannelPropertyFormat(fastybirdDeviceIdentifier().c_str(), channel, property, format);
}

// -----------------------------------------------------------------------------

bool fastybirdApiPropagateChannelControlNames(
    const char * deviceId,
    const char * channel,
    std::vector<String> controls
) {
    uint8_t packet_id;

    String topic = _fastybirdMqttApiCreateChannelTopicString(
        deviceId,
        channel,
        FASTYBIRD_TOPIC_CHANNEL_CONTROL
    );

    if (controls.size() > 0) {
        char payload[80];

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

bool fastybirdApiPropagateChannelControlNames(
    const char * channel,
    std::vector<String> controls
) {
    return fastybirdApiPropagateChannelControlNames(fastybirdDeviceIdentifier().c_str(), channel, controls);
}

// -----------------------------------------------------------------------------

bool fastybirdApiPropagateChannelConfigurationSchema(
    const char * deviceId,
    const char * channel,
    JsonArray& schema
) {
    uint8_t packet_id;

    if (schema.size() > 0) {
        String output;

        schema.printTo(output);

        packet_id = mqttSend(
            _fastybirdMqttApiCreateChannelTopicString(
                deviceId,
                channel,
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

bool fastybirdApiPropagateChannelConfigurationSchema(
    const char * channel,
    JsonArray& schema
) {
    return fastybirdApiPropagateChannelConfigurationSchema(fastybirdDeviceIdentifier().c_str(), channel, schema);
}

// -----------------------------------------------------------------------------

bool fastybirdApiPropagateChannelConfiguration(
    const char * deviceId,
    const char * channel,
    JsonObject& configuration
) {
    if (configuration.size() > 0) {
        String output;

        configuration.printTo(output);

        String topic;

        topic = _fastybirdMqttApiCreateChannelTopicString(
            deviceId,
            channel,
            FASTYBIRD_TOPIC_CHANNEL_CONTROL_DATA,
            "control",
            FASTYBIRD_CHANNEL_CONTROL_CONFIGURE
        );

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

bool fastybirdApiPropagateChannelConfiguration(
    const char * channel,
    JsonObject& configuration
) {
    return fastybirdApiPropagateChannelConfiguration(fastybirdDeviceIdentifier().c_str(), channel, configuration);
}

// -----------------------------------------------------------------------------

bool fastybirdApiPropagateChannelPropertyValue(
    const char * deviceId,
    const char * channel,
    const char * property,
    const char * payload
) {
    uint8_t packet_id;

    packet_id = mqttSend(
        _fastybirdMqttApiCreateChannelTopicString(
            deviceId,
            channel,
            FASTYBIRD_TOPIC_CHANNEL_PROPERTY,
            "property",
            property
        ).c_str(),
        payload,
        true
    );

    if (packet_id == 0) return false;

    return true;
}

// -----------------------------------------------------------------------------

bool fastybirdApiPropagateChannelPropertyValue(
    const char * channel,
    const char * property,
    const char * payload
) {
    return fastybirdApiPropagateChannelPropertyValue(fastybirdDeviceIdentifier().c_str(), channel, property, payload);
}

#endif
