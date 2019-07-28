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
    uint32_t    sending_time    = 0;
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

struct gateway_node_addressing_t {
    bool        state           = false;    // FALSE = node addresing is not finished | TRUE = addressing was successfully accepted by node
    uint32_t    lost            = 0;        // Timestamp when gateway detected node as lost
};

struct gateway_node_searching_t {
    bool        state           = false;    // FALSE = node searching is not finished | TRUE = searching was successfully accepted by node
    uint32_t    registration    = 0;        // Timestamp when node requested for address
    uint8_t     attempts        = 0;
};

struct gateway_register_reading_t {
    uint8_t     start           = 0;
    uint32_t    delay           = 0;
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
    gateway_node_addressing_t addressing;

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
    gateway_register_reading_t digital_inputs_reading;
    gateway_register_reading_t digital_outputs_reading;

    gateway_register_reading_t analog_inputs_reading;
    gateway_register_reading_t analog_outputs_reading;

    gateway_register_reading_t event_inputs_reading;
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
uint32_t _gateway_last_nodes_check = 0;
uint32_t _gateway_last_nodes_scan = 0;
uint32_t _gateway_last_new_nodes_scan = 0;

uint32_t _gateway_nodes_scan_client_id = 0;
bool _gateway_nodes_search_new = false;

const char * _gateway_config_filename = "gateway.conf";

#include "./nodes/storage.h"             // Module configuration storage
#include "./nodes/registers.h"           // Nodes registers methods
#include "./nodes/searching.h"           // Nodes searching methods
#include "./nodes/modules.h"             // 
#include "./nodes/addressing.h"          // Nodes addressing methods
#include "./nodes/initialization.h"      // Nodes initialiazation methods

// -----------------------------------------------------------------------------
// MODULE PRIVATE
// -----------------------------------------------------------------------------

bool _gatewayRegisterValueUpdated(
    const uint8_t id,
    const uint8_t dataRegister,
    const uint8_t address
) {
    #if WEB_SUPPORT && WS_SUPPORT
        DynamicJsonBuffer jsonBuffer;

        JsonObject& message = jsonBuffer.createObject();

        message["module"] = "nodes";

        JsonObject& container = message.createNestedObject("node");

        _gatewayCollectNode(container, id);

        wsSend(message);
    #endif

    #if FASTYBIRD_SUPPORT
        switch (dataRegister)
        {
            case GATEWAY_REGISTER_DI:
                _fastybirdReportNodeChannelValue(
                    _gateway_nodes[id].serial_number,
                    0,
                    address,
                    _gatewayReadDigitalRegisterValue(id, dataRegister, address) ? FASTYBIRD_SWITCH_PAYLOAD_ON : FASTYBIRD_SWITCH_PAYLOAD_OFF
                );
                break;

            case GATEWAY_REGISTER_DO:
                _fastybirdReportNodeChannelValue(
                    _gateway_nodes[id].serial_number,
                    1,
                    address,
                    _gatewayReadDigitalRegisterValue(id, dataRegister, address) ? FASTYBIRD_SWITCH_PAYLOAD_ON : FASTYBIRD_SWITCH_PAYLOAD_OFF
                );
                break;

            case GATEWAY_REGISTER_AI:
                _gatewayReportAnalogRegisterValue(id, dataRegister, address, 2);
                break;

            case GATEWAY_REGISTER_AO:
                _gatewayReportAnalogRegisterValue(id, dataRegister, address, 3);
                break;

            case GATEWAY_REGISTER_EV:
                _gatewayReportEventRegisterValue(id, dataRegister, address, 4);
                break;
        }
    #endif
}

// -----------------------------------------------------------------------------

void _gatewayResetNodeIndex(
    const uint8_t id
) {
    // Reset addressing
    _gateway_nodes[id].addressing.state = false;
    _gateway_nodes[id].addressing.lost = 0;

    _gateway_nodes[id].searching.state = false;
    _gateway_nodes[id].searching.registration = 0;
    _gateway_nodes[id].searching.attempts = 0;

    // Reset initialization steps
    _gateway_nodes[id].initiliazation.step = GATEWAY_PACKET_NONE;
    _gateway_nodes[id].initiliazation.state = false;
    _gateway_nodes[id].initiliazation.attempts = 0;

    _gateway_nodes[id].packet.waiting_for = GATEWAY_PACKET_NONE;
    _gateway_nodes[id].packet.sending_time = 0;
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

        // Packets
        if (
            _gateway_nodes[i].packet.waiting_for != GATEWAY_PACKET_NONE
            && _gateway_nodes[i].packet.sending_time > 0
            && (millis() - _gateway_nodes[i].packet.sending_time) > NODES_GATEWAY_PACKET_REPLY_DELAY
        ) {
            _gateway_nodes[i].packet.waiting_for = GATEWAY_PACKET_NONE;
            _gateway_nodes[i].packet.sending_time = 0;
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

    } else if (_gatewayIsPacketInGroup(packetId, gateway_packets_addresing, GATEWAY_PACKET_ADDRESS_MAX)) {
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
// NODES READING
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
            && _gateway_nodes[i].addressing.lost == 0
            && (
                _gateway_nodes[i].registers_size[GATEWAY_REGISTER_DI] > 0
                || _gateway_nodes[i].registers_size[GATEWAY_REGISTER_DO] > 0
                || _gateway_nodes[i].registers_size[GATEWAY_REGISTER_AI] > 0
                || _gateway_nodes[i].registers_size[GATEWAY_REGISTER_AO] > 0
                || _gateway_nodes[i].registers_size[GATEWAY_REGISTER_EV] > 0
            )
            && _gateway_nodes[i].packet.waiting_for == GATEWAY_PACKET_NONE
        ) {
            _gateway_reading_node_index = i;

            if (
                _gateway_nodes[i].registers_size[GATEWAY_REGISTER_DI] > 0
                && (
                    _gateway_nodes[i].digital_inputs_reading.delay == 0
                    || (millis() - _gateway_nodes[i].digital_inputs_reading.delay) > NODES_GATEWAY_DI_READING_INTERVAL
                )
            ) {
                _gatewayRequestReadingMultipleDigitalInputsRegisters(i);

            } else if (
                _gateway_nodes[i].registers_size[GATEWAY_REGISTER_DO] > 0
                && (
                    _gateway_nodes[i].digital_outputs_reading.delay == 0
                    || (millis() - _gateway_nodes[i].digital_outputs_reading.delay) > NODES_GATEWAY_DO_READING_INTERVAL
                )
            ) {
                _gatewayRequestReadingMultipleDigitalOutputsRegisters(i);

            } else if (
                _gateway_nodes[i].registers_size[GATEWAY_REGISTER_AI] > 0
                && (
                    _gateway_nodes[i].analog_inputs_reading.delay == 0
                    || (millis() - _gateway_nodes[i].analog_inputs_reading.delay) > NODES_GATEWAY_AI_READING_INTERVAL
                )
            ) {
                _gatewayRequestReadingMultipleAnalogInputsRegisters(i);

            } else if (
                _gateway_nodes[i].registers_size[GATEWAY_REGISTER_AO] > 0
                && (
                    _gateway_nodes[i].analog_outputs_reading.delay == 0
                    || (millis() - _gateway_nodes[i].analog_outputs_reading.delay) > NODES_GATEWAY_AO_READING_INTERVAL
                )
            ) {
                _gatewayRequestReadingMultipleAnalogOutputsRegisters(i);

            } else if (
                _gateway_nodes[i].registers_size[GATEWAY_REGISTER_EV] > 0
                && (
                    _gateway_nodes[i].event_inputs_reading.delay == 0
                    || (millis() - _gateway_nodes[i].event_inputs_reading.delay) > NODES_GATEWAY_EV_READING_INTERVAL
                )
            ) {
                _gatewayRequestReadingMultipleEventInputsRegisters(i);
            }

            return;
        }
    }
}

