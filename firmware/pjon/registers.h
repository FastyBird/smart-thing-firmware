/*

NODES MODULE - REGISTERS

Copyright (C) 2018 FastyBird Ltd. <info@fastybird.com>

*/

#if NODES_GATEWAY_SUPPORT

// -----------------------------------------------------------------------------
// REGISTERS UTILS
// -----------------------------------------------------------------------------

uint8_t _gatewayRegistersGetDataType(
    const uint8_t id,
    const uint8_t dataRegister,
    const uint8_t address
) {
    switch (dataRegister)
    {
        case GATEWAY_REGISTER_DI:
            return _gateway_nodes[id].digital_inputs[address].data_type;

        case GATEWAY_REGISTER_DO:
            return _gateway_nodes[id].digital_outputs[address].data_type;

        case GATEWAY_REGISTER_AI:
            return _gateway_nodes[id].analog_inputs[address].data_type;

        case GATEWAY_REGISTER_AO:
            return _gateway_nodes[id].analog_outputs[address].data_type;
    }

    return 0;
}
// -----------------------------------------------------------------------------

bool _gatewayRegisterValueUpdated(
    const uint8_t id,
    const uint8_t dataRegister,
    const uint8_t address,
    const void * storedValue,
    const void * writtenValue
) {
    // TODO: finish notification after value change
}

// -----------------------------------------------------------------------------

// Specialized convenience setters (these do not cost memory because of inlining)
void _gatewayRegisterValueUpdated(const uint8_t id, const uint8_t dataRegister, const uint8_t address, const bool storedValue, const bool writtenValue) {  _gatewayRegisterValueUpdated(id, dataRegister, address, storedValue, writtenValue); }
void _gatewayRegisterValueUpdated(const uint8_t id, const uint8_t dataRegister, const uint8_t address, const uint8_t storedValue, const uint8_t writtenValue) {  _gatewayRegisterValueUpdated(id, dataRegister, address, storedValue, writtenValue); }
void _gatewayRegisterValueUpdated(const uint8_t id, const uint8_t dataRegister, const uint8_t address, const uint16_t storedValue, const uint16_t writtenValue) {  _gatewayRegisterValueUpdated(id, dataRegister, address, storedValue, writtenValue); }
void _gatewayRegisterValueUpdated(const uint8_t id, const uint8_t dataRegister, const uint8_t address, const uint32_t storedValue, const uint32_t writtenValue) {  _gatewayRegisterValueUpdated(id, dataRegister, address, storedValue, writtenValue); }
void _gatewayRegisterValueUpdated(const uint8_t id, const uint8_t dataRegister, const uint8_t address, const int8_t storedValue, const int8_t writtenValue) {  _gatewayRegisterValueUpdated(id, dataRegister, address, storedValue, writtenValue); }
void _gatewayRegisterValueUpdated(const uint8_t id, const uint8_t dataRegister, const uint8_t address, const int16_t storedValue, const int16_t writtenValue) {  _gatewayRegisterValueUpdated(id, dataRegister, address, storedValue, writtenValue); }
void _gatewayRegisterValueUpdated(const uint8_t id, const uint8_t dataRegister, const uint8_t address, const int32_t storedValue, const int32_t writtenValue) {  _gatewayRegisterValueUpdated(id, dataRegister, address, storedValue, writtenValue); }
void _gatewayRegisterValueUpdated(const uint8_t id, const uint8_t dataRegister, const uint8_t address, const float storedValue, const float writtenValue) {  _gatewayRegisterValueUpdated(id, dataRegister, address, storedValue, writtenValue); }

// -----------------------------------------------------------------------------
// REGISTERS READ & WRITE
// -----------------------------------------------------------------------------

bool _gatewayRegistersIsAddressCorrect(
    const uint8_t id,
    const uint8_t dataRegister,
    const uint8_t address
) {
    if (dataRegister == GATEWAY_REGISTER_DI) {
        return (address < _gateway_nodes[id].registers_size[dataRegister]);

    } else if (dataRegister == GATEWAY_REGISTER_DO) {
        return (address < _gateway_nodes[id].registers_size[dataRegister]);

    } else if (dataRegister == GATEWAY_REGISTER_AI) {
        return (address < _gateway_nodes[id].registers_size[dataRegister]);

    } else if (dataRegister == GATEWAY_REGISTER_AO) {
        return (address < _gateway_nodes[id].registers_size[dataRegister]);
    }

    return false;
}

// -----------------------------------------------------------------------------

