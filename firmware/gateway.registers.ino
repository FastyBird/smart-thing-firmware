/*

GATEWAY MODULE - REGISTERS

Copyright (C) 2018 FastyBird Ltd. <info@fastybird.com>

*/

#if FB_GATEWAY_SUPPORT

gateway_registers_t _gateway_nodes_registers[FB_GATEWAY_MAX_NODES];

gateway_register_reading_t _gateway_nodes_registers_reading[FB_GATEWAY_MAX_NODES];

// -----------------------------------------------------------------------------
// MODULE: SUB-MODULE PRIVATE
// -----------------------------------------------------------------------------

bool _gatewayRegistersIsAddressCorrect(
    const uint8_t nodeIndex,
    const uint8_t dataRegister,
    const uint8_t address
) {
    // Validate node index
    if (nodeIndex >= FB_GATEWAY_MAX_NODES) {
        return false;
    }

    if (dataRegister == GATEWAY_REGISTER_DI) {
        return (address < _gateway_nodes_registers[nodeIndex].digital_inputs.size());

    } else if (dataRegister == GATEWAY_REGISTER_DO) {
        return (address < _gateway_nodes_registers[nodeIndex].digital_outputs.size());

    } else if (dataRegister == GATEWAY_REGISTER_AI) {
        return (address < _gateway_nodes_registers[nodeIndex].analog_inputs.size());

    } else if (dataRegister == GATEWAY_REGISTER_AO) {
        return (address < _gateway_nodes_registers[nodeIndex].analog_outputs.size());

    } else if (dataRegister == GATEWAY_REGISTER_EV) {
        return (address < _gateway_nodes_registers[nodeIndex].event_inputs.size());
    }

    return false;
}

// -----------------------------------------------------------------------------

bool _gatewayRegistersIsSizeCorrect(
    const uint8_t nodeIndex,
    const uint8_t dataRegister,
    const uint8_t address,
    const uint8_t size
) {
    // Validate node index
    if (nodeIndex >= FB_GATEWAY_MAX_NODES) {
        return false;
    }

    if (dataRegister == GATEWAY_REGISTER_DI) {
        return (_gateway_nodes_registers[nodeIndex].digital_inputs[address].size == size);

    } else if (dataRegister == GATEWAY_REGISTER_DO) {
        return (_gateway_nodes_registers[nodeIndex].digital_outputs[address].size == size);

    } else if (dataRegister == GATEWAY_REGISTER_AI) {
        return (_gateway_nodes_registers[nodeIndex].analog_inputs[address].size == size);

    } else if (dataRegister == GATEWAY_REGISTER_AO) {
        return (_gateway_nodes_registers[nodeIndex].analog_outputs[address].size == size);

    } else if (dataRegister == GATEWAY_REGISTER_EV) {
        return (_gateway_nodes_registers[nodeIndex].event_inputs[address].size == size);
    }

    return false;
}

// -----------------------------------------------------------------------------

uint8_t _gatewayRegistersGetRegisterDataType(
    const uint8_t nodeIndex,
    const uint8_t dataRegister,
    const uint8_t address
) {
    // Validate node index
    if (nodeIndex >= FB_GATEWAY_MAX_NODES) {
        return GATEWAY_DATA_TYPE_UNKNOWN;
    }

    switch (dataRegister)
    {
        case GATEWAY_REGISTER_DI:
            return _gateway_nodes_registers[nodeIndex].digital_inputs[address].datatype;

        case GATEWAY_REGISTER_DO:
            return _gateway_nodes_registers[nodeIndex].digital_outputs[address].datatype;

        case GATEWAY_REGISTER_AI:
            return _gateway_nodes_registers[nodeIndex].analog_inputs[address].datatype;

        case GATEWAY_REGISTER_AO:
            return _gateway_nodes_registers[nodeIndex].analog_outputs[address].datatype;

        case GATEWAY_REGISTER_EV:
            return _gateway_nodes_registers[nodeIndex].event_inputs[address].datatype;
    }

    return GATEWAY_DATA_TYPE_UNKNOWN;
}

// -----------------------------------------------------------------------------

void _gatewayRegistersUpdateReadingPointer(
    const uint8_t nodeIndex,
    const uint8_t dataRegister
) {
    // Validate node index
    if (nodeIndex >= FB_GATEWAY_MAX_NODES) {
        return;
    }

    // Validate register type index
    if (dataRegister >= 5) {
        return;
    }

    if (
        _gateway_nodes_registers_reading[nodeIndex].register_type == dataRegister
        || _gateway_nodes_registers_reading[nodeIndex].register_type == GATEWAY_REGISTER_NONE
    ) {
        if (
            (
                dataRegister == GATEWAY_REGISTER_DI
                && _gateway_nodes_registers[nodeIndex].digital_inputs.size() > 0
            ) || (
                dataRegister == GATEWAY_REGISTER_DO
                && _gateway_nodes_registers[nodeIndex].digital_outputs.size() > 0
            ) || (
                dataRegister == GATEWAY_REGISTER_AI
                && _gateway_nodes_registers[nodeIndex].analog_inputs.size() > 0
            ) || (
                dataRegister == GATEWAY_REGISTER_AO
                && _gateway_nodes_registers[nodeIndex].analog_outputs.size() > 0
            ) || (
                dataRegister == GATEWAY_REGISTER_EV
                && _gateway_nodes_registers[nodeIndex].event_inputs.size() > 0
            )
        ) {
            _gateway_nodes_registers_reading[nodeIndex].register_type = dataRegister;
            _gateway_nodes_registers_reading[nodeIndex].start = 0;

            return;
        }
    }

    _gateway_nodes_registers_reading[nodeIndex].register_type = GATEWAY_REGISTER_NONE;
}

// -----------------------------------------------------------------------------
// REGISTERS WRITE
// -----------------------------------------------------------------------------

bool _gatewayRegistersWriteValue(
    const uint8_t nodeIndex,
    const uint8_t dataRegister,
    const uint8_t address,
    const void * value,
    const uint8_t size
) {
    // Validate node index
    if (nodeIndex >= FB_GATEWAY_MAX_NODES) {
        return false;
    }

    if (_gatewayRegistersIsAddressCorrect(nodeIndex, dataRegister, address) == false) {
        return false;
    }

    if (_gatewayRegistersIsSizeCorrect(nodeIndex, dataRegister, address, size)) {
        if (dataRegister == GATEWAY_REGISTER_DI) {
            memcpy(_gateway_nodes_registers[nodeIndex].digital_inputs[address].value, value, size);

            return true;

        } else if (dataRegister == GATEWAY_REGISTER_DO) {
            memcpy(_gateway_nodes_registers[nodeIndex].digital_outputs[address].value, value, size);

            return true;

        } else if (dataRegister == GATEWAY_REGISTER_AI) {
            memcpy(_gateway_nodes_registers[nodeIndex].analog_inputs[address].value, value, size);

            return true;

        } else if (dataRegister == GATEWAY_REGISTER_AO) {
            memcpy(_gateway_nodes_registers[nodeIndex].analog_outputs[address].value, value, size);

            return true;

        } else if (dataRegister == GATEWAY_REGISTER_EV) {
            memcpy(_gateway_nodes_registers[nodeIndex].event_inputs[address].value, value, size);

            return true;
        }
    }

    return false;
}

// -----------------------------------------------------------------------------

