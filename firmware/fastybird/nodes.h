/*

FASTYBIRD NODES MODULE

Copyright (C) 2018 FastyBird s.r.o. <info@fastybird.com>

*/

#if FASTYBIRD_SUPPORT && FASTYBIRD_GATEWAY_SUPPORT

std::vector<fastybird_node_t> _fastybird_gateway_nodes;

uint8_t _fastybird_initialize_node = 0xFF;

uint8_t _fastybird_node_advertisement_progress = FASTYBIRD_PUB_CONNECTION;
uint8_t _fastybird_node_channel_advertisement_progress = FASTYBIRD_PUB_CHANNEL_NAME;
uint8_t _fastybird_node_channel_property_advertisement_progress = FASTYBIRD_PUB_CHANNEL_PROPERTY_NAME;

// -----------------------------------------------------------------------------
// MODULE PRIVATE 
// -----------------------------------------------------------------------------

void _fastybirdNodeSendHeartbeat() {
    for (uint8_t i = 0; i < _fastybird_gateway_nodes.size(); i++) {
        if (_fastybird_gateway_nodes[i].initialized) {
            _fastybirdPropagateDeviceProperty(
                _fastybird_gateway_nodes[i].id,
                FASTYBIRD_PROPERTY_UPTIME,
                String(getUptime()).c_str()
            );
        }
    }
}

// -----------------------------------------------------------------------------

bool _fastybirdReportNodeChannelValue(
    const char * node,
    const uint8_t channel,
    const uint8_t channelId,
    const char * payload
) {
    for (uint8_t i = 0; i < _fastybird_gateway_nodes.size(); i++) {
        if (
            strcmp(_fastybird_gateway_nodes[i].id, node) == 0
            && channel < _fastybird_gateway_nodes[i].channels.size()
            && _fastybird_gateway_nodes[i].initialized
        ) {
            return _fastybirdPropagateChannelValue(
                _fastybird_gateway_nodes[i].id,
                _fastybird_gateway_nodes[i].channels[channel],
                _fastybird_gateway_nodes[i].properties[0],
                channelId,
                payload
            );
        }
    }

    return false;
}

// -----------------------------------------------------------------------------

/**
 * Initialize given node device channel property
 */
