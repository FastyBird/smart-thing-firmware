/*

NODES MODULE - NODE ADDRESSING

Copyright (C) 2018 FastyBird Ltd. <info@fastybird.com>

*/

#if NODES_GATEWAY_SUPPORT

// -----------------------------------------------------------------------------
// NODE ADDRESSING UTILS
// -----------------------------------------------------------------------------

bool _gatewayIsNodeSerialNumberUnique(
    const char * nodeSerialNumber
) {
    for (uint8_t i = 0; i < NODES_GATEWAY_MAX_NODES; i++) {
        if (strcmp(_gateway_nodes[i].serial_number, nodeSerialNumber) == 0) {
            DEBUG_MSG(PSTR("[GATEWAY][WARN] Nodes serial number: %s is not unique\n"), nodeSerialNumber);

            return false;
        }
    }

    return true;
}

// -----------------------------------------------------------------------------

uint16_t _gatewayReserveNodeAddress(
    const char * nodeSerialNumber
) {
    if (!_gatewayIsNodeSerialNumberUnique(nodeSerialNumber)) {
        return NODES_GATEWAY_FAIL;
    }

    for (uint8_t i = 0; i < NODES_GATEWAY_MAX_NODES; i++) {
        // Search for free node slot
        if (
            strcmp(_gateway_nodes[i].serial_number, nodeSerialNumber) == 0
            || strcmp(_gateway_nodes[i].serial_number, GATEWAY_DESCRIPTION_NOT_SET) == 0
        ) {
            _gateway_nodes[i].addressing.state = false;
            _gateway_nodes[i].addressing.registration = millis();

            strcpy(_gateway_nodes[i].serial_number, nodeSerialNumber);

            return i + 1;
        }
    }

    DEBUG_MSG(PSTR("[GATEWAY][WARN] Nodes registry is full. No other nodes could be added.\n"));

    return NODES_GATEWAY_NODES_BUFFER_FULL;
}

// -----------------------------------------------------------------------------
// ADDRESING HANDLERS
// -----------------------------------------------------------------------------

/**
 * PAYLOAD:
 * 0    => Packet identifier    => GATEWAY_PACKET_SEARCH_NODES
 * 1    => Node bus address     => 1-250
 * 2    => Max packet size      => 0-255
 * 3    => Node SN length       => 0-255
 * 4-n  => Node parsed SN       => char array (a,b,c,...)
 */
void _gatewaySearchNodeRequestHandler(
    const uint8_t senderAddress,
    uint8_t * payload
) {
    // Extract address returned by node
    uint16_t address = (uint16_t) payload[1];

    uint8_t max_packet_length = (uint8_t) payload[2];

    char node_sn[(uint8_t) payload[3]];

    // Extract node serial number from payload
    for (uint8_t i = 0; i < (uint8_t) payload[3]; i++) {
        node_sn[i] = (char) payload[i + 4];
    }

    // Node has allready assigned bus address
    if (address != PJON_NOT_ASSIGNED) {
        if (address != (uint16_t) senderAddress) {
            DEBUG_MSG(PSTR("[GATEWAY][ERR] Node confirmed node search request, but with addressing mismatch\n"));
            return;
        }

        if (address >= NODES_GATEWAY_MAX_NODES) {
            DEBUG_MSG(PSTR("[GATEWAY][ERR] Node confirmed node search request, but with address out of available range\n"));
            return;
        }

        if (
            strcmp(_gateway_nodes[address - 1].serial_number, node_sn) != 0
            && strcmp(_gateway_nodes[address - 1].serial_number, GATEWAY_DESCRIPTION_NOT_SET) != 0
        ) {
            DEBUG_MSG(PSTR("[GATEWAY][ERR] Node confirmed node search request, but with serial number mismatch\n"));
            return;
        }

        DEBUG_MSG(PSTR("[GATEWAY] Addressing for node: %s was successfully restored. Previously assigned address is: %d\n"), (char *) node_sn, (uint8_t) address);

    // Node is new without bus address
    } else {
        // Check node serial number if is unique & get free address slot
        address = _gatewayReserveNodeAddress(node_sn);

        // Maximum nodes count reached
        if (address == NODES_GATEWAY_NODES_BUFFER_FULL) {
            return;
        }

        // Node SN is allready used in registry
        if (address == NODES_GATEWAY_FAIL) {
            DEBUG_MSG(PSTR("[GATEWAY][ERR] Node: %s is allready in registry\n"), (char *) node_sn);

            return;
        }

        DEBUG_MSG(PSTR("[GATEWAY] New node: %s was successfully added to registry with address: %d\n"), (char *) node_sn, (uint8_t) address);
    }

    strncpy(_gateway_nodes[address - 1].serial_number, node_sn, (uint8_t) payload[3]);
    _gateway_nodes[address - 1].packet.max_length = max_packet_length;

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
void _gatewayConfirmNodeRequestHandler(
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
            DEBUG_MSG(PSTR("[GATEWAY][ERR] Node confirmed node search request, but with addressing mismatch\n"));
            return;
        }

        if (address >= NODES_GATEWAY_MAX_NODES) {
            DEBUG_MSG(PSTR("[GATEWAY][ERR] Node confirmed node search request, but with address out of available range\n"));
            return;
        }

        if (strcmp(_gateway_nodes[address - 1].serial_number, node_sn) != 0) {
            DEBUG_MSG(PSTR("[GATEWAY][ERR] Node confirmed node search request, but with serial number mismatch\n"));
            return;
        }

        // Node addressing finished
        _gateway_nodes[address - 1].addressing.state = true;
        _gateway_nodes[address - 1].addressing.registration = 0;

        // Prepare for initialization
        _gateway_nodes[address - 1].initiliazation.step = GATEWAY_PACKET_HW_MODEL;

        DEBUG_MSG(PSTR("[GATEWAY] Addressing for node: %s was successfully finished. Assigned address is: %d\n"), (char *) node_sn, address);

    } else {
        DEBUG_MSG(PSTR("[GATEWAY][ERR] Node confirmed address acceptation but without setting node address\n"));
    }
}

// -----------------------------------------------------------------------------

void _gatewayDisconnectNodeRequestHandler(
    const uint8_t senderAddress,
    uint8_t * payload
) {
    // TODO: Implement address discarding
}

// -----------------------------------------------------------------------------

void _gatewayAddressRequestHandler(
    const uint8_t packetId,
    const uint8_t address,
    uint8_t * payload
) {
    switch (packetId)
    {
        /**
         * Node has replied to search request
         */
        case GATEWAY_PACKET_SEARCH_NODES:
            _gatewaySearchNodeRequestHandler(address, payload);
            break;

        /**
         * Node confirmed back reserved address
         */
        case GATEWAY_PACKET_NODE_ADDRESS_CONFIRM:
            _gatewayConfirmNodeRequestHandler(address, payload);
            break;

        /**
         * Node requested discarding its address
         */
        case GATEWAY_PACKET_ADDRESS_DISCARD:
            _gatewayDisconnectNodeRequestHandler(address, payload);
            break;
    }
}

#endif // NODES_GATEWAY_SUPPORT