// Specialized convenience setters (these do not cost memory because of inlining)
bool _gatewayRegistersWriteValue(const uint8_t nodeIndex, const uint8_t dataRegister, const uint8_t address, const bool value) { return _gatewayRegistersWriteValue(nodeIndex, dataRegister, address, &value, 1); }
bool _gatewayRegistersWriteValue(const uint8_t nodeIndex, const uint8_t dataRegister, const uint8_t address, const uint8_t value) { return _gatewayRegistersWriteValue(nodeIndex, dataRegister, address, &value, 1); }
bool _gatewayRegistersWriteValue(const uint8_t nodeIndex, const uint8_t dataRegister, const uint8_t address, const uint16_t value) { return _gatewayRegistersWriteValue(nodeIndex, dataRegister, address, &value, 2); }
bool _gatewayRegistersWriteValue(const uint8_t nodeIndex, const uint8_t dataRegister, const uint8_t address, const uint32_t value) { return _gatewayRegistersWriteValue(nodeIndex, dataRegister, address, &value, 4); }
bool _gatewayRegistersWriteValue(const uint8_t nodeIndex, const uint8_t dataRegister, const uint8_t address, const int8_t value) { return _gatewayRegistersWriteValue(nodeIndex, dataRegister, address, &value, 1); }
bool _gatewayRegistersWriteValue(const uint8_t nodeIndex, const uint8_t dataRegister, const uint8_t address, const int16_t value) { return _gatewayRegistersWriteValue(nodeIndex, dataRegister, address, &value, 2); }
bool _gatewayRegistersWriteValue(const uint8_t nodeIndex, const uint8_t dataRegister, const uint8_t address, const int32_t value) { return _gatewayRegistersWriteValue(nodeIndex, dataRegister, address, &value, 4); }
bool _gatewayRegistersWriteValue(const uint8_t nodeIndex, const uint8_t dataRegister, const uint8_t address, const float value) { return _gatewayRegistersWriteValue(nodeIndex, dataRegister, address, &value, 4); }

// -----------------------------------------------------------------------------

/**
 * Transform received 4 bytes value into register value
 */
void _gatewayRegistersWriteReceivedValue(
    const uint8_t nodeIndex,
    const uint8_t dataRegister,
    const uint8_t address,
    char * value
) {
    // Validate node index
    if (nodeIndex >= FB_GATEWAY_MAX_NODES) {
        return;
    }

    switch (_gatewayRegistersGetRegisterDataType(nodeIndex, dataRegister, address))
    {
        case GATEWAY_DATA_TYPE_BOOL:
            bool bool_stored_value;
            BOOL_UNION_t bool_write_value;

            bool_write_value.bytes[0] = value[0];
            bool_write_value.bytes[1] = value[1];
            bool_write_value.bytes[2] = value[2];
            bool_write_value.bytes[3] = value[3];

            gatewayRegistersReadValue(nodeIndex, dataRegister, address, bool_stored_value);

            if (bool_stored_value != bool_write_value.number) {
                if (_gatewayRegistersWriteValue(nodeIndex, dataRegister, address, bool_write_value.number)) {
                    gatewayModulesRegisterValueUpdated(nodeIndex, dataRegister, address, bool_write_value.number);

                } else {
                    DEBUG_MSG(PSTR("[ERR][GATEWAY][REGISTERS] Value could not be stored in register\n"));
                }
            }
            break;

        case GATEWAY_DATA_TYPE_UINT8:
            uint8_t uint8_stored_value;
            UINT8_UNION_t uint8_write_value;

            uint8_write_value.bytes[0] = value[0];
            uint8_write_value.bytes[1] = value[1];
            uint8_write_value.bytes[2] = value[2];
            uint8_write_value.bytes[3] = value[3];

            gatewayRegistersReadValue(nodeIndex, dataRegister, address, uint8_stored_value);

            if (uint8_stored_value != uint8_write_value.number) {
                if (_gatewayRegistersWriteValue(nodeIndex, dataRegister, address, uint8_write_value.number)) {
                    gatewayModulesRegisterValueUpdated(nodeIndex, dataRegister, address, uint8_write_value.number);

                } else {
                    DEBUG_MSG(PSTR("[ERR][GATEWAY][REGISTERS] Value could not be stored in register\n"));
                }
            }
            break;

        case GATEWAY_DATA_TYPE_UINT16:
            uint16_t uint16_stored_value;
            UINT16_UNION_t uint16_write_value;

            uint16_write_value.bytes[0] = value[0];
            uint16_write_value.bytes[1] = value[1];
            uint16_write_value.bytes[2] = value[2];
            uint16_write_value.bytes[3] = value[3];

            gatewayRegistersReadValue(nodeIndex, dataRegister, address, uint16_stored_value);

            if (uint16_stored_value != uint16_write_value.number) {
                if (_gatewayRegistersWriteValue(nodeIndex, dataRegister, address, uint16_write_value.number)) {
                    gatewayModulesRegisterValueUpdated(nodeIndex, dataRegister, address, uint16_write_value.number);

                } else {
                    DEBUG_MSG(PSTR("[ERR][GATEWAY][REGISTERS] Value could not be stored in register\n"));
                }
            }
            break;

        case GATEWAY_DATA_TYPE_UINT32:
            uint32_t uint32_stored_value;
            UINT32_UNION_t uint32_write_value;

            uint32_write_value.bytes[0] = value[0];
            uint32_write_value.bytes[1] = value[1];
            uint32_write_value.bytes[2] = value[2];
            uint32_write_value.bytes[3] = value[3];

            gatewayRegistersReadValue(nodeIndex, dataRegister, address, uint32_stored_value);

            if (uint32_stored_value != uint32_write_value.number) {
                if (_gatewayRegistersWriteValue(nodeIndex, dataRegister, address, uint32_write_value.number)) {
                    gatewayModulesRegisterValueUpdated(nodeIndex, dataRegister, address, uint32_write_value.number);

                } else {
                    DEBUG_MSG(PSTR("[ERR][GATEWAY][REGISTERS] Value could not be stored in register\n"));
                }
            }
            break;

        case GATEWAY_DATA_TYPE_INT8:
            int8_t int8_stored_value;
            INT8_UNION_t int8_write_value;

            int8_write_value.bytes[0] = value[0];
            int8_write_value.bytes[1] = value[1];
            int8_write_value.bytes[2] = value[2];
            int8_write_value.bytes[3] = value[3];

            gatewayRegistersReadValue(nodeIndex, dataRegister, address, int8_stored_value);

            if (int8_stored_value != int8_write_value.number) {
                if (_gatewayRegistersWriteValue(nodeIndex, dataRegister, address, int8_write_value.number)) {
                    gatewayModulesRegisterValueUpdated(nodeIndex, dataRegister, address, int8_write_value.number);

                } else {
                    DEBUG_MSG(PSTR("[ERR][GATEWAY][REGISTERS] Value could not be stored in register\n"));
                }
            }
            break;

        case GATEWAY_DATA_TYPE_INT16:
            int16_t int16_stored_value;
            INT16_UNION_t int16_write_value;

            int16_write_value.bytes[0] = value[0];
            int16_write_value.bytes[1] = value[1];
            int16_write_value.bytes[2] = value[2];
            int16_write_value.bytes[3] = value[3];

            gatewayRegistersReadValue(nodeIndex, dataRegister, address, int16_stored_value);

            if (int16_stored_value != int16_write_value.number) {
                if (_gatewayRegistersWriteValue(nodeIndex, dataRegister, address, int16_write_value.number)) {
                    gatewayModulesRegisterValueUpdated(nodeIndex, dataRegister, address, int16_write_value.number);

                } else {
                    DEBUG_MSG(PSTR("[ERR][GATEWAY][REGISTERS] Value could not be stored in register\n"));
                }
            }
            break;

        case GATEWAY_DATA_TYPE_INT32:
            int32_t int32_stored_value;
            INT32_UNION_t int32_write_value;

            int32_write_value.bytes[0] = value[0];
            int32_write_value.bytes[1] = value[1];
            int32_write_value.bytes[2] = value[2];
            int32_write_value.bytes[3] = value[3];

            gatewayRegistersReadValue(nodeIndex, dataRegister, address, int32_stored_value);

            if (int32_stored_value != int32_write_value.number) {
                if (_gatewayRegistersWriteValue(nodeIndex, dataRegister, address, int32_write_value.number)) {
                    gatewayModulesRegisterValueUpdated(nodeIndex, dataRegister, address, int32_write_value.number);

                } else {
                    DEBUG_MSG(PSTR("[ERR][GATEWAY][REGISTERS] Value could not be stored in register\n"));
                }
            }
            break;

        case GATEWAY_DATA_TYPE_FLOAT32:
            float float_stored_value;
            FLOAT32_UNION_t float_write_value;

            float_write_value.bytes[0] = value[0];
            float_write_value.bytes[1] = value[1];
            float_write_value.bytes[2] = value[2];
            float_write_value.bytes[3] = value[3];

            gatewayRegistersReadValue(nodeIndex, dataRegister, address, float_stored_value);

            if (float_stored_value != float_write_value.number) {
                if (_gatewayRegistersWriteValue(nodeIndex, dataRegister, address, float_write_value.number)) {
                    gatewayModulesRegisterValueUpdated(nodeIndex, dataRegister, address, float_write_value.number);

                } else {
                    DEBUG_MSG(PSTR("[ERR][GATEWAY][REGISTERS] Value could not be stored in register\n"));
                }
            }
            break;

        default:
            DEBUG_MSG(PSTR("[ERR][GATEWAY][REGISTERS] Provided unknown data type for writing into register\n"));
            break;
    }
}

