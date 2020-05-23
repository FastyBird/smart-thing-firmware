/*

GATEWAY MODULE - NODES INITIALIZATION

Copyright (C) 2018 FastyBird Ltd. <info@fastybird.com>

*/

#if FB_GATEWAY_SUPPORT

gateway_node_initiliazation_t _gateway_nodes_initialization[FB_GATEWAY_MAX_NODES];

// -----------------------------------------------------------------------------
// MODULE: SUB-MODULE PRIVATE
// -----------------------------------------------------------------------------

void _gatewayInitializationSendBaseInitializationPacket(
    const uint8_t nodeIndex,
    const uint8_t requestedPacket
) {
    // Validate node index
    if (nodeIndex >= FB_GATEWAY_MAX_NODES) {
        return;
    }

    char output_content[1];

    // Packet identifier at first postion
    output_content[0] = requestedPacket;

    // Send packet to node
    bool result = gatewaySendPacket(gatewayAddressingGet(nodeIndex), output_content, 1);

    // When successfully sent...
    if (result == true) {
        // ...add mark, that gateway is waiting for reply from node
        gatewayCommunicationSetWaitingPacket(nodeIndex, requestedPacket);

    } else {
        // ...sending packet failed, reset incomming handler
        gatewayCommunicationSetWaitingPacket(nodeIndex, GATEWAY_PACKET_NONE);
    }

    // Increment communication counter
    gatewayCommunicationIncrementAttempts(nodeIndex);
}

// -----------------------------------------------------------------------------

void _gatewayInitializationSendRegistersInitializationPacket(
    const uint8_t nodeIndex,
    const uint8_t requestedPacket,
    const uint8_t start
) {
    // Validate node index
    if (nodeIndex >= FB_GATEWAY_MAX_NODES) {
        return;
    }

    char output_content[gatewayCommunicationGetMaxPacketSize(nodeIndex)];

    // Packet identifier at first postion
    output_content[0] = requestedPacket;
    output_content[1] = (char) (start >> 8);
    output_content[2] = (char) (start & 0xFF);
    
    // It is based on maximum packed size reduced by packet header (4 bytes)
    uint8_t max_readable_addresses = start + gatewayCommunicationGetMaxPacketSize(nodeIndex) - 4;

    switch (requestedPacket)
    {
        case GATEWAY_PACKET_AI_REGISTERS_STRUCTURE:
            if (max_readable_addresses <= gatewayRegistersAnalogInputsSize(nodeIndex)) {
                output_content[3] = (char) (max_readable_addresses >> 8);
                output_content[4] = (char) (max_readable_addresses & 0xFF);

            } else {
                output_content[3] = (char) (gatewayRegistersAnalogInputsSize(nodeIndex) >> 8);
                output_content[4] = (char) (gatewayRegistersAnalogInputsSize(nodeIndex) & 0xFF);
            }
            break;

        case GATEWAY_PACKET_AO_REGISTERS_STRUCTURE:
            if (max_readable_addresses <= gatewayRegistersAnalogOutputsSize(nodeIndex)) {
                output_content[3] = (char) (max_readable_addresses >> 8);
                output_content[4] = (char) (max_readable_addresses & 0xFF);

            } else {
                output_content[3] = (char) (gatewayRegistersAnalogOutputsSize(nodeIndex) >> 8);
                output_content[4] = (char) (gatewayRegistersAnalogOutputsSize(nodeIndex) & 0xFF);
            }
            break;
    }

    // Send packet to node
    bool result = gatewaySendPacket(gatewayAddressingGet(nodeIndex), output_content, 5);

    // When successfully sent...
    if (result == true) {
        // ...add mark, that gateway is waiting for reply from node
        gatewayCommunicationSetWaitingPacket(nodeIndex, requestedPacket);

    } else {
        // ...sending packet failed, reset incomming handler
        gatewayCommunicationSetWaitingPacket(nodeIndex, GATEWAY_PACKET_NONE);
    }

    // Increment communication counter
    gatewayCommunicationIncrementAttempts(nodeIndex);
}

