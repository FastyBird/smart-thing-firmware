/*

GATEWAY MODULE

Copyright (C) 2018 FastyBird Ltd. <info@fastybird.com>

*/

#if FB_GATEWAY_SUPPORT

#include <SoftwareSerial.h>

PJON<ThroughSerial> _gateway_bus(FB_GATEWAY_MASTER_ID);

SoftwareSerial * _gateway_serial_bus;

uint8_t _gateway_reading_node_index = 0;

gateway_node_t _gateway_nodes[FB_GATEWAY_MAX_NODES];

// -----------------------------------------------------------------------------
// MODULE PRIVATE
// -----------------------------------------------------------------------------

void _gatewayResetNode(
    const uint8_t nodeIndex
) {
    // Validate node index
    if (nodeIndex >= FB_GATEWAY_MAX_NODES) {
        return;
    }

    // Reset ready status
    _gateway_nodes[nodeIndex].ready = false;

    // Reset lost status
    _gateway_nodes[nodeIndex].lost = 0;

    strcpy(_gateway_nodes[nodeIndex].hardware.manufacturer, (char *) GATEWAY_DESCRIPTION_NOT_SET);
    strcpy(_gateway_nodes[nodeIndex].hardware.model, (char *) GATEWAY_DESCRIPTION_NOT_SET);
    strcpy(_gateway_nodes[nodeIndex].hardware.version, (char *) GATEWAY_DESCRIPTION_NOT_SET);

    strcpy(_gateway_nodes[nodeIndex].firmware.manufacturer, (char *) GATEWAY_DESCRIPTION_NOT_SET);
    strcpy(_gateway_nodes[nodeIndex].firmware.model, (char *) GATEWAY_DESCRIPTION_NOT_SET);
    strcpy(_gateway_nodes[nodeIndex].firmware.version, (char *) GATEWAY_DESCRIPTION_NOT_SET);

    // Reset node initialization process
    gatewayInitializationReset(nodeIndex);

    // Reset node addressing process
    gatewayAddressingReset(nodeIndex);

    // Reset communication process
    gatewayCommunicationReset(nodeIndex);

    // Reset registers structure
    gatewayRegistersReset(nodeIndex);

    // Reset registers reading process
    gatewayRegistersResetReading(nodeIndex);
}

// -----------------------------------------------------------------------------
// PACKETS
// -----------------------------------------------------------------------------

bool _gatewayIsPacketInGroup(
    const uint8_t packetId,
    const int * group,
    const uint8_t length
) {
    for (uint8_t i = 0; i < length; i++) {
        if ((uint8_t) pgm_read_byte(&group[i]) == packetId) {
            return true;
        }
    }

    return false;
}

// -----------------------------------------------------------------------------

uint8_t _gatewayGetPacketIndexInGroup(
    const uint8_t packetId,
    const int * group,
    const uint8_t length
) {
    for (uint8_t i = 0; i < length; i++) {
        if ((uint8_t) pgm_read_byte(&group[i]) == packetId) {
            return i;
        }
    }

    return 0;
}

// -----------------------------------------------------------------------------

String _gatewayPacketName(
    const uint8_t packetId
) {
    char buffer[50] = {0};

    if (_gatewayIsPacketInGroup(packetId, gateway_packets_searching, GATEWAY_PACKET_SEARCH_MAX)) {
        strncpy_P(buffer, gateway_packets_searching_string[_gatewayGetPacketIndexInGroup(packetId, gateway_packets_searching, GATEWAY_PACKET_SEARCH_MAX)], sizeof(buffer));

    } else if (_gatewayIsPacketInGroup(packetId, gateway_packets_node_initialization, GATEWAY_PACKET_NODE_INIT_MAX)) {
        strncpy_P(buffer, gateway_packets_node_initialization_string[_gatewayGetPacketIndexInGroup(packetId, gateway_packets_node_initialization, GATEWAY_PACKET_NODE_INIT_MAX)], sizeof(buffer));

    } else if (_gatewayIsPacketInGroup(packetId, gateway_packets_registers_reading, GATEWAY_PACKET_REGISTERS_READING_MAX)) {
        strncpy_P(buffer, gateway_packets_registers_reading_string[_gatewayGetPacketIndexInGroup(packetId, gateway_packets_registers_reading, GATEWAY_PACKET_REGISTERS_READING_MAX)], sizeof(buffer));

    } else if (_gatewayIsPacketInGroup(packetId, gateway_packets_registers_writing, GATEWAY_PACKET_REGISTERS_WRITING_MAX)) {
        strncpy_P(buffer, gateway_packets_registers_writing_string[_gatewayGetPacketIndexInGroup(packetId, gateway_packets_registers_writing, GATEWAY_PACKET_REGISTERS_WRITING_MAX)], sizeof(buffer));

    } else if (_gatewayIsPacketInGroup(packetId, gateway_packets_misc, GATEWAY_PACKET_MISC_MAX)) {
        strncpy_P(buffer, gateway_packets_misc_string[_gatewayGetPacketIndexInGroup(packetId, gateway_packets_misc, GATEWAY_PACKET_MISC_MAX)], sizeof(buffer));

    } else {
        strncpy_P(buffer, "unknown", sizeof(buffer));
    }
    
    return String(buffer);
}