// -----------------------------------------------------------------------------
// DIGITAL REGISTERS READING
// -----------------------------------------------------------------------------

void _gatewayRegistersRequestReadingDigitalRegisters(
    const uint8_t nodeIndex,
    const uint8_t dataRegister
) {
    // Validate node index
    if (nodeIndex >= FB_GATEWAY_MAX_NODES) {
        return;
    }

    uint8_t packet_id = GATEWAY_PACKET_NONE;

    // Validate register index
    if (dataRegister == GATEWAY_REGISTER_DI) {
        packet_id = GATEWAY_PACKET_READ_MULTI_DI;

    } else if (dataRegister == GATEWAY_REGISTER_DO) {
        packet_id = GATEWAY_PACKET_READ_MULTI_DO;

    } else {
        return;
    }

    uint8_t register_size = 0;

    switch (dataRegister)
    {
        case GATEWAY_REGISTER_DI:
            register_size = _gateway_nodes_registers[nodeIndex].digital_inputs.size();
            break;

        case GATEWAY_REGISTER_DO:
            register_size = _gateway_nodes_registers[nodeIndex].digital_outputs.size();
            break;
    }

    uint8_t start = _gateway_nodes_registers_reading[nodeIndex].start;

    char output_content[5];

    output_content[0] = packet_id;

    // Register reading start address
    output_content[1] = (char) (start >> 8);
    output_content[2] = (char) (start & 0xFF);

    // It is based on maximum packed size reduced by packet header (5 bytes)
    uint8_t max_readable_addresses = (gatewayCommunicationGetMaxPacketSize(nodeIndex) - 5) * 8;

    // Node has less digital registers than one packet can handle
    if (register_size <= max_readable_addresses) {
        // Register reading lenght
        output_content[3] = (char) (register_size >> 8);
        output_content[4] = (char) (register_size & 0xFF);

    // Node has more digital registers than one packet can handle
    } else {
        if (start + max_readable_addresses <= register_size) {
            // Register reading lenght
            output_content[3] = (char) (max_readable_addresses >> 8);
            output_content[4] = (char) (max_readable_addresses & 0xFF);

            if (start + max_readable_addresses < register_size) {
                // Move pointer
                start = start + max_readable_addresses;

            } else {
                // Move pointer
                start = 0;
            }

        } else {
            // Register reading lenght
            output_content[3] = (char) ((register_size - start) >> 8);
            output_content[4] = (char) ((register_size - start) & 0xFF);

            // Move pointer
            start = 0;
        }
    }

    // Record requested packet
    if (gatewaySendPacket((nodeIndex + 1), output_content, 5)) {
        gatewayCommunicationSetWaitingPacket(nodeIndex, packet_id);

        _gateway_nodes_registers_reading[nodeIndex].start = start;

         if (start == 0) {
            _gatewayRegistersUpdateReadingPointer(nodeIndex, dataRegister);
         }
    }

    gatewayCommunicationIncrementAttempts(nodeIndex);
}

// -----------------------------------------------------------------------------

void _gatewayRegistersRequestReadingMultipleDigitalInputRegisters(
    const uint8_t nodeIndex
) {
    // Validate node index
    if (nodeIndex >= FB_GATEWAY_MAX_NODES) {
        return;
    }

    _gatewayRegistersRequestReadingDigitalRegisters(
        nodeIndex,
        GATEWAY_REGISTER_DI
    );
}

// -----------------------------------------------------------------------------

void _gatewayRegistersRequestReadingMultipleDigitalOutputRegisters(
    const uint8_t nodeIndex
) {
    // Validate node index
    if (nodeIndex >= FB_GATEWAY_MAX_NODES) {
        return;
    }

    _gatewayRegistersRequestReadingDigitalRegisters(
        nodeIndex,
        GATEWAY_REGISTER_DO
    );
}

// -----------------------------------------------------------------------------
// ANALOG REGISTERS READING
// -----------------------------------------------------------------------------

void _gatewayRegistersRequestReadingAnalogRegisters(
    const uint8_t nodeIndex,
    const uint8_t dataRegister
) {
    // Validate node index
    if (nodeIndex >= FB_GATEWAY_MAX_NODES) {
        return;
    }

    uint8_t packet_id = GATEWAY_PACKET_NONE;

    // Validate register index
    if (dataRegister == GATEWAY_REGISTER_AI) {
        packet_id = GATEWAY_PACKET_READ_MULTI_AI;

    } else if (dataRegister == GATEWAY_REGISTER_AO) {
        packet_id = GATEWAY_PACKET_READ_MULTI_AO;

    } else {
        return;
    }

    uint8_t register_size = 0;

    switch (dataRegister)
    {
        case GATEWAY_REGISTER_AI:
            register_size = _gateway_nodes_registers[nodeIndex].analog_inputs.size();
            break;

        case GATEWAY_REGISTER_AO:
            register_size = _gateway_nodes_registers[nodeIndex].analog_outputs.size();
            break;
    }

    uint8_t start = _gateway_nodes_registers_reading[nodeIndex].start;

    char output_content[5];

    output_content[0] = packet_id;

    // Register reading start address
    output_content[1] = (char) (start >> 8);
    output_content[2] = (char) (start & 0xFF);

    // It is based on maximum packed size reduced by packet header (5 bytes)
    uint8_t max_readable_addresses = (uint8_t) ((gatewayCommunicationGetMaxPacketSize(nodeIndex) - 5) / 4);

    // Node has less digital registers than one packet can handle
    if (register_size <= max_readable_addresses) {
        // Register reading lenght
        output_content[3] = (char) (register_size >> 8);
        output_content[4] = (char) (register_size & 0xFF);

    // Node has more digital registers than one packet can handle
    } else {
        if (start + max_readable_addresses <= register_size) {
            // Register reading lenght
            output_content[3] = (char) (max_readable_addresses >> 8);
            output_content[4] = (char) (max_readable_addresses & 0xFF);

            if (start + max_readable_addresses < register_size) {
                // Move pointer
                start = start + max_readable_addresses;

            } else {
                // Move pointer
                start = 0;
            }

        } else {
            // Register reading lenght
            output_content[3] = (char) ((register_size - start) >> 8);
            output_content[4] = (char) ((register_size - start) & 0xFF);

            // Move pointer
            start = 0;
        }
    }

    // Record requested packet
    if (gatewaySendPacket((nodeIndex + 1), output_content, 5)) {
        gatewayCommunicationSetWaitingPacket(nodeIndex, packet_id);

        _gateway_nodes_registers_reading[nodeIndex].start = start;

         if (start == 0) {
            _gatewayRegistersUpdateReadingPointer(nodeIndex, dataRegister);
         }
    }

    gatewayCommunicationIncrementAttempts(nodeIndex);
}