bool _gatewayRegistersIsSizeCorrect(
    const uint8_t id,
    const uint8_t dataRegister,
    const uint8_t address,
    const uint8_t size
) {
    if (dataRegister == GATEWAY_REGISTER_DI) {
        return (_gateway_nodes[id].digital_inputs[address].size == size);

    } else if (dataRegister == GATEWAY_REGISTER_DO) {
        return (_gateway_nodes[id].digital_outputs[address].size == size);

    } else if (dataRegister == GATEWAY_REGISTER_AI) {
        return (_gateway_nodes[id].analog_inputs[address].size == size);

    } else if (dataRegister == GATEWAY_REGISTER_AO) {
        return (_gateway_nodes[id].analog_outputs[address].size == size);
    }

    return false;
}

// -----------------------------------------------------------------------------

bool _gatewayWriteRegisterValue(
    const uint8_t id,
    const uint8_t dataRegister,
    const uint8_t address,
    const void * value,
    const uint8_t size
) {
    if (_gatewayRegistersIsAddressCorrect(id, dataRegister, address) == false) {
        return false;
    }

    if (_gatewayRegistersIsSizeCorrect(id, dataRegister, address, size)) {
        if (dataRegister == GATEWAY_REGISTER_DI) {
            memcpy(_gateway_nodes[id].digital_inputs[address].value, value, size);

        } else if (dataRegister == GATEWAY_REGISTER_DO) {
            memcpy(_gateway_nodes[id].digital_outputs[address].value, value, size);

        } else if (dataRegister == GATEWAY_REGISTER_AI) {
            memcpy(_gateway_nodes[id].analog_inputs[address].value, value, size);

        } else if (dataRegister == GATEWAY_REGISTER_AO) {
            memcpy(_gateway_nodes[id].analog_outputs[address].value, value, size);
        }
    }

    return true;
}

// -----------------------------------------------------------------------------

// Specialized convenience setters (these do not cost memory because of inlining)
bool _gatewayWriteRegisterValue(const uint8_t id, const uint8_t dataRegister, const uint8_t address, const bool value) { return _gatewayWriteRegisterValue(id, dataRegister, address, &value, 1); }
bool _gatewayWriteRegisterValue(const uint8_t id, const uint8_t dataRegister, const uint8_t address, const uint8_t value) { return _gatewayWriteRegisterValue(id, dataRegister, address, &value, 1); }
bool _gatewayWriteRegisterValue(const uint8_t id, const uint8_t dataRegister, const uint8_t address, const uint16_t value) { return _gatewayWriteRegisterValue(id, dataRegister, address, &value, 2); }
bool _gatewayWriteRegisterValue(const uint8_t id, const uint8_t dataRegister, const uint8_t address, const uint32_t value) { return _gatewayWriteRegisterValue(id, dataRegister, address, &value, 4); }
bool _gatewayWriteRegisterValue(const uint8_t id, const uint8_t dataRegister, const uint8_t address, const int8_t value) { return _gatewayWriteRegisterValue(id, dataRegister, address, &value, 1); }
bool _gatewayWriteRegisterValue(const uint8_t id, const uint8_t dataRegister, const uint8_t address, const int16_t value) { return _gatewayWriteRegisterValue(id, dataRegister, address, &value, 2); }
bool _gatewayWriteRegisterValue(const uint8_t id, const uint8_t dataRegister, const uint8_t address, const int32_t value) { return _gatewayWriteRegisterValue(id, dataRegister, address, &value, 4); }
bool _gatewayWriteRegisterValue(const uint8_t id, const uint8_t dataRegister, const uint8_t address, const float value) { return _gatewayWriteRegisterValue(id, dataRegister, address, &value, 4); }

// -----------------------------------------------------------------------------

void _gatewayReadRegisterValue(
    const uint8_t id,
    const uint8_t dataRegister,
    const uint8_t address,
    void * value,
    const uint8_t size
) {
    if (
        _gatewayRegistersIsAddressCorrect(id, dataRegister, address)
        && _gatewayRegistersIsSizeCorrect(id, dataRegister, address, size)
    ) {
        if (dataRegister == GATEWAY_REGISTER_DI) {
            memcpy(value, _gateway_nodes[id].digital_inputs[address].value, size);

        } else if (dataRegister == GATEWAY_REGISTER_DO) {
            memcpy(value, _gateway_nodes[id].digital_outputs[address].value, size);

        } else if (dataRegister == GATEWAY_REGISTER_AI) {
            memcpy(value, _gateway_nodes[id].analog_inputs[address].value, size);

        } else if (dataRegister == GATEWAY_REGISTER_AO) {
            memcpy(value, _gateway_nodes[id].analog_outputs[address].value, size);
        }

        return;
    }

    char default_value[4] = { 0 };

    memcpy(value, default_value, size);
}

