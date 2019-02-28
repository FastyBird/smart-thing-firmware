/*

NODES MODULE

Copyright (C) 2018 FastyBird Ltd. <info@fastybird.com>

*/

#if NODES_GATEWAY_SUPPORT

#include <ArduinoJson.h>
#include <vector>
#include <SoftwareSerial.h>

PJON<ThroughSerial> _gateway_bus(PJON_MASTER_ID);

SoftwareSerial _gateway_serial_bus(NODES_GATEWAY_TX_PIN, NODES_GATEWAY_RX_PIN);

struct gateway_node_description_t {
    char    manufacturer[20]    = GATEWAY_DESCRIPTION_NOT_SET;
    char    model[20]           = GATEWAY_DESCRIPTION_NOT_SET;
    char    version[10]         = GATEWAY_DESCRIPTION_NOT_SET;
};

struct gateway_node_initiliazation_t {
    bool        state           = false;
    uint8_t     step            = GATEWAY_NODE_INIT_HW_MODEL;
    uint8_t     attempts        = 0;
    uint32_t    delay           = 0;                        // In case maximum attempts is reached a wait delay is activated
    uint8_t     waiting_reply   = GATEWAY_PACKET_NONE;
    uint32_t    packet_time     = 0;
};

struct gateway_node_addressing_t {
    bool        state           = false;    // FALSE = node addresing is not finished | TRUE = address was successfully accepted by node
    uint32_t    registration    = 0;        // Timestamp when node requested for address
};

struct gateway_digital_register_reading_t {
    uint8_t     start      = 0;
    uint32_t    delay      = 0;
};

struct gateway_analog_register_reading_t {
    uint8_t     start      = 0;
    uint32_t    delay      = 0;
};

struct gateway_node_t {
    // Node addressing process
    gateway_node_addressing_t addressing;

    // Node initiliazation process
    gateway_node_initiliazation_t initiliazation;

    // Node basic info
    char node[40] = GATEWAY_DESCRIPTION_NOT_SET;
    uint8_t max_packet_length = PJON_PACKET_MAX_LENGTH;

    gateway_node_description_t hardware;
    gateway_node_description_t firmware;

    uint32_t last_updated;

    // Data registers
    std::vector<bool>   digital_inputs;
    std::vector<bool>   digital_outputs;

    std::vector<word>  analog_inputs;
    std::vector<word>  analog_outputs;
    
    // Data registers reading
    gateway_digital_register_reading_t  digital_inputs_reading;
    gateway_digital_register_reading_t  digital_outputs_reading;

    gateway_analog_register_reading_t   analog_inputs_reading;
    gateway_analog_register_reading_t   analog_outputs_reading;
};

gateway_node_t _gateway_nodes[NODES_GATEWAY_MAX_NODES];

uint8_t _gateway_reading_node_index = 0;
bool _gateway_refresh_nodes = false;
uint32_t _gateway_last_nodes_check = 0;

// -----------------------------------------------------------------------------
// MODULE PRIVATE
// -----------------------------------------------------------------------------

#if (WEB_SUPPORT && WS_SUPPORT) || FASTYBIRD_SUPPORT
    void _nodesCollectNodes(
        JsonArray& container
    ) {
        for (uint8_t i = 0; i < NODES_GATEWAY_MAX_NODES; i++) {
            JsonObject& node = container.createNestedObject();

            node["addressed"] = _gateway_nodes[i].addressing.state;
            node["initialized"] = _gateway_nodes[i].initiliazation.state;

            JsonObject& addressing = node.createNestedObject("addressing");

            addressing["registration"] = _gateway_nodes[i].addressing.registration;

            // Node basic info
            node["node"] = String(_gateway_nodes[i].node);

            node["address"] = i + 1;

            node["last_updated"] = _gateway_nodes[i].last_updated;

            JsonObject& hardware = node.createNestedObject("hardware");

            hardware["manufacturer"] = (char *) NULL;
            hardware["model"] = (char *) NULL;
            hardware["version"] = (char *) NULL;

            hardware["manufacturer"] = _gateway_nodes[i].hardware.manufacturer;
            hardware["model"] = _gateway_nodes[i].hardware.model;
            hardware["version"] = _gateway_nodes[i].hardware.version;

            JsonObject& firmware = node.createNestedObject("firmware");

            firmware["manufacturer"] = (char *) NULL;
            firmware["model"] = (char *) NULL;
            firmware["version"] = (char *) NULL;

            firmware["manufacturer"] = _gateway_nodes[i].firmware.manufacturer;
            firmware["model"] = _gateway_nodes[i].firmware.model;
            firmware["version"] = _gateway_nodes[i].firmware.version;

            // Node channels schema
            JsonObject& registers = node.createNestedObject("registers");

            JsonArray& digital_inputs_register = registers.createNestedArray("digital_inputs");

            for (uint8_t j = 0; j < _gateway_nodes[i].digital_inputs.size(); j++) {
                digital_inputs_register.add((bool) _gateway_nodes[i].digital_inputs[j]);
            }

            JsonArray& digital_outputs_register = registers.createNestedArray("digital_outputs");

            for (uint8_t j = 0; j < _gateway_nodes[i].digital_outputs.size(); j++) {
                digital_outputs_register.add((bool) _gateway_nodes[i].digital_outputs[j]);
            }

            JsonArray& analog_inputs_register = registers.createNestedArray("analog_inputs");

            for (uint8_t j = 0; j < _gateway_nodes[i].analog_inputs.size(); j++) {
                analog_inputs_register.add((word) _gateway_nodes[i].analog_inputs[j]);
            }

            JsonArray& analog_outputs_register = registers.createNestedArray("analog_outputs");

            for (uint8_t j = 0; j < _gateway_nodes[i].analog_outputs.size(); j++) {
                analog_outputs_register.add((word) _gateway_nodes[i].analog_outputs[j]);
            }
        }
    }
#endif

// -----------------------------------------------------------------------------

#if WEB_SUPPORT && WS_SUPPORT
    // Send module status to WS clients
    void _nodesWebSocketUpdate(
        JsonObject& root
    ) {
        DEBUG_MSG(PSTR("[GATEWAY] Updating nodes to WS clients\n"));

        root["module"] = "nodes";

        // Data container
        JsonObject& data = root.createNestedObject("data");

        // Nodes container
        JsonArray& nodes = data.createNestedArray("nodes");

        _nodesCollectNodes(nodes);
    }

// -----------------------------------------------------------------------------

    // New WS client is connected
    void _nodesWSOnConnect(
        JsonObject& root
    ) {
        JsonArray& modules = root.containsKey("modules") ? root["modules"] : root.createNestedArray("modules");
        JsonObject& module = modules.createNestedObject();

        module["module"] = "nodes";
        module["visible"] = true;

        // Configuration container
        JsonObject& configuration = module.createNestedObject("config");

        // Configuration values container
        JsonObject& configuration_values = configuration.createNestedObject("values");

        // Configuration schema container
        JsonArray& configuration_schema = configuration.createNestedArray("schema");
        
        // Data container
        JsonObject& data = module.createNestedObject("data");

        // Nodes container
        JsonArray& nodes = data.createNestedArray("nodes");

        _nodesCollectNodes(nodes);
    }