// -----------------------------------------------------------------------------

void _gatewayRegistersRequestReadingMultipleAnalogInputRegisters(
    const uint8_t nodeIndex
) {
    // Validate node index
    if (nodeIndex >= FB_GATEWAY_MAX_NODES) {
        return;
    }

    _gatewayRegistersRequestReadingAnalogRegisters(
        nodeIndex,
        GATEWAY_REGISTER_AI
    );
}

// -----------------------------------------------------------------------------

void _gatewayRegistersRequestReadingMultipleAnalogOutputRegisters(
    const uint8_t nodeIndex
) {
    // Validate node index
    if (nodeIndex >= FB_GATEWAY_MAX_NODES) {
        return;
    }

    _gatewayRegistersRequestReadingAnalogRegisters(
        nodeIndex,
        GATEWAY_REGISTER_AO
    );
}

// -----------------------------------------------------------------------------
// EVENT REGISTERS READING
// -----------------------------------------------------------------------------

void _gatewayRegistersRequestReadingEventRegisters(
    const uint8_t nodeIndex,
    const uint8_t dataRegister
) {
    // Validate node index
    if (nodeIndex >= FB_GATEWAY_MAX_NODES) {
        return;
    }

    uint8_t packet_id = GATEWAY_PACKET_NONE;

    // Validate register index
    if (dataRegister == GATEWAY_REGISTER_EV) {
        packet_id = GATEWAY_PACKET_READ_MULTI_EV;

    } else {
        return;
    }

    uint8_t register_size = 0;

    switch (dataRegister)
    {
        case GATEWAY_REGISTER_EV:
            register_size = _gateway_nodes_registers[nodeIndex].event_inputs.size();
            break;
    }

    uint8_t start = _gateway_nodes_registers_reading[nodeIndex].start;

    char output_content[5];

    output_content[0] = packet_id;

    // Register reading start address
    output_content[1] = (char) (start >> 8);
    output_content[2] = (char) (start & 0xFF);

    // It is based on maximum packed size reduced by packet header (5 bytes)
    uint8_t max_readable_addresses = (uint8_t) ((gatewayCommunicationGetMaxPacketSize(nodeIndex) - 5) / 4);

    // Node has less digital registers than one packet can handle
    if (register_size <= max_readable_addresses) {
        // Register reading lenght
        output_content[3] = (char) (register_size >> 8);
        output_content[4] = (char) (register_size & 0xFF);

    // Node has more digital registers than one packet can handle
    } else {
        if (start + max_readable_addresses <= register_size) {
            // Register reading lenght
            output_content[3] = (char) (max_readable_addresses >> 8);
            output_content[4] = (char) (max_readable_addresses & 0xFF);

            if (start + max_readable_addresses < register_size) {
                // Move pointer
                start = start + max_readable_addresses;

            } else {
                // Move pointer
                start = 0;
            }

        } else {
            // Register reading lenght
            output_content[3] = (char) ((register_size - start) >> 8);
            output_content[4] = (char) ((register_size - start) & 0xFF);

            // Move pointer
            start = 0;
        }
    }

    // Record requested packet
    if (gatewaySendPacket((nodeIndex + 1), output_content, 5)) {
        gatewayCommunicationSetWaitingPacket(nodeIndex, packet_id);

        _gateway_nodes_registers_reading[nodeIndex].start = start;

         if (start == 0) {
            _gatewayRegistersUpdateReadingPointer(nodeIndex, dataRegister);
         }
    }

    gatewayCommunicationIncrementAttempts(nodeIndex);
}

// -----------------------------------------------------------------------------

void _gatewayRegistersRequestReadingMultipleEventRegisters(
    const uint8_t nodeIndex
) {
    // Validate node index
    if (nodeIndex >= FB_GATEWAY_MAX_NODES) {
        return;
    }

    _gatewayRegistersRequestReadingEventRegisters(
        nodeIndex,
        GATEWAY_REGISTER_EV
    );
}

// -----------------------------------------------------------------------------
// DIGITAL REGISTERS WRITING (only for DO registers)
// -----------------------------------------------------------------------------

void _gatewayRegistersRequestWritingSingleDigitalRegister(
    const uint8_t nodeIndex,
    const uint8_t registerAddress,
    bool value
) {
    // Validate node index
    if (nodeIndex >= FB_GATEWAY_MAX_NODES) {
        return;
    }

    char output_content[6];

    output_content[0] = GATEWAY_PACKET_WRITE_ONE_DO;

    // Register write address
    output_content[1] = (char) (registerAddress >> 8);
    output_content[2] = (char) (registerAddress & 0xFF);

    uint16_t write_value;

    if (value == true) {
        write_value = 0xFF00;

    } else {
        write_value = 0x0000;
    }

    output_content[3] = (char) (write_value >> 8);
    output_content[4] = (char) (write_value & 0xFF);

    // Record requested packet
    if (gatewaySendPacket((nodeIndex + 1), output_content, 5)) {
        gatewayCommunicationSetWaitingPacket(nodeIndex, GATEWAY_PACKET_WRITE_ONE_DO);
    }

    gatewayCommunicationIncrementAttempts(nodeIndex);
}

// -----------------------------------------------------------------------------
// ANALOG REGISTERS WRITING (only for AO registers)
// -----------------------------------------------------------------------------

void _gatewayRegistersRequestWritingSingleAnalogRegister(
    const uint8_t nodeIndex,
    const uint8_t registerAddress,
    void * value,
    const uint8_t size
) {
    // Validate node index
    if (nodeIndex >= FB_GATEWAY_MAX_NODES) {
        return;
    }

    char output_content[6];

    output_content[0] = GATEWAY_PACKET_WRITE_ONE_AO;

    char buffer[4] = { 0 };

    memcpy(buffer, value, size);

    // Register write address
    output_content[1] = (char) (registerAddress >> 8);
    output_content[2] = (char) (registerAddress & 0xFF);
    output_content[3] = (char) buffer[0];
    output_content[4] = (char) buffer[1];
    output_content[5] = (char) buffer[2];
    output_content[6] = (char) buffer[3];

    // Record requested packet
    if (gatewaySendPacket((nodeIndex + 1), output_content, 6)) {
        gatewayCommunicationSetWaitingPacket(nodeIndex, GATEWAY_PACKET_WRITE_ONE_AO);
    }

    gatewayCommunicationIncrementAttempts(nodeIndex);
}

// -----------------------------------------------------------------------------