// -----------------------------------------------------------------------------

// Specialized convenience setters (these do not cost memory because of inlining)
void _gatewayReadRegisterValue(const uint8_t id, const uint8_t dataRegister, const uint8_t address, bool &value) { _gatewayReadRegisterValue(id, dataRegister, address, &value, 1); }
void _gatewayReadRegisterValue(const uint8_t id, const uint8_t dataRegister, const uint8_t address, uint8_t &value) { _gatewayReadRegisterValue(id, dataRegister, address, &value, 1); }
void _gatewayReadRegisterValue(const uint8_t id, const uint8_t dataRegister, const uint8_t address, uint16_t &value) { _gatewayReadRegisterValue(id, dataRegister, address, &value, 2); }
void _gatewayReadRegisterValue(const uint8_t id, const uint8_t dataRegister, const uint8_t address, uint32_t &value) { _gatewayReadRegisterValue(id, dataRegister, address, &value, 4); }
void _gatewayReadRegisterValue(const uint8_t id, const uint8_t dataRegister, const uint8_t address, int8_t &value) { _gatewayReadRegisterValue(id, dataRegister, address, &value, 1); }
void _gatewayReadRegisterValue(const uint8_t id, const uint8_t dataRegister, const uint8_t address, int16_t &value) { _gatewayReadRegisterValue(id, dataRegister, address, &value, 2); }
void _gatewayReadRegisterValue(const uint8_t id, const uint8_t dataRegister, const uint8_t address, int32_t &value) { _gatewayReadRegisterValue(id, dataRegister, address, &value, 4); }
void _gatewayReadRegisterValue(const uint8_t id, const uint8_t dataRegister, const uint8_t address, float &value) { _gatewayReadRegisterValue(id, dataRegister, address, &value, 4); }

// -----------------------------------------------------------------------------

/**
 * Transform received 4 bytes value into register value
 */
