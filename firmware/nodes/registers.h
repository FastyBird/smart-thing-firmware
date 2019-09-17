/*

NODES MODULE - REGISTERS

Copyright (C) 2018 FastyBird s.r.o. <info@fastybird.com>

*/

#if NODES_GATEWAY_SUPPORT

// -----------------------------------------------------------------------------
// REGISTERS MODULE PRIVATE
// -----------------------------------------------------------------------------

bool __gatewayRegistersIsAddressCorrect(
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

    } else if (dataRegister == GATEWAY_REGISTER_EV) {
        return (address < _gateway_nodes[id].registers_size[dataRegister]);
    }

    return false;
}

// -----------------------------------------------------------------------------

bool __gatewayRegistersIsSizeCorrect(
    const uint8_t id,
    const uint8_t dataRegister,
    const uint8_t address,
    const uint8_t size
) {
    if (dataRegister == GATEWAY_REGISTER_AI) {
        return (_gateway_nodes[id].analog_inputs[address].size == size);

    } else if (dataRegister == GATEWAY_REGISTER_AO) {
        return (_gateway_nodes[id].analog_outputs[address].size == size);
    }

    return false;
}

// -----------------------------------------------------------------------------

bool __gatewayRegistersWriteDigitalValue(
    const uint8_t id,
    const uint8_t dataRegister,
    const uint8_t address,
    const bool value
) {
    if (__gatewayRegistersIsAddressCorrect(id, dataRegister, address) == false) {
        return false;
    }

    if (dataRegister == GATEWAY_REGISTER_DI) {
        _gateway_nodes[id].digital_inputs[address].value = value ? true : false;

        return true;

    } else if (dataRegister == GATEWAY_REGISTER_DO) {
        _gateway_nodes[id].digital_outputs[address].value = value ? true : false;

        return true;
    }

    return false;
}

// -----------------------------------------------------------------------------

bool __gatewayRegistersWriteAnalogValue(
    const uint8_t id,
    const uint8_t dataRegister,
    const uint8_t address,
    const void * value,
    const uint8_t size
) {
    if (__gatewayRegistersIsAddressCorrect(id, dataRegister, address) == false) {
        return false;
    }

    if (__gatewayRegistersIsSizeCorrect(id, dataRegister, address, size)) {
        if (dataRegister == GATEWAY_REGISTER_AI) {
            memcpy(_gateway_nodes[id].analog_inputs[address].value, value, size);

            return true;

        } else if (dataRegister == GATEWAY_REGISTER_AO) {
            memcpy(_gateway_nodes[id].analog_outputs[address].value, value, size);

            return true;
        }
    }

    return false;
}

// -----------------------------------------------------------------------------

// Specialized convenience setters (these do not cost memory because of inlining)
bool __gatewayRegistersWriteAnalogValue(const uint8_t id, const uint8_t dataRegister, const uint8_t address, const uint8_t value) { return __gatewayRegistersWriteAnalogValue(id, dataRegister, address, &value, 1); }
bool __gatewayRegistersWriteAnalogValue(const uint8_t id, const uint8_t dataRegister, const uint8_t address, const uint16_t value) { return __gatewayRegistersWriteAnalogValue(id, dataRegister, address, &value, 2); }
bool __gatewayRegistersWriteAnalogValue(const uint8_t id, const uint8_t dataRegister, const uint8_t address, const uint32_t value) { return __gatewayRegistersWriteAnalogValue(id, dataRegister, address, &value, 4); }
bool __gatewayRegistersWriteAnalogValue(const uint8_t id, const uint8_t dataRegister, const uint8_t address, const int8_t value) { return __gatewayRegistersWriteAnalogValue(id, dataRegister, address, &value, 1); }
bool __gatewayRegistersWriteAnalogValue(const uint8_t id, const uint8_t dataRegister, const uint8_t address, const int16_t value) { return __gatewayRegistersWriteAnalogValue(id, dataRegister, address, &value, 2); }
bool __gatewayRegistersWriteAnalogValue(const uint8_t id, const uint8_t dataRegister, const uint8_t address, const int32_t value) { return __gatewayRegistersWriteAnalogValue(id, dataRegister, address, &value, 4); }
bool __gatewayRegistersWriteAnalogValue(const uint8_t id, const uint8_t dataRegister, const uint8_t address, const float value) { return __gatewayRegistersWriteAnalogValue(id, dataRegister, address, &value, 4); }