// Specialized convenience setters (these do not cost memory because of inlining)
void _gatewayRegistersRequestWritingSingleAnalogRegister(const uint8_t nodeIndex, const uint8_t registerAddress, uint8_t value) { _gatewayRegistersRequestWritingSingleAnalogRegister(nodeIndex, registerAddress, &value, 1); }
void _gatewayRegistersRequestWritingSingleAnalogRegister(const uint8_t nodeIndex, const uint8_t registerAddress, uint16_t value) { _gatewayRegistersRequestWritingSingleAnalogRegister(nodeIndex, registerAddress, &value, 2); }
void _gatewayRegistersRequestWritingSingleAnalogRegister(const uint8_t nodeIndex, const uint8_t registerAddress, uint32_t value) { _gatewayRegistersRequestWritingSingleAnalogRegister(nodeIndex, registerAddress, &value, 4); }
void _gatewayRegistersRequestWritingSingleAnalogRegister(const uint8_t nodeIndex, const uint8_t registerAddress, int8_t value) { _gatewayRegistersRequestWritingSingleAnalogRegister(nodeIndex, registerAddress, &value, 1); }
void _gatewayRegistersRequestWritingSingleAnalogRegister(const uint8_t nodeIndex, const uint8_t registerAddress, int16_t value) { _gatewayRegistersRequestWritingSingleAnalogRegister(nodeIndex, registerAddress, &value, 2); }
void _gatewayRegistersRequestWritingSingleAnalogRegister(const uint8_t nodeIndex, const uint8_t registerAddress, int32_t value) { _gatewayRegistersRequestWritingSingleAnalogRegister(nodeIndex, registerAddress, &value, 4); }
void _gatewayRegistersRequestWritingSingleAnalogRegister(const uint8_t nodeIndex, const uint8_t registerAddress, float value) { _gatewayRegistersRequestWritingSingleAnalogRegister(nodeIndex, registerAddress, &value, 4); }

// -----------------------------------------------------------------------------
// READING HANDLERS 
// -----------------------------------------------------------------------------

void _gatewayRegistersReadMultipleRegisterAddressesHandler(
    const uint8_t nodeIndex,
    const uint8_t dataRegister,
    uint8_t * payload
) {
    // Validate node index
    if (nodeIndex >= FB_GATEWAY_MAX_NODES) {
        return;
    }

    word start_address = (word) payload[1] << 8 | (word) payload[2];

    uint8_t bytes_length = (uint8_t) payload[3];

    // DEBUG_MSG(
    //     PSTR("[INFO][GATEWAY][REGISTERS] Received reading response from node: %d to register: %d at address: %d with byte length: %d\n"),
    //     nodeIndex,
    //     dataRegister,
    //     start_address,
    //     bytes_length
    // );

    uint8_t register_size = 0;

    switch (dataRegister)
    {
        case GATEWAY_REGISTER_DI:
            register_size = _gateway_nodes_registers[nodeIndex].digital_inputs.size();
            break;

        case GATEWAY_REGISTER_DO:
            register_size = _gateway_nodes_registers[nodeIndex].digital_outputs.size();
            break;

        case GATEWAY_REGISTER_AI:
            register_size = _gateway_nodes_registers[nodeIndex].analog_inputs.size();
            break;

        case GATEWAY_REGISTER_AO:
            register_size = _gateway_nodes_registers[nodeIndex].analog_outputs.size();
            break;

        case GATEWAY_REGISTER_EV:
            register_size = _gateway_nodes_registers[nodeIndex].event_inputs.size();
            break;
    }

    if (
        // Read start address must be between <0, register.size()>
        start_address < register_size
    ) {
        if (dataRegister == GATEWAY_REGISTER_DI || dataRegister == GATEWAY_REGISTER_DO) {
            uint8_t write_byte = 1;
            uint8_t data_byte;

            uint8_t write_address = start_address;

            BOOL_UNION_t bool_write_value;

            bool_write_value.number = false;

            char write_value[4] = { 0 };

            while (
                write_address < register_size
                && write_byte <= bytes_length
            ) {
                data_byte = (uint8_t) payload[3 + write_byte];

                for (uint8_t i = 0; i < 8; i++) {
                    bool_write_value.number = (data_byte >> i) & 0x01 ? true : false;

                    // Clear buffer
                    memset(write_value, 0, sizeof(write_value));

                    write_value[0] = bool_write_value.bytes[0];
                    write_value[1] = bool_write_value.bytes[1];
                    write_value[2] = bool_write_value.bytes[2];
                    write_value[3] = bool_write_value.bytes[3];

                    _gatewayRegistersWriteReceivedValue(nodeIndex, dataRegister, write_address, write_value);

                    // DEBUG_MSG(
                    //     PSTR("[INFO][GATEWAY][REGISTERS] Value was written into register: %d at address: %d\n"),
                    //     dataRegister,
                    //     write_address
                    // );

                    write_address++;

                    if (write_address >= register_size) {
                        break;
                    }
                }

                write_byte++;
            }

        } else {
            uint8_t write_byte = 4;
            uint8_t data_byte;

            uint8_t write_address = start_address;

            char write_value[4] = { 0 };

            while (
                write_address < register_size
                && write_byte <= bytes_length
            ) {
                // Clear buffer
                memset(write_value, 0, sizeof(write_value));

                write_value[0] = payload[write_byte];
                write_value[1] = payload[write_byte + 1];
                write_value[2] = payload[write_byte + 2];
                write_value[3] = payload[write_byte + 3];

                _gatewayRegistersWriteReceivedValue(nodeIndex, dataRegister, write_address, write_value);

                // DEBUG_MSG(
                //     PSTR("[INFO][GATEWAY][REGISTERS] Value was written into register: %d at address: %d\n"),
                //     dataRegister,
                //     write_address
                // );

                write_byte = write_byte + 4;
                write_address++;
            }
        }

        gatewayCommunicationResetAttempts(nodeIndex);

    } else {
        DEBUG_MSG(
            PSTR("[ERR][GATEWAY][REGISTERS] Node is trying to write to undefined register: %d address\n"),
            dataRegister
        );
    }
}

// -----------------------------------------------------------------------------
// WRITING HANDLERS
// -----------------------------------------------------------------------------

void _gatewayRegistersWriteSingleDigitalOutputHandler(
    const uint8_t nodeIndex,
    uint8_t * payload
) {
    // Validate node index
    if (nodeIndex >= FB_GATEWAY_MAX_NODES) {
        return;
    }

    word register_address = (word) payload[1] << 8 | (word) payload[2];
    
    char write_value[4] = { 0 };

    write_value[0] = payload[3];
    write_value[1] = payload[4];
    write_value[2] = payload[5];
    write_value[3] = payload[6];

    if (
        // Write address must be between <0, register.size()>
        register_address < _gateway_nodes_registers[nodeIndex].digital_outputs.size()
    ) {
        _gatewayRegistersWriteReceivedValue(nodeIndex, GATEWAY_REGISTER_DO, register_address, write_value);

        // DEBUG_MSG(PSTR("[INFO][GATEWAY][REGISTERS] Value was written into DO register\n"));

        gatewayCommunicationResetAttempts(nodeIndex);
        
    } else {
        DEBUG_MSG(PSTR("[ERR][GATEWAY][REGISTERS] Node is trying to write to undefined DO register address\n"));
    }
}

// -----------------------------------------------------------------------------

