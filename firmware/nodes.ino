/*

NODES MODULE

Copyright (C) 2018 FastyBird s.r.o. <info@fastybird.com>

*/

#if NODES_GATEWAY_SUPPORT

#include <ArduinoJson.h>
#include <vector>
#include <SoftwareSerial.h>

PJON<ThroughSerial> _gateway_bus(PJON_MASTER_ID);

SoftwareSerial _gateway_serial_bus(NODES_GATEWAY_TX_PIN, NODES_GATEWAY_RX_PIN);

struct gateway_node_packet_t {
    uint8_t     max_length      = PJON_PACKET_MAX_LENGTH;
    uint8_t     waiting_for     = GATEWAY_PACKET_NONE;
};

struct gateway_node_description_t {
    char    manufacturer[20]    = GATEWAY_DESCRIPTION_NOT_SET;
    char    model[20]           = GATEWAY_DESCRIPTION_NOT_SET;
    char    version[10]         = GATEWAY_DESCRIPTION_NOT_SET;
};

struct gateway_node_initiliazation_t {
    bool        state           = false;
    uint8_t     step            = GATEWAY_PACKET_NONE;
    uint8_t     attempts        = 0;
    uint32_t    delay           = 0;        // In case maximum attempts is reached a wait delay is activated
};

struct gateway_node_searching_t {
    bool        state           = false;    // FALSE = node searching is not finished | TRUE = searching was successfully accepted by node
    uint32_t    registration    = 0;        // Timestamp when node requested for address
    uint8_t     attempts        = 0;
};

struct gateway_register_reading_t {
    uint8_t     register_type   = 0;
    uint8_t     start           = 0;
};

typedef struct {
    const char *    name;
    bool            value;
    bool            target_value;           // Holds the target status

    uint32_t        delay_on;               // Delay to turn relay ON
    uint32_t        delay_off;              // Delay to turn relay OFF

    uint32_t        fw_start;               // Flood window start time
    uint8_t         fw_count;               // Number of changes within the current flood window
    uint32_t        change_time;            // Scheduled time to change
} gateway_digital_register_t;

typedef struct {
    const char *    name;
    uint8_t         data_type;
    uint8_t         size;
    char            value[4];
} gateway_analog_register_t;

typedef struct {
    const char *    name;
    uint8_t         value;
} gateway_event_register_t;

struct gateway_node_t {
    // Node addressing process
    bool addressing = false;

    gateway_node_searching_t searching;

    // Node initiliazation process
    gateway_node_initiliazation_t initiliazation;

    // Node packet handling
    gateway_node_packet_t packet;

    // Node basic info
    char serial_number[15] = GATEWAY_DESCRIPTION_NOT_SET;

    gateway_node_description_t hardware;
    gateway_node_description_t firmware;

    // Data registers
    std::vector<gateway_digital_register_t> digital_inputs;
    std::vector<gateway_digital_register_t> digital_outputs;

    std::vector<gateway_analog_register_t> analog_inputs;
    std::vector<gateway_analog_register_t> analog_outputs;
    
    std::vector<gateway_event_register_t> event_inputs;

    uint8_t registers_size[5];

    // Data registers reading
    gateway_register_reading_t registers_reading;
};

gateway_node_t _gateway_nodes[NODES_GATEWAY_MAX_NODES];

typedef union {
    bool        number;
    uint8_t     bytes[4];
} BOOL_UNION_t;

typedef union {
    uint8_t     number;
    uint8_t     bytes[4];
} UINT8_UNION_t;

typedef union {
    uint16_t    number;
    uint8_t     bytes[4];
} UINT16_UNION_t;

typedef union {
    uint32_t    number;
    uint8_t     bytes[4];
} UINT32_UNION_t;

typedef union {
    int8_t      number;
    uint8_t     bytes[4];
} INT8_UNION_t;