// -----------------------------------------------------------------------------

    // WS client requested configuration update
    void _nodesWSOnConfigure(
        uint32_t clientId, 
        JsonObject& module
    ) {
        if (module.containsKey("module") && module["module"] == "nodes") {
            if (module.containsKey("config")) {
                // Extract configuration container
                JsonObject& configuration = module["config"].as<JsonObject&>();

                if (configuration.containsKey("values")) {
                    wsSend_P(clientId, PSTR("{\"message\": \"nodes_updated\"}"));

                    // Reload & cache settings
                    firmwareReload();
                }
            }
        }
    }

// -----------------------------------------------------------------------------

    // WS client called action
    void _nodesWSOnAction(
        uint32_t clientId,
        const char * action,
        JsonObject& data
    ) {
        if (
            strcmp(action, "switch") == 0
            && data.containsKey("node")
            && data.containsKey("register")
            && data.containsKey("address")
            && data.containsKey("value")
        ) {
            for (uint8_t i = 0; i < NODES_GATEWAY_MAX_NODES; i++) {
                // Search for node
                if (strcmp(_gateway_nodes[i].node, data["node"].as<const char *>()) == 0) {
                    DEBUG_MSG(PSTR("[GATEWAY] Found node to update\n"));

                    if (strcmp("digital_output", data["register"].as<const char *>()) == 0) {
                        if (data["address"].as<uint8_t>() < _gateway_nodes[i].digital_outputs.size()) {
                            _gatewayWriteNodeDigitalOutputs(i, data["address"].as<uint8_t>(), data["value"].as<bool>());

                        } else {
                            DEBUG_MSG(PSTR("[GATEWAY][ERR] Register address: %d is out of range: 0 - %d\n"), data["address"].as<uint8_t>(), _gateway_nodes[i].digital_outputs.size());
                        }

                    } else if (strcmp("analog_output", data["register"].as<const char *>()) == 0) {
                        if (data["address"].as<uint8_t>() < _gateway_nodes[i].analog_outputs.size()) {
                            // TODO: _gatewayWriteNodeAnalogOutputs(i, data["address"].as<uint8_t>(), data["value"].as<word>());

                        } else {
                            DEBUG_MSG(PSTR("[GATEWAY][ERR] Register address: %d is out of range: 0 - %d\n"), data["address"].as<uint8_t>(), _gateway_nodes[i].analog_outputs.size());
                        }
                    }
                }
            }
        }
    }
#endif

// -----------------------------------------------------------------------------

#if FASTYBIRD_SUPPORT
    void _gatewayRegisterFastybirdNode(
        uint8_t nodeIndex
    ) {
/*
        fastybird_node_hardware_t hardware = {
            "fb_node",
            "0.0.1",
            "fastybird"
        };

        fastybird_node_software_t software = {
            "fb_node",
            "0.0.1",
            "fastybird"
        };

        fastybird_node_t register_node = {
            _gateway_nodes[nodeIndex].node.c_str(),
            _gateway_nodes[nodeIndex].version.c_str(),
            hardware,
            software,
            false
        };

        for (uint8_t i = 0; i < _gateway_nodes[nodeIndex].channels.size(); i++) {
            switch (_gateway_nodes[nodeIndex].channels[i].type)
            {
                case GATEWAY_NODE_CHANNEL_TYPE_BUTTON:
                    register_node.channels.push_back({
                        FASTYBIRD_CHANNEL_TYPE_BUTTON,
                        _gateway_nodes[nodeIndex].channels[i].length,
                        _gateway_nodes[nodeIndex].channels[i].setable
                    });
                    break;
            
                case GATEWAY_NODE_CHANNEL_TYPE_INPUT:
                    break;
            
                case GATEWAY_NODE_CHANNEL_TYPE_OUTPUT:
                    break;
            
                case GATEWAY_NODE_CHANNEL_TYPE_SWITCH:
                    register_node.channels.push_back({
                        FASTYBIRD_CHANNEL_TYPE_SWITCH,
                        _gateway_nodes[nodeIndex].channels[i].length,
                        _gateway_nodes[nodeIndex].channels[i].setable
                    });
                    break;
            }
        }

        for (uint8_t i = 0; i < _gateway_nodes[nodeIndex].settings.size(); i++) {
            register_node.settings.push_back({
                _gateway_nodes[nodeIndex].settings[i].key.c_str(),
                _gateway_nodes[nodeIndex].settings[i].value.c_str()
            });
        }

        fastybirdRegisterNode(register_node);
*/
    }
#endif

// -----------------------------------------------------------------------------

void _gatewayResetNodeIndex(
    uint8_t id
) {
    _gateway_nodes[id].max_packet_length = PJON_PACKET_MAX_LENGTH;

    // Reset addressing
    _gateway_nodes[id].addressing.registration = 0;
    _gateway_nodes[id].addressing.state = false;

    // Reset initialization steps
    _gateway_nodes[id].initiliazation.step = GATEWAY_NODE_INIT_HW_MODEL;
    _gateway_nodes[id].initiliazation.state = false;
    _gateway_nodes[id].initiliazation.attempts = 0;
    _gateway_nodes[id].initiliazation.waiting_reply = GATEWAY_PACKET_NONE;
    _gateway_nodes[id].initiliazation.packet_time = 0;

    // Reset registers
    std::vector<bool> reset_digital_inputs;
    _gateway_nodes[id].digital_inputs = reset_digital_inputs;

    std::vector<bool> reset_digital_outputs;
    _gateway_nodes[id].digital_outputs = reset_digital_outputs;

    std::vector<word> reset_analog_inputs;
    _gateway_nodes[id].analog_inputs = reset_analog_inputs;

    std::vector<word> reset_analog_outputs;
    _gateway_nodes[id].analog_outputs = reset_analog_outputs;
}

// -----------------------------------------------------------------------------

/**
 * Check if wating for reply packet reached maximum delay
 */
void _gatewayCheckPacketsDelays()
{
    for (uint8_t i = 0; i < NODES_GATEWAY_MAX_NODES; i++) {
        // Addressing
        if (
            _gateway_nodes[i].addressing.state == false
            && (millis() - _gateway_nodes[i].addressing.registration) > NODES_GATEWAY_ADDRESSING_TIMEOUT
        ) {
            _gatewayResetNodeIndex(i);
        }

        // Initialization
        if (
            _gateway_nodes[i].initiliazation.state == false
            && _gateway_nodes[i].initiliazation.packet_time > 0
            && (millis() - _gateway_nodes[i].initiliazation.packet_time) > NODES_GATEWAY_INIT_REPLY_DELAY
        ) {
            _gateway_nodes[i].initiliazation.waiting_reply = GATEWAY_PACKET_NONE;
            _gateway_nodes[i].initiliazation.packet_time = 0;
        }
    }
}