// -----------------------------------------------------------------------------
// COMMUNICATION HANDLERS
// -----------------------------------------------------------------------------

void _gatewayReceiveHandler(
    uint8_t * payload,
    const uint16_t length,
    const PJON_Packet_Info &packetInfo
) {
    uint8_t sender_address = PJON_NOT_ASSIGNED;

    // Get sender address from header
    if (packetInfo.header & PJON_TX_INFO_BIT) {
        sender_address = packetInfo.sender_id;
    }

    // Get packet identifier from payload
    uint8_t packet_id = (uint8_t) payload[0];

    // DEBUG_MSG(PSTR("[INFO][GATEWAY] Received packet: %s for node with address: %d\n"), _gatewayPacketName(packet_id).c_str(), sender_address);

    bool handler_result = false;

    // Addressing process handlers
    handler_result = gatewayAddressingRequestHandler(packet_id, sender_address, payload, length);

    // Check if handler was successful
    if (handler_result) {
        return;
    }

    // Node address have to be valid    
    if (sender_address == PJON_NOT_ASSIGNED) {
        DEBUG_MSG(PSTR("[ERR][GATEWAY] Received packet is without sender address\n"));

        return;
    }

    uint8_t node_index = gatewayAddressingGetNodeIndex(sender_address);

    // Validate node index
    if (node_index >= FB_GATEWAY_MAX_NODES) {
        DEBUG_MSG(PSTR("[ERR][GATEWAY] Received packet is for unknown sender address: %d\n"), sender_address);

        return;
    }

    // Check if gateway is waiting for reply from node (initiliazation sequence)
    if (gatewayCommunicationIsWaitingForPacket(node_index, GATEWAY_PACKET_NONE)) {
        DEBUG_MSG(
            PSTR("[ERR][GATEWAY] Received packet for node with address: %d but gateway is not waiting for packet from this node\n"),
            sender_address
        );

        return;
    }

    // Check if gateway is waiting for reply from node (initiliazation sequence)
    if (!gatewayCommunicationIsWaitingForPacket(node_index, packet_id)) {
        DEBUG_MSG(
            PSTR("[ERR][GATEWAY] Received packet: %s for node with address: %d but gateway is waiting for: %s\n"),
            _gatewayPacketName(packet_id).c_str(),
            sender_address,
            _gatewayPacketName(gatewayCommunicationGetPacketWaitingFor(node_index)).c_str()
        );

        return;
    }
    
    handler_result = gatewayInitializationHandler(packet_id, sender_address, payload, length);

    // Check if handler was successful
    if (handler_result) {
        return;
    }

    handler_result = gatewayRegistersHandler(packet_id, sender_address, payload, length);

    // Check if handler was successful
    if (handler_result) {
        return;
    }

    handler_result = gatewayCheckingRequestHandler(packet_id, sender_address, payload, length);

    // Check if handler was successful
    if (handler_result) {
        return;
    }

    DEBUG_MSG(PSTR("[ERR][GATEWAY] Received packet: %s for node with address: %d without any handler\n"), _gatewayPacketName(packet_id).c_str(), sender_address);
}

// -----------------------------------------------------------------------------