void _gatewayRegistersWriteMultipleDigitalOutputHandler(
    const uint8_t nodeIndex,
    uint8_t * payload
) {
    // Validate node index
    if (nodeIndex >= FB_GATEWAY_MAX_NODES) {
        return;
    }

    word register_address = (word) payload[1] << 8 | (word) payload[2];
    word read_length = (word) payload[3] << 8 | (word) payload[4];

    uint8_t bytes_length = (uint8_t) payload[5];

    if (
        // Write start address mus be between <0, register.size()>
        register_address < _gateway_nodes_registers[nodeIndex].digital_outputs.size()
        // Write length have to be same or smaller as registers size
        && (register_address + read_length) <= _gateway_nodes_registers[nodeIndex].digital_outputs.size()
    ) {
        // TODO: implement response handling

        gatewayCommunicationResetAttempts(nodeIndex);

    } else {
        DEBUG_MSG(PSTR("[ERR][GATEWAY][REGISTERS] Node is trying to write to undefined DO registers range\n"));
    }
}

// -----------------------------------------------------------------------------

void _gatewayRegistersWriteSingleAnalogOutputHandler(
    const uint8_t nodeIndex,
    uint8_t * payload
) {
    // Validate node index
    if (nodeIndex >= FB_GATEWAY_MAX_NODES) {
        return;
    }

    word register_address = (word) payload[1] << 8 | (word) payload[2];
    
    char write_value[4] = { 0 };

    write_value[0] = payload[3];
    write_value[1] = payload[4];
    write_value[2] = payload[5];
    write_value[3] = payload[6];

    if (
        // Write address must be between <0, register.size()>
        register_address < _gateway_nodes_registers[nodeIndex].analog_outputs.size()
    ) {
        _gatewayRegistersWriteReceivedValue(nodeIndex, GATEWAY_REGISTER_AO, register_address, write_value);

        // DEBUG_MSG(PSTR("[INFO][GATEWAY][REGISTERS] Value was written into AO register\n"));

        gatewayCommunicationResetAttempts(nodeIndex);
        
    } else {
        DEBUG_MSG(PSTR("[ERR][GATEWAY][REGISTERS] Node is trying to write to undefined AO register address\n"));
    }
}

// -----------------------------------------------------------------------------

void _gatewayRegistersWriteMultipleAnalogOutputHandler(
    const uint8_t nodeIndex,
    uint8_t * payload
) {
    // Validate node index
    if (nodeIndex >= FB_GATEWAY_MAX_NODES) {
        return;
    }

    word register_address = (word) payload[1] << 8 | (word) payload[2];
    word read_length = (word) payload[3] << 8 | (word) payload[4];
    uint8_t bytes_length = (uint8_t) payload[5];

    if (
        // Write start address mus be between <0, register.size()>
        register_address < _gateway_nodes_registers[nodeIndex].analog_outputs.size()
        // Write length have to be same or smaller as registers size
        && (register_address + read_length) <= _gateway_nodes_registers[nodeIndex].analog_outputs.size()
    ) {
        // TODO: implement response handling

        gatewayCommunicationResetAttempts(nodeIndex);

    } else {
        DEBUG_MSG(PSTR("[ERR][GATEWAY][REGISTERS] Node is trying to write to undefined AO registers range\n"));
    }
}

// -----------------------------------------------------------------------------

bool _gatewayRegistersContinueInProcess(
    uint8_t nodeIndex
) {
    if (
        // Node have to be initialized
        gatewayInitializationIsNodeInitialized(nodeIndex) == true
        // Check if node is not lost
        && gatewayIsNodeLost(nodeIndex) == false
        // Check if node is ready to communicate
        && gatewayIsNodeReady(nodeIndex) == true
        // And have at leas one register
        && (
            _gateway_nodes_registers[nodeIndex].digital_inputs.size() > 0
            || _gateway_nodes_registers[nodeIndex].digital_outputs.size() > 0
            || _gateway_nodes_registers[nodeIndex].analog_inputs.size() > 0
            || _gateway_nodes_registers[nodeIndex].analog_outputs.size() > 0
            || _gateway_nodes_registers[nodeIndex].event_inputs.size() > 0
        )
    ) {
        if (
            _gateway_nodes_registers[nodeIndex].digital_inputs.size() > 0
            && (
                _gateway_nodes_registers_reading[nodeIndex].register_type == GATEWAY_REGISTER_DI
                || _gateway_nodes_registers_reading[nodeIndex].register_type == GATEWAY_REGISTER_NONE
            )
        ) {
            _gatewayRegistersRequestReadingMultipleDigitalInputRegisters(nodeIndex);

            return true;

        } else if (
            _gateway_nodes_registers[nodeIndex].digital_outputs.size() > 0
            && (
                _gateway_nodes_registers_reading[nodeIndex].register_type == GATEWAY_REGISTER_DO
                || _gateway_nodes_registers_reading[nodeIndex].register_type == GATEWAY_REGISTER_NONE
            )
        ) {
            _gatewayRegistersRequestReadingMultipleDigitalOutputRegisters(nodeIndex);

            return true;

        } else if (
            _gateway_nodes_registers[nodeIndex].analog_inputs.size() > 0
            && (
                _gateway_nodes_registers_reading[nodeIndex].register_type == GATEWAY_REGISTER_AI
                || _gateway_nodes_registers_reading[nodeIndex].register_type == GATEWAY_REGISTER_NONE
            )
        ) {
            _gatewayRegistersRequestReadingMultipleAnalogInputRegisters(nodeIndex);

            return true;

        } else if (
            _gateway_nodes_registers[nodeIndex].analog_outputs.size() > 0
            && (
                _gateway_nodes_registers_reading[nodeIndex].register_type == GATEWAY_REGISTER_AO
                || _gateway_nodes_registers_reading[nodeIndex].register_type == GATEWAY_REGISTER_NONE
            )
        ) {
            _gatewayRegistersRequestReadingMultipleAnalogOutputRegisters(nodeIndex);

            return true;

        } else if (
            _gateway_nodes_registers[nodeIndex].event_inputs.size() > 0
            && (
                _gateway_nodes_registers_reading[nodeIndex].register_type == GATEWAY_REGISTER_EV
                || _gateway_nodes_registers_reading[nodeIndex].register_type == GATEWAY_REGISTER_NONE
            )
        ) {
            _gatewayRegistersRequestReadingMultipleEventRegisters(nodeIndex);

            return true;
        }
    }

    return false;
}

// -----------------------------------------------------------------------------
// MODULE API
// -----------------------------------------------------------------------------

void gatewayRegistersReset(
    const uint8_t nodeIndex
) {
    // Validate node index
    if (nodeIndex >= FB_GATEWAY_MAX_NODES) {
        return;
    }

    // Reset registers
    _gateway_nodes_registers[nodeIndex].digital_inputs.clear();
    _gateway_nodes_registers[nodeIndex].digital_outputs.clear();
    _gateway_nodes_registers[nodeIndex].analog_inputs.clear();
    _gateway_nodes_registers[nodeIndex].analog_outputs.clear();
    _gateway_nodes_registers[nodeIndex].event_inputs.clear();
}

// -----------------------------------------------------------------------------

void gatewayRegistersResetReading(
    const uint8_t nodeIndex
) {
    // Validate node index
    if (nodeIndex >= FB_GATEWAY_MAX_NODES) {
        return;
    }

    _gateway_nodes_registers_reading[nodeIndex].register_type = GATEWAY_REGISTER_NONE;
    _gateway_nodes_registers_reading[nodeIndex].start = 0;
}

// -----------------------------------------------------------------------------

