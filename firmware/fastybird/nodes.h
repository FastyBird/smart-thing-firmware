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
            _fastybirdPropagateThingProperty(
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
                _fastybird_gateway_nodes[i].channels[channel].properties[0],
                channelId,
                payload
            );
        }
    }

    return false;
}

// -----------------------------------------------------------------------------

/**
 * Initialize given node thing channel property
 */
bool _fastybirdInitializeNodeChannelProperty(
    const uint8_t nodeId,
    fastybird_channel_t channelStructure,
    fastybird_channel_property_t propertyStructure
) {
    if (propertyStructure.initialized) {
        return true;
    }

    switch (_fastybird_node_channel_property_advertisement_progress)
    {
        case FASTYBIRD_PUB_CHANNEL_PROPERTY_NAME:
            if (!_fastybirdPropagateChannelPropertyName(_fastybird_gateway_nodes[nodeId].id, channelStructure, propertyStructure)) {
                return false;
            }

            _fastybird_node_channel_property_advertisement_progress = FASTYBIRD_PUB_CHANNEL_PROPERTY_SETABLE;
            break;

        case FASTYBIRD_PUB_CHANNEL_PROPERTY_SETABLE:
            if (!_fastybirdPropagateChannelPropertySettable(_fastybird_gateway_nodes[nodeId].id, channelStructure, propertyStructure)) {
                return false;
            }

            _fastybird_node_channel_property_advertisement_progress = FASTYBIRD_PUB_CHANNEL_PROPERTY_DATA_TYPE;
            break;

        case FASTYBIRD_PUB_CHANNEL_PROPERTY_QUERYABLE:
            if (!_fastybirdPropagateChannelPropertyQueryable(_fastybird_gateway_nodes[nodeId].id, channelStructure, propertyStructure)) {
                return false;
            }

            _fastybird_node_channel_property_advertisement_progress = FASTYBIRD_PUB_CHANNEL_PROPERTY_DATA_TYPE;
            break;

        case FASTYBIRD_PUB_CHANNEL_PROPERTY_DATA_TYPE:
            if (!_fastybirdPropagateChannelPropertyDataType(_fastybird_gateway_nodes[nodeId].id, channelStructure, propertyStructure)) {
                return false;
            }

            _fastybird_node_channel_property_advertisement_progress = FASTYBIRD_PUB_CHANNEL_PROPERTY_FORMAT;
            break;

        case FASTYBIRD_PUB_CHANNEL_PROPERTY_FORMAT:
            if (!_fastybirdPropagateChannelPropertyFormat(_fastybird_gateway_nodes[nodeId].id, channelStructure, propertyStructure)) {
                return false;
            }

            _fastybird_node_channel_property_advertisement_progress = FASTYBIRD_PUB_CHANNEL_PROPERTY_MAPPING;
            break;

        case FASTYBIRD_PUB_CHANNEL_PROPERTY_MAPPING:
            if (!_fastybirdPropagateChannelPropertyMappings(_fastybird_gateway_nodes[nodeId].id, channelStructure, propertyStructure)) {
                return false;
            }

            _fastybird_node_channel_property_advertisement_progress = FASTYBIRD_PUB_CHANNEL_CONTROL_STRUCTURE;
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
    if (_fastybird_gateway_nodes[nodeId].channels[channelIndex].initialized) {
        return true;
    }

    switch (_fastybird_node_channel_advertisement_progress) {
        case FASTYBIRD_PUB_CHANNEL_NAME:
            if (!_fastybirdPropagateChannelName(_fastybird_gateway_nodes[nodeId].id, _fastybird_gateway_nodes[nodeId].channels[channelIndex], _fastybird_gateway_nodes[nodeId].channels[channelIndex].name.c_str())) {
                return false;
            }

            _fastybird_node_channel_advertisement_progress = FASTYBIRD_PUB_CHANNEL_TYPE;
            break;

        case FASTYBIRD_PUB_CHANNEL_TYPE:
            if (!_fastybirdPropagateChannelType(_fastybird_gateway_nodes[nodeId].id, _fastybird_gateway_nodes[nodeId].channels[channelIndex], _fastybird_gateway_nodes[nodeId].channels[channelIndex].type.c_str())) {
                return false;
            }

            _fastybird_node_channel_advertisement_progress = FASTYBIRD_PUB_CHANNEL_PROPERTIES;
            break;

        case FASTYBIRD_PUB_CHANNEL_PROPERTIES:
            if (!_fastybirdPropagateChannelProperties(_fastybird_gateway_nodes[nodeId].id, _fastybird_gateway_nodes[nodeId].channels[channelIndex], _fastybird_gateway_nodes[nodeId].channels[channelIndex].properties)) {
                return false;
            }

            // Check if channel definition is for single channel or multichannel
            if (_fastybird_gateway_nodes[nodeId].channels[channelIndex].properties.size() > 0) {
                _fastybird_node_channel_advertisement_progress = FASTYBIRD_PUB_CHANNEL_ARRAY;

            } else {
                _fastybird_node_channel_advertisement_progress = FASTYBIRD_PUB_CHANNEL_PROPERTY_NAME;
            }
            break;

        case FASTYBIRD_PUB_CHANNEL_ARRAY:
            if (!_fastybirdPropagateChannelSize(_fastybird_gateway_nodes[nodeId].id, _fastybird_gateway_nodes[nodeId].channels[channelIndex], _fastybird_gateway_nodes[nodeId].channels[channelIndex].length)) {
                return false;
            }

            if (_fastybird_gateway_nodes[nodeId].channels[channelIndex].properties.size() > 0) {
                _fastybird_node_channel_advertisement_progress = FASTYBIRD_PUB_CHANNEL_PROPERTY_NAME;

            } else {
                _fastybird_node_channel_advertisement_progress = FASTYBIRD_PUB_CHANNEL_CONTROL_STRUCTURE;
            }
            break;

// -----------------------------------------------------------------------------
// CHANNEL PROPERTIES
// -----------------------------------------------------------------------------

        case FASTYBIRD_PUB_CHANNEL_PROPERTY:
            if (_fastybird_gateway_nodes[nodeId].channels[channelIndex].properties.size() > 0) {
                for (uint8_t i = 0; i < _fastybird_gateway_nodes[nodeId].channels[channelIndex].properties.size(); i++) {
                    if (_fastybird_gateway_nodes[nodeId].channels[channelIndex].properties[i].initialized) {
                        continue;
                    }

                    if (!_fastybirdInitializeNodeChannelProperty(nodeId, _fastybird_gateway_nodes[nodeId].channels[channelIndex], _fastybird_gateway_nodes[nodeId].channels[channelIndex].properties[i])) {
                        return false;
                    }

                    _fastybird_gateway_nodes[nodeId].channels[channelIndex].properties[i].initialized = true;

                    _fastybird_node_channel_property_advertisement_progress = FASTYBIRD_PUB_CHANNEL_PROPERTY_NAME;
                }
            }

            if (_fastybird_gateway_nodes[nodeId].channels[channelIndex].isConfigurable) {
                _fastybird_node_channel_advertisement_progress = FASTYBIRD_PUB_CHANNEL_CONFIGURATION_SCHEMA;
 
            } else {
                _fastybird_node_channel_advertisement_progress = FASTYBIRD_PUB_CHANNEL_DONE;
            }
            break;

// -----------------------------------------------------------------------------
// CHANNEL CONTROL CONFIGURATION
// -----------------------------------------------------------------------------

        case FASTYBIRD_PUB_CHANNEL_CONTROL_STRUCTURE:
            _fastybird_node_channel_advertisement_progress = FASTYBIRD_PUB_CHANNEL_CONFIGURATION_SCHEMA;
            break;

        case FASTYBIRD_PUB_CHANNEL_CONFIGURATION_SCHEMA:
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

    _fastybird_initialize_node = nodeId;

    std::vector<String> node_properties;
    std::vector<String> node_stats;

    switch (_fastybird_node_advertisement_progress) {
        case FASTYBIRD_PUB_CONNECTION:
            // Notify cloud broker that node thing is sending initialization sequences
            if (!_fastybirdPropagateThingProperty(_fastybird_gateway_nodes[nodeId].id, FASTYBIRD_PROPERTY_STATE, FASTYBIRD_STATUS_INIT)) {
                return;
            }

            // Collect all node thing properties...
            node_properties.push_back(FASTYBIRD_PROPERTY_UPTIME);
            node_properties.push_back(FASTYBIRD_PROPERTY_INTERVAL);
            node_properties.push_back(FASTYBIRD_PROPERTY_STATE);

            // ...and pass them to the broker
            if (!_fastybirdPropagateThingPropertiesStructure(_fastybird_gateway_nodes[nodeId].id, node_properties)) {
                return;
            }

            // Heartbeat interval
            if (!_fastybirdPropagateThingProperty(_fastybird_gateway_nodes[nodeId].id, FASTYBIRD_PROPERTY_INTERVAL, String(HEARTBEAT_INTERVAL / 1000).c_str())) {
                return;
            }

            _fastybird_node_advertisement_progress = FASTYBIRD_PUB_NAME;
            break;

        case FASTYBIRD_PUB_NAME:
            if (!_fastybirdPropagateThingName(_fastybird_gateway_nodes[nodeId].id, _fastybird_gateway_nodes[nodeId].id)) {
                return;
            }

            _fastybird_node_advertisement_progress = FASTYBIRD_PUB_HARDWARE;
            break;

        // Describe thing hardware details to cloud broker
        case FASTYBIRD_PUB_HARDWARE:
            if (!_fastybirdPropagateThingHardwareManufacturer(_fastybird_gateway_nodes[nodeId].id, _fastybird_gateway_nodes[nodeId].hardware.manufacturer)) {
                return;
            }

            if (!_fastybirdPropagateThingHardwareModelName(_fastybird_gateway_nodes[nodeId].id, _fastybird_gateway_nodes[nodeId].hardware.name)) {
                return;
            }

            if (!_fastybirdPropagateThingHardwareVersion(_fastybird_gateway_nodes[nodeId].id, _fastybird_gateway_nodes[nodeId].hardware.version)) {
                return;
            }

            _fastybird_node_advertisement_progress = FASTYBIRD_PUB_FIRMWARE;
            break;

        // Describe thing firmware details to cloud broker
        case FASTYBIRD_PUB_FIRMWARE:
            if (!_fastybirdPropagateThingFirmwareName(_fastybird_gateway_nodes[nodeId].id, _fastybird_gateway_nodes[nodeId].software.name)) {
                return;
            }

            if (!_fastybirdPropagateThingFirmwareVersion(_fastybird_gateway_nodes[nodeId].id, _fastybird_gateway_nodes[nodeId].software.version)) {
                return;
            }

            if (!_fastybirdPropagateThingFirmwareManufacturer(_fastybird_gateway_nodes[nodeId].id, _fastybird_gateway_nodes[nodeId].software.manufacturer)) {
                return;
            }

            _fastybird_node_advertisement_progress = FASTYBIRD_PUB_CHANNELS;
            break;

        case FASTYBIRD_PUB_CHANNELS:
            if (_fastybird_gateway_nodes[nodeId].channels.size() <= 0) {
                _fastybird_node_advertisement_progress = FASTYBIRD_PUB_CONTROL_STRUCTURE;

                return;
            }

            if (!_fastybirdPropagateThingChannels(_fastybird_gateway_nodes[nodeId].id, _fastybird_gateway_nodes[nodeId].channels)) {
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

                    if (_fastybird_gateway_nodes[nodeId].channels[i].length <= 0) {
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
            if (!_fastybirdPropagateThingProperty(_fastybird_gateway_nodes[nodeId].id, FASTYBIRD_PROPERTY_STATE, FASTYBIRD_STATUS_READY)) {
                return;
            }

            _fastybird_node_advertisement_progress = FASTYBIRD_PUB_CONFIGURATION;
            break;

        case FASTYBIRD_PUB_CONFIGURATION:
            _fastybird_node_advertisement_progress = FASTYBIRD_PUB_CHANNELS_CONFIGURATION;
            break;

        case FASTYBIRD_PUB_CHANNELS_CONFIGURATION:
            _fastybird_node_advertisement_progress = FASTYBIRD_PUB_CHANNELS_SCHEDULE;
            break;

        case FASTYBIRD_PUB_CHANNELS_SCHEDULE:
            _fastybird_node_advertisement_progress = FASTYBIRD_PUB_HEARTBEAT;
            break;

        case FASTYBIRD_PUB_HEARTBEAT:
            for (uint8_t j = 0; j < _fastybird_gateway_nodes[nodeId].channels.size(); j++) {
                if (strcmp(_fastybird_gateway_nodes[nodeId].channels[j].type.c_str(), FASTYBIRD_CHANNEL_BINARY_SENSOR) == 0) {
                    for (uint8_t cnt = 0; cnt < _fastybird_gateway_nodes[nodeId].channels[j].length; cnt++) {
                        _fastybirdReportNodeChannelValue(
                            _fastybird_gateway_nodes[nodeId].id,
                            0,
                            cnt,
                            gatewayReadDigitalValue(nodeId, GATEWAY_REGISTER_DI, cnt) ? FASTYBIRD_SWITCH_PAYLOAD_ON : FASTYBIRD_SWITCH_PAYLOAD_OFF
                        );
                    }

                } else if (strcmp(_fastybird_gateway_nodes[nodeId].channels[j].type.c_str(), FASTYBIRD_CHANNEL_BINARY_ACTOR) == 0) {
                    for (uint8_t cnt = 0; cnt < _fastybird_gateway_nodes[nodeId].channels[j].length; cnt++) {
                        _fastybirdReportNodeChannelValue(
                            _fastybird_gateway_nodes[nodeId].id,
                            1,
                            cnt,
                            gatewayReadDigitalValue(nodeId, GATEWAY_REGISTER_DO, cnt) ? FASTYBIRD_SWITCH_PAYLOAD_ON : FASTYBIRD_SWITCH_PAYLOAD_OFF
                        );
                    }
                }
            }

            _fastybird_gateway_nodes[nodeId].initialized = true;

            _fastybird_initialize_node = 0xFF;
            break;
    }
}

// -----------------------------------------------------------------------------

void _fastybirdNodeLoop() {
    if (_fastybirdIsApiReady() && fastybirdIsThingInitialzed()) {
        if (_fastybird_initialize_node != 0xFF) {
            _fastybirdInitializeNode(_fastybird_initialize_node);

        } else {
            for (uint8_t i = 0; i < _fastybird_gateway_nodes.size(); i++) {
                if (_fastybird_gateway_nodes[i].initialized == false) {
                    _fastybird_node_advertisement_progress = FASTYBIRD_PUB_CONNECTION;

                    _fastybirdInitializeNode(i);
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