void _gatewayRegistersWriteValueFromTransfer(
    const uint8_t id,
    const uint8_t dataRegister,
    const uint8_t address,
    char * value
) {
    switch (_gatewayRegistersGetDataType(id, dataRegister, address))
    {
        case GATEWAY_DATA_TYPE_BOOLEAN:
            bool bool_stored_value;
            BOOL_UNION_t bool_write_value;

            bool_write_value.bytes[0] = value[0];
            bool_write_value.bytes[1] = value[1];
            bool_write_value.bytes[2] = value[2];
            bool_write_value.bytes[3] = value[3];

            _gatewayReadRegisterValue(id, dataRegister, address, bool_stored_value);

            if (bool_stored_value != bool_write_value.number) {
                _gatewayWriteRegisterValue(id, dataRegister, address, bool_write_value.number);

                _gatewayRegisterValueUpdated(id, dataRegister, address, bool_stored_value, bool_write_value.number);


                DEBUG_MSG(PSTR("[GATEWAY] Value was written into register\n"));

            } else {
                DEBUG_MSG(PSTR("[GATEWAY] Value to write into register is same as stored. Write skipped\n"));
            }
            break;

        case GATEWAY_DATA_TYPE_UINT8:
            uint8_t uint8_stored_value;
            UINT8_UNION_t uint8_write_value;

            uint8_write_value.bytes[0] = value[0];
            uint8_write_value.bytes[1] = value[1];
            uint8_write_value.bytes[2] = value[2];
            uint8_write_value.bytes[3] = value[3];

            _gatewayReadRegisterValue(id, dataRegister, address, uint8_stored_value);

            if (uint8_stored_value != uint8_write_value.number) {
                _gatewayWriteRegisterValue(id, dataRegister, address, uint8_write_value.number);

                _gatewayRegisterValueUpdated(id, dataRegister, address, uint8_stored_value, uint8_write_value.number);


                DEBUG_MSG(PSTR("[GATEWAY] Value was written into register\n"));

            } else {
                DEBUG_MSG(PSTR("[GATEWAY] Value to write into register is same as stored. Write skipped\n"));
            }
            break;

        case GATEWAY_DATA_TYPE_UINT16:
            uint16_t uint16_stored_value;
            UINT16_UNION_t uint16_write_value;

            uint16_write_value.bytes[0] = value[0];
            uint16_write_value.bytes[1] = value[1];
            uint16_write_value.bytes[2] = value[2];
            uint16_write_value.bytes[3] = value[3];

            _gatewayReadRegisterValue(id, dataRegister, address, uint16_stored_value);

            if (uint16_stored_value != uint16_write_value.number) {
                _gatewayWriteRegisterValue(id, dataRegister, address, uint16_write_value.number);

                _gatewayRegisterValueUpdated(id, dataRegister, address, uint16_stored_value, uint16_write_value.number);

                DEBUG_MSG(PSTR("[GATEWAY] Value was written into register\n"));

            } else {
                DEBUG_MSG(PSTR("[GATEWAY] Value to write into register is same as stored. Write skipped\n"));
            }
            break;

        case GATEWAY_DATA_TYPE_UINT32:
            uint32_t uint32_stored_value;
            UINT32_UNION_t uint32_write_value;

            uint32_write_value.bytes[0] = value[0];
            uint32_write_value.bytes[1] = value[1];
            uint32_write_value.bytes[2] = value[2];
            uint32_write_value.bytes[3] = value[3];

            _gatewayReadRegisterValue(id, dataRegister, address, uint32_stored_value);

            if (uint32_stored_value != uint32_write_value.number) {
                _gatewayWriteRegisterValue(id, dataRegister, address, uint32_write_value.number);

                _gatewayRegisterValueUpdated(id, dataRegister, address, uint32_stored_value, uint32_write_value.number);

                DEBUG_MSG(PSTR("[GATEWAY] Value was written into register\n"));

            } else {
                DEBUG_MSG(PSTR("[GATEWAY] Value to write into register is same as stored. Write skipped\n"));
            }
            break;

        case GATEWAY_DATA_TYPE_INT8:
            int8_t int8_stored_value;
            INT8_UNION_t int8_write_value;

            int8_write_value.bytes[0] = value[0];
            int8_write_value.bytes[1] = value[1];
            int8_write_value.bytes[2] = value[2];
            int8_write_value.bytes[3] = value[3];

            _gatewayReadRegisterValue(id, dataRegister, address, int8_stored_value);

            if (int8_stored_value != int8_write_value.number) {
                _gatewayWriteRegisterValue(id, dataRegister, address, int8_write_value.number);

                _gatewayRegisterValueUpdated(id, dataRegister, address, int8_stored_value, int8_write_value.number);

                DEBUG_MSG(PSTR("[GATEWAY] Value was written into register\n"));

            } else {
                DEBUG_MSG(PSTR("[GATEWAY] Value to write into register is same as stored. Write skipped\n"));
            }
            break;

        case GATEWAY_DATA_TYPE_INT16:
            int16_t int16_stored_value;
            INT16_UNION_t int16_write_value;

            int16_write_value.bytes[0] = value[0];
            int16_write_value.bytes[1] = value[1];
            int16_write_value.bytes[2] = value[2];
            int16_write_value.bytes[3] = value[3];

            _gatewayReadRegisterValue(id, dataRegister, address, int16_stored_value);

            if (int16_stored_value != int16_write_value.number) {
                _gatewayWriteRegisterValue(id, dataRegister, address, int16_write_value.number);

                _gatewayRegisterValueUpdated(id, dataRegister, address, int16_stored_value, int16_write_value.number);

                DEBUG_MSG(PSTR("[GATEWAY] Value was written into register\n"));

            } else {
                DEBUG_MSG(PSTR("[GATEWAY] Value to write into register is same as stored. Write skipped\n"));
            }
            break;

        case GATEWAY_DATA_TYPE_INT32:
            int32_t int32_stored_value;
            INT32_UNION_t int32_write_value;

            int32_write_value.bytes[0] = value[0];
            int32_write_value.bytes[1] = value[1];
            int32_write_value.bytes[2] = value[2];
            int32_write_value.bytes[3] = value[3];

            _gatewayReadRegisterValue(id, dataRegister, address, int32_stored_value);

            if (int32_stored_value != int32_write_value.number) {
                _gatewayWriteRegisterValue(id, dataRegister, address, int32_write_value.number);

                _gatewayRegisterValueUpdated(id, dataRegister, address, int32_stored_value, int32_write_value.number);

                DEBUG_MSG(PSTR("[GATEWAY] Value was written into register\n"));

            } else {
                DEBUG_MSG(PSTR("[GATEWAY] Value to write into register is same as stored. Write skipped\n"));
            }
            break;

        case GATEWAY_DATA_TYPE_FLOAT32:
            float float_stored_value;
            FLOAT32_UNION_t float_write_value;

            float_write_value.bytes[0] = value[0];
            float_write_value.bytes[1] = value[1];
            float_write_value.bytes[2] = value[2];
            float_write_value.bytes[3] = value[3];

            _gatewayReadRegisterValue(id, dataRegister, address, float_stored_value);

            if (float_stored_value != float_write_value.number) {
                _gatewayWriteRegisterValue(id, dataRegister, address, float_write_value.number);

                _gatewayRegisterValueUpdated(id, dataRegister, address, float_stored_value, float_write_value.number);

                DEBUG_MSG(PSTR("[GATEWAY] Value was written into register\n"));

            } else {
                DEBUG_MSG(PSTR("[GATEWAY] Value to write into register is same as stored. Write skipped\n"));
            }
            break;

        default:
            DEBUG_MSG(PSTR("[GATEWAY][ERR] Provided unknown data type for writing into register\n"));
            break;
    }
}