// -----------------------------------------------------------------------------

bool __gatewayRegistersWriteEventValue(
    const uint8_t id,
    const uint8_t dataRegister,
    const uint8_t address,
    const uint8_t value
) {
    if (__gatewayRegistersIsAddressCorrect(id, dataRegister, address) == false) {
        return false;
    }

    if (dataRegister == GATEWAY_REGISTER_EV) {
        _gateway_nodes[id].event_inputs[address].value = value;

        return true;
    }

    return false;
}

// -----------------------------------------------------------------------------
// REGISTERS UTILS
// -----------------------------------------------------------------------------

uint8_t _gatewayGetAnalogRegistersDataType(
    const uint8_t id,
    const uint8_t dataRegister,
    const uint8_t address
) {
    switch (dataRegister)
    {
        case GATEWAY_REGISTER_AI:
            return _gateway_nodes[id].analog_inputs[address].data_type;

        case GATEWAY_REGISTER_AO:
            return _gateway_nodes[id].analog_outputs[address].data_type;
    }

    return 0;
}

// ----------------------------------------------------- ------------------------

void _gatewayRegistersUpdateReadingPointer(
    const uint8_t id
) {
    for (uint8_t i = 0; i < 5; i++) {
        if (
            i == _gateway_nodes[id].registers_reading.register_type
            || _gateway_nodes[id].registers_reading.register_type == GATEWAY_REGISTER_NONE
        ) {
            for (uint8_t j = i; j < 5; j++) {
                if (_gateway_nodes[id].registers_size[j] > 0) {
                    _gateway_nodes[id].registers_reading.register_type = j;
                    _gateway_nodes[id].registers_reading.start = 0;

                    return;
                }
            }
        }
    }

    _gateway_nodes[id].registers_reading.register_type = GATEWAY_REGISTER_NONE;
}

// -----------------------------------------------------------------------------
// REGISTERS READ & WRITE
// -----------------------------------------------------------------------------

bool _gatewayRegistersReadDigitalValue(
    const uint8_t id,
    const uint8_t dataRegister,
    const uint8_t address
) {
    if (__gatewayRegistersIsAddressCorrect(id, dataRegister, address)) {
        if (dataRegister == GATEWAY_REGISTER_DI) {
            return _gateway_nodes[id].digital_inputs[address].value;

        } else if (dataRegister == GATEWAY_REGISTER_DO) {
            return _gateway_nodes[id].digital_outputs[address].value;
        }
    }

    return false;
}

// -----------------------------------------------------------------------------

bool _gatewayRegistersReadDigitalTargetValue(
    const uint8_t id,
    const uint8_t address
) {
    if (__gatewayRegistersIsAddressCorrect(id, GATEWAY_REGISTER_DO, address)) {
        return _gateway_nodes[id].digital_outputs[address].target_value;
    }

    return false;
}

// -----------------------------------------------------------------------------

