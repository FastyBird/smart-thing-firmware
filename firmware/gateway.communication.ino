/*

GATEWAY MODULE - NODES COMMUNICATION

Copyright (C) 2018 FastyBird Ltd. <info@fastybird.com>

*/

#if FB_GATEWAY_SUPPORT

gateway_node_communication_t _gateway_nodes_communication[FB_GATEWAY_MAX_NODES];

// -----------------------------------------------------------------------------
// MODULE: SUB-MODULE API
// -----------------------------------------------------------------------------

void gatewayCommunicationSetWaitingPacket(
    const uint8_t nodeIndex,
    const uint8_t packetId
) {
    // Validate node index
    if (nodeIndex >= FB_GATEWAY_MAX_NODES) {
        return;
    }

    _gateway_nodes_communication[nodeIndex].waiting_for = packetId;
}

// -----------------------------------------------------------------------------

bool gatewayCommunicationIsWaitingForPacket(
    const uint8_t nodeIndex,
    const uint8_t packedId
) {
    // Validate node index
    if (nodeIndex >= FB_GATEWAY_MAX_NODES) {
        return false;
    }

    return _gateway_nodes_communication[nodeIndex].waiting_for == packedId;
}

// -----------------------------------------------------------------------------

uint8_t gatewayCommunicationGetPacketWaitingFor(
    const uint8_t nodeIndex
) {
    // Validate node index
    if (nodeIndex >= FB_GATEWAY_MAX_NODES) {
        return GATEWAY_PACKET_NONE;
    }

    return _gateway_nodes_communication[nodeIndex].waiting_for;
}

// -----------------------------------------------------------------------------

void gatewayCommunicationIncrementAttempts(
    const uint8_t nodeIndex
) {
    // Validate node index
    if (nodeIndex >= FB_GATEWAY_MAX_NODES) {
        return;
    }
    
    _gateway_nodes_communication[nodeIndex].attempts = _gateway_nodes_communication[nodeIndex].attempts + 1;
}

// -----------------------------------------------------------------------------

void gatewayCommunicationResetAttempts(
    const uint8_t nodeIndex
) {
    // Validate node index
    if (nodeIndex >= FB_GATEWAY_MAX_NODES) {
        return;
    }
    
    _gateway_nodes_communication[nodeIndex].attempts = 0;
}

// -----------------------------------------------------------------------------

bool gatewayCommunicationIsMaxFailuresReached(
    const uint8_t nodeIndex
) {
    // Validate node index
    if (nodeIndex >= FB_GATEWAY_MAX_NODES) {
        return true;
    }

    return _gateway_nodes_communication[nodeIndex].attempts >= FB_GATEWAY_MAX_SENDING_FAILURES;
}

// -----------------------------------------------------------------------------

void gatewayCommunicationReset()
{
    for (uint8_t i = 0; i < FB_GATEWAY_MAX_NODES; i++) {
        gatewayCommunicationReset(i);
    }
}

// -----------------------------------------------------------------------------

void gatewayCommunicationReset(
    const uint8_t nodeIndex
) {
    // Validate node index
    if (nodeIndex >= FB_GATEWAY_MAX_NODES) {
        return;
    }

    _gateway_nodes_communication[nodeIndex].waiting_for = GATEWAY_PACKET_NONE;
    _gateway_nodes_communication[nodeIndex].max_packet_size = PJON_PACKET_MAX_LENGTH;
    _gateway_nodes_communication[nodeIndex].attempts = 0;
}

// -----------------------------------------------------------------------------

void gatewayCommunicationSetMaxPacketSize(
    const uint8_t nodeIndex,
    const uint8_t maxPacketSize
) {
    // Validate node index
    if (nodeIndex >= FB_GATEWAY_MAX_NODES) {
        return;
    }

    _gateway_nodes_communication[nodeIndex].max_packet_size = maxPacketSize;
}

// -----------------------------------------------------------------------------

uint8_t gatewayCommunicationGetMaxPacketSize(
    const uint8_t nodeIndex
) {
    // Validate node index
    if (nodeIndex >= FB_GATEWAY_MAX_NODES) {
        return PJON_PACKET_MAX_LENGTH;
    }

    return _gateway_nodes_communication[nodeIndex].max_packet_size;
}

#endif