// -----------------------------------------------------------------------------
// DIGITAL REGISTERS READING
// -----------------------------------------------------------------------------

uint8_t _gatewayRegistersRequestReadingDigitalRegisters(
    const uint8_t packetId,
    const uint8_t id,
    uint8_t start,
    const uint8_t registerSize
) {
    char output_content[5];

    output_content[0] = packetId;

    // Register reading start address
    output_content[1] = (char) (start >> 8);
    output_content[2] = (char) (start & 0xFF);

    // It is based on maximum packed size reduced by packet header (5 bytes)
    uint8_t max_readable_addresses = (_gateway_nodes[id].packet.max_length - 5) * 8;

    // Node has less digital registers than one packet can handle
    if (registerSize <= max_readable_addresses) {
        // Register reading lenght
        output_content[3] = (char) (registerSize >> 8);
        output_content[4] = (char) (registerSize & 0xFF);

    // Node has more digital registers than one packet can handle
    } else {
        if (start + max_readable_addresses <= registerSize) {
            // Register reading lenght
            output_content[3] = (char) (max_readable_addresses >> 8);
            output_content[4] = (char) (max_readable_addresses & 0xFF);

            if (start + max_readable_addresses < registerSize) {
                // Move pointer
                start = start + max_readable_addresses;

            } else {
                // Move pointer
                start = 0;
            }

        } else {
            // Register reading lenght
            output_content[3] = (char) ((registerSize - start) >> 8);
            output_content[4] = (char) ((registerSize - start) & 0xFF);

            // Move pointer
            start = 0;
        }
    }

    // Record requested packet
    if (_gatewaySendPacket((id + 1), output_content, 5)) {
        _gateway_nodes[id].packet.waiting_for = packetId;
        _gateway_nodes[id].packet.sending_time = millis();
    }

    return start;
}

// -----------------------------------------------------------------------------

void _gatewayRequestReadingMultipleDigitalInputsRegisters(
    const uint8_t id
) {
    uint8_t set_start = _gatewayRegistersRequestReadingDigitalRegisters(
        GATEWAY_PACKET_READ_MULTI_DI,
        id,
        _gateway_nodes[id].digital_inputs_reading.start,
        _gateway_nodes[id].registers_size[GATEWAY_REGISTER_DI]
    );

    _gateway_nodes[id].digital_inputs_reading.start = set_start;

    if (set_start == 0) {
        _gateway_nodes[id].digital_inputs_reading.delay = millis();
    }
}

// -----------------------------------------------------------------------------

void _gatewayRequestReadingMultipleDigitalOutputsRegisters(
    const uint8_t id
) {
    uint8_t set_start = _gatewayRegistersRequestReadingDigitalRegisters(
        GATEWAY_PACKET_READ_MULTI_DO,
        id,
        _gateway_nodes[id].digital_outputs_reading.start,
        _gateway_nodes[id].registers_size[GATEWAY_REGISTER_DO]
    );

    _gateway_nodes[id].digital_outputs_reading.start = set_start;

    if (set_start == 0) {
        _gateway_nodes[id].digital_outputs_reading.delay = millis();
    }
}

// -----------------------------------------------------------------------------
// ANALOG REGISTERS READING
// -----------------------------------------------------------------------------

uint8_t _gatewayRegistersRequestReadingAnalogRegisters(
    const uint8_t packetId,
    const uint8_t id,
    uint8_t start,
    const uint8_t registerSize
) {
    char output_content[5];

    output_content[0] = packetId;

    // Register reading start address
    output_content[1] = (char) (start >> 8);
    output_content[2] = (char) (start & 0xFF);

    // It is based on maximum packed size reduced by packet header (5 bytes)
    uint8_t max_readable_addresses = (uint8_t) ((_gateway_nodes[id].packet.max_length - 5) / 4);

    // Node has less digital registers than one packet can handle
    if (registerSize <= max_readable_addresses) {
        // Register reading lenght
        output_content[3] = (char) (registerSize >> 8);
        output_content[4] = (char) (registerSize & 0xFF);

    // Node has more digital registers than one packet can handle
    } else {
        if (start + max_readable_addresses <= registerSize) {
            // Register reading lenght
            output_content[3] = (char) (max_readable_addresses >> 8);
            output_content[4] = (char) (max_readable_addresses & 0xFF);

            if (start + max_readable_addresses < registerSize) {
                // Move pointer
                start = start + max_readable_addresses;

            } else {
                // Move pointer
                start = 0;
            }

        } else {
            // Register reading lenght
            output_content[3] = (char) ((registerSize - start) >> 8);
            output_content[4] = (char) ((registerSize - start) & 0xFF);

            // Move pointer
            start = 0;
        }
    }

    // Record requested packet
    if (_gatewaySendPacket((id + 1), output_content, 5)) {
        _gateway_nodes[id].packet.waiting_for = packetId;
        _gateway_nodes[id].packet.sending_time = millis();
    }

    return start;
}