// -----------------------------------------------------------------------------
// DIGITAL REGISTERS READING
// -----------------------------------------------------------------------------

uint8_t _gatewayReadNodeDigitalRegisters(
    uint8_t packetId,
    uint8_t id,
    uint8_t start,
    uint8_t registerSize
) {
    char output_content[5];

    output_content[0] = packetId;

    // Register reading start address
    output_content[1] = (char) (start >> 8);
    output_content[2] = (char) (start & 0xFF);

    // It is based on maximum packed size reduced by packet header (5 bytes)
    uint8_t max_readable_addresses = _gateway_nodes[id].max_packet_length - 5;

    // Node has less digital inputs than one packet can handle
    if (registerSize <= max_readable_addresses) {
        // Register reading lenght
        output_content[3] = (char) (registerSize >> 8);
        output_content[4] = (char) (registerSize & 0xFF);

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

    _gatewaySendPacket(
        (id + 1),
        output_content,
        5
    );

    return start;
}


// -----------------------------------------------------------------------------

void _gatewayReadNodeDigitalInputs(
    uint8_t id
) {
    uint8_t set_start = _gatewayReadNodeDigitalRegisters(
        GATEWAY_PACKET_READ_MULTI_DI,
        id,
        _gateway_nodes[id].digital_inputs_reading.start,
        _gateway_nodes[id].digital_inputs.size()
    );

    _gateway_nodes[id].digital_inputs_reading.start = set_start;

    if (set_start == 0) {
        _gateway_nodes[id].digital_inputs_reading.delay = millis();
    }
}

// -----------------------------------------------------------------------------

void _gatewayReadNodeDigitalOutputs(
    uint8_t id
) {
    uint8_t set_start = _gatewayReadNodeDigitalRegisters(
        GATEWAY_PACKET_READ_MULTI_DO,
        id,
        _gateway_nodes[id].digital_outputs_reading.start,
        _gateway_nodes[id].digital_outputs.size()
    );

    _gateway_nodes[id].digital_outputs_reading.start = set_start;

    if (set_start == 0) {
        _gateway_nodes[id].digital_outputs_reading.delay = millis();
    }
}

// -----------------------------------------------------------------------------
// ANALOG REGISTERS READING
// -----------------------------------------------------------------------------

void _gatewayReadNodeAnalogInputs(
    uint8_t id
) {

}

// -----------------------------------------------------------------------------

void _gatewayReadNodeAnalogOutputs(
    uint8_t id
) {

}

// -----------------------------------------------------------------------------
// DIGITAL REGISTERS WRITING
// -----------------------------------------------------------------------------

void _gatewayWriteNodeDigitalOutputs(
    uint8_t id,
    uint8_t registerAddress,
    bool value
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

    _gatewaySendPacket(
        (id + 1),
        output_content,
        5
    );
}

// -----------------------------------------------------------------------------
// ANALOG REGISTERS WRITING
// -----------------------------------------------------------------------------

// TODO

// -----------------------------------------------------------------------------

/**
 * Read data registers from node
 */
void _gatewayReadNodes()
{
    if (_gateway_reading_node_index > 0 && _gateway_reading_node_index >= NODES_GATEWAY_MAX_NODES) {
        _gateway_reading_node_index = 0;
    }

    for (uint8_t i = _gateway_reading_node_index; i < NODES_GATEWAY_MAX_NODES; i++) {
        if (
            _gateway_nodes[i].initiliazation.state == true
            && (
                _gateway_nodes[i].digital_inputs.size() > 0
                || _gateway_nodes[i].digital_outputs.size() > 0
                || _gateway_nodes[i].analog_inputs.size() > 0
                || _gateway_nodes[i].analog_outputs.size() > 0
            )
        ) {
            _gateway_reading_node_index = i;

            if (
                _gateway_nodes[i].digital_inputs.size() > 0
                && (
                    _gateway_nodes[i].digital_inputs_reading.delay == 0
                    || (millis() - _gateway_nodes[i].digital_inputs_reading.delay) > NODES_GATEWAY_DI_READING_INTERVAL
                )
            ) {
                _gatewayReadNodeDigitalInputs(i);

            } else if (
                _gateway_nodes[i].digital_outputs.size() > 0
                && (
                    _gateway_nodes[i].digital_outputs_reading.delay == 0
                    || (millis() - _gateway_nodes[i].digital_outputs_reading.delay) > NODES_GATEWAY_DO_READING_INTERVAL
                )
            ) {
                _gatewayReadNodeDigitalOutputs(i);

            } else if (
                _gateway_nodes[i].analog_inputs.size() > 0
                && (
                    _gateway_nodes[i].analog_inputs_reading.delay == 0
                    || (millis() - _gateway_nodes[i].analog_inputs_reading.delay) > NODES_GATEWAY_AI_READING_INTERVAL
                )
            ) {
                _gatewayReadNodeAnalogInputs(i);

            } else if (
                _gateway_nodes[i].analog_outputs.size() > 0
                && (
                    _gateway_nodes[i].analog_outputs_reading.delay == 0
                    || (millis() - _gateway_nodes[i].analog_outputs_reading.delay) > NODES_GATEWAY_AO_READING_INTERVAL
                )
            ) {
                _gatewayReadNodeAnalogOutputs(i);
            }

            return;
        }
    }
}

// -----------------------------------------------------------------------------

void _gatewayNodeReadDigitalOutputsHandler(
    uint8_t address,
    uint8_t * payload
) {
    word register_address = (word) payload[1] << 8 | (word) payload[2];
    uint8_t bytes_length = (uint8_t) payload[3];

    DEBUG_MSG(PSTR("[GATEWAY] Requested write to digital buffer for node at address: %d to register: %d with byte length: %d\n"), address, register_address, bytes_length);

    if (
        // Read start address mus be between <0, buffer.size()>
        register_address < _gateway_nodes[(address - 1)].digital_outputs.size()
    ) {
        uint8_t write_byte = 1;
        uint8_t data_byte;

        uint8_t write_address = register_address;

        while (
            write_address < _gateway_nodes[(address - 1)].digital_outputs.size()
            && write_byte <= bytes_length
        ) {
            data_byte = (uint8_t) payload[3 + write_byte];
            bool write_value = false;

            for (uint8_t i = 0; i < 8; i++) {
                write_value = (data_byte >> i) & 0x01 ? true : false;

                if (_gateway_nodes[(address - 1)].digital_outputs[write_address] != write_value) {
                    _gateway_nodes[(address - 1)].digital_outputs[write_address] = write_value;

                    DEBUG_MSG(PSTR("[GATEWAY] Value was written into digital register at address: %d\n"), write_address);

                } else {
                    DEBUG_MSG(PSTR("[GATEWAY] Value to write into digital register at address: %d is same as stored. Write skipped\n"), write_address);
                }

                write_address++;

                if (write_address >= _gateway_nodes[(address - 1)].digital_outputs.size()) {
                    break;
                }
            }

            write_byte++;
        }

    } else {
        DEBUG_MSG(PSTR("[GATEWAY][ERR] Node is trying to write to undefined DO register address\n"));
    }
}

// -----------------------------------------------------------------------------

void _gatewayNodeWriteDigitalOutputHandler(
    uint8_t address,
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
        register_address < _gateway_nodes[(address - 1)].digital_outputs.size()
    ) {
        if (_gateway_nodes[(address - 1)].digital_outputs[register_address] != (bool) write_value) {
            _gateway_nodes[(address - 1)].digital_outputs[register_address] = (bool) write_value;
            DEBUG_MSG(PSTR("[GATEWAY] Value was written into digital register\n"));

        } else {
            DEBUG_MSG(PSTR("[GATEWAY] Value to write into digital register is same as stored. Write skipped\n"));
        }

    } else {
        DEBUG_MSG(PSTR("[GATEWAY][ERR] Node is trying to write to undefined DO register address\n"));
    }
}

