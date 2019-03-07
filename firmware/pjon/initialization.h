/*

NODES MODULE - INITIALIZATION

Copyright (C) 2018 FastyBird Ltd. <info@fastybird.com>

*/

#if NODES_GATEWAY_SUPPORT

// -----------------------------------------------------------------------------
// NODE INITIALIZATION UTILS
// -----------------------------------------------------------------------------

/**
 * Parse received payload - Node description
 * 
 * 0    => Received packet identifier   => GATEWAY_PACKET_HW_MODEL|GATEWAY_PACKET_HW_MANUFACTURER|GATEWAY_PACKET_HW_VERSION|GATEWAY_PACKET_FW_MODEL|GATEWAY_PACKET_FW_MANUFACTURER|GATEWAY_PACKET_FW_VERSION
 * 1    => Description string length    => 1-255
 * 2-n  => Description content          => char array (a,b,c,...)
 */
void _gatewayExtractAndStoreDescription(
    const uint8_t packetId,
    const uint8_t id,
    uint8_t * payload
) {
    uint8_t content_length = (uint8_t) payload[1];

    char content[content_length];

    // Extract text content from payload
    for (uint8_t i = 0; i < content_length; i++) {
        content[i] = (char) payload[i + 2];
    }

    DEBUG_MSG(
        PSTR("[GATEWAY] Received node description: %s for address: %d\n"),
        (char *) content,
        (id + 1)
    );

    switch (packetId)
    {

    /**
     * HARDWARE
     */

        case GATEWAY_PACKET_HW_MODEL:
            _gateway_nodes[id].initiliazation.step = GATEWAY_PACKET_HW_MANUFACTURER;
            strncpy(_gateway_nodes[id].hardware.model, content, content_length);
            break;

        case GATEWAY_PACKET_HW_MANUFACTURER:
            _gateway_nodes[id].initiliazation.step = GATEWAY_PACKET_HW_VERSION;
            strncpy(_gateway_nodes[id].hardware.manufacturer, content, content_length);
            break;

        case GATEWAY_PACKET_HW_VERSION:
            _gateway_nodes[id].initiliazation.step = GATEWAY_PACKET_FW_MODEL;
            strncpy(_gateway_nodes[id].hardware.version, content, content_length);
            break;

    /**
     * FIRMWARE
     */

        case GATEWAY_PACKET_FW_MODEL:
            _gateway_nodes[id].initiliazation.step = GATEWAY_PACKET_FW_MANUFACTURER;
            strncpy(_gateway_nodes[id].firmware.model, content, content_length);
            break;

        case GATEWAY_PACKET_FW_MANUFACTURER:
            _gateway_nodes[id].initiliazation.step = GATEWAY_PACKET_FW_VERSION;
            strncpy(_gateway_nodes[id].firmware.manufacturer, content, content_length);
            break;

        case GATEWAY_PACKET_FW_VERSION:
            _gateway_nodes[id].initiliazation.step = GATEWAY_PACKET_REGISTERS_SIZE;
            strncpy(_gateway_nodes[id].firmware.version, content, content_length);
            break;

    /**
     * Unknown init sequence
     */

        default:
            return;
    }

    _gateway_nodes[id].initiliazation.attempts = 0;

    _gateway_nodes[id].packet.waiting_for = GATEWAY_PACKET_NONE;
    _gateway_nodes[id].packet.sending_time = 0;
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
 */
void _gatewayExtractAndStoreRegistersDefinitions(
    const uint8_t id,
    uint8_t * payload
) {
    std::vector<gateway_digital_register_t> reset_digital_inputs;
    _gateway_nodes[id].digital_inputs = reset_digital_inputs;

    std::vector<gateway_digital_register_t> reset_digital_outputs;
    _gateway_nodes[id].digital_outputs = reset_digital_outputs;

    std::vector<gateway_analog_register_t> reset_analog_inputs;
    _gateway_nodes[id].analog_inputs = reset_analog_inputs;

    std::vector<gateway_analog_register_t> reset_analog_outputs;
    _gateway_nodes[id].analog_outputs = reset_analog_outputs;

    _gateway_nodes[id].registers_size[GATEWAY_REGISTER_DI] = (uint8_t) payload[1];
    _gateway_nodes[id].registers_size[GATEWAY_REGISTER_DO] = (uint8_t) payload[2];
    _gateway_nodes[id].registers_size[GATEWAY_REGISTER_AI] = (uint8_t) payload[3];
    _gateway_nodes[id].registers_size[GATEWAY_REGISTER_AO] = (uint8_t) payload[4];

    _gateway_nodes[id].digital_inputs_reading.start = 0;
    _gateway_nodes[id].digital_inputs_reading.delay = 0;
    _gateway_nodes[id].digital_outputs_reading.start = 0;
    _gateway_nodes[id].digital_outputs_reading.delay = 0;
    _gateway_nodes[id].analog_inputs_reading.start = 0;
    _gateway_nodes[id].analog_inputs_reading.delay = 0;
    _gateway_nodes[id].analog_outputs_reading.start = 0;
    _gateway_nodes[id].analog_outputs_reading.delay = 0;
    
    for (uint8_t i = 0; i < _gateway_nodes[id].registers_size[GATEWAY_REGISTER_DI]; i++) {
        _gateway_nodes[id].digital_inputs.push_back((gateway_digital_register_t) {
            GATEWAY_DESCRIPTION_NOT_SET,
            false
        });
    }

    for (uint8_t i = 0; i < _gateway_nodes[id].registers_size[GATEWAY_REGISTER_DO]; i++) {
        _gateway_nodes[id].digital_outputs.push_back((gateway_digital_register_t) {
            GATEWAY_DESCRIPTION_NOT_SET,
            false
        });
    }

    for (uint8_t i = 0; i < _gateway_nodes[id].registers_size[GATEWAY_REGISTER_AI]; i++) {
        _gateway_nodes[id].analog_inputs.push_back((gateway_analog_register_t) {
            GATEWAY_DESCRIPTION_NOT_SET,
            GATEWAY_DATA_TYPE_UNKNOWN
        });
    }
 
    for (uint8_t i = 0; i < _gateway_nodes[id].registers_size[GATEWAY_REGISTER_AO]; i++) {
        _gateway_nodes[id].analog_outputs.push_back((gateway_analog_register_t) {
            GATEWAY_DESCRIPTION_NOT_SET,
            GATEWAY_DATA_TYPE_UNKNOWN
        });
    }

    DEBUG_MSG(
        PSTR("[GATEWAY] Received node registers structure (DI: %d, DO: %d, AI: %d, AO: %d) for node with address: %d\n"),
        _gateway_nodes[id].registers_size[GATEWAY_REGISTER_DI],
        _gateway_nodes[id].registers_size[GATEWAY_REGISTER_DO],
        _gateway_nodes[id].registers_size[GATEWAY_REGISTER_AI],
        _gateway_nodes[id].registers_size[GATEWAY_REGISTER_AO],
        (id + 1)
    );

    if (_gateway_nodes[id].registers_size[GATEWAY_REGISTER_AI] > 0) {
        _gateway_nodes[id].initiliazation.step = GATEWAY_PACKET_AI_REGISTERS_STRUCTURE;

    } else if (_gateway_nodes[id].registers_size[GATEWAY_REGISTER_AO] > 0) {
        _gateway_nodes[id].initiliazation.step = GATEWAY_PACKET_AO_REGISTERS_STRUCTURE;

    } else {
        // Node initiliazation successfully finished
        _gatewayMarkNodeAsInitialized(id);
    }
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
void _gatewayExtractAndStoreRegisterStructure(
    const uint8_t packetId,
    const uint8_t id,
    uint8_t * payload,
    const uint8_t payloadLength,
    const uint8_t dataRegister
) {
    word register_address = (word) payload[1] << 8 | (word) payload[2];

    uint8_t bytes_length = (uint8_t) payload[3];

    if ((bytes_length + 4) > payloadLength) {
        DEBUG_MSG(PSTR("[GATEWAY][ERR] Packet structure is invalid. Bytes length is higher than packet length\n"));

        return;
    }

    uint8_t byte_pointer = 4;

    for (uint8_t i = register_address; i <= (register_address + bytes_length); i++) {
        switch (packetId)
        {
            case GATEWAY_PACKET_AI_REGISTERS_STRUCTURE:
                switch ((uint8_t) payload[byte_pointer])
                {
                    case GATEWAY_DATA_TYPE_UINT8:
                        _gateway_nodes[id].analog_inputs[i].data_type = GATEWAY_DATA_TYPE_UINT8;
                        _gateway_nodes[id].analog_inputs[i].size = 1;
                        break;

                    case GATEWAY_DATA_TYPE_INT8:
                        _gateway_nodes[id].analog_inputs[i].data_type = GATEWAY_DATA_TYPE_INT8;
                        _gateway_nodes[id].analog_inputs[i].size = 1;
                        break;

                    case GATEWAY_DATA_TYPE_UINT16:
                        _gateway_nodes[id].analog_inputs[i].data_type = GATEWAY_DATA_TYPE_UINT16;
                        _gateway_nodes[id].analog_inputs[i].size = 2;
                        break;

                    case GATEWAY_DATA_TYPE_INT16:
                        _gateway_nodes[id].analog_inputs[i].data_type = GATEWAY_DATA_TYPE_INT16;
                        _gateway_nodes[id].analog_inputs[i].size = 2;
                        break;

                    case GATEWAY_DATA_TYPE_UINT32:
                        _gateway_nodes[id].analog_inputs[i].data_type = GATEWAY_DATA_TYPE_UINT32;
                        _gateway_nodes[id].analog_inputs[i].size = 4;
                        break;

                    case GATEWAY_DATA_TYPE_INT32:
                        _gateway_nodes[id].analog_inputs[i].data_type = GATEWAY_DATA_TYPE_INT32;
                        _gateway_nodes[id].analog_inputs[i].size = 4;
                        break;

                    case GATEWAY_DATA_TYPE_FLOAT32:
                        _gateway_nodes[id].analog_inputs[i].data_type = GATEWAY_DATA_TYPE_FLOAT32;
                        _gateway_nodes[id].analog_inputs[i].size = 4;
                        break;
                }
                break;

            case GATEWAY_PACKET_AO_REGISTERS_STRUCTURE:
                switch ((uint8_t) payload[byte_pointer])
                {
                    case GATEWAY_DATA_TYPE_UINT8:
                        _gateway_nodes[id].analog_outputs[i].data_type = GATEWAY_DATA_TYPE_UINT8;
                        _gateway_nodes[id].analog_outputs[i].size = 1;
                        break;

                    case GATEWAY_DATA_TYPE_INT8:
                        _gateway_nodes[id].analog_outputs[i].data_type = GATEWAY_DATA_TYPE_INT8;
                        _gateway_nodes[id].analog_outputs[i].size = 1;
                        break;

                    case GATEWAY_DATA_TYPE_UINT16:
                        _gateway_nodes[id].analog_outputs[i].data_type = GATEWAY_DATA_TYPE_UINT16;
                        _gateway_nodes[id].analog_outputs[i].size = 2;
                        break;

                    case GATEWAY_DATA_TYPE_INT16:
                        _gateway_nodes[id].analog_outputs[i].data_type = GATEWAY_DATA_TYPE_INT16;
                        _gateway_nodes[id].analog_outputs[i].size = 2;
                        break;

                    case GATEWAY_DATA_TYPE_UINT32:
                        _gateway_nodes[id].analog_outputs[i].data_type = GATEWAY_DATA_TYPE_UINT32;
                        _gateway_nodes[id].analog_outputs[i].size = 4;
                        break;

                    case GATEWAY_DATA_TYPE_INT32:
                        _gateway_nodes[id].analog_outputs[i].data_type = GATEWAY_DATA_TYPE_INT32;
                        _gateway_nodes[id].analog_outputs[i].size = 4;
                        break;

                    case GATEWAY_DATA_TYPE_FLOAT32:
                        _gateway_nodes[id].analog_outputs[i].data_type = GATEWAY_DATA_TYPE_FLOAT32;
                        _gateway_nodes[id].analog_outputs[i].size = 4;
                        break;
                }
                break;
        }

        byte_pointer++;
    }

    switch (packetId)
    {
        case GATEWAY_PACKET_AI_REGISTERS_STRUCTURE:
            if (_gateway_nodes[id].registers_size[GATEWAY_REGISTER_AI] > bytes_length) {
                _gatewaySendRegisterInitializationPacket(id, GATEWAY_PACKET_AI_REGISTERS_STRUCTURE, (register_address + bytes_length));

                return;
            }

            if (_gateway_nodes[id].registers_size[GATEWAY_REGISTER_AO] > 0) {
                _gateway_nodes[id].initiliazation.step = GATEWAY_PACKET_AO_REGISTERS_STRUCTURE;

                return;
            }
            break;

        case GATEWAY_PACKET_AO_REGISTERS_STRUCTURE:
            if (_gateway_nodes[id].registers_size[GATEWAY_REGISTER_AO] > bytes_length) {
                _gatewaySendRegisterInitializationPacket(id, GATEWAY_PACKET_AO_REGISTERS_STRUCTURE, (register_address + bytes_length));

                return;
            }
            break;
    }
    
    // Node initiliazation successfully finished
    _gatewayMarkNodeAsInitialized(id);
}

// -----------------------------------------------------------------------------
// INITIALIZATION HANDLERS
// -----------------------------------------------------------------------------

void _gatewayNodeInitializationHandler(
    const uint8_t packetId,
    const uint8_t address,
    uint8_t * payload
) {
    switch (packetId)
    {
        case GATEWAY_PACKET_HW_MODEL:
        case GATEWAY_PACKET_HW_MANUFACTURER:
        case GATEWAY_PACKET_HW_VERSION:
        case GATEWAY_PACKET_FW_MODEL:
        case GATEWAY_PACKET_FW_MANUFACTURER:
        case GATEWAY_PACKET_FW_VERSION:
            _gatewayExtractAndStoreDescription(packetId, (address - 1), payload);
            break;
    }
}

// -----------------------------------------------------------------------------

void _gatewayRegistersInitializationHandler(
    const uint8_t packetId,
    const uint8_t address,
    uint8_t * payload,
    const uint8_t payloadLength
) {
    switch (packetId)
    {
        case GATEWAY_PACKET_REGISTERS_SIZE:
            _gatewayExtractAndStoreRegistersDefinitions((address - 1), payload);
            break;

        case GATEWAY_PACKET_AI_REGISTERS_STRUCTURE:
            _gatewayExtractAndStoreRegisterStructure(packetId, (address - 1), payload, payloadLength, GATEWAY_REGISTER_AI);
            break;

        case GATEWAY_PACKET_AO_REGISTERS_STRUCTURE:
            _gatewayExtractAndStoreRegisterStructure(packetId, (address - 1), payload, payloadLength, GATEWAY_REGISTER_AO);
            break;
    }
}

#endif // NODES_GATEWAY_SUPPORT