void _gatewayRegistersReadAnalogValue(
    const uint8_t id,
    const uint8_t dataRegister,
    const uint8_t address,
    void * value,
    const uint8_t size
) {
    if (
        __gatewayRegistersIsAddressCorrect(id, dataRegister, address)
        && __gatewayRegistersIsSizeCorrect(id, dataRegister, address, size)
    ) {
        if (dataRegister == GATEWAY_REGISTER_AI) {
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
void _gatewayRegistersReadAnalogValue(const uint8_t id, const uint8_t dataRegister, const uint8_t address, uint8_t &value) { _gatewayRegistersReadAnalogValue(id, dataRegister, address, &value, 1); }
void _gatewayRegistersReadAnalogValue(const uint8_t id, const uint8_t dataRegister, const uint8_t address, uint16_t &value) { _gatewayRegistersReadAnalogValue(id, dataRegister, address, &value, 2); }
void _gatewayRegistersReadAnalogValue(const uint8_t id, const uint8_t dataRegister, const uint8_t address, uint32_t &value) { _gatewayRegistersReadAnalogValue(id, dataRegister, address, &value, 4); }
void _gatewayRegistersReadAnalogValue(const uint8_t id, const uint8_t dataRegister, const uint8_t address, int8_t &value) { _gatewayRegistersReadAnalogValue(id, dataRegister, address, &value, 1); }
void _gatewayRegistersReadAnalogValue(const uint8_t id, const uint8_t dataRegister, const uint8_t address, int16_t &value) { _gatewayRegistersReadAnalogValue(id, dataRegister, address, &value, 2); }
void _gatewayRegistersReadAnalogValue(const uint8_t id, const uint8_t dataRegister, const uint8_t address, int32_t &value) { _gatewayRegistersReadAnalogValue(id, dataRegister, address, &value, 4); }
void _gatewayRegistersReadAnalogValue(const uint8_t id, const uint8_t dataRegister, const uint8_t address, float &value) { _gatewayRegistersReadAnalogValue(id, dataRegister, address, &value, 4); }

// -----------------------------------------------------------------------------

uint8_t _gatewayRegistersReadEventValue(
    const uint8_t id,
    const uint8_t dataRegister,
    const uint8_t address
) {
    if (__gatewayRegistersIsAddressCorrect(id, dataRegister, address)) {
        if (dataRegister == GATEWAY_REGISTER_EV) {
            return _gateway_nodes[id].event_inputs[address].value;
        }
    }

    return 0;
}

// -----------------------------------------------------------------------------

/**
 * Transform received 4 bytes value into register value
 */
void _gatewayWriteReceivedAnalogValue(
    const uint8_t id,
    const uint8_t dataRegister,
    const uint8_t address,
    char * value
) {
    switch (_gatewayGetAnalogRegistersDataType(id, dataRegister, address))
    {
        case GATEWAY_DATA_TYPE_UINT8:
            uint8_t uint8_stored_value;
            UINT8_UNION_t uint8_write_value;

            uint8_write_value.bytes[0] = value[0];
            uint8_write_value.bytes[1] = value[1];
            uint8_write_value.bytes[2] = value[2];
            uint8_write_value.bytes[3] = value[3];

            _gatewayRegistersReadAnalogValue(id, dataRegister, address, uint8_stored_value);

            if (uint8_stored_value != uint8_write_value.number) {
                __gatewayRegistersWriteAnalogValue(id, dataRegister, address, uint8_write_value.number);

                _gatewayRegisterValueUpdated(id, dataRegister, address);


                DEBUG_GW_MSG(PSTR("[GATEWAY] Value was written into register\n"));
            }
            break;

        case GATEWAY_DATA_TYPE_UINT16:
            uint16_t uint16_stored_value;
            UINT16_UNION_t uint16_write_value;

            uint16_write_value.bytes[0] = value[0];
            uint16_write_value.bytes[1] = value[1];
            uint16_write_value.bytes[2] = value[2];
            uint16_write_value.bytes[3] = value[3];

            _gatewayRegistersReadAnalogValue(id, dataRegister, address, uint16_stored_value);

            if (uint16_stored_value != uint16_write_value.number) {
                __gatewayRegistersWriteAnalogValue(id, dataRegister, address, uint16_write_value.number);

                _gatewayRegisterValueUpdated(id, dataRegister, address);

                DEBUG_GW_MSG(PSTR("[GATEWAY] Value was written into register\n"));
            }
            break;

        case GATEWAY_DATA_TYPE_UINT32:
            uint32_t uint32_stored_value;
            UINT32_UNION_t uint32_write_value;

            uint32_write_value.bytes[0] = value[0];
            uint32_write_value.bytes[1] = value[1];
            uint32_write_value.bytes[2] = value[2];
            uint32_write_value.bytes[3] = value[3];

            _gatewayRegistersReadAnalogValue(id, dataRegister, address, uint32_stored_value);

            if (uint32_stored_value != uint32_write_value.number) {
                __gatewayRegistersWriteAnalogValue(id, dataRegister, address, uint32_write_value.number);

                _gatewayRegisterValueUpdated(id, dataRegister, address);

                DEBUG_GW_MSG(PSTR("[GATEWAY] Value was written into register\n"));
            }
            break;

        case GATEWAY_DATA_TYPE_INT8:
            int8_t int8_stored_value;
            INT8_UNION_t int8_write_value;

            int8_write_value.bytes[0] = value[0];
            int8_write_value.bytes[1] = value[1];
            int8_write_value.bytes[2] = value[2];
            int8_write_value.bytes[3] = value[3];

            _gatewayRegistersReadAnalogValue(id, dataRegister, address, int8_stored_value);

            if (int8_stored_value != int8_write_value.number) {
                __gatewayRegistersWriteAnalogValue(id, dataRegister, address, int8_write_value.number);

                _gatewayRegisterValueUpdated(id, dataRegister, address);

                DEBUG_GW_MSG(PSTR("[GATEWAY] Value was written into register\n"));
            }
            break;

        case GATEWAY_DATA_TYPE_INT16:
            int16_t int16_stored_value;
            INT16_UNION_t int16_write_value;

            int16_write_value.bytes[0] = value[0];
            int16_write_value.bytes[1] = value[1];
            int16_write_value.bytes[2] = value[2];
            int16_write_value.bytes[3] = value[3];

            _gatewayRegistersReadAnalogValue(id, dataRegister, address, int16_stored_value);

            if (int16_stored_value != int16_write_value.number) {
                __gatewayRegistersWriteAnalogValue(id, dataRegister, address, int16_write_value.number);

                _gatewayRegisterValueUpdated(id, dataRegister, address);

                DEBUG_GW_MSG(PSTR("[GATEWAY] Value was written into register\n"));
            }
            break;

        case GATEWAY_DATA_TYPE_INT32:
            int32_t int32_stored_value;
            INT32_UNION_t int32_write_value;

            int32_write_value.bytes[0] = value[0];
            int32_write_value.bytes[1] = value[1];
            int32_write_value.bytes[2] = value[2];
            int32_write_value.bytes[3] = value[3];

            _gatewayRegistersReadAnalogValue(id, dataRegister, address, int32_stored_value);

            if (int32_stored_value != int32_write_value.number) {
                __gatewayRegistersWriteAnalogValue(id, dataRegister, address, int32_write_value.number);

                _gatewayRegisterValueUpdated(id, dataRegister, address);

                DEBUG_GW_MSG(PSTR("[GATEWAY] Value was written into register\n"));
            }
            break;

        case GATEWAY_DATA_TYPE_FLOAT32:
            float float_stored_value;
            FLOAT32_UNION_t float_write_value;

            float_write_value.bytes[0] = value[0];
            float_write_value.bytes[1] = value[1];
            float_write_value.bytes[2] = value[2];
            float_write_value.bytes[3] = value[3];

            _gatewayRegistersReadAnalogValue(id, dataRegister, address, float_stored_value);

            if (float_stored_value != float_write_value.number) {
                __gatewayRegistersWriteAnalogValue(id, dataRegister, address, float_write_value.number);

                _gatewayRegisterValueUpdated(id, dataRegister, address);

                DEBUG_GW_MSG(PSTR("[GATEWAY] Value was written into register\n"));
            }
            break;

        default:
            DEBUG_GW_MSG(PSTR("[GATEWAY][ERR] Provided unknown data type for writing into register\n"));
            break;
    }
}

// -----------------------------------------------------------------------------
// DIGITAL REGISTERS READING
// -----------------------------------------------------------------------------

void _gatewayRequestReadingDigitalRegisters(
    const uint8_t id,
    const uint8_t registerType
) {
    uint8_t packet_id = GATEWAY_PACKET_NONE;
    const uint8_t register_size = _gateway_nodes[id].registers_size[registerType];
    uint8_t start = _gateway_nodes[id].registers_reading.start;

    if (registerType == GATEWAY_REGISTER_DI) {
        packet_id = GATEWAY_PACKET_READ_MULTI_DI;

    } else if (registerType == GATEWAY_REGISTER_DO) {
        packet_id = GATEWAY_PACKET_READ_MULTI_DO;

    } else {
        return;
    }

    char output_content[5];

    output_content[0] = packet_id;

    // Register reading start address
    output_content[1] = (char) (start >> 8);
    output_content[2] = (char) (start & 0xFF);

    // It is based on maximum packed size reduced by packet header (5 bytes)
    uint8_t max_readable_addresses = (_gateway_nodes[id].packet.max_length - 5) * 8;

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
    if (_gatewaySendPacket((id + 1), output_content, 5)) {
        _gateway_nodes[id].packet.waiting_for = packet_id;

        _gateway_nodes[id].registers_reading.start = start;

         if (start == 0) {
            _gatewayRegistersUpdateReadingPointer(id);
         }
    }
}

// -----------------------------------------------------------------------------

void _gatewayRequestReadingMultipleDigitalInputsRegisters(
    const uint8_t id
) {
    _gatewayRequestReadingDigitalRegisters(
       id,
        GATEWAY_REGISTER_DI
    );
}

// -----------------------------------------------------------------------------

void _gatewayRequestReadingMultipleDigitalOutputsRegisters(
    const uint8_t id
) {
    _gatewayRequestReadingDigitalRegisters(
        id,
        GATEWAY_REGISTER_DO
    );
}

// -----------------------------------------------------------------------------
// ANALOG REGISTERS READING
// -----------------------------------------------------------------------------

void _gatewayRequestReadingAnalogRegisters(
    const uint8_t id,
    const uint8_t registerType
) {
    uint8_t packet_id = GATEWAY_PACKET_NONE;
    const uint8_t register_size = _gateway_nodes[id].registers_size[registerType];
    uint8_t start = _gateway_nodes[id].registers_reading.start;

    if (registerType == GATEWAY_REGISTER_AI) {
        packet_id = GATEWAY_PACKET_READ_MULTI_AI;

    } else if (registerType == GATEWAY_REGISTER_AO) {
        packet_id = GATEWAY_PACKET_READ_MULTI_AO;

    } else {
        return;
    }

    char output_content[5];

    output_content[0] = packet_id;

    // Register reading start address
    output_content[1] = (char) (start >> 8);
    output_content[2] = (char) (start & 0xFF);

    // It is based on maximum packed size reduced by packet header (5 bytes)
    uint8_t max_readable_addresses = (uint8_t) ((_gateway_nodes[id].packet.max_length - 5) / 4);

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
    if (_gatewaySendPacket((id + 1), output_content, 5)) {
        _gateway_nodes[id].packet.waiting_for = packet_id;

        _gateway_nodes[id].registers_reading.start = start;

         if (start == 0) {
            _gatewayRegistersUpdateReadingPointer(id);
         }
    }
}

// -----------------------------------------------------------------------------

void _gatewayRequestReadingMultipleAnalogInputsRegisters(
    const uint8_t id
) {
    _gatewayRequestReadingAnalogRegisters(
        id,
        GATEWAY_REGISTER_AI
    );
}

// -----------------------------------------------------------------------------

void _gatewayRequestReadingMultipleAnalogOutputsRegisters(
    const uint8_t id
) {
    _gatewayRequestReadingAnalogRegisters(
        id,
        GATEWAY_REGISTER_AO
    );
}

// -----------------------------------------------------------------------------
// EVENT REGISTERS READING
// -----------------------------------------------------------------------------

void _gatewayRequestReadingEventRegisters(
    const uint8_t id,
    const uint8_t registerType
) {
    uint8_t packet_id = GATEWAY_PACKET_NONE;
    const uint8_t register_size = _gateway_nodes[id].registers_size[registerType];
    uint8_t start = _gateway_nodes[id].registers_reading.start;

    if (registerType == GATEWAY_REGISTER_EV) {
        packet_id = GATEWAY_PACKET_READ_MULTI_EV;

    } else {
        return;
    }

    char output_content[5];

    output_content[0] = packet_id;

    // Register reading start address
    output_content[1] = (char) (start >> 8);
    output_content[2] = (char) (start & 0xFF);

    // It is based on maximum packed size reduced by packet header (5 bytes)
    uint8_t max_readable_addresses = (uint8_t) ((_gateway_nodes[id].packet.max_length - 5) / 4);

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
    if (_gatewaySendPacket((id + 1), output_content, 5)) {
        _gateway_nodes[id].packet.waiting_for = packet_id;

        _gateway_nodes[id].registers_reading.start = start;

         if (start == 0) {
            _gatewayRegistersUpdateReadingPointer(id);
         }
    }
}

// -----------------------------------------------------------------------------

void _gatewayRequestReadingMultipleEventInputsRegisters(
    const uint8_t id
) {
    _gatewayRequestReadingEventRegisters(
        id,
        GATEWAY_REGISTER_EV
    );
}

// -----------------------------------------------------------------------------
// DIGITAL REGISTERS WRITING (only for DO registers)
// -----------------------------------------------------------------------------

void _gatewayRequestWritingSingleDigitalRegister(
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
    }
}

// -----------------------------------------------------------------------------
// ANALOG REGISTERS WRITING (only for AO registers)
// -----------------------------------------------------------------------------

void _gatewayRequestWritingSingleAnalogRegister(
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
    }
}

// -----------------------------------------------------------------------------

// Specialized convenience setters (these do not cost memory because of inlining)
void _gatewayRequestWritingSingleAnalogRegister(const uint8_t id, const uint8_t registerAddress, uint8_t value) { _gatewayRequestWritingSingleAnalogRegister(id, registerAddress, &value, 1); }
void _gatewayRequestWritingSingleAnalogRegister(const uint8_t id, const uint8_t registerAddress, uint16_t value) { _gatewayRequestWritingSingleAnalogRegister(id, registerAddress, &value, 2); }
void _gatewayRequestWritingSingleAnalogRegister(const uint8_t id, const uint8_t registerAddress, uint32_t value) { _gatewayRequestWritingSingleAnalogRegister(id, registerAddress, &value, 4); }
void _gatewayRequestWritingSingleAnalogRegister(const uint8_t id, const uint8_t registerAddress, int8_t value) { _gatewayRequestWritingSingleAnalogRegister(id, registerAddress, &value, 1); }
void _gatewayRequestWritingSingleAnalogRegister(const uint8_t id, const uint8_t registerAddress, int16_t value) { _gatewayRequestWritingSingleAnalogRegister(id, registerAddress, &value, 2); }
void _gatewayRequestWritingSingleAnalogRegister(const uint8_t id, const uint8_t registerAddress, int32_t value) { _gatewayRequestWritingSingleAnalogRegister(id, registerAddress, &value, 4); }
void _gatewayRequestWritingSingleAnalogRegister(const uint8_t id, const uint8_t registerAddress, float value) { _gatewayRequestWritingSingleAnalogRegister(id, registerAddress, &value, 4); }

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

    DEBUG_GW_MSG(
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

        bool write_value = false;
        bool stored_value = false;

        while (
            write_address < _gateway_nodes[id].registers_size[dataRegister]
            && write_byte <= bytes_length
        ) {
            data_byte = (uint8_t) payload[3 + write_byte];

            for (uint8_t i = 0; i < 8; i++) {
                write_value = (data_byte >> i) & 0x01 ? true : false;

                stored_value = _gatewayRegistersReadDigitalValue(id, dataRegister, write_address);

                if (stored_value != write_value) {
                    __gatewayRegistersWriteDigitalValue(id, dataRegister, write_address, write_value);

                    _gatewayRegisterValueUpdated(id, dataRegister, write_address);

                    DEBUG_GW_MSG(PSTR("[GATEWAY] Value was written into digital register at address: %d\n"), write_address);
                }

                write_address++;

                if (write_address >= _gateway_nodes[id].registers_size[dataRegister]) {
                    break;
                }
            }

            write_byte++;
        }

    } else {
        DEBUG_GW_MSG(PSTR("[GATEWAY][ERR] Node is trying to write to undefined digital register address\n"));
    }
}