void _gatewayErrorHandler(
    const uint8_t code,
    const uint16_t data,
    void * customPointer
) {
    if (code == PJON_CONNECTION_LOST) {
        DEBUG_MSG(PSTR("[ERR][GATEWAY] Node with address: %d is lost\n"), (_gateway_bus.packets[data].content[0] - 1));

    } else if (code == PJON_PACKETS_BUFFER_FULL) {
        DEBUG_MSG(PSTR("[ERR][GATEWAY] Buffer is full\n"));

    } else if (code == PJON_CONTENT_TOO_LONG) {
        DEBUG_MSG(PSTR("[ERR][GATEWAY] Content is long\n"));

    } else {
        DEBUG_MSG(PSTR("[ERR][GATEWAY] Unknown error\n"));
    }
}

// -----------------------------------------------------------------------------
// COMMUNICATION
// -----------------------------------------------------------------------------

bool gatewaySendPacket(
    const uint8_t address,
    const char * payload,
    const uint8_t length
) {
    uint16_t result = _gateway_bus.send_packet(
        address,    // Node address
        payload,    // Content
        length      // Content length
    );

    if (result != PJON_ACK) {
        if (result == PJON_BUSY ) {
            DEBUG_MSG(PSTR("[ERR][GATEWAY] Sending packet: %s for node: %d failed, bus is busy\n"), _gatewayPacketName(payload[0]).c_str(), address);

        } else if (result == PJON_FAIL) {
            DEBUG_MSG(PSTR("[ERR][GATEWAY] Sending packet: %s for node: %d failed\n"), _gatewayPacketName(payload[0]).c_str(), address);

        } else {
            DEBUG_MSG(PSTR("[ERR][GATEWAY] Sending packet: %s for node: %d failed, unknonw error\n"), _gatewayPacketName(payload[0]).c_str(), address);
        }

        return false;
    }

    if (address == PJON_BROADCAST) {
        // DEBUG_MSG(PSTR("[INFO][GATEWAY] Successfully sent broadcast packet: %s\n"), _gatewayPacketName(payload[0]).c_str());

    } else {
        // DEBUG_MSG(PSTR("[INFO][GATEWAY] Successfully sent packet: %s for node with address: %d\n"), _gatewayPacketName(payload[0]).c_str(), address);
    }

    return true;
}

// -----------------------------------------------------------------------------

void gatewayBroadcastPacket(
    const char * payload,
    const uint8_t length,
    const uint8_t waitingTime
) {
    gatewaySendPacket(
        PJON_BROADCAST,
        payload,
        length
    );

    if (waitingTime > 0) {
        // Store start timestamp
        uint32_t time = millis();
    
        while((millis() - time) <= waitingTime) {
            if (_gateway_bus.receive() == PJON_ACK) {
                return;
            }
        }
    }
}

// -----------------------------------------------------------------------------
// MODULE API
// -----------------------------------------------------------------------------

gateway_node_t gatewayGetNode(
    const uint8_t nodeIndex
) {
    return _gateway_nodes[nodeIndex];
}

// -----------------------------------------------------------------------------

void gatewaySetSerialNumber(
    const uint8_t nodeIndex,
    const char * serialNumber
) {
    // Validate node index
    if (nodeIndex >= FB_GATEWAY_MAX_NODES) {
        return;
    }

    strcpy(_gateway_nodes[nodeIndex].serial_number, serialNumber);
}

// -----------------------------------------------------------------------------

void gatewayGetSerialNumber(
    const uint8_t nodeIndex,
    char * serialNumber
) {
    // Validate node index
    if (nodeIndex >= FB_GATEWAY_MAX_NODES) {
        return;
    }

    strcpy(serialNumber, _gateway_nodes[nodeIndex].serial_number);
}

// -----------------------------------------------------------------------------

void gatewaySetHardwareModel(
    const uint8_t nodeIndex,
    const char * model
) {
    // Validate node index
    if (nodeIndex >= FB_GATEWAY_MAX_NODES) {
        return;
    }

    strcpy(_gateway_nodes[nodeIndex].hardware.model, model);
}

// -----------------------------------------------------------------------------

void gatewayGetHardwareModel(
    const uint8_t nodeIndex,
    char * model
) {
    // Validate node index
    if (nodeIndex >= FB_GATEWAY_MAX_NODES) {
        return;
    }

    strcpy(model, _gateway_nodes[nodeIndex].hardware.model);
}

// -----------------------------------------------------------------------------

void gatewaySetHardwareManufacturer(
    const uint8_t nodeIndex,
    const char * manufacturer
) {
    // Validate node index
    if (nodeIndex >= FB_GATEWAY_MAX_NODES) {
        return;
    }

    strcpy(_gateway_nodes[nodeIndex].hardware.manufacturer, manufacturer);
}