// -----------------------------------------------------------------------------

void _gatewayRequestReadingMultipleAnalogInputsRegisters(
    const uint8_t id
) {
    uint8_t set_start = _gatewayRegistersRequestReadingAnalogRegisters(
        GATEWAY_PACKET_READ_MULTI_AI,
        id,
        _gateway_nodes[id].analog_inputs_reading.start,
        _gateway_nodes[id].registers_size[GATEWAY_REGISTER_AI]
    );

    _gateway_nodes[id].analog_inputs_reading.start = set_start;

    if (set_start == 0) {
        _gateway_nodes[id].analog_inputs_reading.delay = millis();
    }
}

// -----------------------------------------------------------------------------

void _gatewayRequestReadingMultipleAnalogOutputsRegisters(
    const uint8_t id
) {
    uint8_t set_start = _gatewayRegistersRequestReadingAnalogRegisters(
        GATEWAY_PACKET_READ_MULTI_AO,
        id,
        _gateway_nodes[id].analog_outputs_reading.start,
        _gateway_nodes[id].registers_size[GATEWAY_REGISTER_AO]
    );

    _gateway_nodes[id].analog_outputs_reading.start = set_start;

    if (set_start == 0) {
        _gateway_nodes[id].analog_outputs_reading.delay = millis();
    }
}

// -----------------------------------------------------------------------------
// DIGITAL REGISTERS WRITING (only for DO registers)
// -----------------------------------------------------------------------------

void _gatewayRequestWritingSingleDigitalOutputRegister(
    const uint8_t id,
    const uint8_t registerAddress,
    const bool value
) {
    char output_content[5];

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
    if (_gatewaySendPacket((id + 1), output_content, 5)) {
        _gateway_nodes[id].packet.waiting_for = GATEWAY_PACKET_WRITE_ONE_DO;
        _gateway_nodes[id].packet.sending_time = millis();
    }
}

// -----------------------------------------------------------------------------
// ANALOG REGISTERS WRITING (only for AO registers)
// -----------------------------------------------------------------------------

void _gatewayRequestWritingSingleAnalogOutputRegister(
    const uint8_t id,
    const uint8_t registerAddress,
    void * value,
    const uint8_t size
) {
    char output_content[6];

    output_content[0] = GATEWAY_PACKET_WRITE_ONE_DO;

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
    if (_gatewaySendPacket((id + 1), output_content, 6)) {
        _gateway_nodes[id].packet.waiting_for = GATEWAY_PACKET_WRITE_ONE_AO;
        _gateway_nodes[id].packet.sending_time = millis();
    }
}

// -----------------------------------------------------------------------------

// Specialized convenience setters (these do not cost memory because of inlining)
void _gatewayRequestWritingSingleAnalogOutputRegister(const uint8_t id, const uint8_t registerAddress, uint8_t value) { _gatewayRequestWritingSingleAnalogOutputRegister(id, registerAddress, &value, 1); }
void _gatewayRequestWritingSingleAnalogOutputRegister(const uint8_t id, const uint8_t registerAddress, uint16_t value) { _gatewayRequestWritingSingleAnalogOutputRegister(id, registerAddress, &value, 2); }
void _gatewayRequestWritingSingleAnalogOutputRegister(const uint8_t id, const uint8_t registerAddress, uint32_t value) { _gatewayRequestWritingSingleAnalogOutputRegister(id, registerAddress, &value, 4); }
void _gatewayRequestWritingSingleAnalogOutputRegister(const uint8_t id, const uint8_t registerAddress, int8_t value) { _gatewayRequestWritingSingleAnalogOutputRegister(id, registerAddress, &value, 1); }
void _gatewayRequestWritingSingleAnalogOutputRegister(const uint8_t id, const uint8_t registerAddress, int16_t value) { _gatewayRequestWritingSingleAnalogOutputRegister(id, registerAddress, &value, 2); }
void _gatewayRequestWritingSingleAnalogOutputRegister(const uint8_t id, const uint8_t registerAddress, int32_t value) { _gatewayRequestWritingSingleAnalogOutputRegister(id, registerAddress, &value, 4); }
void _gatewayRequestWritingSingleAnalogOutputRegister(const uint8_t id, const uint8_t registerAddress, float value) { _gatewayRequestWritingSingleAnalogOutputRegister(id, registerAddress, &value, 4); }