// -----------------------------------------------------------------------------

void _gatewayReadMultipleAnalogRegisterHandler(
    const uint8_t id,
    const uint8_t dataRegister,
    uint8_t * payload
) {
    word start_address = (word) payload[1] << 8 | (word) payload[2];

    uint8_t bytes_length = (uint8_t) payload[3];

    DEBUG_GW_MSG(
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

            _gatewayWriteReceivedAnalogValue(id, dataRegister, write_address, write_value);

            write_byte = write_byte + 4;
            write_address++;
        }

    } else {
        DEBUG_GW_MSG(PSTR("[GATEWAY][ERR] Node is trying to write to undefined analog register address\n"));
    }
}

// -----------------------------------------------------------------------------

void _gatewayReadMultipleEventRegisterHandler(
    const uint8_t id,
    const uint8_t dataRegister,
    uint8_t * payload
) {
    word start_address = (word) payload[1] << 8 | (word) payload[2];

    uint8_t bytes_length = (uint8_t) payload[3];

    DEBUG_GW_MSG(
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

        uint8_t write_address = start_address;

        uint8_t stored_value = 0xFF;
        uint8_t received_value;

        while (
            write_address < _gateway_nodes[id].registers_size[dataRegister]
            && write_byte <= bytes_length
        ) {
            received_value = (uint8_t) payload[3 + write_byte];
            stored_value = _gatewayRegistersReadEventValue(id, dataRegister, write_address);

            if (stored_value != received_value) {
                __gatewayRegistersWriteEventValue(id, dataRegister, write_address, received_value);

                _gatewayRegisterValueUpdated(id, dataRegister, write_address);

                DEBUG_GW_MSG(PSTR("[GATEWAY] Value was written into event register at address: %d\n"), write_address);
            }

            write_byte++;
            write_address++;
        }

    } else {
        DEBUG_GW_MSG(PSTR("[GATEWAY][ERR] Node is trying to write to undefined digital register address\n"));
    }
}