// -----------------------------------------------------------------------------

void _gatewayInitializationMarkNodeAsInitialized(
    const uint8_t nodeIndex
) {
    _gateway_nodes_initialization[nodeIndex].state = true;
    _gateway_nodes_initialization[nodeIndex].step = GATEWAY_PACKET_NONE;

    // Initialized node is automatically ready for communication
    gatewayMarkNodeAsReady(nodeIndex);

    // Reset communication counter
    gatewayCommunicationResetAttempts(nodeIndex);

    // Notify other modules
    gatewayModulesNodeInitialized(nodeIndex);
}

// -----------------------------------------------------------------------------

void _gatewayInitializationContinueInProcess(
    const uint8_t nodeIndex
) {
    switch (_gateway_nodes_initialization[nodeIndex].step)
    {
        // Hardware info
        case GATEWAY_PACKET_HW_MODEL:
        case GATEWAY_PACKET_HW_MANUFACTURER:
        case GATEWAY_PACKET_HW_VERSION:
        // Firmware info
        case GATEWAY_PACKET_FW_MODEL:
        case GATEWAY_PACKET_FW_MANUFACTURER:
        case GATEWAY_PACKET_FW_VERSION:
        // Registers info
        case GATEWAY_PACKET_REGISTERS_SIZE:
            _gatewayInitializationSendBaseInitializationPacket(nodeIndex, _gateway_nodes_initialization[nodeIndex].step);
            break;
        
        // Analog registers
        case GATEWAY_PACKET_AI_REGISTERS_STRUCTURE:
            _gatewayInitializationSendRegistersInitializationPacket(nodeIndex, GATEWAY_PACKET_AI_REGISTERS_STRUCTURE, 0);
            break;
        
        case GATEWAY_PACKET_AO_REGISTERS_STRUCTURE:
            _gatewayInitializationSendRegistersInitializationPacket(nodeIndex, GATEWAY_PACKET_AO_REGISTERS_STRUCTURE, 0);
            break;

        // Start initialization
        case GATEWAY_PACKET_NONE:
            _gatewayInitializationSendBaseInitializationPacket(nodeIndex, GATEWAY_PACKET_HW_MODEL);
            break;

        default:
            break;
    }
}

// -----------------------------------------------------------------------------

bool _gatewayInitializationIsUnfinished(
    const uint8_t nodeIndex
) {
    if (
        // Check if node has finished addressing procedure
        gatewayAddressingHasAssignedAddress(nodeIndex) == true
        // Check if node is not lost
        && gatewayIsNodeLost(nodeIndex) == false
        // Check if node is not initialized
        && _gateway_nodes_initialization[nodeIndex].state == false
    ) {
        return true;
    }

    return false;
}

// -----------------------------------------------------------------------------

/**
 * Parse received payload - Node description
 * 
 * 0    => Received packet identifier   => GATEWAY_PACKET_HW_MODEL|GATEWAY_PACKET_HW_MANUFACTURER|GATEWAY_PACKET_HW_VERSION|GATEWAY_PACKET_FW_MODEL|GATEWAY_PACKET_FW_MANUFACTURER|GATEWAY_PACKET_FW_VERSION
 * 1    => Description string length    => 1-255
 * 2-n  => Description content          => char array (a,b,c,...)
 */