// -----------------------------------------------------------------------------

void _gatewayNodeWriteMultipleDigitalOutputsHandler(
    uint8_t address,
    uint8_t * payload
) {
    word register_address = (word) payload[1] << 8 | (word) payload[2];
    word read_length = (word) payload[3] << 8 | (word) payload[4];
    uint8_t bytes_length = (uint8_t) payload[5];

    if (
        // Write start address mus be between <0, buffer.size()>
        register_address < _gateway_nodes[(address - 1)].digital_outputs.size()
        // Write length have to be same or smaller as registers size
        && (register_address + read_length) <= _gateway_nodes[(address - 1)].digital_outputs.size()
    ) {
        _gatewayReadNodeDigitalOutputs((address - 1));

    } else {
        DEBUG_MSG(PSTR("[GATEWAY][ERR] Node is trying to write to undefined DO registers range\n"));
    }
}

// -----------------------------------------------------------------------------
// NODE ADDRESS HANDLER
// -----------------------------------------------------------------------------

void _gatewayRemoveNode(
    uint8_t address = 0
) {
    // Remove all nodes
    if (address == 0) {
        for (uint8_t i = 0; i < NODES_GATEWAY_MAX_NODES; i++) {
            // Remove only node which is successfully addressed
            if (_gateway_nodes[i].addressing.state != false) {
                _gatewayResetNodeIndex(i);
            }
        }

    // Remove selected node
    } else if (address > 0 && address < NODES_GATEWAY_MAX_NODES) {
        // Remove only node which is successfully addressed
        if (_gateway_nodes[address - 1].addressing.state != false) {
            _gatewayResetNodeIndex(address - 1);
        }
    }

    #if WEB_SUPPORT && WS_SUPPORT
        // Propagate nodes structure to WS clients
        wsSend(_nodesWebSocketUpdate);
    #endif
}

// -----------------------------------------------------------------------------

bool _gatewayIsNodeSerialNumberUnique(
    char * nodeSerialNumber
) {
    for (uint8_t i = 0; i < NODES_GATEWAY_MAX_NODES; i++) {
        if (strcmp(_gateway_nodes[i].node, nodeSerialNumber) == 0) {
            DEBUG_MSG(PSTR("[GATEWAY][WARN] Nodes serial number is not unique\n"));

            return false;
        }
    }

    return true;
}

// -----------------------------------------------------------------------------

uint16_t _gatewayReserveNodeAddress(
    char * nodeSerialNumber
) {
    if (!_gatewayIsNodeSerialNumberUnique(nodeSerialNumber)) {
        return NODES_GATEWAY_FAIL;
    }

    for (uint8_t i = 0; i < NODES_GATEWAY_MAX_NODES; i++) {
        // Search for free node slot
        if (_gateway_nodes[i].addressing.state == false && strcmp(_gateway_nodes[i].node, GATEWAY_DESCRIPTION_NOT_SET) == 0) {
            // Add reservation stamp
            _gateway_nodes[i].addressing.registration = millis();
            // Add node RID
            strcpy(_gateway_nodes[i].node, nodeSerialNumber);

            return i + 1;
        }
    }

    DEBUG_MSG(PSTR("[GATEWAY][WARN] Nodes registry is full. No other nodes could be added.\n"));

    return NODES_GATEWAY_NODES_BUFFER_FULL;
}

// -----------------------------------------------------------------------------

/**
 * PAYLOAD:
 * 0    => Packet identifier
 * 1    => Node bus address
 * 2    => Max packet size
 * 3    => Node SN length
 * 4-n  => Node parsed SN
 */