// -----------------------------------------------------------------------------
// WRITING HANDLERS
// -----------------------------------------------------------------------------

void _gatewayWriteSingleDigitalOutputHandler(
    const uint8_t id,
    uint8_t * payload
) {
    word register_address = (word) payload[1] << 8 | (word) payload[2];
    word write_value = (word) payload[3] << 8 | (word) payload[4];

    // Check if value is TRUE|FALSE or 1|0
    if (write_value != 0xFF00 && write_value != 0x0000) {
        DEBUG_GW_MSG(PSTR("[GATEWAY][ERR] Digital register accept only bool value\n"));
        return;
    }

    if (
        // Write address must be between <0, buffer.size()>
        register_address < _gateway_nodes[id].registers_size[GATEWAY_REGISTER_DO]
    ) {
        bool stored_value = _gatewayRegistersReadDigitalValue(id, GATEWAY_REGISTER_DO, register_address);

        if (stored_value != (bool) write_value) {
            __gatewayRegistersWriteDigitalValue(id, GATEWAY_REGISTER_DO, register_address, (bool) write_value ? true : false);

            _gatewayRegisterValueUpdated(id, GATEWAY_REGISTER_DO, register_address);

            DEBUG_GW_MSG(PSTR("[GATEWAY] Value was written into DO register\n"));
        }

    } else {
        DEBUG_GW_MSG(PSTR("[GATEWAY][ERR] Node is trying to write to undefined DO register address\n"));
    }
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
        DEBUG_GW_MSG(PSTR("[GATEWAY][ERR] Node is trying to write to undefined DO registers range\n"));
    }
}