bool _fastybirdInitializeNodeChannelProperty(
    const uint8_t nodeId,
    uint8_t channelIndex,
    uint8_t propertyIndex
) {
    if (
        channelIndex >= _fastybird_gateway_nodes[nodeId].channels.size()
        || propertyIndex >= _fastybird_gateway_nodes[nodeId].properties.size()
        || _fastybird_gateway_nodes[nodeId].properties[propertyIndex].initialized
    ) {
        DEBUG_MSG(PSTR("[TEST] Prop initialized: %i:%i:%i\n"), nodeId, channelIndex, propertyIndex);
        return true;
    }

    fastybird_channel_t channel = _fastybird_gateway_nodes[nodeId].channels[channelIndex];
    fastybird_channel_property_t property = _fastybird_gateway_nodes[nodeId].properties[propertyIndex];

    switch (_fastybird_node_channel_property_advertisement_progress)
    {
        case FASTYBIRD_PUB_CHANNEL_PROPERTY_NAME:
            if (!_fastybirdPropagateChannelPropertyName(_fastybird_gateway_nodes[nodeId].id, channel, property)) {
                return false;
            }

            _fastybird_node_channel_property_advertisement_progress = FASTYBIRD_PUB_CHANNEL_PROPERTY_TYPE;
            break;

        case FASTYBIRD_PUB_CHANNEL_PROPERTY_TYPE:
            if (!_fastybirdPropagateChannelPropertyType(_fastybird_gateway_nodes[nodeId].id, channel, property)) {
                return false;
            }

            _fastybird_node_channel_property_advertisement_progress = FASTYBIRD_PUB_CHANNEL_PROPERTY_SETABLE;
            break;

        case FASTYBIRD_PUB_CHANNEL_PROPERTY_SETABLE:
            if (!_fastybirdPropagateChannelPropertySettable(_fastybird_gateway_nodes[nodeId].id, channel, property)) {
                return false;
            }

            _fastybird_node_channel_property_advertisement_progress = FASTYBIRD_PUB_CHANNEL_PROPERTY_QUERYABLE;
            break;

        case FASTYBIRD_PUB_CHANNEL_PROPERTY_QUERYABLE:
            if (!_fastybirdPropagateChannelPropertyQueryable(_fastybird_gateway_nodes[nodeId].id, channel, property)) {
                return false;
            }

            _fastybird_node_channel_property_advertisement_progress = FASTYBIRD_PUB_CHANNEL_PROPERTY_DATA_TYPE;
            break;

        case FASTYBIRD_PUB_CHANNEL_PROPERTY_DATA_TYPE:
            if (!_fastybirdPropagateChannelPropertyDataType(_fastybird_gateway_nodes[nodeId].id, channel, property)) {
                return false;
            }

            _fastybird_node_channel_property_advertisement_progress = FASTYBIRD_PUB_CHANNEL_PROPERTY_UNITS;
            break;

        case FASTYBIRD_PUB_CHANNEL_PROPERTY_UNITS:
            if (!_fastybirdPropagateChannelPropertyUnit(_fastybird_gateway_nodes[nodeId].id, channel, property)) {
                return false;
            }

            _fastybird_node_channel_property_advertisement_progress = FASTYBIRD_PUB_CHANNEL_PROPERTY_FORMAT;
            break;

        case FASTYBIRD_PUB_CHANNEL_PROPERTY_FORMAT:
            if (!_fastybirdPropagateChannelPropertyFormat(_fastybird_gateway_nodes[nodeId].id, channel, property)) {
                return false;
            }

            _fastybird_node_channel_property_advertisement_progress = FASTYBIRD_PUB_CHANNEL_PROPERTY_DONE;
            break;

// -----------------------------------------------------------------------------
// CHANNEL PROPERTY INITIALIZATION IS DONE
// -----------------------------------------------------------------------------

        case FASTYBIRD_PUB_CHANNEL_PROPERTY_DONE:
            return true;
            break;
    }

    return false;
}

// -----------------------------------------------------------------------------