void _gatewaySearchNodeRequestHandler(
    uint8_t senderAddress,
    uint8_t * payload,
    uint16_t length
) {
    // Extract address returned by node
    uint8_t address = (uint8_t) payload[1];

    uint8_t max_packet_length = (uint8_t) payload[2];

    char node_sn[(uint8_t) payload[3]];

    // Extract node serial number from payload
    for (uint8_t i = 0; i < (uint8_t) payload[3]; i++) {
        node_sn[i] = (char) payload[i + 4];
    }

    // Node has allready assigned bus address
    if (address != PJON_NOT_ASSIGNED) {
        if (address != senderAddress) {
            DEBUG_MSG(PSTR("[GATEWAY][ERR] Node confirmed to node search but with addressing mismatch\n"));
            return;
        }

        if (address >= NODES_GATEWAY_MAX_NODES) {
            DEBUG_MSG(PSTR("[GATEWAY][ERR] Node confirmed to node search but with address out of available range\n"));
            return;
        }

        if (strcmp(_gateway_nodes[address - 1].node, node_sn) != 0 && strcmp(_gateway_nodes[address - 1].node, GATEWAY_DESCRIPTION_NOT_SET) != 0) {
            DEBUG_MSG(PSTR("[GATEWAY][ERR] Node confirmed to node search but with serial number mismatch\n"));
            return;
        }

        // Node addressing finished
        _gateway_nodes[address - 1].addressing.state = true;
        _gateway_nodes[address - 1].addressing.registration = 0;

        strncpy(_gateway_nodes[address - 1].node, node_sn, (uint8_t) payload[3]);
        _gateway_nodes[address - 1].max_packet_length = max_packet_length;

        DEBUG_MSG(PSTR("[GATEWAY] Addressing for node: %s was successfully finished. Previously assigned address is: %d\n"), (char *) node_sn, address);

    // Node is new without bus address
    } else {
        // Check node serial number if is unique & get free address slot
        uint16_t reserved_address = _gatewayReserveNodeAddress(node_sn);

        // Maximum nodes count reached
        if (reserved_address == NODES_GATEWAY_NODES_BUFFER_FULL) {
            return;
        }

        // Node SN is allready used in registry
        if (reserved_address == NODES_GATEWAY_FAIL) {
            DEBUG_MSG(PSTR("[GATEWAY][ERR] Node: %s is allready in registry\n"), (char *) node_sn);

            return;
        }

        strncpy(_gateway_nodes[reserved_address - 1].node, node_sn, (uint8_t) payload[3]);
        _gateway_nodes[reserved_address - 1].max_packet_length = max_packet_length;

        DEBUG_MSG(PSTR("[GATEWAY] New node: %s was successfully added to registry with address: %d\n"), (char *) node_sn, reserved_address);

        // Store start timestamp
        uint32_t time = millis();

        char output_content[PJON_PACKET_MAX_LENGTH];

        // 0    => Packet identifier
        // 1    => Node reserved bus address
        // 2    => Node SN length
        // 3-n  => Node parsed SN
        output_content[0] = (uint8_t) GATEWAY_PACKET_NODE_ADDRESS_CONFIRM;
        output_content[1] = (uint8_t) reserved_address;
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
}

// -----------------------------------------------------------------------------

/**
 * PAYLOAD:
 * 0    => Packet identifier
 * 1    => Node bus address
 * 2    => Node SN length
 * 3-n  => Node parsed SN
 */
void _gatewayConfirmNodeRequestHandler(
    uint8_t senderAddress,
    uint8_t * payload
) {
    uint8_t address = (uint8_t) payload[1];

    char node_sn[(uint8_t) payload[2]];

    // Extract node serial number from payload
    for (uint8_t i = 0; i < (uint8_t) payload[2]; i++) {
        node_sn[i] = (char) payload[i + 3];
    }

    // Node has allready assigned bus address
    if (address != PJON_NOT_ASSIGNED) {
        if (address != senderAddress) {
            DEBUG_MSG(PSTR("[GATEWAY][ERR] Node confirmed address acceptation but with addressing mismatch\n"));
            return;
        }

        if (address >= NODES_GATEWAY_MAX_NODES) {
            DEBUG_MSG(PSTR("[GATEWAY][ERR] Node confirmed address acceptation but with address out of available range\n"));
            return;
        }

        if (strcmp(_gateway_nodes[address - 1].node, node_sn) != 0) {
            DEBUG_MSG(PSTR("[GATEWAY][ERR] Node confirmed address acceptation but with serial number mismatch\n"));
            return;
        }

        // Node addressing finished
        _gateway_nodes[address - 1].addressing.state = true;
        _gateway_nodes[address - 1].addressing.registration = 0;

        DEBUG_MSG(PSTR("[GATEWAY] Addressing for node: %s was successfully finished. Assigned address is: %d\n"), (char *) node_sn, address);

    } else {
        DEBUG_MSG(PSTR("[GATEWAY][ERR] Node confirmed address acceptation but without setting node address\n"));
    }
}

// -----------------------------------------------------------------------------

void _gatewayAddressRequestHandler(
    uint8_t packetId,
    uint8_t address,
    uint8_t * payload,
    uint16_t length
) {
    switch (packetId)
    {
        /**
         * Node has replied to search request
         */
        case GATEWAY_PACKET_SEARCH_NODES:
            _gatewaySearchNodeRequestHandler(address, payload, length);
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
            // TODO
            break;
    }
}

// -----------------------------------------------------------------------------
// NODE INITIALIZATION
// -----------------------------------------------------------------------------

/**
 * Parse received payload - Node description
 * 
 * 0    => Received packet identifier   => GATEWAY_PACKET_HW_MODEL|GATEWAY_PACKET_HW_MANUFACTURER|GATEWAY_PACKET_HW_VERSION|GATEWAY_PACKET_FW_MODEL|GATEWAY_PACKET_FW_MANUFACTURER|GATEWAY_PACKET_FW_VERSION
 * 1    => Description string length    => 1-255
 * 2-n  => Description content          => char array (a,b,c,...)
 */
void _gatewayExtractAndStoreDescription(
    uint8_t packetId,
    uint8_t id,
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
        case GATEWAY_PACKET_HW_MODEL:
            _gateway_nodes[id].initiliazation.step = GATEWAY_NODE_INIT_HW_MANUFACTURER;
            strncpy(_gateway_nodes[id].hardware.model, content, content_length);
            break;

        case GATEWAY_PACKET_HW_MANUFACTURER:
            _gateway_nodes[id].initiliazation.step = GATEWAY_NODE_INIT_HW_VERSION;
            strncpy(_gateway_nodes[id].hardware.manufacturer, content, content_length);
            break;

        case GATEWAY_PACKET_HW_VERSION:
            _gateway_nodes[id].initiliazation.step = GATEWAY_NODE_INIT_FW_MODEL;
            strncpy(_gateway_nodes[id].hardware.version, content, content_length);
            break;

        case GATEWAY_PACKET_FW_MODEL:
            _gateway_nodes[id].initiliazation.step = GATEWAY_NODE_INIT_FW_MANUFACTURER;
            strncpy(_gateway_nodes[id].firmware.model, content, content_length);
            break;

        case GATEWAY_PACKET_FW_MANUFACTURER:
            _gateway_nodes[id].initiliazation.step = GATEWAY_NODE_INIT_FW_VERSION;
            strncpy(_gateway_nodes[id].firmware.manufacturer, content, content_length);
            break;

        case GATEWAY_PACKET_FW_VERSION:
            _gateway_nodes[id].initiliazation.step = GATEWAY_NODE_INIT_REGISTERS;
            strncpy(_gateway_nodes[id].firmware.version, content, content_length);
            break;

        // Unknown init sequence
        default:
            return;
    }

    DEBUG_MSG(PSTR("[GATEWAY] Initialization step for node with address: %d is updated and set to: %d\n"), (id + 1), _gateway_nodes[id].initiliazation.step);

    _gateway_nodes[id].initiliazation.waiting_reply = GATEWAY_PACKET_NONE;
    _gateway_nodes[id].initiliazation.attempts = 0;
    _gateway_nodes[id].initiliazation.packet_time = 0;
}

// -----------------------------------------------------------------------------