// -----------------------------------------------------------------------------

void gatewayGetHardwareManufacturer(
    const uint8_t nodeIndex,
    char * manufacturer
) {
    // Validate node index
    if (nodeIndex >= FB_GATEWAY_MAX_NODES) {
        return;
    }

    strcpy(manufacturer, _gateway_nodes[nodeIndex].hardware.manufacturer);
}

// -----------------------------------------------------------------------------

void gatewaySetHardwareVersion(
    const uint8_t nodeIndex,
    const char * version
) {
    // Validate node index
    if (nodeIndex >= FB_GATEWAY_MAX_NODES) {
        return;
    }

    strcpy(_gateway_nodes[nodeIndex].hardware.version, version);
}

// -----------------------------------------------------------------------------

void gatewayGetHardwareVersion(
    const uint8_t nodeIndex,
    char * version
) {
    // Validate node index
    if (nodeIndex >= FB_GATEWAY_MAX_NODES) {
        return;
    }

    strcpy(version, _gateway_nodes[nodeIndex].hardware.version);
}

// -----------------------------------------------------------------------------

void gatewaySetFirmwareModel(
    const uint8_t nodeIndex,
    const char * model
) {
    // Validate node index
    if (nodeIndex >= FB_GATEWAY_MAX_NODES) {
        return;
    }

    strcpy(_gateway_nodes[nodeIndex].firmware.model, model);
}

// -----------------------------------------------------------------------------

void gatewayGetFirmwareModel(
    const uint8_t nodeIndex,
    char * model
) {
    // Validate node index
    if (nodeIndex >= FB_GATEWAY_MAX_NODES) {
        return;
    }

    strcpy(model, _gateway_nodes[nodeIndex].firmware.model);
}

// -----------------------------------------------------------------------------

void gatewaySetFirmwareManufacturer(
    const uint8_t nodeIndex,
    const char * manufacturer
) {
    // Validate node index
    if (nodeIndex >= FB_GATEWAY_MAX_NODES) {
        return;
    }

    strcpy(_gateway_nodes[nodeIndex].firmware.manufacturer, manufacturer);
}

// -----------------------------------------------------------------------------

void gatewayGetFirmwareManufacturer(
    const uint8_t nodeIndex,
    char * manufacturer
) {
    // Validate node index
    if (nodeIndex >= FB_GATEWAY_MAX_NODES) {
        return;
    }

    strcpy(manufacturer, _gateway_nodes[nodeIndex].firmware.manufacturer);
}

// -----------------------------------------------------------------------------

void gatewaySetFirmwareVersion(
    const uint8_t nodeIndex,
    const char * version
) {
    // Validate node index
    if (nodeIndex >= FB_GATEWAY_MAX_NODES) {
        return;
    }

    strcpy(_gateway_nodes[nodeIndex].firmware.version, version);
}

// -----------------------------------------------------------------------------

void gatewayGetFirmwareVersion(
    const uint8_t nodeIndex,
    char * version
) {
    // Validate node index
    if (nodeIndex >= FB_GATEWAY_MAX_NODES) {
        return;
    }

    strcpy(version, _gateway_nodes[nodeIndex].firmware.version);
}

// -----------------------------------------------------------------------------

bool gatewayIsNodeLost(
    const uint8_t nodeIndex
) {
    // Validate node index
    if (nodeIndex >= FB_GATEWAY_MAX_NODES) {
        return false;
    }

    return _gateway_nodes[nodeIndex].lost > 0;
}

// -----------------------------------------------------------------------------

void gatewayMarkNodeAsLost(
    const uint8_t nodeIndex
) {
    // Validate node index
    if (nodeIndex >= FB_GATEWAY_MAX_NODES) {
        return;
    }

    bool was_lost = false;

    if (_gateway_nodes[nodeIndex].lost > 0) {
        was_lost = true;
    }

    // Set lost status
    _gateway_nodes[nodeIndex].lost = millis();
    
    if (was_lost) {
        DEBUG_MSG(PSTR("[WARN][GATEWAY] Node with address: %d is still lost\n"), gatewayAddressingGet(nodeIndex));

    } else {
        DEBUG_MSG(PSTR("[WARN][GATEWAY] Node with address: %d is lost\n"), gatewayAddressingGet(nodeIndex));

        // Reset counter
        gatewayCommunicationResetAttempts(nodeIndex);

        // Lost node have to be checked when is back alive
        _gateway_nodes[nodeIndex].ready = false;

        // Lost node is not waiting for any packet
        gatewayCommunicationSetWaitingPacket(nodeIndex, GATEWAY_PACKET_NONE);

        // Notify other modules
        gatewayModulesNodeIsLost(nodeIndex);
    }
}