bool _fastybirdInitializeNodeChannel(
    const uint8_t nodeId,
    uint8_t channelIndex
) {
    if (
        channelIndex >= _fastybird_gateway_nodes[nodeId].channels.size()
        || _fastybird_gateway_nodes[nodeId].channels[channelIndex].initialized
    ) {
        return true;
    }

    DynamicJsonBuffer jsonBuffer;

    JsonArray& configurationSchema = jsonBuffer.createArray();

    std::vector<String> channel_properties;
    std::vector<String> channel_controls;

    fastybird_channel_t channel = _fastybird_gateway_nodes[nodeId].channels[channelIndex];

    switch (_fastybird_node_channel_advertisement_progress)
    {
        
// -----------------------------------------------------------------------------
// CHANNEL BASIC INFO
// -----------------------------------------------------------------------------

        case FASTYBIRD_PUB_CHANNEL_NAME:
            if (!_fastybirdPropagateChannelName(_fastybird_gateway_nodes[nodeId].id, channel)) {
                return false;
            }

            _fastybird_node_channel_advertisement_progress = FASTYBIRD_PUB_CHANNEL_ARRAY;
            break;

        case FASTYBIRD_PUB_CHANNEL_ARRAY:
            if (!_fastybirdPropagateChannelSize(_fastybird_gateway_nodes[nodeId].id, channel)) {
                return false;
            }

            _fastybird_node_channel_advertisement_progress = FASTYBIRD_PUB_CHANNEL_PROPERTIES;
            break;

        case FASTYBIRD_PUB_CHANNEL_PROPERTIES:
            if (channel.properties.size() > 0) {
                for (uint8_t i = 0; i < channel.properties.size(); i++) {
                    uint8_t propertyIndex = channel.properties[i];

                    if (propertyIndex < _fastybird_gateway_nodes[nodeId].properties.size()) {
                        channel_properties.push_back(_fastybird_gateway_nodes[nodeId].properties[propertyIndex].name);
                    }
                }
            }

            if (channel_properties.size() > 0 && !_fastybirdPropagateChannelProperties(_fastybird_gateway_nodes[nodeId].id, channel, channel_properties)) {
                return false;
            }

            _fastybird_node_channel_advertisement_progress = FASTYBIRD_PUB_CHANNEL_CONTROL_STRUCTURE;
            break;

        case FASTYBIRD_PUB_CHANNEL_CONTROL_STRUCTURE:
            if (channel.configurationCallback.size()) {
                channel_controls.push_back(FASTYBIRD_CHANNEL_CONTROL_CONFIGURE);
            }

            if (!_fastybirdPropagateChannelControlConfiguration(_fastybird_gateway_nodes[nodeId].id, channel, channel_controls)) {
                return false;
            }

            if (channel.configurationSchemaCallback.size() > 0) {
                _fastybird_node_channel_advertisement_progress = FASTYBIRD_PUB_CHANNEL_CONFIGURATION_SCHEMA;
 
            } else if (channel.properties.size() > 0) {
                _fastybird_node_channel_advertisement_progress = FASTYBIRD_PUB_CHANNEL_PROPERTY;

            } else {
                _fastybird_node_channel_advertisement_progress = FASTYBIRD_PUB_CHANNEL_DONE;
            }
            break;

// -----------------------------------------------------------------------------
// CHANNEL CONTROL CONFIGURATION
// -----------------------------------------------------------------------------

        case FASTYBIRD_PUB_CHANNEL_CONFIGURATION_SCHEMA:
            for (uint8_t i = 0; i < channel.configurationSchemaCallback.size(); i++) {
                (channel.configurationSchemaCallback[i])(configurationSchema);
            }

            if (configurationSchema.size() > 0) {
                if (!_fastybirdPropagateChannelConfigurationSchema(_fastybird_gateway_nodes[nodeId].id, channel, configurationSchema)) {
                    return false;
                }
            }

            if (channel.properties.size() > 0) {
                _fastybird_node_channel_advertisement_progress = FASTYBIRD_PUB_CHANNEL_PROPERTY;
 
            } else {
                _fastybird_node_channel_advertisement_progress = FASTYBIRD_PUB_CHANNEL_DONE;
            }
            break;

// -----------------------------------------------------------------------------
// CHANNEL PROPERTIES
// -----------------------------------------------------------------------------

        case FASTYBIRD_PUB_CHANNEL_PROPERTY:
            if (channel.properties.size() > 0) {
                DEBUG_MSG(PSTR("[TEST] Channel props cnt: %i\n"), channel.properties.size());
                for (uint8_t i = 0; i < channel.properties.size(); i++) {
                    uint8_t propertyIndex = channel.properties[i];
                    DEBUG_MSG(PSTR("[TEST] Channel prop index: %i\n"), propertyIndex);

                    if (
                        propertyIndex >= _fastybird_gateway_nodes[nodeId].properties.size()
                        || _fastybird_gateway_nodes[nodeId].properties[propertyIndex].initialized
                    ) {
                        DEBUG_MSG(PSTR("[TEST] Property initializedi\n"));
                        continue;
                    }

                    if (!_fastybirdInitializeNodeChannelProperty(nodeId, channelIndex, propertyIndex)) {
                        return false;
                    }

                    _fastybird_gateway_nodes[nodeId].properties[propertyIndex].initialized = true;

                    _fastybird_node_channel_property_advertisement_progress = FASTYBIRD_PUB_CHANNEL_PROPERTY_NAME;
                }
            }

            _fastybird_node_channel_advertisement_progress = FASTYBIRD_PUB_CHANNEL_DONE;
            break;

// -----------------------------------------------------------------------------
// CHANNEL INITIALIZATION IS DONE
// -----------------------------------------------------------------------------

        case FASTYBIRD_PUB_CHANNEL_DONE:
            return true;
            break;
    }

    return false;
}

// -----------------------------------------------------------------------------