void _gatewayInitializationBaseDefinitionHandler(
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

    char content[bytes_length];

    // Validate packet structure
    if ((bytes_length + 2) != payloadLength) {
        DEBUG_MSG(PSTR("[ERR][GATEWAY][INITIALIZATION] Packet structure is invalid. Packet lengt is not as expected\n"));

        return;
    }

    // Extract text content from payload
    for (uint8_t i = 0; i < bytes_length; i++) {
        content[i] = (char) payload[i + 2];
    }

    DEBUG_MSG(
        PSTR("[INFO][GATEWAY][INITIALIZATION] Received node description: %s for node with address: %d\n"),
        (char *) content,
        gatewayAddressingGet(nodeIndex)
    );

    switch (packetId)
    {

    /**
     * HARDWARE
     */

        case GATEWAY_PACKET_HW_MODEL:
            gatewaySetHardwareModel(nodeIndex, content);

            _gateway_nodes_initialization[nodeIndex].step = GATEWAY_PACKET_HW_MANUFACTURER;
            break;

        case GATEWAY_PACKET_HW_MANUFACTURER:
            gatewaySetHardwareManufacturer(nodeIndex, content);

            _gateway_nodes_initialization[nodeIndex].step = GATEWAY_PACKET_HW_VERSION;
            break;

        case GATEWAY_PACKET_HW_VERSION:
            gatewaySetHardwareVersion(nodeIndex, content);

            _gateway_nodes_initialization[nodeIndex].step = GATEWAY_PACKET_FW_MODEL;
            break;

    /**
     * FIRMWARE
     */

        case GATEWAY_PACKET_FW_MODEL:
            gatewaySetFirmwareModel(nodeIndex, content);

            _gateway_nodes_initialization[nodeIndex].step = GATEWAY_PACKET_FW_MANUFACTURER;
            break;

        case GATEWAY_PACKET_FW_MANUFACTURER:
            gatewaySetFirmwareManufacturer(nodeIndex, content);

            _gateway_nodes_initialization[nodeIndex].step = GATEWAY_PACKET_FW_VERSION;
            break;

        case GATEWAY_PACKET_FW_VERSION:
            gatewaySetFirmwareVersion(nodeIndex, content);

            _gateway_nodes_initialization[nodeIndex].step = GATEWAY_PACKET_REGISTERS_SIZE;
            break;

    /**
     * Unknown init sequence
     */

        default:
            return;
    }

    // Reset incomming handler
    gatewayCommunicationSetWaitingPacket(nodeIndex, GATEWAY_PACKET_NONE);
    
    // Reset reading attempts counter
    gatewayCommunicationResetAttempts(nodeIndex);
}

// -----------------------------------------------------------------------------

/**
 * Parse received payload - Registers definitions
 * 
 * PAYLOAD
 * 0    => Received packet identifier   => GATEWAY_PACKET_REGISTERS_SIZE
 * 1    => DI buffer size               => 0-255
 * 2    => DO buffer size               => 0-255
 * 3    => AI buffer size               => 0-255
 * 4    => AO buffer size               => 0-255
 * 5    => EV buffer size               => 0-255
 */
void _gatewayInitializationRegistersDefinitionsHandler(
    const uint8_t nodeIndex,
    uint8_t * payload,
    const uint8_t payloadLength
) {
    // Validate node index
    if (nodeIndex >= FB_GATEWAY_MAX_NODES) {
        return;
    }

    // Validate packet structure
    if (payloadLength != 6) {
        DEBUG_MSG(PSTR("[ERR][GATEWAY][INITIALIZATION] Packet structure is invalid. Packet lengt is not as expected\n"));

        return;
    }

    gatewayRegistersResetReading(nodeIndex);

    gatewayRegistersInitializeDigitalInputs(nodeIndex, (uint8_t) payload[1]);
    gatewayRegistersInitializeDigitalOutputs(nodeIndex, (uint8_t) payload[2]);
    gatewayRegistersInitializeAnalogInputs(nodeIndex, (uint8_t) payload[3]);
    gatewayRegistersInitializeAnalogOutputs(nodeIndex, (uint8_t) payload[4]);
    gatewayRegistersInitializeEventInputs(nodeIndex, (uint8_t) payload[5]);
 
    DEBUG_MSG(
        PSTR("[INFO][GATEWAY][INITIALIZATION] Received node registers structure (DI: %d, DO: %d, AI: %d, AO: %d, EV: %d) for node with address: %d\n"),
        gatewayRegistersDigitalInputsSize(nodeIndex),
        gatewayRegistersDigitalOutputsSize(nodeIndex),
        gatewayRegistersAnalogInputsSize(nodeIndex),
        gatewayRegistersAnalogOutputsSize(nodeIndex),
        gatewayRegistersEventInputsSize(nodeIndex),
        gatewayAddressingGet(nodeIndex)
    );

    if (gatewayRegistersAnalogInputsSize(nodeIndex) > 0) {
        _gateway_nodes_initialization[nodeIndex].step = GATEWAY_PACKET_AI_REGISTERS_STRUCTURE;
        _gateway_nodes_initialization[nodeIndex].register_position = 0;

    } else if (gatewayRegistersAnalogOutputsSize(nodeIndex) > 0) {
        _gateway_nodes_initialization[nodeIndex].step = GATEWAY_PACKET_AO_REGISTERS_STRUCTURE;
        _gateway_nodes_initialization[nodeIndex].register_position = 0;

    } else {
        // Node initiliazation successfully finished
        _gatewayInitializationMarkNodeAsInitialized(nodeIndex);
    }

    // Reset incomming handler
    gatewayCommunicationSetWaitingPacket(nodeIndex, GATEWAY_PACKET_NONE);
    
    // Reset reading attempts counter
    gatewayCommunicationResetAttempts(nodeIndex);
}