void gatewayRegistersInitialize(
    const uint8_t nodeIndex,
    const uint8_t dataRegister,
    const uint8_t size
) {
    // Validate node index
    if (nodeIndex >= FB_GATEWAY_MAX_NODES) {
        return;
    }

    switch (dataRegister)
    {
        case GATEWAY_REGISTER_DI:
            _gateway_nodes_registers[nodeIndex].digital_inputs.clear();

            for (uint8_t i = 0; i < size; i++) {
                _gateway_nodes_registers[nodeIndex].digital_inputs.push_back((gateway_register_t) {
                    GATEWAY_DATA_TYPE_BOOL,     // datatype
                    1                           // value size
                });
            }
            break;
            
        case GATEWAY_REGISTER_DO:
            _gateway_nodes_registers[nodeIndex].digital_outputs.clear();

            for (uint8_t i = 0; i < size; i++) {
                _gateway_nodes_registers[nodeIndex].digital_outputs.push_back((gateway_register_t) {
                    GATEWAY_DATA_TYPE_BOOL,     // datatype
                    1                           // value size
                });
            }
            break;

        case GATEWAY_REGISTER_AI:
            _gateway_nodes_registers[nodeIndex].analog_inputs.clear();

            for (uint8_t i = 0; i < size; i++) {
                _gateway_nodes_registers[nodeIndex].analog_inputs.push_back((gateway_register_t) {
                    GATEWAY_DATA_TYPE_UNKNOWN,  // datatype
                    0                           // value size
                });
            }
            break;
            
        case GATEWAY_REGISTER_AO:
            _gateway_nodes_registers[nodeIndex].analog_outputs.clear();

            for (uint8_t i = 0; i < size; i++) {
                _gateway_nodes_registers[nodeIndex].analog_outputs.push_back((gateway_register_t) {
                    GATEWAY_DATA_TYPE_UNKNOWN,  // datatype
                    0                           // value size
                });
            }
            break;
            
        case GATEWAY_REGISTER_EV:
            _gateway_nodes_registers[nodeIndex].event_inputs.clear();

            for (uint8_t i = 0; i < size; i++) {
                _gateway_nodes_registers[nodeIndex].event_inputs.push_back((gateway_register_t) {
                    GATEWAY_DATA_TYPE_UINT8,    // datatype
                    1                           // value size
                });
            }
            break;
    }
}

// -----------------------------------------------------------------------------

void gatewayRegistersInitializeDigitalInputs(
    const uint8_t nodeIndex,
    const uint8_t size
) {
    gatewayRegistersInitialize(nodeIndex, GATEWAY_REGISTER_DI, size);
}

// -----------------------------------------------------------------------------

void gatewayRegistersInitializeDigitalOutputs(
    const uint8_t nodeIndex,
    const uint8_t size
) {
    gatewayRegistersInitialize(nodeIndex, GATEWAY_REGISTER_DO, size);
}

// -----------------------------------------------------------------------------

void gatewayRegistersInitializeAnalogInputs(
    const uint8_t nodeIndex,
    const uint8_t size
) {
    gatewayRegistersInitialize(nodeIndex, GATEWAY_REGISTER_AI, size);
}

// -----------------------------------------------------------------------------

void gatewayRegistersInitializeAnalogOutputs(
    const uint8_t nodeIndex,
    const uint8_t size
) {
    gatewayRegistersInitialize(nodeIndex, GATEWAY_REGISTER_AO, size);
}

// -----------------------------------------------------------------------------

void gatewayRegistersInitializeEventInputs(
    const uint8_t nodeIndex,
    const uint8_t size
) {
    gatewayRegistersInitialize(nodeIndex, GATEWAY_REGISTER_EV, size);
}

// -----------------------------------------------------------------------------

uint8_t gatewayRegistersSize(
    const uint8_t nodeIndex,
    const uint8_t dataRegister
) {
    // Validate node index
    if (nodeIndex >= FB_GATEWAY_MAX_NODES) {
        return 0;
    }

    switch (dataRegister)
    {
        case GATEWAY_REGISTER_DI:
            return _gateway_nodes_registers[nodeIndex].digital_inputs.size();
            
        case GATEWAY_REGISTER_DO:
            return _gateway_nodes_registers[nodeIndex].digital_outputs.size();

        case GATEWAY_REGISTER_AI:
            return _gateway_nodes_registers[nodeIndex].analog_inputs.size();
            
        case GATEWAY_REGISTER_AO:
            return _gateway_nodes_registers[nodeIndex].analog_outputs.size();
            
        case GATEWAY_REGISTER_EV:
            return _gateway_nodes_registers[nodeIndex].event_inputs.size();
    }

    return 0;
}

// -----------------------------------------------------------------------------

uint8_t gatewayRegistersDigitalInputsSize(
    const uint8_t nodeIndex
) {
    return gatewayRegistersSize(nodeIndex, GATEWAY_REGISTER_DI);
}

// -----------------------------------------------------------------------------

uint8_t gatewayRegistersDigitalOutputsSize(
    const uint8_t nodeIndex
) {
    return gatewayRegistersSize(nodeIndex, GATEWAY_REGISTER_DO);
}

// -----------------------------------------------------------------------------

uint8_t gatewayRegistersAnalogInputsSize(
    const uint8_t nodeIndex
) {
    return gatewayRegistersSize(nodeIndex, GATEWAY_REGISTER_AI);
}

// -----------------------------------------------------------------------------

uint8_t gatewayRegistersAnalogOutputsSize(
    const uint8_t nodeIndex
) {
    return gatewayRegistersSize(nodeIndex, GATEWAY_REGISTER_AO);
}

// -----------------------------------------------------------------------------

uint8_t gatewayRegistersEventInputsSize(
    const uint8_t nodeIndex
) {
    return gatewayRegistersSize(nodeIndex, GATEWAY_REGISTER_EV);
}

// -----------------------------------------------------------------------------

void gatewayRegistersSetDataType(
    const uint8_t nodeIndex,
    const uint8_t dataRegister,
    const uint8_t address,
    const uint8_t datatype
) {
    // Validate node index
    if (nodeIndex >= FB_GATEWAY_MAX_NODES) {
        return;
    }

    uint8_t size = 0;

    switch (datatype)
    {
        case GATEWAY_DATA_TYPE_UINT8:
            size = 1;
            break;

        case GATEWAY_DATA_TYPE_INT8:
            size = 1;
            break;

        case GATEWAY_DATA_TYPE_UINT16:
            size = 2;
            break;

        case GATEWAY_DATA_TYPE_INT16:
            size = 2;
            break;

        case GATEWAY_DATA_TYPE_UINT32:
            size = 4;
            break;

        case GATEWAY_DATA_TYPE_INT32:
            size = 4;
            break;

        case GATEWAY_DATA_TYPE_FLOAT32:
            size = 4;
            break;

        default:
            return; 
    }

    // Check for valid datatype size    
    if (size == 0) {
        return;
    }

    switch (dataRegister)
    {
        case GATEWAY_REGISTER_DI:
            _gateway_nodes_registers[nodeIndex].digital_inputs[address].datatype = datatype;
            _gateway_nodes_registers[nodeIndex].digital_inputs[address].size = size;
            break;

        case GATEWAY_REGISTER_DO:
            _gateway_nodes_registers[nodeIndex].digital_outputs[address].datatype = datatype;
            _gateway_nodes_registers[nodeIndex].digital_outputs[address].size = size;
            break;

        case GATEWAY_REGISTER_AI:
            _gateway_nodes_registers[nodeIndex].analog_inputs[address].datatype = datatype;
            _gateway_nodes_registers[nodeIndex].analog_inputs[address].size = size;
            break;
            
        case GATEWAY_REGISTER_AO:
            _gateway_nodes_registers[nodeIndex].analog_outputs[address].datatype = datatype;
            _gateway_nodes_registers[nodeIndex].analog_outputs[address].size = size;
            break;
            
        case GATEWAY_REGISTER_EV:
            _gateway_nodes_registers[nodeIndex].event_inputs[address].datatype = datatype;
            _gateway_nodes_registers[nodeIndex].event_inputs[address].size = size;
            break;
    }
}