void _fastybirdInitializeNode(
    const uint8_t nodeId
) {
    if (_fastybird_gateway_nodes[nodeId].initialized) {
        _fastybird_initialize_node = 0xFF;

        return;
    }

    std::vector<String> node_properties;

    switch (_fastybird_node_advertisement_progress) {
        case FASTYBIRD_PUB_CONNECTION:
            // Notify cloud broker that node device is sending initialization sequences
            if (!_fastybirdPropagateDeviceProperty(_fastybird_gateway_nodes[nodeId].id, FASTYBIRD_PROPERTY_STATE, FASTYBIRD_STATUS_INIT)) {
                return;
            }

            // Collect all node device properties...
            node_properties.push_back(FASTYBIRD_PROPERTY_UPTIME);
            node_properties.push_back(FASTYBIRD_PROPERTY_INTERVAL);
            node_properties.push_back(FASTYBIRD_PROPERTY_STATE);

            // ...and pass them to the broker
            if (!_fastybirdPropagateDevicePropertiesStructure(_fastybird_gateway_nodes[nodeId].id, node_properties)) {
                return;
            }

            // Heartbeat interval
            if (!_fastybirdPropagateDeviceProperty(_fastybird_gateway_nodes[nodeId].id, FASTYBIRD_PROPERTY_INTERVAL, String(HEARTBEAT_INTERVAL / 1000).c_str())) {
                return;
            }

            _fastybird_node_advertisement_progress = FASTYBIRD_PUB_NAME;
            break;

        case FASTYBIRD_PUB_NAME:
            if (!_fastybirdPropagateDeviceName(_fastybird_gateway_nodes[nodeId].id, _fastybird_gateway_nodes[nodeId].id)) {
                return;
            }

            _fastybird_node_advertisement_progress = FASTYBIRD_PUB_HARDWARE;
            break;

        // Describe device hardware details to cloud broker
        case FASTYBIRD_PUB_HARDWARE:
            if (!_fastybirdPropagateDeviceHardwareManufacturer(_fastybird_gateway_nodes[nodeId].id, _fastybird_gateway_nodes[nodeId].hardware.manufacturer)) {
                return;
            }

            if (!_fastybirdPropagateDeviceHardwareModelName(_fastybird_gateway_nodes[nodeId].id, _fastybird_gateway_nodes[nodeId].hardware.name)) {
                return;
            }

            if (!_fastybirdPropagateDeviceHardwareVersion(_fastybird_gateway_nodes[nodeId].id, _fastybird_gateway_nodes[nodeId].hardware.version)) {
                return;
            }

            _fastybird_node_advertisement_progress = FASTYBIRD_PUB_FIRMWARE;
            break;

        // Describe device firmware details to cloud broker
        case FASTYBIRD_PUB_FIRMWARE:
            if (!_fastybirdPropagateDeviceFirmwareManufacturer(_fastybird_gateway_nodes[nodeId].id, _fastybird_gateway_nodes[nodeId].software.manufacturer)) {
                return;
            }

            if (!_fastybirdPropagateDeviceFirmwareName(_fastybird_gateway_nodes[nodeId].id, _fastybird_gateway_nodes[nodeId].software.name)) {
                return;
            }

            if (!_fastybirdPropagateDeviceFirmwareVersion(_fastybird_gateway_nodes[nodeId].id, _fastybird_gateway_nodes[nodeId].software.version)) {
                return;
            }

            _fastybird_node_advertisement_progress = FASTYBIRD_PUB_CHANNELS;
            break;

        case FASTYBIRD_PUB_CHANNELS:
            if (_fastybird_gateway_nodes[nodeId].channels.size() <= 0) {
                _fastybird_node_advertisement_progress = FASTYBIRD_PUB_CONTROL_STRUCTURE;

                return;
            }

            if (!_fastybirdPropagateDeviceChannels(_fastybird_gateway_nodes[nodeId].id, _fastybird_gateway_nodes[nodeId].channels)) {
                return;
            }

            _fastybird_node_advertisement_progress = FASTYBIRD_PUB_CONTROL_STRUCTURE;
            break;

        case FASTYBIRD_PUB_CONTROL_STRUCTURE:
            _fastybird_node_advertisement_progress = FASTYBIRD_PUB_CONFIGURATION_SCHEMA;
            break;

        case FASTYBIRD_PUB_CONFIGURATION_SCHEMA:
            _fastybird_node_advertisement_progress = FASTYBIRD_PUB_INITIALIZE_CHANNELS;
            break;

        case FASTYBIRD_PUB_INITIALIZE_CHANNELS:
            if (_fastybird_gateway_nodes[nodeId].channels.size() > 0) {
                for (uint8_t i = 0; i < _fastybird_gateway_nodes[nodeId].channels.size(); i++) {
                    if (_fastybird_gateway_nodes[nodeId].channels[i].initialized) {
                        continue;
                    }

                    if (_fastybird_gateway_nodes[nodeId].channels[i].length == 0) {
                        _fastybird_gateway_nodes[nodeId].channels[i].initialized = true;

                        continue;
                    }

                    if (!_fastybirdInitializeNodeChannel(nodeId, i)) {
                        return;
                    }

                    _fastybird_gateway_nodes[nodeId].channels[i].initialized = true;

                    _fastybird_node_channel_advertisement_progress = FASTYBIRD_PUB_CHANNEL_NAME;

                    return;
                }
            }

            _fastybird_node_advertisement_progress = FASTYBIRD_PUB_READY;
            break;

        case FASTYBIRD_PUB_READY:
            if (!_fastybirdPropagateDeviceProperty(_fastybird_gateway_nodes[nodeId].id, FASTYBIRD_PROPERTY_STATE, FASTYBIRD_STATUS_READY)) {
                return;
            }

            _fastybird_node_advertisement_progress = FASTYBIRD_PUB_CONFIGURATION;
            break;

        case FASTYBIRD_PUB_CONFIGURATION:
            _fastybird_node_advertisement_progress = FASTYBIRD_PUB_CHANNELS_CONFIGURATION;
            break;

        case FASTYBIRD_PUB_CHANNELS_CONFIGURATION:
            _fastybird_node_advertisement_progress = FASTYBIRD_PUB_HEARTBEAT;
            break;

        case FASTYBIRD_PUB_HEARTBEAT:
            _fastybird_gateway_nodes[nodeId].initialized = true;

            _fastybird_initialize_node = 0xFF;
            break;
    }
}

