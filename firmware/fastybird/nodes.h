/*

FASTYBIRD NODES MODULE

Copyright (C) 2018 FastyBird Ltd. <info@fastybird.com>

*/

#if FASTYBIRD_SUPPORT && FASTYBIRD_GATEWAY_SUPPORT

#include <cppQueue.h>

std::vector<fastybird_node_t> _fastybird_gateway_nodes;

const char * _fastybird_initialize_node = NULL;

Queue _fastybird_gateway_nodes_initialize(sizeof(_fastybird_initialize_node), 50, FIFO);

unsigned int _fastybird_node_advertisement_progress;

// -----------------------------------------------------------------------------
// MODULE PRIVATE 
// -----------------------------------------------------------------------------

void _fastybirdNodeSendHeartbeat(fastybird_node_t& node) {

}

// -----------------------------------------------------------------------------

void _fastybirdInitializeNode(fastybird_node_t& node) {
    if (node.initialized) {
        _fastybird_initialize_node = NULL;

        return;
    }

    _fastybird_initialize_node = node.id;

    std::vector<String> node_properties;
    std::vector<String> node_stats;

    switch (_fastybird_node_advertisement_progress) {
        case FASTYBIRD_PUB_CONNECTION:
            // Notify cloud broker that node thing is sending initialization sequences
            if (!_fastybirdPropagateThingProperty(node.id, FASTYBIRD_PROPERTY_STATE, FASTYBIRD_STATUS_INIT)) {
                return;
            }

            // Collect all node thing properties...
            node_properties.push_back(FASTYBIRD_PROPERTY_STATE);

            // ...and pass them to the broker
            if (!_fastybirdPropagateThingPropertiesStructure(node.id, node_properties)) {
                return;
            }

            _fastybird_node_advertisement_progress = FASTYBIRD_PUB_NAME;
            break;

        case FASTYBIRD_PUB_NAME:
            if (!_fastybirdPropagateThingName(node.id, node.id)) {
                return;
            }

            _fastybird_node_advertisement_progress = FASTYBIRD_PUB_HARDWARE;
            break;

        // Describe thing hardware details to cloud broker
        case FASTYBIRD_PUB_HARDWARE:
            if (!_fastybirdPropagateThingHardwareName(node.id, node.hardware.name)) {
                return;
            }

            if (!_fastybirdPropagateThingHardwareModelName(node.id, node.hardware.version)) {
                return;
            }

            if (!_fastybirdPropagateThingHardwareManufacturer(node.id, node.hardware.manufacturer)) {
                return;
            }

            _fastybird_node_advertisement_progress = FASTYBIRD_PUB_FIRMWARE;
            break;

        // Describe thing firmware details to cloud broker
        case FASTYBIRD_PUB_FIRMWARE:
            if (!_fastybirdPropagateThingFirmwareName(node.id, node.software.name)) {
                return;
            }

            if (!_fastybirdPropagateThingFirmwareVersion(node.id, node.software.version)) {
                return;
            }

            if (!_fastybirdPropagateThingFirmwareManufacturer(node.id, node.software.manufacturer)) {
                return;
            }

            _fastybird_node_advertisement_progress = FASTYBIRD_PUB_CHANNELS;
            break;

        case FASTYBIRD_PUB_CHANNELS:
            _fastybird_node_advertisement_progress = FASTYBIRD_PUB_STATS;
            break;

        case FASTYBIRD_PUB_STATS:
            node_stats.push_back(FASTYBIRD_STAT_UPTIME);

            if (!_fastybirdPropagateThingStatsStructure(node.id, node_stats)) {
                return;
            }

            // Heartbeat interval
            if (!_fastybirdPropagateThingStat(node.id, FASTYBIRD_STAT_INTERVAL, String(HEARTBEAT_INTERVAL / 1000).c_str())) {
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
            _fastybird_node_advertisement_progress = FASTYBIRD_PUB_READY;
            break;

        case FASTYBIRD_PUB_READY:
            if (!_fastybirdPropagateThingProperty(node.id, FASTYBIRD_PROPERTY_STATE, FASTYBIRD_STATUS_READY)) {
                return;
            }

            _fastybird_node_advertisement_progress = FASTYBIRD_PUB_CONFIGURATION;
            break;

        case FASTYBIRD_PUB_CONFIGURATION:
            _fastybird_node_advertisement_progress = FASTYBIRD_PUB_CHANNELS_CONFIGURATION;
            break;

        case FASTYBIRD_PUB_CHANNELS_CONFIGURATION:
            _fastybird_node_advertisement_progress = FASTYBIRD_PUB_CHANNELS_DIRECT_CONTROL;
            break;

        case FASTYBIRD_PUB_CHANNELS_DIRECT_CONTROL:
            _fastybird_node_advertisement_progress = FASTYBIRD_PUB_CHANNELS_SCHEDULE;
            break;

        case FASTYBIRD_PUB_CHANNELS_SCHEDULE:
            _fastybird_node_advertisement_progress = FASTYBIRD_PUB_HEARTBEAT;
            break;

        case FASTYBIRD_PUB_HEARTBEAT:
            node.initialized = true;
            _fastybirdNodeSendHeartbeat(node);
            break;
    }
}

// -----------------------------------------------------------------------------

void _fastybirdNodeLoop() {
    if (_fastybirdIsApiReady() && fastybirdIsThingInitialzed()) {
        if (_fastybird_initialize_node != NULL) {
            for (unsigned int i = 0; i < _fastybird_gateway_nodes.size(); i++) {
                if (_fastybird_gateway_nodes[i].id == _fastybird_initialize_node) {
                    _fastybirdInitializeNode(_fastybird_gateway_nodes[i]);
                }
            }

        } else if (_fastybird_gateway_nodes_initialize.isEmpty() == false) {
            const char * node_id;

            if (_fastybird_gateway_nodes_initialize.pop(&node_id)) {
                for (unsigned int i = 0; i < _fastybird_gateway_nodes.size(); i++) {
                    if (_fastybird_gateway_nodes[i].id == node_id) {
                        _fastybirdInitializeNode(_fastybird_gateway_nodes[i]);
                    }
                }
            }
        }
    }
}

// -----------------------------------------------------------------------------

void _fastybirdNodeSetup() {
    firmwareRegisterLoop(_fastybirdNodeLoop);
}

// -----------------------------------------------------------------------------
// MODULE API
// -----------------------------------------------------------------------------

void fastybirdRegisterNode(fastybird_node_t node) {
    // New node has to be initialized
    node.initialized = false;

    _fastybird_gateway_nodes.push_back(node);

    // Push node ID into init queue
    _fastybird_gateway_nodes_initialize.push(&node.id);
}

// -----------------------------------------------------------------------------

void fastybirdUnregisterNode(const char * nodeId) {

}

#endif // FASTYBIRD_SUPPORT && FASTYBIRD_GATEWAY_SUPPORT
