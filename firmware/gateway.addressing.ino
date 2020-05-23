/*

GATEWAY MODULE - NODES ADDRESSING

Copyright (C) 2018 FastyBird Ltd. <info@fastybird.com>

Communication sequence:
-----------------------
 1. Gateway send broatcast packet GATEWAY_PACKET_SEARCH_NODES
 2. Unadressed node catch packet GATEWAY_PACKET_SEARCH_NODES and send reply GATEWAY_PACKET_SEARCH_NODES_CONFIRM with its own SN
 3. Gateway reserve address for given node SN
 4. Gateway send broadcast packet GATEWAY_PACKET_NODE_ADDRESS with reserved address and SN
 5. Unadressed node catch packet, check if SN is same. If yes, provided address is stored in node
 6. Node confirm assigned address with packet GATEWAY_PACKET_NODE_ADDRESS_CONFIRM
 7. Gateway mark node as added and store it to the memory

*/

#if FB_GATEWAY_SUPPORT

gateway_node_addressing_t _gateway_nodes_addressing[FB_GATEWAY_MAX_NODES];

bool _gateway_addressing_search_for_nodes = false;
uint32_t _gateway_addressing_last_node_search = 0;

// -----------------------------------------------------------------------------
// MODULE: SUB-MODULE PRIVATE
// -----------------------------------------------------------------------------

bool _gatewayAddressingIsNodeSerialNumberUnique(
    const char * serialNumber
) {
    char stored_sn[15] = GATEWAY_DESCRIPTION_NOT_SET;

    for (uint8_t i = 0; i < FB_GATEWAY_MAX_NODES; i++) {
        gatewayGetSerialNumber(i, stored_sn);

        if (
            // If in registry is node with same serial number...
            strcmp(stored_sn, serialNumber) == 0
            // ...and addressing is still in process
            && _gateway_nodes_addressing[i].state == true
        ) {
            DEBUG_MSG(PSTR("[WARN][GATEWAY][ADDRESSING] Node serial number: %s is not unique\n"), serialNumber);

            return false;
        }

        strcpy(stored_sn, GATEWAY_DESCRIPTION_NOT_SET);
    }

    return true;
}

// -----------------------------------------------------------------------------

uint16_t _gatewayAddressingReserveNodeAddress(
    const char * serialNumber
) {
    if (!_gatewayAddressingIsNodeSerialNumberUnique(serialNumber)) {
        return FB_GATEWAY_FAIL;
    }

    char stored_sn[15] = GATEWAY_DESCRIPTION_NOT_SET;

    for (uint8_t i = 0; i < FB_GATEWAY_MAX_NODES; i++) {
        gatewayGetSerialNumber(i, stored_sn);

        // Search for free node slot
        if (strcmp(stored_sn, serialNumber) == 0) {
            return i + 1;
        }

        strcpy(stored_sn, GATEWAY_DESCRIPTION_NOT_SET);
    }

    for (uint8_t i = 0; i < FB_GATEWAY_MAX_NODES; i++) {
        gatewayGetSerialNumber(i, stored_sn);

        // Search for free node slot
        if (strcmp(stored_sn, GATEWAY_DESCRIPTION_NOT_SET) == 0) {
            gatewaySetSerialNumber(i, serialNumber);

            return i + 1;
        }

        strcpy(stored_sn, GATEWAY_DESCRIPTION_NOT_SET);
    }

    DEBUG_MSG(PSTR("[WARN][GATEWAY][ADDRESSING] Nodes registry is full. No other nodes could be added.\n"));

    return FB_GATEWAY_NODES_BUFFER_FULL;
}

// -----------------------------------------------------------------------------

/**
 * Gateway searching for new nodes which are not addressed yet
 */
void _gatewayAddressingSearchForNodes()
{
    char output_content[1];

    output_content[0] = GATEWAY_PACKET_SEARCH_NODES;

    gatewayBroadcastPacket(
        output_content,
        1,
        FB_GATEWAY_SEARCH_NODES_BROADCAST_TIME
    );
}

// -----------------------------------------------------------------------------