void _gatewayNodeInitializationHandler(
    uint8_t packetId,
    uint8_t address,
    uint8_t * payload
) {
    // Check if gateway is waiting for reply from node (initiliazation sequence)
    if (_gateway_nodes[(address - 1)].initiliazation.waiting_reply == GATEWAY_PACKET_NONE) {
        DEBUG_MSG(
            PSTR("[GATEWAY][ERR] Received packet for node with address: %d but gateway is not waiting for packet from this node\n"),
            address
        );

        return;
    }

    // Check if gateway is waiting for reply from node (initiliazation sequence)
    if (_gateway_nodes[(address - 1)].initiliazation.waiting_reply != packetId) {
        DEBUG_MSG(
            PSTR("[GATEWAY][ERR] Received packet: %s for node with address: %d but gateway is waiting for: %s\n"),
            _gatewayPacketName(packetId).c_str(),
            address,
            _gatewayPacketName(_gateway_nodes[(address - 1)].initiliazation.waiting_reply).c_str()
        );

        return;
    }

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
// REGISTERS INITIALIZATION HANDLER
// -----------------------------------------------------------------------------

/**
 * Parse received payload - Registers definitions
 * 
 * 0    => Received packet identifier   => GATEWAY_PACKET_REGISTERS_SIZE
 * 1    => DI buffer size               => 0-255
 * 2    => DO buffer size               => 0-255
 * 3    => AI buffer size               => 0-255
 * 4    => AO buffer size               => 0-255
 */
void _gatewayExtractAndStoreRegistersDefinitions(
    uint8_t id,
    uint8_t * payload
) {
    std::vector<bool> reset_digital_inputs;
    _gateway_nodes[id].digital_inputs = reset_digital_inputs;

    std::vector<bool> reset_digital_outputs;
    _gateway_nodes[id].digital_outputs = reset_digital_outputs;

    std::vector<word> reset_analog_inputs;
    _gateway_nodes[id].analog_inputs = reset_analog_inputs;

    std::vector<word> reset_analog_outputs;
    _gateway_nodes[id].analog_outputs = reset_analog_outputs;

    for (uint8_t i = 0; i < (uint8_t) payload[1]; i++) {
        _gateway_nodes[id].digital_inputs.push_back(false);
        _gateway_nodes[id].digital_inputs_reading.start = 0;
        _gateway_nodes[id].digital_inputs_reading.delay = 0;
    }
    
    for (uint8_t i = 0; i < (uint8_t) payload[2]; i++) {
        _gateway_nodes[id].digital_outputs.push_back(false);
        _gateway_nodes[id].digital_outputs_reading.start = 0;
        _gateway_nodes[id].digital_outputs_reading.delay = 0;
    }

    for (uint8_t i = 0; i < (uint8_t) payload[3]; i++) {
        _gateway_nodes[id].analog_inputs.push_back(0);
    }
    
    for (uint8_t i = 0; i < (uint8_t) payload[4]; i++) {
        _gateway_nodes[id].analog_outputs.push_back(0);
    }

    // Node initiliazation successfully finished
    _gatewayMarkNodeAsInitialized(id);

    DEBUG_MSG(
        PSTR("[GATEWAY] Received node registers structure (DI: %d, DO: %d, AI: %d, AO: %d) for node with address: %d\n"),
        _gateway_nodes[id].digital_inputs.size(),
        _gateway_nodes[id].digital_outputs.size(),
        _gateway_nodes[id].analog_inputs.size(),
        _gateway_nodes[id].analog_outputs.size(),
        (id + 1)
    );
}

// -----------------------------------------------------------------------------

void _gatewayRegistersInitializationHandler(
    uint8_t packetId,
    uint8_t address,
    uint8_t * payload
) {
    // Check if gateway is waiting for reply from node (initiliazation sequence)
    if (_gateway_nodes[(address - 1)].initiliazation.waiting_reply == GATEWAY_PACKET_NONE) {
        DEBUG_MSG(
            PSTR("[GATEWAY][ERR] Received packet for node with address: %d but gateway is not waiting for packet from this node\n"),
            address
        );

        return;
    }

    // Check if gateway is waiting for reply from node (initiliazation sequence)
    if (_gateway_nodes[(address - 1)].initiliazation.waiting_reply != packetId) {
        DEBUG_MSG(
            PSTR("[GATEWAY][ERR] Received packet: %s for node with address: %d but gateway is waiting for: %s\n"),
            _gatewayPacketName(packetId).c_str(),
            address,
            _gatewayPacketName(_gateway_nodes[(address - 1)].initiliazation.waiting_reply).c_str()
        );

        return;
    }

    switch (packetId)
    {
        case GATEWAY_PACKET_REGISTERS_SIZE:
            _gatewayExtractAndStoreRegistersDefinitions((address - 1), payload);
            break;

        case GATEWAY_PACKET_DI_REGISTERS_STRUCTURE:
            break;

        case GATEWAY_PACKET_DO_REGISTERS_STRUCTURE:
            break;

        case GATEWAY_PACKET_AI_REGISTERS_STRUCTURE:
            break;

        case GATEWAY_PACKET_AO_REGISTERS_STRUCTURE:
            break;
    }
}

// -----------------------------------------------------------------------------
// COMMUNICATION HANDLERS
// -----------------------------------------------------------------------------

void _gatewayReceiveHandler(
    uint8_t * payload,
    uint16_t length,
    const PJON_Packet_Info &packetInfo
) {
    uint8_t sender_address = PJON_NOT_ASSIGNED;

    // Get sender address from header
    if (packetInfo.header & PJON_TX_INFO_BIT) {
        sender_address = packetInfo.sender_id;
    }

    // Get packet identifier from payload
    uint8_t packet_id = (uint8_t) payload[0];

    DEBUG_MSG(PSTR("[GATEWAY] Received packet: %s for node with address: %d\n"), _gatewayPacketName(packet_id).c_str(), sender_address);

    // Node is trying to acquire address
    if (_gatewayIsPacketInGroup(packet_id, gateway_packets_addresing, GATEWAY_PACKET_ADDRESS_MAX)) {
        _gatewayAddressRequestHandler(packet_id, sender_address, payload, length);

    // Node send reply to request
    } else {
        if (sender_address == PJON_NOT_ASSIGNED) {
            DEBUG_MSG(PSTR("[GATEWAY][ERR] Received packet is without sender address\n"));

            return;
        }

        if (_gatewayIsPacketInGroup(packet_id, gateway_packets_node_initialization, GATEWAY_PACKET_NODE_INIT_MAX)) {
            _gatewayNodeInitializationHandler(packet_id, sender_address, payload);

        } else if (_gatewayIsPacketInGroup(packet_id, gateway_packets_registers_initialization, GATEWAY_PACKET_REGISTERS_INIT_MAX)) {
            _gatewayRegistersInitializationHandler(packet_id, sender_address, payload);

        } else {
            switch (packet_id)
            {
                case GATEWAY_PACKET_READ_SINGLE_DI:
                    break;

                case GATEWAY_PACKET_READ_MULTI_DI:
                    break;

                case GATEWAY_PACKET_READ_SINGLE_DO:
                    break;

                case GATEWAY_PACKET_READ_MULTI_DO:
                    _gatewayNodeReadDigitalOutputsHandler(sender_address, payload);
                    break;

                case GATEWAY_PACKET_READ_AI:
                    break;

                case GATEWAY_PACKET_READ_AO:
                    break;

                case GATEWAY_PACKET_WRITE_ONE_DO:
                    _gatewayNodeWriteDigitalOutputHandler(sender_address, payload);
                    break;

                case GATEWAY_PACKET_WRITE_ONE_AO:
                    break;

                case GATEWAY_PACKET_WRITE_MULTI_DO:
                    _gatewayNodeWriteMultipleDigitalOutputsHandler(sender_address, payload);
                    break;
            }
        }
    }
}

// -----------------------------------------------------------------------------

void _gatewayErrorHandler(
    uint8_t code,
    uint16_t data,
    void *customPointer
) {
    if (code == PJON_CONNECTION_LOST) {
        _gatewayRemoveNode(_gateway_bus.packets[data].content[0]);

        DEBUG_MSG(PSTR("[GATEWAY][ERR] Connection lost with node\n"));

    } else if (code == PJON_PACKETS_BUFFER_FULL) {
        DEBUG_MSG(PSTR("[GATEWAY][ERR] Buffer is full\n"));

    } else if (code == PJON_CONTENT_TOO_LONG) {
        DEBUG_MSG(PSTR("[GATEWAY][ERR] Content is long\n"));

    } else {
        DEBUG_MSG(PSTR("[GATEWAY][ERR] Unknown error\n"));
    }
}

// -----------------------------------------------------------------------------

bool _gatewayIsPacketInGroup(
    uint8_t packetId,
    const int * group,
    uint8_t length
) {
    for (uint8_t i = 0; i < length; i++) {
        if ((uint8_t) pgm_read_byte(&group[i]) == packetId) {
            return true;
        }
    }

    return false;
}

// -----------------------------------------------------------------------------

uint8_t _gatewayGetPacketIndexInGroup(
    uint8_t packetId,
    const int * group,
    uint8_t length
) {
    for (uint8_t i = 0; i < length; i++) {
        if ((uint8_t) pgm_read_byte(&group[i]) == packetId) {
            return i;
        }
    }

    return 0;
}

// -----------------------------------------------------------------------------

String _gatewayPacketName(
    uint8_t packetId
) {
    char buffer[50] = {0};

    if (_gatewayIsPacketInGroup(packetId, gateway_packets_addresing, GATEWAY_PACKET_ADDRESS_MAX)) {
        strncpy_P(buffer, gateway_packets_addresing_string[_gatewayGetPacketIndexInGroup(packetId, gateway_packets_addresing, GATEWAY_PACKET_ADDRESS_MAX)], sizeof(buffer));

    } else if (_gatewayIsPacketInGroup(packetId, gateway_packets_node_initialization, GATEWAY_PACKET_NODE_INIT_MAX)) {
        strncpy_P(buffer, gateway_packets_node_initialization_string[_gatewayGetPacketIndexInGroup(packetId, gateway_packets_node_initialization, GATEWAY_PACKET_NODE_INIT_MAX)], sizeof(buffer));

    } else if (_gatewayIsPacketInGroup(packetId, gateway_packets_registers_initialization, GATEWAY_PACKET_REGISTERS_INIT_MAX)) {
        strncpy_P(buffer, gateway_packets_registers_initialization_string[_gatewayGetPacketIndexInGroup(packetId, gateway_packets_registers_initialization, GATEWAY_PACKET_REGISTERS_INIT_MAX)], sizeof(buffer));

    } else if (_gatewayIsPacketInGroup(packetId, gateway_packets_registers_reading, GATEWAY_PACKET_REGISTERS_REDING_MAX)) {
        strncpy_P(buffer, gateway_packets_registers_reading_string[_gatewayGetPacketIndexInGroup(packetId, gateway_packets_registers_reading, GATEWAY_PACKET_REGISTERS_REDING_MAX)], sizeof(buffer));

    } else if (_gatewayIsPacketInGroup(packetId, gateway_packets_registers_writing, GATEWAY_PACKET_REGISTERS_WRITING_MAX)) {
        strncpy_P(buffer, gateway_packets_registers_writing_string[_gatewayGetPacketIndexInGroup(packetId, gateway_packets_registers_writing, GATEWAY_PACKET_REGISTERS_WRITING_MAX)], sizeof(buffer));

    } else if (_gatewayIsPacketInGroup(packetId, gateway_packets_misc, GATEWAY_PACKET_MISC_MAX)) {
        strncpy_P(buffer, gateway_packets_misc_string[_gatewayGetPacketIndexInGroup(packetId, gateway_packets_misc, GATEWAY_PACKET_MISC_MAX)], sizeof(buffer));

    } else {
        strncpy_P(buffer, "unknown", sizeof(buffer));
    }
    
    return String(buffer);
}

// -----------------------------------------------------------------------------

bool _gatewaySendPacket(
    uint8_t address,
    char * payload,
    uint8_t length
) {
    uint16_t result = _gateway_bus.send_packet_blocking(
        address,    // Master address
        payload,    // Content
        length      // Content length
    );

    if (result != PJON_ACK) {
        if (result == PJON_BUSY ) {
            DEBUG_MSG(PSTR("[GATEWAY][ERR] Sending packet: %s for node: %d failed, bus is busy\n"), _gatewayPacketName(payload[0]).c_str(), address);

        } else if (result == PJON_FAIL) {
            DEBUG_MSG(PSTR("[GATEWAY][ERR] Sending packet: %s for node: %d failed\n"), _gatewayPacketName(payload[0]).c_str(), address);

        } else {
            DEBUG_MSG(PSTR("[GATEWAY][ERR] Sending packet: %s for node: %d failed, unknonw error\n"), _gatewayPacketName(payload[0]).c_str(), address);
        }

        return false;
    }

    if (address == PJON_BROADCAST) {
        DEBUG_MSG(PSTR("[GATEWAY] Successfully sent broadcast packet: %s\n"), _gatewayPacketName(payload[0]).c_str());

    } else {
        DEBUG_MSG(PSTR("[GATEWAY] Successfully sent packet: %s for node: %d\n"), _gatewayPacketName(payload[0]).c_str(), address);
    }

    return true;
}

// -----------------------------------------------------------------------------

/**
 * Gateway searching for active nodes
 */
void _gatewaySearchForNodes()
{
    // Store start timestamp
    uint32_t time = millis();

    char output_content[1];

    output_content[0] = GATEWAY_PACKET_SEARCH_NODES;

    _gatewaySendPacket(
        PJON_BROADCAST,
        output_content,
        1
    );

    while((millis() - time) <= NODES_GATEWAY_LIST_ADDRESSES_TIME) {
        if (_gateway_bus.receive() == PJON_ACK) {
            return;
        }
    }
}

// -----------------------------------------------------------------------------

/**
 * Iterate all registered nodes and PING them
 * If node is not able to receive PING request, remove it from list
 */
void _gatewayCheckNodesPresence()
{
    char output_content[1];

    uint8_t counter = 0;

    // Packet identifier at first postion
    output_content[0] = GATEWAY_PACKET_GATEWAY_PING;
    
    DEBUG_MSG(PSTR("[GATEWAY] Checking node presence\n"));

    // Process all nodes
    for (uint8_t i = 0; i < NODES_GATEWAY_MAX_NODES; i++) {
        if (_gateway_nodes[i].addressing.state == true) {
            // Check if node can receive PING
            if (
                _gatewaySendPacket(
                    i + 1,              // Node address
                    output_content,     // Payload
                    1                   // Payload length
                ) != true
            ) {
                DEBUG_MSG(PSTR("[GATEWAY] Node with address: %d is lost\n"), (i + 1));

                _gatewayRemoveNode(i + 1);

            } else {
                counter++;
            }
        }
    }

    if (counter > 0) {
        DEBUG_MSG(PSTR("[GATEWAY] Sent PING to: %d nodes\n"), counter);

    } else {
        DEBUG_MSG(PSTR("[GATEWAY] No nodes registered to gateway\n"));
    }
}

// -----------------------------------------------------------------------------

uint8_t _gatewayGetNodeAddressToInitialize() {
    for (uint8_t i = 0; i < NODES_GATEWAY_MAX_NODES; i++) {
        if (
            // Check if node has finished addressing procedure
            _gateway_nodes[i].addressing.state == true
            // Check if node is not initialized
            && _gateway_nodes[i].initiliazation.state == false
            && _gateway_nodes[i].initiliazation.waiting_reply == GATEWAY_PACKET_NONE
        ) {
            // Attempts counter reached maximum
            if (_gateway_nodes[i].initiliazation.attempts > NODES_GATEWAY_MAX_INIT_ATTEMPTS) {
                // Set delay
                if (_gateway_nodes[i].initiliazation.delay == 0) {
                    _gateway_nodes[i].initiliazation.delay = millis();

                // If the delay is finished...
                } else if ((millis() - _gateway_nodes[i].initiliazation.delay) >= NODES_GATEWAY_INIT_DELAY) {
                    // ...reset it and continue in initiliazation
                    _gateway_nodes[i].initiliazation.attempts = 0;
                    _gateway_nodes[i].initiliazation.delay = 0;

                    return (i + 1);
                }

            } else {
                return (i + 1);
            }
        }
    }

    return 0;
}

// -----------------------------------------------------------------------------

void _gatewayMarkNodeAsInitialized(
    uint8_t id
) {
    _gateway_nodes[id].initiliazation.step = GATEWAY_NODE_INIT_FINISHED;
    _gateway_nodes[id].initiliazation.state = true;
    _gateway_nodes[id].initiliazation.attempts = 0;
    _gateway_nodes[id].initiliazation.waiting_reply = GATEWAY_PACKET_NONE;
    _gateway_nodes[id].initiliazation.packet_time = 0;

    #if WEB_SUPPORT && WS_SUPPORT
        // Propagate nodes structure to WS clients
        wsSend(_nodesWebSocketUpdate);
    #endif
}

// -----------------------------------------------------------------------------

/**
 * Send initiliaziation packet to node
 */
void _gatewaySendInitializationPacket(
    uint8_t id,
    uint8_t requestState
) {
    char output_content[1];

    // Packet identifier at first postion
    output_content[0] = requestState;

    // Send packet to node
    bool result = _gatewaySendPacket((id + 1), output_content, 1);

    // When successfully sent...
    if (result == true) {
        // ...add mark, that gateway is waiting for reply from node
        _gateway_nodes[id].initiliazation.waiting_reply = requestState;
        _gateway_nodes[id].initiliazation.packet_time = millis();
        _gateway_nodes[id].initiliazation.attempts = _gateway_nodes[id].initiliazation.attempts + 1;
    }
}

// -----------------------------------------------------------------------------

/**
 * Get first not initialized node from the list & try to continue in initiliazation
 */
void _gatewayContinueInNodeInitialization(
    uint8_t address
) {
    // Convert node address to index
    uint8_t index = address - 1;

    if (
        // Check if node has finished addressing procedure
        _gateway_nodes[index].addressing.state == true
        // Check if node is not initialized
        && _gateway_nodes[index].initiliazation.state == false
        && _gateway_nodes[index].initiliazation.attempts <= NODES_GATEWAY_MAX_INIT_ATTEMPTS
        && _gateway_nodes[index].initiliazation.waiting_reply == GATEWAY_PACKET_NONE
    ) {
        switch (_gateway_nodes[index].initiliazation.step)
        {
            // Request hw model description from node
            case GATEWAY_NODE_INIT_HW_MODEL:
                _gatewaySendInitializationPacket(index, GATEWAY_PACKET_HW_MODEL);
                break;

            // Request hw manufacturer name from node
            case GATEWAY_NODE_INIT_HW_MANUFACTURER:
                _gatewaySendInitializationPacket(index, GATEWAY_PACKET_HW_MANUFACTURER);
                break;

            // Request hw version from node
            case GATEWAY_NODE_INIT_HW_VERSION:
                _gatewaySendInitializationPacket(index, GATEWAY_PACKET_HW_VERSION);
                break;

            // Request fw model description from node
            case GATEWAY_NODE_INIT_FW_MODEL:
                _gatewaySendInitializationPacket(index, GATEWAY_PACKET_FW_MODEL);
                break;

            // Request fw name from node
            case GATEWAY_NODE_INIT_FW_MANUFACTURER:
                _gatewaySendInitializationPacket(index, GATEWAY_PACKET_FW_MANUFACTURER);
                break;

            // Request fw version from node
            case GATEWAY_NODE_INIT_FW_VERSION:
                _gatewaySendInitializationPacket(index, GATEWAY_PACKET_FW_VERSION);
                break;

            // Request registers description from node
            case GATEWAY_NODE_INIT_REGISTERS:
                _gatewaySendInitializationPacket(index, GATEWAY_PACKET_REGISTERS_SIZE);
                break;
        }
    }
}

// -----------------------------------------------------------------------------
// MODULE CORE
// -----------------------------------------------------------------------------

void gatewaySetup() {
    _gateway_serial_bus.begin(SERIAL_BAUDRATE);

    _gateway_bus.strategy.set_serial(&_gateway_serial_bus);

    // Communication callbacks
    _gateway_bus.set_receiver(_gatewayReceiveHandler);
    _gateway_bus.set_error(_gatewayErrorHandler);

    _gateway_bus.begin();

    #if WEB_SUPPORT && WS_SUPPORT
        wsOnConnectRegister(_nodesWSOnConnect);
        wsOnConfigureRegister(_nodesWSOnConfigure);
        wsOnActionRegister(_nodesWSOnAction);
    #endif

    firmwareRegisterLoop(gatewayLoop);
}

// -----------------------------------------------------------------------------

void gatewayLoop() {
    //_gatewayCheckPacketsDelays();

    if (
        _gateway_refresh_nodes == true
        || millis() < NODES_GATEWAY_ADDRESSING_TIMEOUT
    ) {
        _gatewaySearchForNodes();

    // Check nodes presence in given interval
    } else if (millis() - _gateway_last_nodes_check > NODES_GATEWAY_NODES_CHECK_INTERVAL) {
        _gateway_last_nodes_check = millis();

        _gatewayCheckNodesPresence();

    // Check if all connected nodes are initialized
    } else if (_gatewayGetNodeAddressToInitialize() != 0) {
        _gatewayContinueInNodeInitialization(_gatewayGetNodeAddressToInitialize());

    // Continue in nodes registers reading
    } else {
        _gatewayReadNodes();
    }

    _gateway_bus.receive(50000);
    _gateway_bus.update();
}

#endif // NODES_GATEWAY_SUPPORT