// -----------------------------------------------------------------------------

/**
 * Parse received payload - Register structure
 * 
 * PAYLOAD
 * 0    => Received packet identifier       => GATEWAY_PACKET_AI_REGISTERS_STRUCTURE|GATEWAY_PACKET_AO_REGISTERS_STRUCTURE
 * 1    => High byte of register address    => 0-255
 * 2    => Low byte of register address     => 0-255
 * 3    => Register length                  => 0-255
 * 4-n  => Register data type               => 0-255
 */
void _gatewayInitializationRegistersStructureHandler(
    const uint8_t packetId,
    const uint8_t nodeIndex,
    uint8_t * payload,
    const uint8_t payloadLength
) {
    // Validate node index
    if (nodeIndex >= FB_GATEWAY_MAX_NODES) {
        return;
    }

    word register_address = (word) payload[1] << 8 | (word) payload[2];

    uint8_t bytes_length = (uint8_t) payload[3];

    if ((bytes_length + 4) > payloadLength) {
        DEBUG_MSG(PSTR("[ERR][GATEWAY][INITIALIZATION] Packet structure is invalid. Packet lengt is not as expected\n"));

        return;
    }

    uint8_t byte_pointer = 4;

    for (uint8_t i = register_address; i <= (register_address + bytes_length); i++) {
        switch (packetId)
        {
            case GATEWAY_PACKET_AI_REGISTERS_STRUCTURE:
                gatewayRegistersSetAnalogInputsDataType(nodeIndex, i, (uint8_t) payload[byte_pointer]);
                break;

            case GATEWAY_PACKET_AO_REGISTERS_STRUCTURE:
                gatewayRegistersSetAnalogOutputsDataType(nodeIndex, i, (uint8_t) payload[byte_pointer]);
                break;
        }

        byte_pointer++;
    }

    // Reset incomming handler
    gatewayCommunicationSetWaitingPacket(nodeIndex, GATEWAY_PACKET_NONE);
    
    // Reset reading attempts counter
    gatewayCommunicationResetAttempts(nodeIndex);

    switch (packetId)
    {
        case GATEWAY_PACKET_AI_REGISTERS_STRUCTURE:
            if (gatewayRegistersAnalogInputsSize(nodeIndex) > bytes_length) {
                _gateway_nodes_initialization[nodeIndex].step = GATEWAY_PACKET_AI_REGISTERS_STRUCTURE;
                _gateway_nodes_initialization[nodeIndex].register_position = (register_address + bytes_length);

                return;
            }

            if (gatewayRegistersAnalogOutputsSize(nodeIndex) > 0) {
                _gateway_nodes_initialization[nodeIndex].step = GATEWAY_PACKET_AO_REGISTERS_STRUCTURE;
                _gateway_nodes_initialization[nodeIndex].register_position = 0;

                return;
            }
            break;

        case GATEWAY_PACKET_AO_REGISTERS_STRUCTURE:
            if (gatewayRegistersAnalogOutputsSize(nodeIndex) > bytes_length) {
                _gateway_nodes_initialization[nodeIndex].step = GATEWAY_PACKET_AO_REGISTERS_STRUCTURE;
                _gateway_nodes_initialization[nodeIndex].register_position = (register_address + bytes_length);

                return;
            }
            break;
    }
    
    // Node initiliazation successfully finished
    _gatewayInitializationMarkNodeAsInitialized(nodeIndex);
}