// -----------------------------------------------------------------------------
// READING HANDLERS 
// -----------------------------------------------------------------------------

void _gatewayReadMultipleDigitalRegisterHandler(
    const uint8_t id,
    const uint8_t dataRegister,
    uint8_t * payload
) {
    word start_address = (word) payload[1] << 8 | (word) payload[2];

    uint8_t bytes_length = (uint8_t) payload[3];

    DEBUG_MSG(
        PSTR("[GATEWAY] Received reading response from node: %d to register: %d at address: %d with byte length: %d\n"),
        id,
        dataRegister,
        start_address,
        bytes_length
    );

    if (
        // Read start address must be between <0, buffer.size()>
        start_address < _gateway_nodes[id].registers_size[dataRegister]
    ) {
        uint8_t write_byte = 1;
        uint8_t data_byte;

        uint8_t write_address = start_address;

        while (
            write_address < _gateway_nodes[id].registers_size[dataRegister]
            && write_byte <= bytes_length
        ) {
            data_byte = (uint8_t) payload[3 + write_byte];

            bool write_value = false;
            bool stored_value;

            for (uint8_t i = 0; i < 8; i++) {
                write_value = (data_byte >> i) & 0x01 ? true : false;

                _gatewayReadRegisterValue(id, dataRegister, write_address, stored_value);

                if (stored_value != write_value) {
                    _gatewayWriteRegisterValue(id, dataRegister, write_address, write_value);

                    _gatewayRegisterValueUpdated(id, dataRegister, write_address, stored_value, write_value);

                    DEBUG_MSG(PSTR("[GATEWAY] Value was written into digital register at address: %d\n"), write_address);

                } else {
                    DEBUG_MSG(PSTR("[GATEWAY] Value to write into digital register at address: %d is same as stored. Write skipped\n"), write_address);
                }

                write_address++;

                if (write_address >= _gateway_nodes[id].registers_size[dataRegister]) {
                    break;
                }
            }

            write_byte++;
        }

    } else {
        DEBUG_MSG(PSTR("[GATEWAY][ERR] Node is trying to write to undefined digital register address\n"));
    }

    _gateway_nodes[id].packet.waiting_for = GATEWAY_PACKET_NONE;
    _gateway_nodes[id].packet.sending_time = 0;
}

// -----------------------------------------------------------------------------

void _gatewayReadMultipleAnalogRegisterHandler(
    const uint8_t id,
    const uint8_t dataRegister,
    uint8_t * payload
) {
    word start_address = (word) payload[1] << 8 | (word) payload[2];

    uint8_t bytes_length = (uint8_t) payload[3];

    DEBUG_MSG(
        PSTR("[GATEWAY] Received reading response from node: %d to register: %d at address: %d with byte length: %d\n"),
        id,
        dataRegister,
        start_address,
        bytes_length
    );

    if (
        // Read start address must be between <0, buffer.size()>
        start_address < _gateway_nodes[id].registers_size[dataRegister]
    ) {
        uint8_t write_byte = 4;
        uint8_t data_byte;

        uint8_t write_address = start_address;

        char write_value[4] = { 0 };

        while (
            write_address < _gateway_nodes[id].registers_size[dataRegister]
            && write_byte <= bytes_length
        ) {
            // Clear buffer
            memset(write_value, 0, sizeof(write_value));

            write_value[0] = payload[write_byte];
            write_value[1] = payload[write_byte + 1];
            write_value[2] = payload[write_byte + 2];
            write_value[3] = payload[write_byte + 3];

            _gatewayRegistersWriteValueFromTransfer(id, dataRegister, write_address, write_value);

            write_byte = write_byte + 4;
            write_address++;
        }

    } else {
        DEBUG_MSG(PSTR("[GATEWAY][ERR] Node is trying to write to undefined analog register address\n"));
    }

    _gateway_nodes[id].packet.waiting_for = GATEWAY_PACKET_NONE;
    _gateway_nodes[id].packet.sending_time = 0;
}

// -----------------------------------------------------------------------------
// WRITING HANDLERS
// -----------------------------------------------------------------------------