typedef union {
    int16_t     number;
    uint8_t     bytes[4];
} INT16_UNION_t;

typedef union {
    int32_t     number;
    uint8_t     bytes[4];
} INT32_UNION_t;

typedef union {
    float       number;
    uint8_t     bytes[4];
} FLOAT32_UNION_t;

uint8_t _gateway_reading_node_index = 0;

#include "./nodes/storage.h"             // Module configuration storage
#include "./nodes/registers.h"           // Nodes registers methods
#include "./nodes/addressing.h"          // Nodes addressing methods
#include "./nodes/modules.h"             // 
#include "./nodes/initialization.h"      // Nodes initialiazation methods

// -----------------------------------------------------------------------------
// MODULE PRIVATE
// -----------------------------------------------------------------------------

bool _gatewayRegisterValueUpdated(
    const uint8_t id,
    const uint8_t dataRegister,
    const uint8_t address
) {
    DEBUG_MSG(PSTR("[GATEWAY] Node: %i register %d:%d updated\n"), (id + 1), dataRegister, address);

    #if FASTYBIRD_SUPPORT
        switch (dataRegister)
        {
            case GATEWAY_REGISTER_DI:
                _gatewayFastybirdReportDigitalRegisterValue(id, dataRegister, address, 0);
                break;

            case GATEWAY_REGISTER_DO:
                _gatewayFastybirdReportDigitalRegisterValue(id, dataRegister, address, 1);
                break;

            case GATEWAY_REGISTER_AI:
                _gatewayFastybirdReportAnalogRegisterValue(id, dataRegister, address, 2);
                break;

            case GATEWAY_REGISTER_AO:
                _gatewayFastybirdReportAnalogRegisterValue(id, dataRegister, address, 3);
                break;

            case GATEWAY_REGISTER_EV:
                _gatewayFastybirdReportEventRegisterValue(id, dataRegister, address, 4);
                break;
        }
    #endif
}

// -----------------------------------------------------------------------------

void _gatewayResetNodeIndex(
    const uint8_t id
) {
    // Reset addressing
    _gateway_nodes[id].addressing = false;

    _gateway_nodes[id].searching.state = false;
    _gateway_nodes[id].searching.registration = 0;
    _gateway_nodes[id].searching.attempts = 0;

    // Reset initialization steps
    _gateway_nodes[id].initiliazation.step = GATEWAY_PACKET_NONE;
    _gateway_nodes[id].initiliazation.state = false;
    _gateway_nodes[id].initiliazation.attempts = 0;

    _gateway_nodes[id].packet.waiting_for = GATEWAY_PACKET_NONE;
    _gateway_nodes[id].packet.max_length = PJON_PACKET_MAX_LENGTH;

    strcpy(_gateway_nodes[id].hardware.manufacturer, (char *) GATEWAY_DESCRIPTION_NOT_SET);
    strcpy(_gateway_nodes[id].hardware.model, (char *) GATEWAY_DESCRIPTION_NOT_SET);
    strcpy(_gateway_nodes[id].hardware.version, (char *) GATEWAY_DESCRIPTION_NOT_SET);

    strcpy(_gateway_nodes[id].firmware.manufacturer, (char *) GATEWAY_DESCRIPTION_NOT_SET);
    strcpy(_gateway_nodes[id].firmware.model, (char *) GATEWAY_DESCRIPTION_NOT_SET);
    strcpy(_gateway_nodes[id].firmware.version, (char *) GATEWAY_DESCRIPTION_NOT_SET);

    _gateway_nodes[id].registers_size[GATEWAY_REGISTER_DI] = 0;
    _gateway_nodes[id].registers_size[GATEWAY_REGISTER_DO] = 0;
    _gateway_nodes[id].registers_size[GATEWAY_REGISTER_AI] = 0;
    _gateway_nodes[id].registers_size[GATEWAY_REGISTER_AO] = 0;

    _gateway_nodes[id].registers_reading.register_type = GATEWAY_REGISTER_NONE;
    _gateway_nodes[id].registers_reading.start = 0;

    // Reset registers
    std::vector<gateway_digital_register_t> reset_digital_inputs;
    _gateway_nodes[id].digital_inputs = reset_digital_inputs;

    std::vector<gateway_digital_register_t> reset_digital_outputs;
    _gateway_nodes[id].digital_outputs = reset_digital_outputs;

    std::vector<gateway_analog_register_t> reset_analog_inputs;
    _gateway_nodes[id].analog_inputs = reset_analog_inputs;

    std::vector<gateway_analog_register_t> reset_analog_outputs;
    _gateway_nodes[id].analog_outputs = reset_analog_outputs;

    std::vector<gateway_event_register_t> reset_event_inputs;
    _gateway_nodes[id].event_inputs = reset_event_inputs;
}

