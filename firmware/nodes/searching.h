/*

NODES MODULE - NODE SEARCHING

Copyright (C) 2018 FastyBird s.r.o. <info@fastybird.com>

*/

#if NODES_GATEWAY_SUPPORT

// -----------------------------------------------------------------------------
// MODULE PRIVATE
// -----------------------------------------------------------------------------

bool _gatewayIsNodeSerialNumberUnique(
    const char * serialNumber
) {
    for (uint8_t i = 0; i < NODES_GATEWAY_MAX_NODES; i++) {
        if (
            strcmp(_gateway_nodes[i].serial_number, serialNumber) == 0
            && _gateway_nodes[i].searching.state == false
        ) {
            DEBUG_GW_MSG(PSTR("[GATEWAY][WARN] Nodes serial number: %s is not unique\n"), serialNumber);

            return false;
        }
    }

    return true;
}

// -----------------------------------------------------------------------------

uint16_t _gatewayReserveNodeAddress(
    const char * serialNumber
) {
    if (!_gatewayIsNodeSerialNumberUnique(serialNumber)) {
        return NODES_GATEWAY_FAIL;
    }

    for (uint8_t i = 0; i < NODES_GATEWAY_MAX_NODES; i++) {
        // Search for free node slot
        if (strcmp(_gateway_nodes[i].serial_number, serialNumber) == 0) {
            _gateway_nodes[i].searching.state = true;
            _gateway_nodes[i].searching.registration = millis();

            strcpy(_gateway_nodes[i].serial_number, serialNumber);

            return i + 1;
        }
    }

    for (uint8_t i = 0; i < NODES_GATEWAY_MAX_NODES; i++) {
        // Search for free node slot
        if (strcmp(_gateway_nodes[i].serial_number, GATEWAY_DESCRIPTION_NOT_SET) == 0) {
            _gateway_nodes[i].searching.state = true;
            _gateway_nodes[i].searching.registration = millis();

            strcpy(_gateway_nodes[i].serial_number, serialNumber);

            return i + 1;
        }
    }

    DEBUG_GW_MSG(PSTR("[GATEWAY][WARN] Nodes registry is full. No other nodes could be added.\n"));

    return NODES_GATEWAY_NODES_BUFFER_FULL;
}

// -----------------------------------------------------------------------------

void _gatewaySearchNewNodesStart() {
    _gateway_nodes_search_new = true;
    _gateway_last_new_nodes_scan = millis();
}

// -----------------------------------------------------------------------------

/**
 * Gateway searching for new nodes
 */
void _gatewaySearchForNewNodes()
{
    // Store start timestamp
    uint32_t time = millis();

    char output_content[1];

    output_content[0] = GATEWAY_PACKET_SEARCH_NEW_NODES;

    _gatewayBroadcastPacket(output_content, 1);

    while((millis() - time) <= NODES_GATEWAY_LIST_ADDRESSES_TIME) {
        if (_gateway_bus.receive() == PJON_ACK) {
            return;
        }
    }
}

// -----------------------------------------------------------------------------

uint8_t _gatewaySearchUnfinishedAddress() {
    for (uint8_t i = 0; i < NODES_GATEWAY_MAX_NODES; i++) {
        if (
            // Check if node has finished addressing procedure
            _gateway_nodes[i].searching.state == true
            // Check if node is not addressed
            && _gateway_nodes[i].addressing.state == false
            // Check if node is not initialized
            && _gateway_nodes[i].initiliazation.state == false
        ) {
            // Attempts counter reached maximum
            if (_gateway_nodes[i].searching.attempts > NODES_GATEWAY_MAX_SEARCH_ATTEMPTS) {
                _gatewayResetNodeIndex(i);

            } else {
                return (i + 1);
            }
        }
    }

    return 0;
}

// -----------------------------------------------------------------------------