void _gatewayAddressingContinueInProcess(
    const uint8_t nodeIndex
) {
    // Validate computed node index
    if (nodeIndex >= FB_GATEWAY_MAX_NODES) {
        return;
    }

    // Convert node index to address
    uint8_t address = (nodeIndex - 1);

    char output_content[gatewayCommunicationGetMaxPacketSize(nodeIndex)];

    char stored_sn[15] = GATEWAY_DESCRIPTION_NOT_SET;

    gatewayGetSerialNumber(nodeIndex, stored_sn);

    // 0    => Packet identifier
    // 1    => Node reserved bus address
    // 2    => Node SN length
    // 3-n  => Node parsed SN
    output_content[0] = (uint8_t) GATEWAY_PACKET_NODE_ADDRESS;
    output_content[1] = (uint8_t) address;
    output_content[2] = (uint8_t) (strlen(stored_sn) + 1);

    uint8_t byte_pointer = 3;

    for (uint8_t i = 0; i < strlen(stored_sn); i++) {
        output_content[byte_pointer] = stored_sn[i];

        byte_pointer++;
    }

    output_content[byte_pointer] = 0; // Be sure to set the null terminator!!!

    // Increment communication counter
    gatewayCommunicationIncrementAttempts(nodeIndex);

    // ...add mark, that gateway is waiting for reply from node
    gatewayCommunicationSetWaitingPacket(nodeIndex, GATEWAY_PACKET_NODE_ADDRESS_CONFIRM);

    gatewayBroadcastPacket(
        output_content,
        (byte_pointer + 1),
        FB_GATEWAY_SEARCH_NODES_BROADCAST_TIME
    );
}

// -----------------------------------------------------------------------------

bool _gatewayAddressingIsUnfinished(
    const uint8_t nodeIndex
) {
    // Validate computed node index
    if (nodeIndex >= FB_GATEWAY_MAX_NODES) {
        return false;
    }

    if (
        // Check if node is in searching mode
        _gateway_nodes_addressing[nodeIndex].registration > 0
    ) {
        return true;
    }

    return false;
}

// -----------------------------------------------------------------------------

void _gatewayAddressingCheckPacketsDelays()
{
    for (uint8_t i = 0; i < FB_GATEWAY_MAX_NODES; i++) {
        // Searching
        if (
            _gateway_nodes_addressing[i].state == false
            && _gateway_nodes_addressing[i].registration > 0
            && (millis() - _gateway_nodes_addressing[i].registration) > FB_GATEWAY_SEARCHING_WAITING_TIMEOUT
        ) {
            // Node does not respond in reserved time window
            // Node slot is free to use by other node
            gatewayAddressingReset(i);
        }
    }
}

// -----------------------------------------------------------------------------

/**
 * PAYLOAD:
 * 0    => Packet identifier    => GATEWAY_PACKET_SEARCH_NODES_CONFIRM
 * 1    => Max packet size      => 0-255
 * 2    => Node SN length       => 0-255
 * 3-n  => Node parsed SN       => char array (a,b,c,...)
 */
void _gatewayAddressingSearchNodesHandler(
    const uint8_t senderAddress,
    uint8_t * payload,
    const uint8_t payloadLength
) {
    // Get node supported max packet size
    uint8_t max_packet_length = (uint8_t) payload[1];

    uint8_t bytes_length = (uint8_t) payload[2];

    char node_sn[bytes_length];

    // Validate packet structure
    if ((bytes_length + 3) != payloadLength) {
        DEBUG_MSG(PSTR("[ERR][GATEWAY][ADDRESSING] Packet structure is invalid. Packet lengt is not as expected\n"));

        return;
    }

    // Extract node serial number from payload
    for (uint8_t i = 0; i < bytes_length; i++) {
        node_sn[i] = (char) payload[i + 3];
    }

    // Check node serial number if is unique & get free address slot
    uint16_t address = _gatewayAddressingReserveNodeAddress(node_sn);

    // Maximum nodes count reached
    if (address == FB_GATEWAY_NODES_BUFFER_FULL) {
        return;
    }

    // Node SN is allready used in registry
    if (address == FB_GATEWAY_FAIL) {
        DEBUG_MSG(PSTR("[ERR][GATEWAY][ADDRESSING] Node: %s is allready in registry\n"), (char *) node_sn);

        return;
    }

    // Transform address to node memory index
    uint8_t node_index = (address - 1);

    // Validate computed node index
    if (node_index >= FB_GATEWAY_MAX_NODES) {
        return;
    }

    DEBUG_MSG(PSTR("[INFO][GATEWAY][ADDRESSING] New node: %s was successfully added to registry with address: %d\n"), (char *) node_sn, (uint8_t) address);

    // Addressing process info
    _gateway_nodes_addressing[node_index].address = address;
    _gateway_nodes_addressing[node_index].registration = millis();

    // Communication info
    gatewayCommunicationSetMaxPacketSize(node_index, max_packet_length);
    gatewayCommunicationResetAttempts(node_index);

    // Send address info to node
    _gatewayAddressingContinueInProcess(node_index);
}