// -----------------------------------------------------------------------------

void _gatewayWriteSingleAnalogOutputHandler(
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
        _gatewayWriteReceivedAnalogValue(id, GATEWAY_REGISTER_AO, register_address, write_value);

    } else {
        DEBUG_GW_MSG(PSTR("[GATEWAY][ERR] Node is trying to write to undefined AO register address\n"));
    }
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
        DEBUG_GW_MSG(PSTR("[GATEWAY][ERR] Node is trying to write to undefined AO registers range\n"));
    }
}

// -----------------------------------------------------------------------------

/**
 * Walks the relay vector processing only those relays
 * that have to change to the requested mode
 * @bool mode Requested mode
 */
void _gatewayDigitalRegisterProcess(
    const bool mode
) {
    uint32_t current_time = millis();

    for (uint8_t id = 0; id < NODES_GATEWAY_MAX_NODES; id++) {
        for (uint8_t address = 0; address < _gateway_nodes[id].digital_outputs.size(); address++) {
            bool target = _gateway_nodes[id].digital_outputs[address].target_value;

            // Only process the addresses we have to change
            if (target == _gateway_nodes[id].digital_outputs[address].value) {
                continue;
            }

            // Only process the addresses we have to change to the requested mode
            if (target != mode) {
                continue;
            }

            // Only process if the change_time has arrived
            if (current_time < _gateway_nodes[id].digital_outputs[address].change_time) {
                continue;
            }

            DEBUG_GW_MSG(PSTR("[GATEWAY] #%d:DO:%d set to %s\n"), id, address, target ? "ON" : "OFF");

            // Call the provider to perform the action
            _gatewayRequestWritingSingleDigitalRegister(id, address, target);
        }
    }
}