// -----------------------------------------------------------------------------

void gatewayRegistersSetAnalogInputsDataType(
    const uint8_t nodeIndex,
    const uint8_t address,
    const uint8_t datatype
) {
    gatewayRegistersSetDataType(nodeIndex, GATEWAY_REGISTER_AI, address, datatype);
}

// -----------------------------------------------------------------------------

void gatewayRegistersSetAnalogOutputsDataType(
    const uint8_t nodeIndex,
    const uint8_t address,
    const uint8_t datatype
) {
    gatewayRegistersSetDataType(nodeIndex, GATEWAY_REGISTER_AO, address, datatype);
}

// -----------------------------------------------------------------------------

void gatewayRegistersReadValue(
    const uint8_t nodeIndex,
    const uint8_t dataRegister,
    const uint8_t address,
    void * value,
    const uint8_t size
) {
    // Validate node index
    if (nodeIndex >= FB_GATEWAY_MAX_NODES) {
        return;
    }

    if (
        _gatewayRegistersIsAddressCorrect(nodeIndex, dataRegister, address)
        && _gatewayRegistersIsSizeCorrect(nodeIndex, dataRegister, address, size)
    ) {
        if (dataRegister == GATEWAY_REGISTER_DI) {
            memcpy(value, _gateway_nodes_registers[nodeIndex].digital_inputs[address].value, size);

        } else if (dataRegister == GATEWAY_REGISTER_DO) {
            memcpy(value, _gateway_nodes_registers[nodeIndex].digital_outputs[address].value, size);

        } else if (dataRegister == GATEWAY_REGISTER_AI) {
            memcpy(value, _gateway_nodes_registers[nodeIndex].analog_inputs[address].value, size);

        } else if (dataRegister == GATEWAY_REGISTER_AO) {
            memcpy(value, _gateway_nodes_registers[nodeIndex].analog_outputs[address].value, size);

        } else if (dataRegister == GATEWAY_REGISTER_EV) {
            memcpy(value, _gateway_nodes_registers[nodeIndex].event_inputs[address].value, size);
        }

        return;
    }

    char default_value[4] = { 0 };

    memcpy(value, default_value, size);
}

// -----------------------------------------------------------------------------

// Specialized convenience setters (these do not cost memory because of inlining)
void gatewayRegistersReadValue(const uint8_t nodeIndex, const uint8_t dataRegister, const uint8_t address, bool &value) { gatewayRegistersReadValue(nodeIndex, dataRegister, address, &value, 1); }
void gatewayRegistersReadValue(const uint8_t nodeIndex, const uint8_t dataRegister, const uint8_t address, uint8_t &value) { gatewayRegistersReadValue(nodeIndex, dataRegister, address, &value, 1); }
void gatewayRegistersReadValue(const uint8_t nodeIndex, const uint8_t dataRegister, const uint8_t address, uint16_t &value) { gatewayRegistersReadValue(nodeIndex, dataRegister, address, &value, 2); }
void gatewayRegistersReadValue(const uint8_t nodeIndex, const uint8_t dataRegister, const uint8_t address, uint32_t &value) { gatewayRegistersReadValue(nodeIndex, dataRegister, address, &value, 4); }
void gatewayRegistersReadValue(const uint8_t nodeIndex, const uint8_t dataRegister, const uint8_t address, int8_t &value) { gatewayRegistersReadValue(nodeIndex, dataRegister, address, &value, 1); }
void gatewayRegistersReadValue(const uint8_t nodeIndex, const uint8_t dataRegister, const uint8_t address, int16_t &value) { gatewayRegistersReadValue(nodeIndex, dataRegister, address, &value, 2); }
void gatewayRegistersReadValue(const uint8_t nodeIndex, const uint8_t dataRegister, const uint8_t address, int32_t &value) { gatewayRegistersReadValue(nodeIndex, dataRegister, address, &value, 4); }
void gatewayRegistersReadValue(const uint8_t nodeIndex, const uint8_t dataRegister, const uint8_t address, float &value) { gatewayRegistersReadValue(nodeIndex, dataRegister, address, &value, 4); }

// -----------------------------------------------------------------------------

bool gatewayRegistersHandler(
    const uint8_t packetId,
    const uint8_t address,
    uint8_t * payload,
    const uint8_t payloadLength
) {
    switch (packetId)
    {

    /**
     * DIGITAL REGISTERS READING
     */

        case GATEWAY_PACKET_READ_SINGLE_DI:
            // TODO: implement
            break;

        case GATEWAY_PACKET_READ_MULTI_DI:
            _gatewayRegistersReadMultipleRegisterAddressesHandler((address - 1), GATEWAY_REGISTER_DI, payload);
            break;

        case GATEWAY_PACKET_READ_SINGLE_DO:
            // TODO: implement
            break;

        case GATEWAY_PACKET_READ_MULTI_DO:
            _gatewayRegistersReadMultipleRegisterAddressesHandler((address - 1), GATEWAY_REGISTER_DO, payload);
            break;

    /**
     * ANALOG REGISTERS READING
     */

        case GATEWAY_PACKET_READ_SINGLE_AI:
            // TODO: implement
            break;

        case GATEWAY_PACKET_READ_MULTI_AI:
            _gatewayRegistersReadMultipleRegisterAddressesHandler((address - 1), GATEWAY_REGISTER_AI, payload);
            break;

        case GATEWAY_PACKET_READ_SINGLE_AO:
            // TODO: implement
            break;

        case GATEWAY_PACKET_READ_MULTI_AO:
            _gatewayRegistersReadMultipleRegisterAddressesHandler((address - 1), GATEWAY_REGISTER_AO, payload);
            break;

    /**
     * EVENT REGISTERS READING
     */

        case GATEWAY_PACKET_READ_SINGLE_EV:
            // TODO: implement
            break;

        case GATEWAY_PACKET_READ_MULTI_EV:
            _gatewayRegistersReadMultipleRegisterAddressesHandler((address - 1), GATEWAY_REGISTER_EV, payload);
            break;

    /**
     * DIGITAL REGISTERS WRITING
     */

        case GATEWAY_PACKET_WRITE_ONE_DO:
            _gatewayRegistersWriteSingleDigitalOutputHandler((address - 1), payload);
            break;

        case GATEWAY_PACKET_WRITE_MULTI_DO:
            _gatewayRegistersWriteMultipleDigitalOutputHandler((address - 1), payload);
            break;

    /**
     * ANALOG REGISTERS WRITING
     */

        case GATEWAY_PACKET_WRITE_ONE_AO:
            _gatewayRegistersWriteSingleAnalogOutputHandler((address - 1), payload);
            break;

        case GATEWAY_PACKET_WRITE_MULTI_AO:
            _gatewayRegistersWriteMultipleAnalogOutputHandler((address - 1), payload);
            break;

        default:
            return false;
    }

    return true;
}

// -----------------------------------------------------------------------------
// MODULE: SUB-MODULE CORE
// -----------------------------------------------------------------------------

bool gatewayRegistersLoop(
    const uint8_t nodeIndex
) {
    bool result = _gatewayRegistersContinueInProcess(nodeIndex);

    return result;
}

#endif // FB_GATEWAY_SUPPORT