// -----------------------------------------------------------------------------

/**
 * PAYLOAD:
 * 0    => Packet identifier    => GATEWAY_PACKET_NODE_ADDRESS_CONFIRM
 * 1    => Node bus address     => 1-250
 * 2    => Node SN length       => 0-255
 * 3-n  => Node parsed SN       => char array (a,b,c,...)
 */
void _gatewayAddressingConfirmNodeHandler(
    const uint8_t senderAddress,
    uint8_t * payload,
    const uint8_t payloadLength
) {
    // Extract address returned by node
    uint8_t address = (uint8_t) payload[1];

    // Transform node address to node index
    uint8_t node_index = (address - 1);

    // Validate computed node index
    if (node_index >= FB_GATEWAY_MAX_NODES) {
        return;
    }

    uint8_t bytes_length = (uint8_t) payload[2];

    // Validate packet structure
    if ((bytes_length + 3) != payloadLength) {
        DEBUG_MSG(PSTR("[ERR][GATEWAY][ADDRESSING] Packet structure is invalid. Packet lengt is not as expected\n"));

        return;
    }

    // Node has allready assigned bus address
    if (address != PJON_NOT_ASSIGNED) {
        char node_sn[bytes_length];

        // Extract node serial number from payload
        for (uint8_t i = 0; i < bytes_length; i++) {
            node_sn[i] = (char) payload[i + 3];
        }
        
        if (address != senderAddress) {
            DEBUG_MSG(PSTR("[ERR][GATEWAY][ADDRESSING] Node confirmed node search request, but with addressing mismatch\n"));
            return;
        }

        char stored_sn[15] = GATEWAY_DESCRIPTION_NOT_SET;

        gatewayGetSerialNumber(node_index, stored_sn);

        if (strcmp(stored_sn, node_sn) != 0) {
            DEBUG_MSG(PSTR("[ERR][GATEWAY][ADDRESSING] Node confirmed node search request, but with serial number mismatch\n"));
            return;
        }

        // Update addressing process info
        gatewayAddressingSet(node_index, address);

        // Gateway is not waiting any message from node
        gatewayCommunicationSetWaitingPacket(node_index, GATEWAY_PACKET_NONE);

        // Store new node info into storage memory
        gatewayStorageAddNode(node_index);

        // Reset communication counter
        gatewayCommunicationResetAttempts(node_index);

        DEBUG_MSG(PSTR("[INFO][GATEWAY][ADDRESSING] Addressing for new node: %s was successfully finished. Assigned address is: %d\n"), (char *) node_sn, address);

    } else {
        DEBUG_MSG(PSTR("[ERR][GATEWAY][ADDRESSING] Node confirmed address acceptation but without setting node address\n"));
    }
}

// -----------------------------------------------------------------------------

void _gatewayAddressingDisconnectNodeHandler(
    const uint8_t senderAddress,
    uint8_t * payload,
    const uint8_t payloadLength
) {
    // TODO: Implement address discarding
}

// -----------------------------------------------------------------------------
// MODULE: SUB-MODULE API
// -----------------------------------------------------------------------------

void gatewayAddressingSet(
    const uint8_t nodeIndex,
    const uint8_t address
) {
    // Validate node index
    if (nodeIndex >= FB_GATEWAY_MAX_NODES) {
        return;
    }

    // Update addressing process info
    _gateway_nodes_addressing[nodeIndex].state = true;
    _gateway_nodes_addressing[nodeIndex].address = address;
    _gateway_nodes_addressing[nodeIndex].registration = 0;
}

