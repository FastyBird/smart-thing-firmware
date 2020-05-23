/*

FASTYBIRD NODES MODULE

Copyright (C) 2018 FastyBird Ltd. <info@fastybird.com>

*/

#if FASTYBIRD_SUPPORT && FASTYBIRD_NODES_SUPPORT

// Initilalization pointers
uint8_t _fastybird_initialize_node = INDEX_NONE;
uint8_t _fastybird_initialize_node_property = INDEX_NONE;
uint8_t _fastybird_initialize_node_channel = INDEX_NONE;
uint8_t _fastybird_initialize_node_channel_property = INDEX_NONE;

// Initialization step pointers
uint8_t _fastybird_node_advertisement_progress = FASTYBIRD_PUB_CONNECTION;
uint8_t _fastybird_node_property_advertisement_progress = FASTYBIRD_PUB_PROPERTY_NAME;
uint8_t _fastybird_node_channel_advertisement_progress = FASTYBIRD_PUB_CHANNEL_NAME;
uint8_t _fastybird_node_channel_property_advertisement_progress = FASTYBIRD_PUB_PROPERTY_NAME;

// Collections
std::vector<fastybird_node_t> _fastybird_nodes;
std::vector<fastybird_node_channel_t> _fastybird_nodes_channels;
std::vector<fastybird_node_property_t> _fastybird_nodes_properties;

// -----------------------------------------------------------------------------
// MODULE PRIVATE 
// -----------------------------------------------------------------------------

void _fastybirdNodesSendHeartbeat()
{
    for (uint8_t i = 0; i < _fastybird_nodes.size(); i++) {
        if (
            _fastybird_nodes[i].initialized
            && _fastybird_nodes[i].ready
        ) {
            fastybirdApiPropagateDevicePropertyValue(
                _fastybird_nodes[i].id,
                FASTYBIRD_PROPERTY_UPTIME,
                String(getUptime()).c_str()
            );
        }
    }
}

// -----------------------------------------------------------------------------

/**
 * Initialize given node device channel property
 */
bool _fastybirdNodesInitializeChannelProperty(
    const uint8_t nodeIndex,
    const uint8_t channelIndex,
    const uint8_t propertyIndex
) {
    if (
        nodeIndex >= _fastybird_nodes.size()
        || channelIndex >= _fastybird_nodes_channels.size()
        || propertyIndex >= _fastybird_nodes_properties.size()
    ) {
        return true;
    }

    fastybird_node_t node = _fastybird_nodes[nodeIndex];
    fastybird_node_channel_t channel = _fastybird_nodes_channels[channelIndex];
    fastybird_node_property_t property = _fastybird_nodes_properties[propertyIndex];

    switch (_fastybird_node_channel_property_advertisement_progress)
    {
        case FASTYBIRD_PUB_PROPERTY_NAME:
            if (!fastybirdApiPropagateChannelPropertyName(node.id, channel.name.c_str(), property.name.c_str(), property.name.c_str())) {
                return false;
            }

            _fastybird_node_channel_property_advertisement_progress = FASTYBIRD_PUB_PROPERTY_SETABLE;
            break;

        case FASTYBIRD_PUB_PROPERTY_SETABLE:
            if (!fastybirdApiPropagateChannelPropertySettable(node.id, channel.name.c_str(), property.name.c_str(), property.settable)) {
                return false;
            }

            _fastybird_node_channel_property_advertisement_progress = FASTYBIRD_PUB_PROPERTY_QUERYABLE;
            break;

        case FASTYBIRD_PUB_PROPERTY_QUERYABLE:
            if (!fastybirdApiPropagateChannelPropertyQueryable(node.id, channel.name.c_str(), property.name.c_str(), property.queryable)) {
                return false;
            }

            _fastybird_node_channel_property_advertisement_progress = FASTYBIRD_PUB_PROPERTY_DATA_TYPE;
            break;

        case FASTYBIRD_PUB_PROPERTY_DATA_TYPE:
            if (!fastybirdApiPropagateChannelPropertyDataType(node.id, channel.name.c_str(), property.name.c_str(), property.datatype.c_str())) {
                return false;
            }

            _fastybird_node_channel_property_advertisement_progress = FASTYBIRD_PUB_PROPERTY_UNIT;
            break;

        case FASTYBIRD_PUB_PROPERTY_UNIT:
            if (!fastybirdApiPropagateChannelPropertyUnit(node.id, channel.name.c_str(), property.name.c_str(), property.unit.c_str())) {
                return false;
            }

            _fastybird_node_channel_property_advertisement_progress = FASTYBIRD_PUB_PROPERTY_FORMAT;
            break;

        case FASTYBIRD_PUB_PROPERTY_FORMAT:
            if (!fastybirdApiPropagateChannelPropertyFormat(node.id, channel.name.c_str(), property.name.c_str(), property.format.c_str())) {
                return false;
            }

            _fastybird_node_channel_property_advertisement_progress = FASTYBIRD_PUB_PROPERTY_DONE;
            break;

// -----------------------------------------------------------------------------
// CHANNEL PROPERTY INITIALIZATION IS DONE
// -----------------------------------------------------------------------------

        case FASTYBIRD_PUB_PROPERTY_DONE:
            return true;
    }

    return false;
}