// -----------------------------------------------------------------------------
// MODULE API
// -----------------------------------------------------------------------------

bool gatewayDigitalRegisterStatus(
    const uint8_t id,
    const uint8_t address,
    const bool value
) {
    bool changed = false;

    if (!__gatewayRegistersIsAddressCorrect(id, GATEWAY_REGISTER_DO, address)) {
        return false;
    }

    if (_gatewayRegistersReadDigitalValue(id, GATEWAY_REGISTER_DO, address) == value) {
        if (_gatewayRegistersReadDigitalTargetValue(id, address) != value) {
            DEBUG_GW_MSG(PSTR("[GATEWAY] #%d:DO:%d scheduled change cancelled\n"), id, address);

            _gateway_nodes[id].digital_outputs[address].target_value = value;

            changed = true;
        }

    } else {
        uint32_t current_time = millis();

        uint32_t fw_end = _gateway_nodes[id].digital_outputs[address].fw_start + 1000 * NODES_GATEWAY_FLOOD_WINDOW;
        uint32_t delay = value ? _gateway_nodes[id].digital_outputs[address].delay_on : _gateway_nodes[id].digital_outputs[address].delay_off;

        _gateway_nodes[id].digital_outputs[address].fw_count++;
        _gateway_nodes[id].digital_outputs[address].change_time = current_time + delay;

        // If current_time is off-limits the floodWindow...
        if (current_time < _gateway_nodes[id].digital_outputs[address].fw_start || fw_end <= current_time) {
            // We reset the floodWindow
            _gateway_nodes[id].digital_outputs[address].fw_start = current_time;
            _gateway_nodes[id].digital_outputs[address].fw_count = 1;

        // If current_time is in the floodWindow and there have been too many requests...
        } else if (_gateway_nodes[id].digital_outputs[address].fw_count >= NODES_GATEWAY_FLOOD_CHANGES) {
            // We schedule the changes to the end of the floodWindow
            // unless it's already delayed beyond that point
            if (fw_end - delay > current_time) {
                _gateway_nodes[id].digital_outputs[address].change_time = fw_end;
            }
        }

        _gateway_nodes[id].digital_outputs[address].target_value = value;

        DEBUG_GW_MSG(PSTR("[GATEWAY] #%d:DO:%d scheduled %s in %u ms\n"),
                id, address, value ? "ON" : "OFF",
                (_gateway_nodes[id].digital_outputs[address].change_time - current_time));

        changed = true;
    }

    return changed;
}

#endif // NODES_GATEWAY_SUPPORT