// -----------------------------------------------------------------------------

void gatewayAddressingReset()
{
    for (uint8_t i = 0; i < FB_GATEWAY_MAX_NODES; i++) {
        gatewayAddressingReset(i);
    }
}

// -----------------------------------------------------------------------------

void gatewayAddressingReset(
    const uint8_t nodeIndex
) {
    // Validate node index
    if (nodeIndex >= FB_GATEWAY_MAX_NODES) {
        return;
    }

    _gateway_nodes_addressing[nodeIndex].state = false;
    _gateway_nodes_addressing[nodeIndex].address = PJON_NOT_ASSIGNED;
    _gateway_nodes_addressing[nodeIndex].registration = 0;
}

// -----------------------------------------------------------------------------

uint8_t gatewayAddressingGet(
    const uint8_t nodeIndex
) {
    // Validate node index
    if (nodeIndex >= FB_GATEWAY_MAX_NODES) {
        return INDEX_NONE;
    }

    return _gateway_nodes_addressing[nodeIndex].address;
}

// -----------------------------------------------------------------------------

bool gatewayAddressingHasAssignedAddress(
    const uint8_t nodeIndex
) {
    // Validate node index
    if (nodeIndex >= FB_GATEWAY_MAX_NODES) {
        return false;
    }

    return _gateway_nodes_addressing[nodeIndex].address != PJON_NOT_ASSIGNED;
}

// -----------------------------------------------------------------------------

uint8_t gatewayAddressingGetNodeIndex(
    const uint8_t address
) {
    for (uint8_t i = 0; i < FB_GATEWAY_MAX_NODES; i++) {
        if (_gateway_nodes_addressing[i].address == address) {
            return i;
        }
    }

    return INDEX_NONE;
}

// -----------------------------------------------------------------------------

/**
 * Set flags for searching nodes
 */
void gatewayAddressingStartSearching()
{
    _gateway_addressing_search_for_nodes = true;
    _gateway_addressing_last_node_search = millis();
}

// -----------------------------------------------------------------------------

bool gatewayAddressingRequestHandler(
    const uint8_t packetId,
    const uint8_t address,
    uint8_t * payload,
    const uint8_t payloadLength
) {
    switch (packetId)
    {
        /**
         * Unaddressed node responded to search request
         */
        case GATEWAY_PACKET_SEARCH_NODES_CONFIRM:
            _gatewayAddressingSearchNodesHandler(address, payload, payloadLength);
            break;

        /**
         * Node confirmed back reserved address
         */
        case GATEWAY_PACKET_NODE_ADDRESS_CONFIRM:
            _gatewayAddressingConfirmNodeHandler(address, payload, payloadLength);
            break;

        /**
         * Node requested discarding its address
         */
        case GATEWAY_PACKET_ADDRESS_DISCARD:
            _gatewayAddressingDisconnectNodeHandler(address, payload, payloadLength);
            break;

        default:
            return false;
    }

    return true;
}

// -----------------------------------------------------------------------------
// MODULE: SUB-MODULE CORE
// -----------------------------------------------------------------------------

bool gatewayAddressingLoop(
    const uint8_t nodeIndex
) {
    _gatewayAddressingCheckPacketsDelays();

    if (
        _gateway_addressing_search_for_nodes == true
        && (_gateway_addressing_last_node_search + FB_GATEWAY_SEARCHING_TIMEOUT) <= millis()
    ) {
        _gateway_addressing_search_for_nodes = false;
        _gateway_addressing_last_node_search = 0;
    }

    // Search for new unaddressed nodes
    if (
        _gateway_addressing_search_for_nodes == true
        && (_gateway_addressing_last_node_search + FB_GATEWAY_SEARCHING_TIMEOUT) > millis()
    ) {
        _gatewayAddressingSearchForNodes();
    }

    // Check if all connected nodes have finished searching process
    if (_gatewayAddressingIsUnfinished(nodeIndex)) {
        _gatewayAddressingContinueInProcess(nodeIndex);

        return true;
    }

    return false;
}

#endif // FB_GATEWAY_SUPPORT