// -----------------------------------------------------------------------------

void _fastybirdNodeLoop() {
    if (_fastybirdIsApiReady() && fastybirdIsDeviceInitialzed()) {
        if (_fastybird_initialize_node != 0xFF) {
            _fastybirdInitializeNode(_fastybird_initialize_node);

        } else {
            for (uint8_t i = 0; i < _fastybird_gateway_nodes.size(); i++) {
                if (_fastybird_gateway_nodes[i].initialized == false) {
                    _fastybird_initialize_node = i;

                    _fastybird_node_advertisement_progress = FASTYBIRD_PUB_CONNECTION;

                    _fastybirdInitializeNode(i);
                    break;
                }
            }
        }
    }
}

// -----------------------------------------------------------------------------

void _fastybirdNodeSetup() {
    firmwareRegisterLoop(_fastybirdNodeLoop);
    systemOnHeartbeatRegister(_fastybirdNodeSendHeartbeat);
}

// -----------------------------------------------------------------------------
// MODULE API
// -----------------------------------------------------------------------------

void fastybirdRegisterNode(
    fastybird_node_t node
) {
    // New node has to be initialized
    node.initialized = false;

    _fastybird_gateway_nodes.push_back(node);
}

// -----------------------------------------------------------------------------

void fastybirdUnregisterNode(
    const char * nodeId
) {
    std::vector<fastybird_node_t> new_gateway_nodes;

    for (uint8_t i = 0; i < _fastybird_gateway_nodes.size(); i++) {
        if (strcmp(_fastybird_gateway_nodes[i].id, nodeId) != 0) {
            new_gateway_nodes.push_back(_fastybird_gateway_nodes[i]);
        }
    }

	_fastybird_gateway_nodes.clear();

    for (uint8_t i = 0; i < new_gateway_nodes.size(); i++) {
        new_gateway_nodes.push_back(new_gateway_nodes[i]);
    }
}

#endif // FASTYBIRD_SUPPORT && FASTYBIRD_GATEWAY_SUPPORT