void _gatewaySearchContinueInProcess(
    const uint8_t address
) {
    // Convert node address to index
    uint8_t index = address - 1;

    if (
        // Check if node has finished searching procedure
        _gateway_nodes[index].searching.state == true
        && _gateway_nodes[index].searching.attempts <= NODES_GATEWAY_MAX_SEARCH_ATTEMPTS
        // Check if node is not addressed
        && _gateway_nodes[index].addressing.state == false
        // Check if node is not initialized
        && _gateway_nodes[index].initiliazation.state == false
    ) {
        char output_content[PJON_PACKET_MAX_LENGTH];

        // 0    => Packet identifier
        // 1    => Node reserved bus address
        // 2    => Node SN length
        // 3-n  => Node parsed SN
        output_content[0] = (uint8_t) GATEWAY_PACKET_NODE_ADDRESS_CONFIRM;
        output_content[1] = (uint8_t) address;
        output_content[2] = (uint8_t) (strlen((char *) _gateway_nodes[index].serial_number) + 1);

        uint8_t byte_pointer = 3;

        for (uint8_t i = 0; i < strlen((char *) _gateway_nodes[index].serial_number); i++) {
            output_content[byte_pointer] = ((char *) _gateway_nodes[index].serial_number)[i];

            byte_pointer++;
        }

        output_content[byte_pointer] = 0; // Be sure to set the null terminator!!!

        // Send packet to node
        bool result = _gatewayBroadcastPacket(output_content, (byte_pointer + 1));

        // When successfully sent...
        if (result == true) {
            // ...add mark, that gateway is waiting for reply from node
            _gateway_nodes[index].packet.waiting_for = GATEWAY_PACKET_NODE_ADDRESS_CONFIRM;
            _gateway_nodes[index].packet.sending_time = millis();

            _gateway_nodes[index].searching.attempts = _gateway_nodes[index].searching.attempts + 1;
        }
    }
}

// -----------------------------------------------------------------------------
// SEARCHING HANDLERS
// -----------------------------------------------------------------------------

/**
 * PAYLOAD:
 * 0    => Packet identifier    => GATEWAY_PACKET_SEARCH_NEW_NODES
 * 1    => Max packet size      => 0-255
 * 2    => Node SN length       => 0-255
 * 3-n  => Node parsed SN       => char array (a,b,c,...)
 */
void _gatewaySearchNewNodeRequestHandler(
    const uint8_t senderAddress,
    uint8_t * payload
) {
    uint8_t max_packet_length = (uint8_t) payload[1];

    char node_sn[(uint8_t) payload[2]];

    // Extract node serial number from payload
    for (uint8_t i = 0; i < (uint8_t) payload[2]; i++) {
        node_sn[i] = (char) payload[i + 3];
    }

    // Check node serial number if is unique & get free address slot
    uint16_t address = _gatewayReserveNodeAddress(node_sn);

    // Maximum nodes count reached
    if (address == NODES_GATEWAY_NODES_BUFFER_FULL) {
        return;
    }

    // Node SN is allready used in registry
    if (address == NODES_GATEWAY_FAIL) {
        DEBUG_GW_MSG(PSTR("[GATEWAY][ERR] Node: %s is allready in registry\n"), (char *) node_sn);

        return;
    }

    DEBUG_GW_MSG(PSTR("[GATEWAY] New node: %s was successfully added to registry with address: %d\n"), (char *) node_sn, (uint8_t) address);

    strncpy(_gateway_nodes[address - 1].serial_number, node_sn, (uint8_t) payload[2]);
    _gateway_nodes[address - 1].packet.max_length = max_packet_length;

    _gateway_nodes[address - 1].searching.state = true;
    _gateway_nodes[address - 1].searching.registration = millis();

    _gateway_nodes[address - 1].addressing.state = false;

    _gateway_nodes[address - 1].initiliazation.state = false;
    _gateway_nodes[address - 1].initiliazation.attempts = 0;
    _gateway_nodes[address - 1].initiliazation.step = GATEWAY_PACKET_NONE;

    _gateway_nodes[address - 1].packet.waiting_for == GATEWAY_PACKET_NONE;
    _gateway_nodes[address - 1].packet.sending_time = 0;

    // Store start timestamp
    uint32_t time = millis();

    char output_content[PJON_PACKET_MAX_LENGTH];

    // 0    => Packet identifier
    // 1    => Node reserved bus address
    // 2    => Node SN length
    // 3-n  => Node parsed SN
    output_content[0] = (uint8_t) GATEWAY_PACKET_NODE_ADDRESS_CONFIRM;
    output_content[1] = (uint8_t) address;
    output_content[2] = (uint8_t) (strlen((char *) node_sn) + 1);

    uint8_t byte_pointer = 3;

    for (uint8_t i = 0; i < strlen((char *) node_sn); i++) {
        output_content[byte_pointer] = ((char *) node_sn)[i];

        byte_pointer++;
    }

    output_content[byte_pointer] = 0; // Be sure to set the null terminator!!!

    _gatewaySendPacket(
        PJON_BROADCAST,
        output_content,
        (byte_pointer + 1)
    );

    while((millis() - time) <= NODES_GATEWAY_LIST_ADDRESSES_TIME) {
        if (_gateway_bus.receive() == PJON_ACK) {
            return;
        }
    }
}