// -----------------------------------------------------------------------------

/**
 * Check if wating for reply packet reached maximum delay
 */
void _gatewayCheckPacketsDelays()
{
    for (uint8_t i = 0; i < NODES_GATEWAY_MAX_NODES; i++) {
        // Searching
        if (
            _gateway_nodes[i].searching.state == true
            && (millis() - _gateway_nodes[i].searching.registration) > NODES_GATEWAY_SEARCHING_WAITING_TIMEOUT
        ) {
            // Node does not respons in reserved time window
            // Node slot is free to use by other node
            _gatewayResetNodeIndex(i);
        }
    }
}

// -----------------------------------------------------------------------------
// PACKETS
// -----------------------------------------------------------------------------

bool _gatewayIsPacketInGroup(
    const uint8_t packetId,
    const int * group,
    const uint8_t length
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
    const uint8_t packetId,
    const int * group,
    const uint8_t length
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
    const uint8_t packetId
) {
    char buffer[50] = {0};

    if (_gatewayIsPacketInGroup(packetId, gateway_packets_searching, GATEWAY_PACKET_SEARCH_MAX)) {
        strncpy_P(buffer, gateway_packets_searching_string[_gatewayGetPacketIndexInGroup(packetId, gateway_packets_searching, GATEWAY_PACKET_SEARCH_MAX)], sizeof(buffer));

    } else if (_gatewayIsPacketInGroup(packetId, gateway_packets_node_initialization, GATEWAY_PACKET_NODE_INIT_MAX)) {
        strncpy_P(buffer, gateway_packets_node_initialization_string[_gatewayGetPacketIndexInGroup(packetId, gateway_packets_node_initialization, GATEWAY_PACKET_NODE_INIT_MAX)], sizeof(buffer));

    } else if (_gatewayIsPacketInGroup(packetId, gateway_packets_registers_reading, GATEWAY_PACKET_REGISTERS_READING_MAX)) {
        strncpy_P(buffer, gateway_packets_registers_reading_string[_gatewayGetPacketIndexInGroup(packetId, gateway_packets_registers_reading, GATEWAY_PACKET_REGISTERS_READING_MAX)], sizeof(buffer));

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
// COMMUNICATION
// -----------------------------------------------------------------------------

bool _gatewayBroadcastPacket(
    char * payload,
    const uint8_t length
) {
    return _gatewaySendPacket(PJON_BROADCAST, payload, length);
}

// -----------------------------------------------------------------------------

bool _gatewaySendPacket(
    const uint8_t address,
    char * payload,
    const uint8_t length
) {
    uint16_t result = _gateway_bus.send_packet(
        address,    // Node address
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
        DEBUG_GW_MSG(PSTR("[GATEWAY] Successfully sent broadcast packet: %s\n"), _gatewayPacketName(payload[0]).c_str());

    } else {
        DEBUG_GW_MSG(PSTR("[GATEWAY] Successfully sent packet: %s for node: %d\n"), _gatewayPacketName(payload[0]).c_str(), address);
    }

    return true;
}

// -----------------------------------------------------------------------------
// NODES READING
// -----------------------------------------------------------------------------

/**
 * Read data registers from node
 */
bool _gatewayReadNode(
    uint8_t id
) {
    if (
        _gateway_nodes[id].initiliazation.state == true
        && (
            _gateway_nodes[id].registers_size[GATEWAY_REGISTER_DI] > 0
            || _gateway_nodes[id].registers_size[GATEWAY_REGISTER_DO] > 0
            || _gateway_nodes[id].registers_size[GATEWAY_REGISTER_AI] > 0
            || _gateway_nodes[id].registers_size[GATEWAY_REGISTER_AO] > 0
            || _gateway_nodes[id].registers_size[GATEWAY_REGISTER_EV] > 0
        )
    ) {
        if (
            _gateway_nodes[id].registers_size[GATEWAY_REGISTER_DI] > 0
            && _gateway_nodes[id].registers_reading.register_type == GATEWAY_REGISTER_DI
        ) {
            _gatewayRequestReadingMultipleDigitalInputsRegisters(id);

            return true;

        } else if (
            _gateway_nodes[id].registers_size[GATEWAY_REGISTER_DO] > 0
            && _gateway_nodes[id].registers_reading.register_type == GATEWAY_REGISTER_DO
        ) {
            _gatewayRequestReadingMultipleDigitalOutputsRegisters(id);

            return true;

        } else if (
            _gateway_nodes[id].registers_size[GATEWAY_REGISTER_AI] > 0
            && _gateway_nodes[id].registers_reading.register_type == GATEWAY_REGISTER_AI
        ) {
            _gatewayRequestReadingMultipleAnalogInputsRegisters(id);

            return true;

        } else if (
            _gateway_nodes[id].registers_size[GATEWAY_REGISTER_AO] > 0
            && _gateway_nodes[id].registers_reading.register_type == GATEWAY_REGISTER_AO
        ) {
            _gatewayRequestReadingMultipleAnalogOutputsRegisters(id);

            return true;

        } else if (
            _gateway_nodes[id].registers_size[GATEWAY_REGISTER_EV] > 0
            && _gateway_nodes[id].registers_reading.register_type == GATEWAY_REGISTER_EV
        ) {
            _gatewayRequestReadingMultipleEventInputsRegisters(id);

            return true;
        }
    }

    return false;
}

// -----------------------------------------------------------------------------
// COMMUNICATION HANDLERS
// -----------------------------------------------------------------------------

void _gatewayReceiveHandler(
    uint8_t * payload,
    const uint16_t length,
    const PJON_Packet_Info &packetInfo
) {
    uint8_t sender_address = PJON_NOT_ASSIGNED;

    // Get sender address from header
    if (packetInfo.header & PJON_TX_INFO_BIT) {
        sender_address = packetInfo.sender_id;
    }

    // Get packet identifier from payload
    uint8_t packet_id = (uint8_t) payload[0];

    DEBUG_GW_MSG(PSTR("[GATEWAY] Received packet: %s for node with address: %d\n"), _gatewayPacketName(packet_id).c_str(), sender_address);

    // Node is trying to acquire address
    if (_gatewayIsPacketInGroup(packet_id, gateway_packets_searching, GATEWAY_PACKET_SEARCH_MAX)) {
        _gatewayAddressingRequestHandler(packet_id, sender_address, payload);

    // Node send reply to request
    } else {
        if (sender_address == PJON_NOT_ASSIGNED) {
            DEBUG_GW_MSG(PSTR("[GATEWAY][ERR] Received packet is without sender address\n"));

            return;
        }

        // Check if gateway is waiting for reply from node (initiliazation sequence)
        if (_gateway_nodes[(sender_address - 1)].packet.waiting_for == GATEWAY_PACKET_NONE) {
            DEBUG_GW_MSG(
                PSTR("[GATEWAY][ERR] Received packet for node with address: %d but gateway is not waiting for packet from this node\n"),
                sender_address
            );

            return;
        }

        // Check if gateway is waiting for reply from node (initiliazation sequence)
        if (_gateway_nodes[(sender_address - 1)].packet.waiting_for != packet_id) {
            DEBUG_GW_MSG(
                PSTR("[GATEWAY][ERR] Received packet: %s for node with address: %d but gateway is waiting for: %s\n"),
                _gatewayPacketName(packet_id).c_str(),
                sender_address,
                _gatewayPacketName(_gateway_nodes[(sender_address - 1)].packet.waiting_for).c_str()
            );

            return;
        }
        
        if (_gatewayIsPacketInGroup(packet_id, gateway_packets_node_initialization, GATEWAY_PACKET_NODE_INIT_MAX)) {
            _gatewayInitializationHandler(packet_id, sender_address, payload, length);

        } else {
            switch (packet_id)
            {

            /**
             * DIGITAL REGISTERS READING
             */

                case GATEWAY_PACKET_READ_SINGLE_DI:
                    // TODO: implement
                    break;

                case GATEWAY_PACKET_READ_MULTI_DI:
                    _gatewayReadMultipleDigitalRegisterHandler((sender_address - 1), GATEWAY_REGISTER_DI, payload);
                    break;

                case GATEWAY_PACKET_READ_SINGLE_DO:
                    // TODO: implement
                    break;

                case GATEWAY_PACKET_READ_MULTI_DO:
                    _gatewayReadMultipleDigitalRegisterHandler((sender_address - 1), GATEWAY_REGISTER_DO, payload);
                    break;

            /**
             * ANALOG REGISTERS READING
             */

                case GATEWAY_PACKET_READ_SINGLE_AI:
                    // TODO: implement
                    break;

                case GATEWAY_PACKET_READ_MULTI_AI:
                    _gatewayReadMultipleAnalogRegisterHandler((sender_address - 1), GATEWAY_REGISTER_AI, payload);
                    break;

                case GATEWAY_PACKET_READ_SINGLE_AO:
                    // TODO: implement
                    break;

                case GATEWAY_PACKET_READ_MULTI_AO:
                    _gatewayReadMultipleAnalogRegisterHandler((sender_address - 1), GATEWAY_REGISTER_AO, payload);
                    break;

            /**
             * EVENT REGISTERS READING
             */

                case GATEWAY_PACKET_READ_SINGLE_EV:
                    // TODO: implement
                    break;

                case GATEWAY_PACKET_READ_MULTI_EV:
                    _gatewayReadMultipleEventRegisterHandler((sender_address - 1), GATEWAY_REGISTER_EV, payload);
                    break;

            /**
             * DIGITAL REGISTERS WRITING
             */

                case GATEWAY_PACKET_WRITE_ONE_DO:
                    _gatewayWriteSingleDigitalOutputHandler((sender_address - 1), payload);
                    break;

                case GATEWAY_PACKET_WRITE_MULTI_DO:
                    _gatewayWriteMultipleDigitalOutputsHandler((sender_address - 1), payload);
                    break;

            /**
             * ANALOG REGISTERS WRITING
             */

                case GATEWAY_PACKET_WRITE_ONE_AO:
                    _gatewayWriteSingleAnalogOutputHandler((sender_address - 1), payload);
                    break;

                case GATEWAY_PACKET_WRITE_MULTI_AO:
                    _gatewayWriteMultipleAnalogOutputsHandler((sender_address - 1), payload);
                    break;
            }
        }
    }
}

// -----------------------------------------------------------------------------

void _gatewayErrorHandler(
    const uint8_t code,
    const uint16_t data,
    void *customPointer
) {
    if (code == PJON_CONNECTION_LOST) {
        DEBUG_GW_MSG(PSTR("[GATEWAY][ERR] Node with address: %d is lost\n"), (_gateway_bus.packets[data].content[0] -1));

    } else if (code == PJON_PACKETS_BUFFER_FULL) {
        DEBUG_GW_MSG(PSTR("[GATEWAY][ERR] Buffer is full\n"));

    } else if (code == PJON_CONTENT_TOO_LONG) {
        DEBUG_GW_MSG(PSTR("[GATEWAY][ERR] Content is long\n"));

    } else {
        DEBUG_GW_MSG(PSTR("[GATEWAY][ERR] Unknown error\n"));
    }
}

// -----------------------------------------------------------------------------
// MODULE API
// -----------------------------------------------------------------------------

bool gatewayReadDigitalValue(
    const uint8_t id,
    const uint8_t dataRegister,
    const uint8_t address
) {
    return _gatewayRegistersReadDigitalValue(id, dataRegister, address);
}

// -----------------------------------------------------------------------------
// MODULE CORE
// -----------------------------------------------------------------------------

void gatewaySetup() {
    _gateway_serial_bus.begin(SERIAL_BAUDRATE);

    _gateway_bus.strategy.set_serial(&_gateway_serial_bus);

    _gateway_bus.set_synchronous_acknowledge(false);

    // Communication callbacks
    _gateway_bus.set_receiver(_gatewayReceiveHandler);
    _gateway_bus.set_error(_gatewayErrorHandler);

    _gateway_bus.begin();

    #if WEB_SUPPORT
        webServer()->on(WEB_API_GATEWAY_CONFIGURATION, HTTP_GET, _gatewayOnGetConfig);
        webServer()->on(WEB_API_GATEWAY_CONFIGURATION, HTTP_POST, _gatewayOnPostConfig, _gatewayOnPostConfigData);
    #endif

    #if FASTYBIRD_SUPPORT
        // Module schema report
        fastybirdReportConfigurationSchemaRegister(_gatewayReportConfigurationSchema);
        fastybirdReportConfigurationRegister(_gatewayReportConfiguration);
        fastybirdOnConfigureRegister(_gatewayUpdateConfiguration);

        fastybirdOnControlRegister(
            [](const char * payload) {
                DEBUG_MSG(PSTR("[GATEWAY] Searching for new nodes\n"));

                gatewayAddressingStartSearchingNodes();
            },
            FASTYBIRD_THING_CONTROL_SEARCH_FOR_NODES
        );

        fastybirdOnControlRegister(
            [](const char * payload) {
                DEBUG_MSG(PSTR("[GATEWAY] Disconnection selected node\n"));

                // TODO: implement
            },
            FASTYBIRD_THING_CONTROL_DISCONNECT_NODE
        );
    #endif

    // Restore nodes structure
    _gatewayStorageRestoreFromMemory();

    firmwareRegisterLoop(gatewayLoop);
}

// -----------------------------------------------------------------------------

void gatewayLoop() {
    // Little delay before gateway start
    if (millis() < NODES_GATEWAY_START_DELAY) {
        return;
    }

    _gatewayCheckPacketsDelays();

    if (!_gatewayAddressingLoop()) {
        if (_gateway_reading_node_index >= NODES_GATEWAY_MAX_NODES) {
            _gateway_reading_node_index = 0;
        }

        for (uint8_t i = _gateway_reading_node_index; i < NODES_GATEWAY_MAX_NODES; i++) {
            _gateway_reading_node_index++;

            if (_gatewayInitializationIsUnfinished(i)) {
                _gatewayInitializationContinueInProcess(i);

                break;

            } else {
                // Continue in nodes registers reading
                if (_gatewayReadNode(i)) {
                    break;
                }
            }
        }
    }

    _gatewayDigitalRegisterProcess(false);
    _gatewayDigitalRegisterProcess(true);

    _gateway_bus.update();
    _gateway_bus.receive(50000);
}

#endif // NODES_GATEWAY_SUPPORT