// -----------------------------------------------------------------------------

void gatewayMarkNodeAsAlive(
    const uint8_t nodeIndex
) {
    // Validate node index
    if (nodeIndex >= FB_GATEWAY_MAX_NODES) {
        return;
    }

    DEBUG_MSG(PSTR("[INFO][GATEWAY] Node with address: %d is back alive\n"), gatewayAddressingGet(nodeIndex));

    // Reset lost status
    _gateway_nodes[nodeIndex].lost = 0;

    gatewayModulesNodeIsAlive(nodeIndex);
}

// -----------------------------------------------------------------------------

bool gatewayIsNodeReady(
    const uint8_t nodeIndex
) {
    // Validate node index
    if (nodeIndex >= FB_GATEWAY_MAX_NODES) {
        return false;
    }

    return _gateway_nodes[nodeIndex].ready == true;
}

// -----------------------------------------------------------------------------

void gatewayMarkNodeAsReady(
    const uint8_t nodeIndex
) {
    // Validate node index
    if (nodeIndex >= FB_GATEWAY_MAX_NODES) {
        return;
    }

    DEBUG_MSG(PSTR("[INFO][GATEWAY] Node with address: %d is ready\n"), gatewayAddressingGet(nodeIndex));

    // Reset lost status
    _gateway_nodes[nodeIndex].ready = true;

    gatewayModulesNodeIsReady(nodeIndex);
}

// -----------------------------------------------------------------------------

void gatewayResetNodeFlags(
    const uint8_t nodeIndex
) {
    // Validate node index
    if (nodeIndex >= FB_GATEWAY_MAX_NODES) {
        return;
    }

    _gateway_nodes[nodeIndex].ready = false;
    _gateway_nodes[nodeIndex].lost = 0;
}

// -----------------------------------------------------------------------------
// MODULE CORE
// -----------------------------------------------------------------------------

void gatewaySetup()
{
    _gateway_serial_bus = new SoftwareSerial(FB_GATEWAY_TX_PIN, FB_GATEWAY_RX_PIN);
    _gateway_serial_bus->begin(SERIAL_BAUDRATE);

    _gateway_bus.strategy.set_serial(_gateway_serial_bus);

     // Communication callbacks
    _gateway_bus.set_receiver(_gatewayReceiveHandler);
    _gateway_bus.set_error(_gatewayErrorHandler);

    _gateway_bus.begin();

    gatewayStorageSetup();
    gatewayModulesSetup();

    firmwareRegisterLoop(gatewayLoop);
}

// -----------------------------------------------------------------------------

void gatewayLoop()
{
    // Get actual timestamp
    uint32_t time = millis();

    // Little delay before gateway start
    if (time < FB_GATEWAY_START_DELAY) {
        return;
    }

    if (_gateway_reading_node_index >= FB_GATEWAY_MAX_NODES) {
        _gateway_reading_node_index = 0;
    }

    for (uint8_t i = _gateway_reading_node_index; i < FB_GATEWAY_MAX_NODES; i++) {
        _gateway_reading_node_index++;

        if (gatewayIsNodeLost(i)) {
            if ((time - _gateway_nodes[i].lost) < FB_GATEWAY_LOST_DELAY) {
                break;
            }

            // Update lost timestamp
            _gateway_nodes[i].lost = time;
        }

        // Attempts counter reached maximum
        if (gatewayCommunicationIsMaxFailuresReached(i)) {
            // Mark node as lost
            gatewayMarkNodeAsLost(i);

            break;
        }
        
        if (gatewayAddressingLoop(i)) {
            break;
        }

        if (gatewayInitializationLoop(i)) {
            break;
        }

        if (gatewayCheckingLoop(i)) {
            break;
        }

        if (gatewayRegistersLoop(i)) {
            break;
        }
    }

    _gateway_bus.update();
    _gateway_bus.receive(50000);
}

#endif // FB_GATEWAY_SUPPORT