// -----------------------------------------------------------------------------

/**
 * PAYLOAD:
 * 0    => Packet identifier    => GATEWAY_PACKET_NODE_ADDRESS_CONFIRM
 * 1    => Node bus address     => 1-250
 * 2    => Node SN length       => 0-255
 * 3-n  => Node parsed SN       => char array (a,b,c,...)
 */
void _gatewaySearchConfirmNodeRequestHandler(
    const uint8_t senderAddress,
    uint8_t * payload
) {
    // Extract address returned by node
    uint8_t address = (uint8_t) payload[1];

    char node_sn[(uint8_t) payload[2]];

    // Extract node serial number from payload
    for (uint8_t i = 0; i < (uint8_t) payload[2]; i++) {
        node_sn[i] = (char) payload[i + 3];
    }

    // Node has allready assigned bus address
    if (address != PJON_NOT_ASSIGNED) {
        if (address != senderAddress) {
            DEBUG_GW_MSG(PSTR("[GATEWAY][ERR] Node confirmed node search request, but with addressing mismatch\n"));
            return;
        }

        if (address >= NODES_GATEWAY_MAX_NODES) {
            DEBUG_GW_MSG(PSTR("[GATEWAY][ERR] Node confirmed node search request, but with address out of available range\n"));
            return;
        }

        if (strcmp(_gateway_nodes[address - 1].serial_number, node_sn) != 0) {
            DEBUG_GW_MSG(PSTR("[GATEWAY][ERR] Node confirmed node search request, but with serial number mismatch\n"));
            return;
        }

        // Node is successfully addressed
        _gateway_nodes[address - 1].addressing.state = true;

        _gateway_nodes[address - 1].searching.state = false;
        _gateway_nodes[address - 1].searching.registration = 0;
        _gateway_nodes[address - 1].searching.attempts = 0;

        _gateway_nodes[address - 1].initiliazation.state = false;
        _gateway_nodes[address - 1].initiliazation.attempts = 0;
        _gateway_nodes[address - 1].initiliazation.step = GATEWAY_PACKET_HW_MODEL;

        _gateway_nodes[address - 1].packet.waiting_for = GATEWAY_PACKET_NONE;

        DEBUG_GW_MSG(PSTR("[GATEWAY] Addressing for new node: %s was successfully finished. Assigned address is: %d\n"), (char *) node_sn, address);

    } else {
        DEBUG_GW_MSG(PSTR("[GATEWAY][ERR] Node confirmed address acceptation but without setting node address\n"));
    }
}

// -----------------------------------------------------------------------------

void _gatewaySearchRequestHandler(
    const uint8_t packetId,
    const uint8_t address,
    uint8_t * payload
) {
    switch (packetId)
    {
        /**
         * Unaddressed node responded to search request
         */
        case GATEWAY_PACKET_SEARCH_NEW_NODES:
            _gatewaySearchNewNodeRequestHandler(address, payload);
            break;

        /**
         * Node confirmed back reserved address
         */
        case GATEWAY_PACKET_NODE_ADDRESS_CONFIRM:
            _gatewaySearchConfirmNodeRequestHandler(address, payload);
            break;
    }
}

#endif