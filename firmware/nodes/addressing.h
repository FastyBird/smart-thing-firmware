/*

NODES MODULE - NODE ADDRESSING

Copyright (C) 2018 FastyBird s.r.o. <info@fastybird.com>

*/

#if NODES_GATEWAY_SUPPORT

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

    // Node has to have allready assigned bus address
    if (address != PJON_NOT_ASSIGNED) {
        if (address != (uint16_t) senderAddress) {
            DEBUG_GW_MSG(PSTR("[GATEWAY][ERR] Node confirmed node search request, but with addressing mismatch\n"));
            return;
        }

        if (address >= NODES_GATEWAY_MAX_NODES) {
            DEBUG_GW_MSG(PSTR("[GATEWAY][ERR] Node confirmed node search request, but with address out of available range\n"));
            return;
        }

        if (
            strcmp(_gateway_nodes[address - 1].serial_number, node_sn) != 0
            && strcmp(_gateway_nodes[address - 1].serial_number, GATEWAY_DESCRIPTION_NOT_SET) != 0
        ) {
            DEBUG_GW_MSG(PSTR("[GATEWAY][ERR] Node confirmed node search request, but with serial number mismatch\n"));
            return;
        }

        if (
            strcmp(_gateway_nodes[address - 1].serial_number, "") == 0
            || strcmp(_gateway_nodes[address - 1].serial_number, GATEWAY_DESCRIPTION_NOT_SET) == 0
            || strcmp(_gateway_nodes[address - 1].serial_number, node_sn) == 0
        ) {
            // Extract node basic info from received packet
            strncpy(_gateway_nodes[address - 1].serial_number, node_sn, (uint8_t) payload[3]);
            _gateway_nodes[address - 1].packet.max_length = max_packet_length;

            // Node is successfully addressed
            _gateway_nodes[address - 1].addressing.state = true;

            _gateway_nodes[address - 1].initiliazation.state = false;
            _gateway_nodes[address - 1].initiliazation.attempts = 0;
            _gateway_nodes[address - 1].initiliazation.step = GATEWAY_PACKET_HW_MODEL;

            _gateway_nodes[address - 1].packet.waiting_for = GATEWAY_PACKET_NONE;

            DEBUG_GW_MSG(PSTR("[GATEWAY] Addressing for node: %s was successfully restored. Previously assigned address is: %d\n"), (char *) node_sn, (uint8_t) address);

        } else {
            DEBUG_GW_MSG(PSTR("[GATEWAY][ERR] Node address missmatch, node with SN: %s has colision on address %d with node with SN: %s\n"), (char *) node_sn, (uint8_t) address, (char *) _gateway_nodes[address - 1].serial_number);
        }
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
         * Node requested discarding its address
         */
        case GATEWAY_PACKET_ADDRESS_DISCARD:
            _gatewayDisconnectNodeRequestHandler(address, payload);
            break;
    }
}

#endif // NODES_GATEWAY_SUPPORT