// -----------------------------------------------------------------------------
// NODES CHECKING
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

    _gatewayBroadcastPacket(
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
                if (_gateway_nodes[i].addressing.lost == 0) {
                    DEBUG_MSG(PSTR("[GATEWAY] Node with address: %d is lost\n"), (i + 1));
                }

                _gateway_nodes[i].addressing.lost = millis();

            } else {
                if (_gateway_nodes[i].addressing.lost > 0) {
                    DEBUG_MSG(PSTR("[GATEWAY] Node with address: %d is back alive\n"), (i + 1));
                }

                _gateway_nodes[i].addressing.lost = 0;

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

    DEBUG_MSG(PSTR("[GATEWAY] Received packet: %s for node with address: %d\n"), _gatewayPacketName(packet_id).c_str(), sender_address);

    // Node is trying to acquire address
    if (_gatewayIsPacketInGroup(packet_id, gateway_packets_searching, GATEWAY_PACKET_SEARCH_MAX)) {
        _gatewaySearchRequestHandler(packet_id, sender_address, payload);

    // Node is trying to acquire address
    } else if (_gatewayIsPacketInGroup(packet_id, gateway_packets_addresing, GATEWAY_PACKET_ADDRESS_MAX)) {
        _gatewayAddressRequestHandler(packet_id, sender_address, payload);

    // Node send reply to request
    } else {
        if (sender_address == PJON_NOT_ASSIGNED) {
            DEBUG_MSG(PSTR("[GATEWAY][ERR] Received packet is without sender address\n"));

            return;
        }

        // Check if gateway is waiting for reply from node (initiliazation sequence)
        if (_gateway_nodes[(sender_address - 1)].packet.waiting_for == GATEWAY_PACKET_NONE) {
            //DEBUG_MSG(
            //    PSTR("[GATEWAY][ERR] Received packet for node with address: %d but gateway is not waiting for packet from this node\n"),
            //    sender_address
            //);

            return;
        }

        // Check if gateway is waiting for reply from node (initiliazation sequence)
        if (_gateway_nodes[(sender_address - 1)].packet.waiting_for != packet_id) {
            //DEBUG_MSG(
            //    PSTR("[GATEWAY][ERR] Received packet: %s for node with address: %d but gateway is waiting for: %s\n"),
            //    _gatewayPacketName(packet_id).c_str(),
            //    sender_address,
            //    _gatewayPacketName(_gateway_nodes[(sender_address - 1)].packet.waiting_for).c_str()
            //);

            return;
        }
        
        if (_gatewayIsPacketInGroup(packet_id, gateway_packets_node_initialization, GATEWAY_PACKET_NODE_INIT_MAX)) {
            _gatewayNodeInitializationHandler(packet_id, sender_address, payload);

        } else if (_gatewayIsPacketInGroup(packet_id, gateway_packets_registers_initialization, GATEWAY_PACKET_REGISTERS_INIT_MAX)) {
            _gatewayRegistersInitializationHandler(packet_id, sender_address, payload, length);

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
        _gateway_nodes[_gateway_bus.packets[data].content[0] -1].addressing.lost = millis();

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
// MODULE API
// -----------------------------------------------------------------------------

bool gatewayReadDigitalValue(
    const uint8_t id,
    const uint8_t dataRegister,
    const uint8_t address
) {
    return _gatewayReadDigitalRegisterValue(id, dataRegister, address);
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

    #if WEB_SUPPORT
        webOnRequestRegister("/gateway/config", HTTP_GET, _gatewayOnGetConfig);
        webOnRequestRegister("/gateway/config", HTTP_POST, _gatewayOnPostConfig);

        #if WS_SUPPORT
            wsOnConnectRegister(_gatewayWSOnConnect);
            wsOnConfigureRegister(_gatewayWSOnConfigure);
            wsOnActionRegister(_gatewayWSOnAction);
        #endif
    #endif

    #if FASTYBIRD_SUPPORT
        fastybirdOnControlRegister(
            [](const char * payload) {
                DEBUG_MSG(PSTR("[GATEWAY] Searching for new nodes\n"));

                _gatewaySearchNewNodesStart();
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

    _gatewayRestoreStorageFromMemory();

    firmwareRegisterLoop(gatewayLoop);
}

// -----------------------------------------------------------------------------

void gatewayLoop() {
    // Little delay before gateway start
    if (millis() < NODES_GATEWAY_START_DELAY) {
        return;
    }

    _gatewayCheckPacketsDelays();

    // If some node is not initialized, get its address
    uint8_t node_address_to_initialize = _gatewayInitializationUnfinishedAddress();
    uint8_t node_address_to_search = _gatewaySearchUnfinishedAddress();

    if (_gateway_nodes_search_new == true && (_gateway_last_new_nodes_scan + NODES_GATEWAY_SEARCHING_TIMEOUT) <= millis()) {
        _gateway_nodes_search_new = false;
        _gateway_last_new_nodes_scan = 0;

        #if WEB_SUPPORT && WS_SUPPORT
            if (_gateway_nodes_scan_client_id != 0) {
                DynamicJsonBuffer jsonBuffer;

                JsonObject& scan_result = jsonBuffer.createObject();

                scan_result["module"] = "nodes";
                scan_result["result"] = String("OK");

                String converted_output;

                scan_result.printTo(converted_output);

                wsSend(_gateway_nodes_scan_client_id, converted_output.c_str());  
            }
        #endif
    }

    // Search for new unaddressed nodes
    if (_gateway_nodes_search_new == true && (_gateway_last_new_nodes_scan + NODES_GATEWAY_SEARCHING_TIMEOUT) > millis()) {
        _gatewaySearchForNewNodes();

    // Search for nodes after bootup
    } else if (_gateway_last_nodes_scan == 0 || (_gateway_last_nodes_scan + NODES_GATEWAY_ADDRESSING_TIMEOUT) > millis()) {
        if (_gateway_last_nodes_scan == 0) {
            _gateway_last_nodes_scan = millis();
        }

        _gatewaySearchForNodes();

    // Check nodes presence in given interval
    } else if (millis() - _gateway_last_nodes_check > NODES_GATEWAY_NODES_CHECK_INTERVAL) {
        _gateway_last_nodes_check = millis();

        _gatewayCheckNodesPresence();

    // Check if all connected nodes are initialized
    } else if (node_address_to_initialize != 0) {
        _gatewayInitializationContinueInProcess(node_address_to_initialize);

    // Check if all connected nodes have finished searching process
    } else if (node_address_to_search != 0) {
        _gatewaySearchContinueInProcess(node_address_to_search);

    // Continue in nodes registers reading
    } else {
        _gatewayReadNodes();
    }

    _gatewayDigitalRegisterProcess(false);
    _gatewayDigitalRegisterProcess(true);

    _gateway_bus.receive(50000);
    _gateway_bus.update();
}

#endif // NODES_GATEWAY_SUPPORT