void _gatewayWriteOneDigitalOutputHandler(
    const uint8_t id,
    uint8_t * payload
) {
    word register_address = (word) payload[1] << 8 | (word) payload[2];
    word write_value = (word) payload[3] << 8 | (word) payload[4];

    // Check if value is TRUE|FALSE or 1|0
    if (write_value != 0xFF00 && write_value != 0x0000) {
        DEBUG_MSG(PSTR("[GATEWAY][ERR] Digital register accept only bool value\n"));
        return;
    }

    if (
        // Write address must be between <0, buffer.size()>
        register_address < _gateway_nodes[id].registers_size[GATEWAY_REGISTER_DO]
    ) {
        bool stored_value;

        _gatewayReadRegisterValue(id, GATEWAY_REGISTER_DO, register_address, stored_value);

        if (stored_value != (bool) write_value) {
            _gatewayWriteRegisterValue(id, GATEWAY_REGISTER_DO, register_address, (bool) write_value);

            _gatewayRegisterValueUpdated(id, GATEWAY_REGISTER_DO, register_address, stored_value, (bool) write_value);

            DEBUG_MSG(PSTR("[GATEWAY] Value was written into DO register\n"));

        } else {
            DEBUG_MSG(PSTR("[GATEWAY] Value to write into DO register is same as stored. Write skipped\n"));
        }

    } else {
        DEBUG_MSG(PSTR("[GATEWAY][ERR] Node is trying to write to undefined DO register address\n"));
    }

    _gateway_nodes[id].packet.waiting_for = GATEWAY_PACKET_NONE;
    _gateway_nodes[id].packet.sending_time = 0;
}

// -----------------------------------------------------------------------------

void _gatewayWriteMultipleDigitalOutputsHandler(
    const uint8_t id,
    uint8_t * payload
) {
    word register_address = (word) payload[1] << 8 | (word) payload[2];
    word read_length = (word) payload[3] << 8 | (word) payload[4];

    uint8_t bytes_length = (uint8_t) payload[5];

    if (
        // Write start address mus be between <0, buffer.size()>
        register_address < _gateway_nodes[id].registers_size[GATEWAY_REGISTER_DO]
        // Write length have to be same or smaller as registers size
        && (register_address + read_length) <= _gateway_nodes[id].registers_size[GATEWAY_REGISTER_DO]
    ) {
        // Nothing to do here. Multiple DO register were successfully written

    } else {
        DEBUG_MSG(PSTR("[GATEWAY][ERR] Node is trying to write to undefined DO registers range\n"));
    }

    _gateway_nodes[id].packet.waiting_for = GATEWAY_PACKET_NONE;
    _gateway_nodes[id].packet.sending_time = 0;
}

// -----------------------------------------------------------------------------

void _gatewayWriteOneAnalogOutputHandler(
    const uint8_t id,
    uint8_t * payload
) {
    word register_address = (word) payload[1] << 8 | (word) payload[2];
    
    char write_value[4] = { 0 };

    write_value[0] = payload[3];
    write_value[1] = payload[4];
    write_value[2] = payload[5];
    write_value[3] = payload[6];

    if (
        // Write address must be between <0, buffer.size()>
        register_address < _gateway_nodes[id].registers_size[GATEWAY_REGISTER_AO]
    ) {
        _gatewayRegistersWriteValueFromTransfer(id, GATEWAY_REGISTER_AO, register_address, write_value);

    } else {
        DEBUG_MSG(PSTR("[GATEWAY][ERR] Node is trying to write to undefined AO register address\n"));
    }

    _gateway_nodes[id].packet.waiting_for = GATEWAY_PACKET_NONE;
    _gateway_nodes[id].packet.sending_time = 0;
}

// -----------------------------------------------------------------------------

void _gatewayWriteMultipleAnalogOutputsHandler(
    const uint8_t id,
    uint8_t * payload
) {
    word register_address = (word) payload[1] << 8 | (word) payload[2];
    word read_length = (word) payload[3] << 8 | (word) payload[4];
    uint8_t bytes_length = (uint8_t) payload[5];

    if (
        // Write start address mus be between <0, buffer.size()>
        register_address < _gateway_nodes[id].registers_size[GATEWAY_REGISTER_AO]
        // Write length have to be same or smaller as registers size
        && (register_address + read_length) <= _gateway_nodes[id].registers_size[GATEWAY_REGISTER_AO]
    ) {
        // Nothing to do here. Multiple AO register were successfully written

    } else {
        DEBUG_MSG(PSTR("[GATEWAY][ERR] Node is trying to write to undefined AO registers range\n"));
    }

    _gateway_nodes[id].packet.waiting_for = GATEWAY_PACKET_NONE;
    _gateway_nodes[id].packet.sending_time = 0;
}

#endif // NODES_GATEWAY_SUPPORT