// -----------------------------------------------------------------------------

bool _fastybirdNodesInitializeChannel(
    const uint8_t nodeIndex,
    const uint8_t channelIndex
) {
    if (
        nodeIndex >= _fastybird_nodes.size()
        || channelIndex >= _fastybird_nodes_channels.size()
    ) {
        return true;
    }

    DynamicJsonBuffer jsonBuffer;

    JsonArray& configurationSchema = jsonBuffer.createArray();

    std::vector<String> properties;
    std::vector<String> controls;

    fastybird_node_t node = _fastybird_nodes[nodeIndex];
    fastybird_node_channel_t channel = _fastybird_nodes_channels[channelIndex];

    switch (_fastybird_node_channel_advertisement_progress)
    {
        
// -----------------------------------------------------------------------------
// CHANNEL BASIC INFO
// -----------------------------------------------------------------------------

        case FASTYBIRD_PUB_CHANNEL_NAME:
            if (!fastybirdApiPropagateChannelName(node.id, channel.name.c_str(), channel.name.c_str())) {
                return false;
            }

            _fastybird_node_channel_advertisement_progress = FASTYBIRD_PUB_CHANNEL_PROPERTIES;
            break;

        case FASTYBIRD_PUB_CHANNEL_PROPERTIES:
            if (channel.properties.size() > 0) {
                for (uint8_t i = 0; i < channel.properties.size(); i++) {
                    if (channel.properties[i] < _fastybird_nodes_properties.size()) {
                        properties.push_back(_fastybird_nodes_properties[channel.properties[i]].name);
                    }
                }
            }

            if (!fastybirdApiPropagateChannelPropertiesNames(node.id, channel.name.c_str(), properties)) {
                return false;
            }

            _fastybird_node_channel_advertisement_progress = FASTYBIRD_PUB_CHANNEL_CONTROL_STRUCTURE;
            break;

        case FASTYBIRD_PUB_CHANNEL_CONTROL_STRUCTURE:
            if (!fastybirdApiPropagateChannelControlNames(node.id, channel.name.c_str(), controls)) {
                return false;
            }

            _fastybird_node_channel_advertisement_progress = FASTYBIRD_PUB_CHANNEL_PROPERTY;
            break;

// -----------------------------------------------------------------------------
// CHANNEL PROPERTIES
// -----------------------------------------------------------------------------

        case FASTYBIRD_PUB_CHANNEL_PROPERTY:
            if (channel.properties.size() > 0) {
                // Take properties pointer
                uint8_t propertyIndex = _fastybird_initialize_node_channel_property;

                // No properties were initialized...
                if (propertyIndex == INDEX_NONE) {
                    // ...pick first from channel properties collection
                    propertyIndex = channel.properties[0];

                    // Update property pointer
                    _fastybird_initialize_node_channel_property = propertyIndex;
                }

                // Check if property index is in valid range
                if (propertyIndex < _fastybird_nodes_properties.size()) {
                    // Process property initialization
                    if (!_fastybirdNodesInitializeChannelProperty(nodeIndex, channelIndex, propertyIndex)) {
                        return false;
                    }

                    // Property was fully initialized
                    _fastybird_node_channel_property_advertisement_progress = FASTYBIRD_PUB_PROPERTY_NAME;

                    // Search for new property index
                    for (uint8_t i = 0; i < channel.properties.size(); i++) {
                        // Search for property index position in collection...
                        if (channel.properties[i] == propertyIndex) {
                            // Check if there is left any uninitialized property...
                            if ((i + 1) < channel.properties.size()) {
                                // ...if yes, update pointer
                                _fastybird_initialize_node_channel_property = channel.properties[i + 1];

                                return false;
                            }

                            // ...if not, finish properties initialization
                        }
                    }
                }
            }

            // Reset property initialize step pointer
            _fastybird_node_channel_property_advertisement_progress = FASTYBIRD_PUB_PROPERTY_NAME;

            // Reset properties process pointer
            _fastybird_initialize_node_channel_property = INDEX_NONE;

            // Move to the next step
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

void _fastybirdNodesInitialize(
    const uint8_t nodeIndex
) {
    if (
        nodeIndex >= _fastybird_nodes.size()
        || _fastybird_nodes[nodeIndex].initialized
    ) {
        _fastybird_initialize_node = INDEX_NONE;

        return;
    }

    fastybird_node_t node = _fastybird_nodes[nodeIndex];

    std::vector<String> channels;
    std::vector<String> properties;
    std::vector<String> controls;

    switch (_fastybird_node_advertisement_progress)
    {
        case FASTYBIRD_PUB_CONNECTION:
            // Notify broker that node is sending initialization sequences
            if (!fastybirdApiPropagateDeviceState(node.id, FASTYBIRD_STATUS_INIT)) {
                return;
            }

            // Collect all node default properties...
            properties.push_back(FASTYBIRD_PROPERTY_UPTIME);
            properties.push_back(FASTYBIRD_PROPERTY_INTERVAL);

            // ...and pass them to the broker
            if (!fastybirdApiPropagateDevicePropertiesNames(node.id, properties)) {
                return;
            }

            // Heartbeat interval
            if (!fastybirdApiPropagateDevicePropertyValue(node.id, FASTYBIRD_PROPERTY_INTERVAL, String(HEARTBEAT_INTERVAL / 1000).c_str())) {
                return;
            }

            _fastybird_node_advertisement_progress = FASTYBIRD_PUB_NAME;
            break;

        case FASTYBIRD_PUB_NAME:
            if (!fastybirdApiPropagateDeviceName(node.id, node.id)) {
                return;
            }

            _fastybird_node_advertisement_progress = FASTYBIRD_PUB_PARENT;
            break;

        case FASTYBIRD_PUB_PARENT:
            if (!fastybirdApiPropagateDeviceParent(node.id, fastybirdDeviceIdentifier().c_str())) {
                return;
            }

            _fastybird_node_advertisement_progress = FASTYBIRD_PUB_HARDWARE;
            break;

        // Describe device hardware details to cloud broker
        case FASTYBIRD_PUB_HARDWARE:
            if (!fastybirdApiPropagateDeviceHardwareManufacturer(node.id, node.hardware.manufacturer)) {
                return;
            }

            if (!fastybirdApiPropagateDeviceHardwareModelName(node.id, node.hardware.model)) {
                return;
            }

            if (!fastybirdApiPropagateDeviceHardwareVersion(node.id, node.hardware.version)) {
                return;
            }

            if (!fastybirdApiPropagateDeviceHardwareSerialNumber(node.id, node.id)) {
                return;
            }

            _fastybird_node_advertisement_progress = FASTYBIRD_PUB_FIRMWARE;
            break;

        // Describe device firmware details to cloud broker
        case FASTYBIRD_PUB_FIRMWARE:
            if (!fastybirdApiPropagateDeviceFirmwareManufacturer(node.id, node.firmware.manufacturer)) {
                return;
            }

            if (!fastybirdApiPropagateDeviceFirmwareName(node.id, node.firmware.name)) {
                return;
            }

            if (!fastybirdApiPropagateDeviceFirmwareVersion(node.id, node.firmware.version)) {
                return;
            }

            _fastybird_node_advertisement_progress = FASTYBIRD_PUB_CHANNELS;
            break;

        case FASTYBIRD_PUB_CHANNELS:
            for (uint8_t i = 0; i < node.channels.size(); i++) {
                if (node.channels[i] < _fastybird_nodes_channels.size()) {
                    channels.push_back(_fastybird_nodes_channels[node.channels[i]].name);
                }
            }

            if (!fastybirdApiPropagateDeviceChannelsNames(node.id, channels)) {
                return;
            }

            _fastybird_node_advertisement_progress = FASTYBIRD_PUB_CONTROL_STRUCTURE;
            break;

        case FASTYBIRD_PUB_CONTROL_STRUCTURE:
            if (!fastybirdApiPropagateDeviceControlNames(node.id, controls)) {
                return;
            }

            _fastybird_node_advertisement_progress = FASTYBIRD_PUB_CONFIGURATION_SCHEMA;
            break;

        case FASTYBIRD_PUB_CONFIGURATION_SCHEMA:
            _fastybird_node_advertisement_progress = FASTYBIRD_PUB_INITIALIZE_CHANNELS;
            break;

        case FASTYBIRD_PUB_INITIALIZE_CHANNELS:
            if (node.channels.size() > 0) {
                // Take channels pointer
                uint8_t channelIndex = _fastybird_initialize_node_channel;

                // No channels were initialized...
                if (channelIndex == INDEX_NONE) {
                    // ...pick first from node channels collection
                    channelIndex = node.channels[0];

                    // Update channel pointer
                    _fastybird_initialize_node_channel = channelIndex;
                }

                // Check if channel index is in valid range
                if (channelIndex < _fastybird_nodes_channels.size()) {
                    // Process channel initialization
                    if (!_fastybirdNodesInitializeChannel(nodeIndex, channelIndex)) {
                        return;
                    }

                    // Channel was fully initialized
                    _fastybird_node_channel_advertisement_progress = FASTYBIRD_PUB_CHANNEL_NAME;

                    // Search for new channel index
                    for (uint8_t i = 0; i < node.channels.size(); i++) {
                        // Search for channel index position in collection...
                        if (node.channels[i] == channelIndex) {
                            // Check if there is left any uninitialized channel...
                            if ((i + 1) < node.channels.size()) {
                                // ...if yes, update pointer
                                _fastybird_initialize_node_channel = node.channels[i + 1];

                                return;
                            }

                            // ...if not, finish channels initialization
                        }
                    }
                }
            }

            // Reset channel initialize step pointer
            _fastybird_node_channel_advertisement_progress = FASTYBIRD_PUB_CHANNEL_NAME;

            // Reset channels process pointer
            _fastybird_initialize_node_channel = INDEX_NONE;

            // Move to the next step
            _fastybird_node_advertisement_progress = FASTYBIRD_PUB_READY;
            break;

        case FASTYBIRD_PUB_READY:
            if (node.ready) {
                if (!fastybirdApiPropagateDeviceState(node.id, FASTYBIRD_STATUS_READY)) {
                    return;
                }

            } else {
                if (!fastybirdApiPropagateDeviceState(node.id, FASTYBIRD_STATUS_DISCONNECTED)) {
                    return;
                }
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
            _fastybird_nodes[nodeIndex].initialized = true;

            _fastybird_initialize_node = INDEX_NONE;
            break;
    }
}

// -----------------------------------------------------------------------------
// MODULE: SUB-MODULE API
// -----------------------------------------------------------------------------

uint8_t fastybirdNodesRegisterNode(
    const char * serialNumber,
    const char * hardwareModel,
    const char * hardwareVersion,
    const char * hardwareManufacturer,
    const char * firmwareName,
    const char * firmwareVersion,
    const char * firmwareManufacturer,
    bool ready
) {
    for (uint8_t i = 0; i < _fastybird_nodes.size(); i++) {
        // Check if node with same serial no is allready registered
        if (strcmp(_fastybird_nodes[i].id, serialNumber)) {
            return i;
        }
    }

    fastybird_node_hardware_t hardware = {
        "",
        "",
        "",
    };

    strcpy(hardware.model, hardwareModel);
    strcpy(hardware.version, hardwareVersion);
    strcpy(hardware.manufacturer, hardwareManufacturer);

    fastybird_node_firmware_t firmware = {
        "",
        "",
        "",
    };

    strcpy(firmware.name, firmwareName);
    strcpy(firmware.version, firmwareVersion);
    strcpy(firmware.manufacturer, firmwareManufacturer);

    fastybird_node_t node = {
        "",
        hardware,
        firmware,
        false,
        false,
        ready
    };

    strcpy(node.id, serialNumber);

    _fastybird_nodes.push_back(node);

    DEBUG_MSG(PSTR("[INFO][FASTYBIRD][NODE] Inserting node: %s\n"), node.id);

    return (_fastybird_nodes.size() - 1);
}

// -----------------------------------------------------------------------------

void fastybirdNodesUnregisterNode(
    const uint8_t nodeIndex
) {
    // TODO: Implement node unregistration process
}

// -----------------------------------------------------------------------------

fastybird_node_t fastybirdNodesGetNode(
    const uint8_t nodeIndex
) {
    return _fastybird_nodes[nodeIndex];
}

// -----------------------------------------------------------------------------

void fastybirdNodesNodeIsLost(
    const uint8_t nodeIndex
) {
    if (nodeIndex >= _fastybird_nodes.size()) {
        return;
    }

    _fastybird_nodes[nodeIndex].ready = false;

    fastybird_node_t node = _fastybird_nodes[nodeIndex];

    fastybirdApiPropagateDeviceState(node.id, FASTYBIRD_STATUS_LOST);
}

// -----------------------------------------------------------------------------

void fastybirdNodesNodeIsReady(
    const uint8_t nodeIndex
) {
    if (nodeIndex >= _fastybird_nodes.size()) {
        return;
    }

    _fastybird_nodes[nodeIndex].ready = true;

    fastybird_node_t node = _fastybird_nodes[nodeIndex];

    fastybirdApiPropagateDeviceState(node.id, FASTYBIRD_STATUS_READY);
}

// -----------------------------------------------------------------------------

uint8_t fastybirdNodesFindNodeIndex(
    String serialNumber
) {
    String serial_no_tmp;

    serialNumber.toLowerCase();

    for (uint8_t i = 0; i < _fastybird_nodes.size(); i++) {
        serial_no_tmp = _fastybird_nodes[i].id;
        serial_no_tmp.toLowerCase();

        if (serialNumber.equals(serial_no_tmp)) {
            return i;
        }
    }

    return INDEX_NONE;
}

// -----------------------------------------------------------------------------

uint8_t fastybirdNodesRegisterNodeChannel(
    const char * name
) {
    fastybird_node_channel_t channel = {
        String(name)
    };

    _fastybird_nodes_channels.push_back(channel);

    return (_fastybird_nodes_channels.size() - 1);
}

// -----------------------------------------------------------------------------

bool fastybirdNodesMapChannelToNode(
    const uint8_t nodeIndex,
    const uint8_t channelIndex
) {
    if (
        nodeIndex >= _fastybird_nodes.size()
        || channelIndex >= _fastybird_nodes_channels.size()
    ) {
        return false;
    }

    for (uint8_t i = 0; i < _fastybird_nodes[nodeIndex].channels.size(); i++) {
        // Check if channel is not mapped with node
        if (channelIndex == _fastybird_nodes[nodeIndex].channels[i]) {
            return true;
        }
    }

    DEBUG_MSG(
        PSTR("[INFO][FASTYBIRD][NODE] Map channel: %s to node: %s\n"),
        _fastybird_nodes_channels[channelIndex].name.c_str(),
        _fastybird_nodes[nodeIndex].id
    );

    _fastybird_nodes[nodeIndex].channels.push_back(channelIndex);

    return true;
}

// -----------------------------------------------------------------------------

uint8_t fastybirdNodesFindChannelIndex(
    const uint8_t nodeIndex,
    String name
) {
    if (nodeIndex >= _fastybird_nodes.size()) {
        return INDEX_NONE;
    }

    fastybird_node_t node = _fastybird_nodes[nodeIndex];

    for (uint8_t i = 0; i < node.channels.size(); i++) {
        if (_fastybird_nodes_channels[node.channels[i]].name.equals(name)) {
            return node.channels[i];
        }
    }

    return INDEX_NONE;
}

// -----------------------------------------------------------------------------

fastybird_node_channel_t fastybirdNodesGetChannel(
    const uint8_t channelIndex
) {
    return _fastybird_nodes_channels[channelIndex];
}

// -----------------------------------------------------------------------------

uint8_t fastybirdNodesRegisterNodeProperty(
    char name[],
    char dataType[],
    char units[],
    char format[]
) {
    fastybird_node_property_t property = {
        String(name),
        false,
        false,
        String(dataType),
        String(units),
        String(format)
    };

    _fastybird_nodes_properties.push_back(property);

    return (_fastybird_nodes_properties.size() - 1);
}

// -----------------------------------------------------------------------------

uint8_t fastybirdNodesRegisterNodeProperty(
    char name[],
    char dataType[],
    char units[],
    char format[],
    fastybird_node_properties_process_payload_f payloadCallback
) {
    uint8_t propertyIndex = fastybirdNodesRegisterNodeProperty(
        name,
        dataType,
        units,
        format
    );

    _fastybird_nodes_properties[propertyIndex].settable = true;
    _fastybird_nodes_properties[propertyIndex].payload_callback = payloadCallback;

    return propertyIndex;
}

// -----------------------------------------------------------------------------

uint8_t fastybirdNodesRegisterNodeProperty(
    char name[],
    char dataType[],
    char units[],
    char format[],
    fastybird_node_properties_process_query_f queryCallback
) {
    uint8_t propertyIndex = fastybirdNodesRegisterNodeProperty(
        name,
        dataType,
        units,
        format
    );

    _fastybird_nodes_properties[propertyIndex].queryable = true;
    _fastybird_nodes_properties[propertyIndex].query_callback = queryCallback;

    return propertyIndex;
}

// -----------------------------------------------------------------------------

uint8_t fastybirdNodesRegisterNodeProperty(
    char name[],
    char dataType[],
    char units[],
    char format[],
    fastybird_node_properties_process_payload_f payloadCallback,
    fastybird_node_properties_process_query_f queryCallback
) {
    uint8_t propertyIndex = fastybirdNodesRegisterNodeProperty(
        name,
        dataType,
        units,
        format
    );

    _fastybird_nodes_properties[propertyIndex].settable = true;
    _fastybird_nodes_properties[propertyIndex].payload_callback = payloadCallback;

    _fastybird_nodes_properties[propertyIndex].queryable = true;
    _fastybird_nodes_properties[propertyIndex].query_callback = queryCallback;

    return propertyIndex;
}

// -----------------------------------------------------------------------------

uint8_t fastybirdNodesFindChannelPropertyIndex(
    const uint8_t channelIndex,
    String name
) {
    if (channelIndex >= _fastybird_nodes_channels.size()) {
        return INDEX_NONE;
    }

    fastybird_node_channel_t channel = _fastybird_nodes_channels[channelIndex];

    for (uint8_t i = 0; i < channel.properties.size(); i++) {
        if (_fastybird_nodes_properties[channel.properties[i]].name.equals(name)) {
            return channel.properties[i];
        }
    }

    return INDEX_NONE;
}

// -----------------------------------------------------------------------------

fastybird_node_property_t fastybirdNodesGetProperty(
    const uint8_t propertyIndex
) {
    return _fastybird_nodes_properties[propertyIndex];
}

// -----------------------------------------------------------------------------

bool fastybirdNodesMapPropertyToChannel(
    const uint8_t channelIndex,
    const uint8_t propertyIndex
) {
    if (
        channelIndex >= _fastybird_nodes_channels.size()
        || propertyIndex >= _fastybird_nodes_properties.size()
    ) {
        return false;
    }

    for (uint8_t i = 0; i < _fastybird_nodes_channels[channelIndex].properties.size(); i++) {
        // Check if property is not mapped with channel
        if (propertyIndex == _fastybird_nodes_channels[channelIndex].properties[i]) {
            return true;
        }
    }

    DEBUG_MSG(
        PSTR("[INFO][FASTYBIRD][NODE] Map property: %s to channel: %s\n"),
        _fastybird_nodes_properties[propertyIndex].name.c_str(),
        _fastybird_nodes_channels[channelIndex].name.c_str()
    );

    _fastybird_nodes_channels[channelIndex].properties.push_back(propertyIndex);

    return true;
}

// -----------------------------------------------------------------------------

bool fastybirdNodesReportChannelPropertyValue(
    const uint8_t nodeIndex,
    const uint8_t channelIndex,
    const uint8_t propertyIndex,
    const char * payload
) {
    if (nodeIndex >= _fastybird_nodes.size()) {
        return false;
    }

    fastybird_node_t node = _fastybird_nodes[nodeIndex];

    // Node have to be initialized
    if (node.initialized) {
        if (channelIndex >= _fastybird_nodes_channels.size()) {
            DEBUG_MSG(PSTR("[FB NODES] Channel index is not valid\n"));

            return false;
        }

        fastybird_node_channel_t channel = _fastybird_nodes_channels[channelIndex];

        if (propertyIndex >= _fastybird_nodes_properties.size()) {
            DEBUG_MSG(PSTR("[FB NODES] Property index is not valid\n"));

            return false;
        }

        fastybird_node_property_t property = _fastybird_nodes_properties[propertyIndex];

        return fastybirdApiPropagateChannelPropertyValue(
            node.id,
            channel.name.c_str(),
            property.name.c_str(),
            payload
        );

    } else {
        DEBUG_MSG(PSTR("[FB NODES] Node is not initialized\n"));
    }

    return false;
}

// -----------------------------------------------------------------------------

void fastybirdNodesResetNodesInitialization()
{
    _fastybird_initialize_node = INDEX_NONE;
    _fastybird_initialize_node_property = INDEX_NONE;
    _fastybird_initialize_node_channel = INDEX_NONE;
    _fastybird_initialize_node_channel_property = INDEX_NONE;

    _fastybird_node_advertisement_progress = FASTYBIRD_PUB_CONNECTION;
    _fastybird_node_property_advertisement_progress = FASTYBIRD_PUB_PROPERTY_NAME;
    _fastybird_node_channel_advertisement_progress = FASTYBIRD_PUB_CHANNEL_NAME;
    _fastybird_node_channel_property_advertisement_progress = FASTYBIRD_PUB_PROPERTY_NAME;

    for (uint8_t i = 0; i < _fastybird_nodes.size(); i++) {
        _fastybird_nodes[i].initialized = false;
    }
}

// -----------------------------------------------------------------------------
// MODULE: SUB-MODULE CORE
// -----------------------------------------------------------------------------

void fastybirdNodesSetup()
{
    systemOnHeartbeatRegister(_fastybirdNodesSendHeartbeat);
}

// -----------------------------------------------------------------------------

void fastybirdNodesLoop()
{
    // FastyBird API have to be initialized & also device have to be initialized
    if (fastybirdApiIsReady() && fastybirdIsDeviceInitialzed()) {
        if (_fastybird_initialize_node != INDEX_NONE) {
            _fastybirdNodesInitialize(_fastybird_initialize_node);

        } else {
            for (uint8_t i = 0; i < _fastybird_nodes.size(); i++) {
                if (
                    _fastybird_nodes[i].initialized == false
                    && _fastybird_nodes[i].channels.size() > 0 // Node have to have at leas one channel
                ) {
                    _fastybird_initialize_node = i;

                    _fastybird_node_advertisement_progress = FASTYBIRD_PUB_CONNECTION;

                    _fastybirdNodesInitialize(_fastybird_initialize_node);
                    break;
                }
            }
        }
    }
}

#endif // FASTYBIRD_SUPPORT && FASTYBIRD_NODES_SUPPORT