// -----------------------------------------------------------------------------
// MODULE: SUB-MODULE API
// -----------------------------------------------------------------------------

void gatewayInitializationReset()
{
    for (uint8_t i = 0; i < FB_GATEWAY_MAX_NODES; i++) {
        gatewayInitializationReset(i);
    }
}

// -----------------------------------------------------------------------------

void gatewayInitializationReset(
    const uint8_t nodeIndex
) {
    // Validate node index
    if (nodeIndex >= FB_GATEWAY_MAX_NODES) {
        return;
    }

    _gateway_nodes_initialization[nodeIndex].state = false;
    _gateway_nodes_initialization[nodeIndex].step = GATEWAY_PACKET_NONE;
    _gateway_nodes_initialization[nodeIndex].register_position = 0;
}

// -----------------------------------------------------------------------------

bool gatewayInitializationIsNodeInitialized(
    const uint8_t nodeIndex
) {
    // Validate node index
    if (nodeIndex >= FB_GATEWAY_MAX_NODES) {
        return false;
    }

    return _gateway_nodes_initialization[nodeIndex].state == true;
}

// -----------------------------------------------------------------------------

bool gatewayInitializationHandler(
    const uint8_t packetId,
    const uint8_t address,
    uint8_t * payload,
    const uint8_t payloadLength
) {
    switch (packetId)
    {
        case GATEWAY_PACKET_HW_MODEL:
        case GATEWAY_PACKET_HW_MANUFACTURER:
        case GATEWAY_PACKET_HW_VERSION:
        case GATEWAY_PACKET_FW_MODEL:
        case GATEWAY_PACKET_FW_MANUFACTURER:
        case GATEWAY_PACKET_FW_VERSION:
            _gatewayInitializationBaseDefinitionHandler(packetId, gatewayAddressingGetNodeIndex(address), payload, payloadLength);
            break;

        case GATEWAY_PACKET_REGISTERS_SIZE:
            _gatewayInitializationRegistersDefinitionsHandler(gatewayAddressingGetNodeIndex(address), payload, payloadLength);
            break;

        case GATEWAY_PACKET_AI_REGISTERS_STRUCTURE:
            _gatewayInitializationRegistersStructureHandler(packetId, gatewayAddressingGetNodeIndex(address), payload, payloadLength);
            break;

        case GATEWAY_PACKET_AO_REGISTERS_STRUCTURE:
            _gatewayInitializationRegistersStructureHandler(packetId, gatewayAddressingGetNodeIndex(address), payload, payloadLength);
            break;

        default:
            return false;
    }

    return true;
}

// -----------------------------------------------------------------------------
// MODULE: SUB-MODULE CORE
// -----------------------------------------------------------------------------

bool gatewayInitializationLoop(
    const uint8_t nodeIndex
) {
    // Validate node index
    if (nodeIndex >= FB_GATEWAY_MAX_NODES) {
        return false;
    }

    // Check if all connected nodes have finished initialization process
    if (_gatewayInitializationIsUnfinished(nodeIndex)) {
        _gatewayInitializationContinueInProcess(nodeIndex);

        return true;
    }

    return false;
}

#endif // FB_GATEWAY_SUPPORT
