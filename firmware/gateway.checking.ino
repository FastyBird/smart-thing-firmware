/*

GATEWAY MODULE - NODES CHECKING

Copyright (C) 2018 FastyBird Ltd. <info@fastybird.com>

*/

#if FB_GATEWAY_SUPPORT

// -----------------------------------------------------------------------------
// MODULE: SUB-MODULE PRIVATE
// -----------------------------------------------------------------------------

bool _gatewayCheckingIsLost(
    const uint8_t nodeIndex
) {
    // Validate node index
    if (nodeIndex >= FB_GATEWAY_MAX_NODES) {
        return false;
    }

    if (
        // Check if node has finished addressing procedure
        gatewayAddressingHasAssignedAddress(nodeIndex) == true
        // Check if node is lost
        && gatewayIsNodeLost(nodeIndex) == true
    ) {
        return true;
    }

    return false;
}

// -----------------------------------------------------------------------------

void _gatewayCheckingPingNode(
    const uint8_t nodeIndex
) {
    // Validate node index
    if (nodeIndex >= FB_GATEWAY_MAX_NODES) {
        return;
    }

    if (
        // Check if node has finished addressing procedure
        gatewayAddressingHasAssignedAddress(nodeIndex)
    ) {
        char output_content[1];

        // Packet identifier at first postion
        output_content[0] = GATEWAY_PACKET_PING;

        // Send packet to node
        bool result = gatewaySendPacket(gatewayAddressingGet(nodeIndex), output_content, 1);

        // When successfully sent...
        if (result == true) {
            // ...add mark, that gateway is waiting for reply from node
            gatewayCommunicationSetWaitingPacket(nodeIndex, GATEWAY_PACKET_PONG);

        } else {
            // ...sending packet failed, reset incomming handler
            gatewayCommunicationSetWaitingPacket(nodeIndex, GATEWAY_PACKET_NONE);
        }
    }
}

// -----------------------------------------------------------------------------

bool _gatewayCheckingIsReady(
    const uint8_t nodeIndex
) {
    // Validate node index
    if (nodeIndex >= FB_GATEWAY_MAX_NODES) {
        return false;
    }

    if (
        // Check if node has finished addressing procedure
        gatewayAddressingHasAssignedAddress(nodeIndex) == true
        // Check if node is not lost
        && gatewayIsNodeLost(nodeIndex) == false
        && gatewayIsNodeReady(nodeIndex) == false
    ) {
        return true;
    }

    return false;
}

// -----------------------------------------------------------------------------

void _gatewayCheckingHelloNode(
    const uint8_t nodeIndex
) {
    // Validate node index
    if (nodeIndex >= FB_GATEWAY_MAX_NODES) {
        return;
    }

    if (
        // Check if node has finished addressing procedure
        gatewayAddressingHasAssignedAddress(nodeIndex)
    ) {
        char output_content[1];

        // Packet identifier at first postion
        output_content[0] = GATEWAY_PACKET_HELLO;

        // Send packet to node
        bool result = gatewaySendPacket(gatewayAddressingGet(nodeIndex), output_content, 1);

        // When successfully sent...
        if (result == true) {
            // ...add mark, that gateway is waiting for reply from node
            gatewayCommunicationSetWaitingPacket(nodeIndex, GATEWAY_PACKET_HELLO);

        } else {
            // ...sending packet failed, reset incomming handler
            gatewayCommunicationSetWaitingPacket(nodeIndex, GATEWAY_PACKET_NONE);
        }

        // Increment communication counter
        gatewayCommunicationIncrementAttempts(nodeIndex);
    }
}

// -----------------------------------------------------------------------------

/**
 * Parse received payload - Node description
 * 
 * 0    => Received packet identifier   => GATEWAY_PACKET_PONG
 */
void _gatewayCheckingPongHandler(
    const uint8_t packetId,
    const uint8_t nodeIndex,
    uint8_t * payload,
    const uint8_t payloadLength
) {
    // Validate node index
    if (nodeIndex >= FB_GATEWAY_MAX_NODES) {
        return;
    }

    // Node could be marked as alive
    gatewayMarkNodeAsAlive(nodeIndex);

    // Node is ready to communicate
    gatewayCommunicationSetWaitingPacket(nodeIndex, GATEWAY_PACKET_NONE);
}

// -----------------------------------------------------------------------------

/**
 * Parse received payload - Node description
 * 
 * 0    => Received packet identifier   => GATEWAY_PACKET_HELLO
 * 1    => Node SN length
 * 2-n  => Node SN
 */
void _gatewayCheckingHelloHandler(
    const uint8_t packetId,
    const uint8_t nodeIndex,
    uint8_t * payload,
    const uint8_t payloadLength
) {
    // Validate node index
    if (nodeIndex >= FB_GATEWAY_MAX_NODES) {
        return;
    }

    uint8_t bytes_length = (uint8_t) payload[1];

    char node_sn[bytes_length];

    // Validate packet structure
    if ((bytes_length + 2) != payloadLength) {
        DEBUG_MSG(PSTR("[ERR][GATEWAY][CHECKING] Packet structure is invalid. Packet lengt is not as expected\n"));

        return;
    }

    // Extract node serial number from payload
    for (uint8_t i = 0; i < bytes_length; i++) {
        node_sn[i] = (char) payload[i + 2];
    }

    char stored_sn[15] = GATEWAY_DESCRIPTION_NOT_SET;

    gatewayGetSerialNumber(nodeIndex, stored_sn);

    if (strcmp(stored_sn, node_sn) != 0) {
        DEBUG_MSG(
            PSTR("[ERR][GATEWAY][CHECKING] Node confirmed node hello request, but with serial number mismatch. Received: %s and assigned: %s\n"),
            node_sn,
            stored_sn
        );
        return;
    }

    // Node could be marked as ready
    gatewayMarkNodeAsReady(nodeIndex);

    // Node is ready to communicate
    gatewayCommunicationSetWaitingPacket(nodeIndex, GATEWAY_PACKET_NONE);
}

// -----------------------------------------------------------------------------
// MODULE: SUB-MODULE API
// -----------------------------------------------------------------------------

bool gatewayCheckingRequestHandler(
    const uint8_t packetId,
    const uint8_t address,
    uint8_t * payload,
    const uint8_t payloadLength
) {
    switch (packetId)
    {
        /**
         * Node responded to PING
         */
        case GATEWAY_PACKET_PONG:
            _gatewayCheckingPongHandler(packetId, gatewayAddressingGetNodeIndex(address), payload, payloadLength);
            break;

        /**
         * Node confirmed hello message
         */
        case GATEWAY_PACKET_HELLO:
            _gatewayCheckingHelloHandler(packetId, gatewayAddressingGetNodeIndex(address), payload, payloadLength);
            break;

        default:
            return false;
    }

    return true;
}

// -----------------------------------------------------------------------------
// MODULE: SUB-MODULE CORE
// -----------------------------------------------------------------------------

bool gatewayCheckingLoop(
    const uint8_t nodeIndex
) {
    bool result = false;

    if (_gatewayCheckingIsLost(nodeIndex)) {
        _gatewayCheckingPingNode(nodeIndex);

        result = true;
    }

    if (_gatewayCheckingIsReady(nodeIndex)) {
        _gatewayCheckingHelloNode(nodeIndex);

        result = true;
    }

    return result;
}

#endif // FB_GATEWAY_SUPPORT
