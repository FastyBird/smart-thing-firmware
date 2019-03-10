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
    bool        state           = false;    // FALSE = node addresing is not finished | TRUE = address was successfully accepted by node
    uint32_t    registration    = 0;        // Timestamp when node requested for address
    uint32_t    lost            = 0;        // Timestamp when gateway detected node as lost
};

struct gateway_register_reading_t {
    uint8_t     start           = 0;
    uint32_t    delay           = 0;
};

typedef struct {
    const char *    name;
    bool            value;
} gateway_digital_register_t;

typedef struct {
    const char *    name;
    uint8_t         data_type;
    uint8_t         size;
    char            value[4];
} gateway_analog_register_t;

struct gateway_node_t {
    // Node addressing process
    gateway_node_addressing_t addressing;

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
    
    uint8_t registers_size[4];

    // Data registers reading
    gateway_register_reading_t digital_inputs_reading;
    gateway_register_reading_t digital_outputs_reading;

    gateway_register_reading_t analog_inputs_reading;
    gateway_register_reading_t analog_outputs_reading;
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

#include "./pjon/addressing.h"
#include "./pjon/initialization.h"
#include "./pjon/registers.h"

// -----------------------------------------------------------------------------
// MODULE PRIVATE
// -----------------------------------------------------------------------------

#if (WEB_SUPPORT && WS_SUPPORT) || FASTYBIRD_SUPPORT
    void _gatewayCollectNode(
        JsonObject& node,
        const uint8_t id
    ) {
        bool value_bool;
        uint8_t value_uint8;
        uint16_t value_uint16;
        uint32_t value_uint32;
        int8_t value_int8;
        int16_t value_int16;
        int32_t value_int32;
        float value_float;

        node["addressed"] = _gateway_nodes[id].addressing.state;
        node["initialized"] = _gateway_nodes[id].initiliazation.state;
        node["lost"] = _gateway_nodes[id].addressing.lost;

        JsonObject& addressing = node.createNestedObject("addressing");

        addressing["registration"] = _gateway_nodes[id].addressing.registration;

        // Node basic info
        node["node"] = String(_gateway_nodes[id].serial_number);

        node["address"] = id + 1;

        JsonObject& hardware = node.createNestedObject("hardware");

        hardware["manufacturer"] = _gateway_nodes[id].hardware.manufacturer;
        hardware["model"] = _gateway_nodes[id].hardware.model;
        hardware["version"] = _gateway_nodes[id].hardware.version;

        JsonObject& firmware = node.createNestedObject("firmware");

        firmware["manufacturer"] = _gateway_nodes[id].firmware.manufacturer;
        firmware["model"] = _gateway_nodes[id].firmware.model;
        firmware["version"] = _gateway_nodes[id].firmware.version;

        // Node channels schema
        JsonObject& registers = node.createNestedObject("registers");

        JsonArray& digital_inputs_register = registers.createNestedArray("digital_inputs");

        for (uint8_t j = 0; j < _gateway_nodes[id].registers_size[GATEWAY_REGISTER_DI]; j++) {
            JsonObject& di_register = digital_inputs_register.createNestedObject();

            di_register["data_type"] = "b1";
            di_register["value"] = _gatewayReadDigitalRegisterValue(id, GATEWAY_REGISTER_DI, j);
        }

        JsonArray& digital_outputs_register = registers.createNestedArray("digital_outputs");

        for (uint8_t j = 0; j < _gateway_nodes[id].registers_size[GATEWAY_REGISTER_DO]; j++) {
            JsonObject& do_register = digital_outputs_register.createNestedObject();

            do_register["data_type"] = "b1";
            do_register["value"] = _gatewayReadDigitalRegisterValue(id, GATEWAY_REGISTER_DO, j);
        }

        JsonArray& analog_inputs_register = registers.createNestedArray("analog_inputs");

        for (uint8_t j = 0; j < _gateway_nodes[id].registers_size[GATEWAY_REGISTER_AI]; j++) {
            JsonObject& ai_register = analog_inputs_register.createNestedObject();

            switch (_gateway_nodes[id].analog_inputs[j].data_type)
            {
                case GATEWAY_DATA_TYPE_UINT8:
                    _gatewayReadAnalogRegisterValue(id, GATEWAY_REGISTER_AI, j, value_uint8);

                    ai_register["data_type"] = "u1";
                    ai_register["value"] = value_uint8;
                    break;

                case GATEWAY_DATA_TYPE_UINT16:
                    _gatewayReadAnalogRegisterValue(id, GATEWAY_REGISTER_AI, j, value_uint16);

                    ai_register["data_type"] = "u2";
                    ai_register["value"] = value_uint16;
                    break;

                case GATEWAY_DATA_TYPE_UINT32:
                    _gatewayReadAnalogRegisterValue(id, GATEWAY_REGISTER_AI, j, value_uint32);

                    ai_register["data_type"] = "u4";
                    ai_register["value"] = value_uint32;
                    break;

                case GATEWAY_DATA_TYPE_INT8:
                    _gatewayReadAnalogRegisterValue(id, GATEWAY_REGISTER_AI, j, value_int8);

                    ai_register["data_type"] = "i1";
                    ai_register["value"] = value_uint8;
                    break;

                case GATEWAY_DATA_TYPE_INT16:
                    _gatewayReadAnalogRegisterValue(id, GATEWAY_REGISTER_AI, j, value_int16);

                    ai_register["data_type"] = "i2";
                    ai_register["value"] = value_uint16;
                    break;

                case GATEWAY_DATA_TYPE_INT32:
                    _gatewayReadAnalogRegisterValue(id, GATEWAY_REGISTER_AI, j, value_int32);

                    ai_register["data_type"] = "i4";
                    ai_register["value"] = value_uint32;
                    break;

                case GATEWAY_DATA_TYPE_FLOAT32:
                    _gatewayReadAnalogRegisterValue(id, GATEWAY_REGISTER_AI, j, value_float);

                    ai_register["data_type"] = "f4";
                    ai_register["value"] = value_float;
                    break;

                default:
                    ai_register["data_type"] = "na";
                    ai_register["value"] = 0;
                    break;
            }
        }

        JsonArray& analog_outputs_register = registers.createNestedArray("analog_outputs");

        for (uint8_t j = 0; j < _gateway_nodes[id].registers_size[GATEWAY_REGISTER_AO]; j++) {
            JsonObject& ao_register = analog_outputs_register.createNestedObject();

            switch (_gateway_nodes[id].analog_outputs[j].data_type)
            {
                case GATEWAY_DATA_TYPE_UINT8:
                    _gatewayReadAnalogRegisterValue(id, GATEWAY_REGISTER_AO, j, value_uint8);

                    ao_register["data_type"] = "u1";
                    ao_register["value"] = value_uint8;
                    break;

                case GATEWAY_DATA_TYPE_UINT16:
                    _gatewayReadAnalogRegisterValue(id, GATEWAY_REGISTER_AO, j, value_uint16);

                    ao_register["data_type"] = "u2";
                    ao_register["value"] = value_uint16;
                    break;

                case GATEWAY_DATA_TYPE_UINT32:
                    _gatewayReadAnalogRegisterValue(id, GATEWAY_REGISTER_AO, j, value_uint32);

                    ao_register["data_type"] = "u4";
                    ao_register["value"] = value_uint32;
                    break;

                case GATEWAY_DATA_TYPE_INT8:
                    _gatewayReadAnalogRegisterValue(id, GATEWAY_REGISTER_AO, j, value_int8);

                    ao_register["data_type"] = "i1";
                    ao_register["value"] = value_uint8;
                    break;

                case GATEWAY_DATA_TYPE_INT16:
                    _gatewayReadAnalogRegisterValue(id, GATEWAY_REGISTER_AO, j, value_int16);

                    ao_register["data_type"] = "i2";
                    ao_register["value"] = value_uint16;
                    break;

                case GATEWAY_DATA_TYPE_INT32:
                    _gatewayReadAnalogRegisterValue(id, GATEWAY_REGISTER_AO, j, value_int32);

                    ao_register["data_type"] = "i4";
                    ao_register["value"] = value_uint32;
                    break;

                case GATEWAY_DATA_TYPE_FLOAT32:
                    _gatewayReadAnalogRegisterValue(id, GATEWAY_REGISTER_AO, j, value_float);

                    ao_register["data_type"] = "f4";
                    ao_register["value"] = value_float;
                    break;

                default:
                    ao_register["data_type"] = "na";
                    ao_register["value"] = 0;
                    break;
            }
        }
    }
#endif

// -----------------------------------------------------------------------------

#if WEB_SUPPORT && WS_SUPPORT
    // Send module status to WS clients
    void _gatewayWebSocketUpdate(
        JsonObject& root
    ) {
        DEBUG_MSG(PSTR("[GATEWAY] Updating nodes to WS clients\n"));

        root["module"] = "nodes";

        // Data container
        JsonObject& data = root.createNestedObject("data");

        // Nodes container
        JsonArray& nodes = data.createNestedArray("nodes");

        for (uint8_t i = 0; i < NODES_GATEWAY_MAX_NODES; i++) {
            JsonObject& node = nodes.createNestedObject();

            _gatewayCollectNode(node, i);
        }
    }

// -----------------------------------------------------------------------------

    // New WS client is connected
    void _gatewayWSOnConnect(
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

        for (uint8_t i = 0; i < NODES_GATEWAY_MAX_NODES; i++) {
            JsonObject& node = nodes.createNestedObject();

            _gatewayCollectNode(node, i);
        }
    }

// -----------------------------------------------------------------------------

    // WS client requested configuration update
    void _gatewayWSOnConfigure(
        const uint32_t clientId, 
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
    void _gatewayWSOnAction(
        const uint32_t clientId,
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
                if (strcmp(_gateway_nodes[i].serial_number, data["node"].as<const char *>()) == 0) {
                    DEBUG_MSG(PSTR("[GATEWAY] Found node to update\n"));

                    if (strcmp("digital_output", data["register"].as<const char *>()) == 0) {
                        if (data["address"].as<uint8_t>() < _gateway_nodes[i].registers_size[GATEWAY_REGISTER_DO]) {
                            _gatewayRequestWritingSingleDigitalRegister(i, data["address"].as<uint8_t>(), data["value"].as<bool>());

                        } else {
                            DEBUG_MSG(PSTR("[GATEWAY][ERR] Register address: %d is out of range: 0 - %d\n"), data["address"].as<uint8_t>(), _gateway_nodes[i].registers_size[GATEWAY_REGISTER_DO]);
                        }

                    } else if (strcmp("analog_output", data["register"].as<const char *>()) == 0) {
                        if (data["address"].as<uint8_t>() < _gateway_nodes[i].registers_size[GATEWAY_REGISTER_AO]) {
                            // TODO: _gatewayRequestWritingSingleAnalogRegister(i, data["address"].as<uint8_t>(), data["value"].as<float>());

                        } else {
                            DEBUG_MSG(PSTR("[GATEWAY][ERR] Register address: %d is out of range: 0 - %d\n"), data["address"].as<uint8_t>(), _gateway_nodes[i].registers_size[GATEWAY_REGISTER_AO]);
                        }
                    }
                }
            }
        }
    }
#endif

// -----------------------------------------------------------------------------

#if FASTYBIRD_SUPPORT
    /**
     * Node channel property structure
     */
    fastybird_channel_property_t _gatewayFastybirdGetChannelProperty(
        const uint8_t id,
        const uint8_t dataRegister
    ) {
        fastybird_channel_property_t register_property = {
            FASTYBIRD_PROPERTY_STATE,
            "Channel state"
        };

        uint8_t nodeId = id;

        switch (dataRegister)
        {
            case GATEWAY_REGISTER_DI:
                register_property.settable = false;
                register_property.dataType = FASTYBIRD_PROPERTY_DATA_TYPE_BOOLEAN;

                register_property.format.push_back(FASTYBIRD_SWITCH_PAYLOAD_ON);
                register_property.format.push_back(FASTYBIRD_SWITCH_PAYLOAD_OFF);

                register_property.mappings.push_back({
                    FASTYBIRD_SWITCH_PAYLOAD_ON,
                    FASTYBIRD_SWITCH_PAYLOAD_ON
                });

                register_property.mappings.push_back({
                    FASTYBIRD_SWITCH_PAYLOAD_OFF,
                    FASTYBIRD_SWITCH_PAYLOAD_OFF
                });
                break;

            case GATEWAY_REGISTER_DO:
                register_property.settable = true;
                register_property.dataType = FASTYBIRD_PROPERTY_DATA_TYPE_BOOLEAN;

                register_property.format.push_back(FASTYBIRD_SWITCH_PAYLOAD_ON);
                register_property.format.push_back(FASTYBIRD_SWITCH_PAYLOAD_OFF);
                register_property.format.push_back(FASTYBIRD_SWITCH_PAYLOAD_TOGGLE);
                register_property.format.push_back(FASTYBIRD_SWITCH_PAYLOAD_QUERY);

                register_property.mappings.push_back({
                    FASTYBIRD_SWITCH_PAYLOAD_ON,
                    FASTYBIRD_SWITCH_PAYLOAD_ON
                });

                register_property.mappings.push_back({
                    FASTYBIRD_SWITCH_PAYLOAD_OFF,
                    FASTYBIRD_SWITCH_PAYLOAD_OFF
                });

                register_property.mappings.push_back({
                    FASTYBIRD_SWITCH_PAYLOAD_TOGGLE,
                    FASTYBIRD_SWITCH_PAYLOAD_TOGGLE
                });

                register_property.mappings.push_back({
                    FASTYBIRD_SWITCH_PAYLOAD_QUERY,
                    FASTYBIRD_SWITCH_PAYLOAD_QUERY
                });

                // Add register payload callback
                register_property.payloadCallback = ([nodeId](uint8_t id, const char * payload) {
                    // Action to perform
                    if (strcmp(payload, FASTYBIRD_SWITCH_PAYLOAD_TOGGLE) == 0) {
                        _gatewayRequestWritingSingleDigitalRegister(nodeId, id, _gatewayReadDigitalRegisterValue(nodeId, GATEWAY_REGISTER_DO, id) ? false : true);

                    } else {
                        _gatewayRequestWritingSingleDigitalRegister(nodeId, id, strcmp(payload, FASTYBIRD_SWITCH_PAYLOAD_ON) == 0);
                    }
                });
                break;

            case GATEWAY_REGISTER_AI:
                register_property.settable = false;
                register_property.dataType = FASTYBIRD_PROPERTY_DATA_TYPE_FLOAT;
                break;

            case GATEWAY_REGISTER_AO:
                register_property.settable = true;
                register_property.dataType = FASTYBIRD_PROPERTY_DATA_TYPE_FLOAT;

                // Add register payload callback
                register_property.payloadCallback = ([nodeId](uint8_t id, const char * payload) {
                    // TODO: implement it
                    // TODO: _gatewayRequestWritingSingleAnalogRegister(nodeId, id, payload);
                });
                break;
        }

        return register_property;
    }

// -----------------------------------------------------------------------------

    /**
     * Node channel structure
     */
    fastybird_channel_t _gatewayFastybirdGetChannelStructure(
        const uint8_t id,
        const uint8_t dataRegister
    ) {
        fastybird_channel_t register_channel;

        if (dataRegister == GATEWAY_REGISTER_DI) {
            register_channel = {
                FASTYBIRD_CHANNEL_TYPE_BINARY_SENSOR,
                FASTYBIRD_CHANNEL_TYPE_BINARY_SENSOR,
                _gateway_nodes[id].registers_size[dataRegister],
                false,
                false,
                false,
                false
            };

        } else if (dataRegister == GATEWAY_REGISTER_DO) {
            register_channel = {
                FASTYBIRD_CHANNEL_TYPE_BINARY_ACTOR,
                FASTYBIRD_CHANNEL_TYPE_BINARY_ACTOR,
                _gateway_nodes[id].registers_size[dataRegister],
                false,
                false,
                false,
                false
            };

        } else if (dataRegister == GATEWAY_REGISTER_AI) {
            register_channel = {
                FASTYBIRD_CHANNEL_TYPE_ANALOG_SENSOR,
                FASTYBIRD_CHANNEL_TYPE_ANALOG_SENSOR,
                _gateway_nodes[id].registers_size[dataRegister],
                false,
                false,
                false,
                false
            };

        } else if (dataRegister == GATEWAY_REGISTER_AO) {
            register_channel = {
                FASTYBIRD_CHANNEL_TYPE_ANALOG_ACTOR,
                FASTYBIRD_CHANNEL_TYPE_ANALOG_ACTOR,
                _gateway_nodes[id].registers_size[dataRegister],
                false,
                false,
                false,
                false
            };
        }

        if (_gateway_nodes[id].registers_size[dataRegister] > 0) {
            fastybird_channel_property_t register_property = _gatewayFastybirdGetChannelProperty(id, dataRegister);

            register_channel.properties.push_back(register_property);
        }

        return register_channel;
    }

// -----------------------------------------------------------------------------

    void _gatewayRegisterFastybirdNode(
        const uint8_t id
    ) {
        fastybird_node_hardware_t hardware = {
            _gateway_nodes[id].hardware.model,
            _gateway_nodes[id].hardware.version,
            _gateway_nodes[id].hardware.manufacturer
        };

        fastybird_node_software_t software = {
            _gateway_nodes[id].firmware.model,
            _gateway_nodes[id].firmware.version,
            _gateway_nodes[id].firmware.manufacturer
        };

        fastybird_node_t register_node = {
            _gateway_nodes[id].serial_number,
            hardware,
            software,
            false
        };

        register_node.channels.push_back(_gatewayFastybirdGetChannelStructure(id, GATEWAY_REGISTER_DI));
        register_node.channels.push_back(_gatewayFastybirdGetChannelStructure(id, GATEWAY_REGISTER_DO));
        register_node.channels.push_back(_gatewayFastybirdGetChannelStructure(id, GATEWAY_REGISTER_AI));
        register_node.channels.push_back(_gatewayFastybirdGetChannelStructure(id, GATEWAY_REGISTER_AO));

        /*
        for (uint8_t i = 0; i < _gateway_nodes[nodeIndex].settings.size(); i++) {
            register_node.settings.push_back({
                _gateway_nodes[nodeIndex].settings[i].key.c_str(),
                _gateway_nodes[nodeIndex].settings[i].value.c_str()
            });
        }
        */

        fastybirdRegisterNode(register_node);
    }
#endif

// -----------------------------------------------------------------------------

void _gatewayBootup()
{
    String node_sn;

    for (uint8_t i = 0; i < NODES_GATEWAY_MAX_NODES; i++) {
        node_sn = getSetting("nodeSn", i, GATEWAY_DESCRIPTION_NOT_SET);

        if (node_sn.equals(GATEWAY_DESCRIPTION_NOT_SET) == false) {
            strncpy(_gateway_nodes[i].serial_number, node_sn.c_str(), (node_sn.length() + 1));
        }
    }
}

// -----------------------------------------------------------------------------

#if FASTYBIRD_SUPPORT
    void _gatewayReportAnalogRegisterValue(
        const uint8_t id,
        const uint8_t dataRegister,
        const uint8_t address,
        const uint8_t channel
    ) {
        String payload;

        switch (_gatewayGetAnalogRegistersDataType(id, dataRegister, address))
        {
            case GATEWAY_DATA_TYPE_UINT8:
                uint8_t uint8_value;

                _gatewayReadAnalogRegisterValue(id, dataRegister, address, uint8_value);

                payload = String(uint8_value);
                break;

            case GATEWAY_DATA_TYPE_UINT16:
                uint16_t uint16_value;

                _gatewayReadAnalogRegisterValue(id, dataRegister, address, uint16_value);

                payload = String(uint16_value);
                break;

            case GATEWAY_DATA_TYPE_UINT32:
                uint32_t uint32_value;

                _gatewayReadAnalogRegisterValue(id, dataRegister, address, uint32_value);

                payload = String(uint32_value);
                break;

            case GATEWAY_DATA_TYPE_INT8:
                int8_t int8_value;

                _gatewayReadAnalogRegisterValue(id, dataRegister, address, int8_value);

                payload = String(int8_value);
                break;

            case GATEWAY_DATA_TYPE_INT16:
                int16_t int16_value;

                _gatewayReadAnalogRegisterValue(id, dataRegister, address, int16_value);

                payload = String(int16_value);
                break;

            case GATEWAY_DATA_TYPE_INT32:
                int32_t int32_value;

                _gatewayReadAnalogRegisterValue(id, dataRegister, address, int32_value);

                payload = String(int32_value);
                break;

            case GATEWAY_DATA_TYPE_FLOAT32:
                float float_value;

                _gatewayReadAnalogRegisterValue(id, dataRegister, address, float_value);

                payload = String(float_value);
                break;
        }

        _fastybirdReportNodeChannelValue(
            _gateway_nodes[id].serial_number,
            channel,
            address,
            payload.c_str()
        );
    }
#endif

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
        }
    #endif
}

// -----------------------------------------------------------------------------

void _gatewayResetNodeIndex(
    const uint8_t id
) {
    // Reset addressing
    _gateway_nodes[id].addressing.registration = 0;
    _gateway_nodes[id].addressing.state = false;
    _gateway_nodes[id].addressing.lost = 0;

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
// NODE INITIALIZATION
// -----------------------------------------------------------------------------

/**
 * Node is fully initialized & ready for receiving commands
 */
void _gatewayMarkNodeAsInitialized(
    const uint8_t id
) {
    _gateway_nodes[id].initiliazation.step = GATEWAY_PACKET_NONE;
    _gateway_nodes[id].initiliazation.state = true;
    _gateway_nodes[id].initiliazation.attempts = 0;

    _gateway_nodes[id].packet.waiting_for = GATEWAY_PACKET_NONE;
    _gateway_nodes[id].packet.sending_time = 0;

    // Store node SN into memory
    setSetting("nodeSn", id, _gateway_nodes[id].serial_number);

    #if WEB_SUPPORT && WS_SUPPORT
        // Propagate nodes structure to WS clients
        wsSend(_gatewayWebSocketUpdate);
    #endif

    #if FASTYBIRD_SUPPORT
        _gatewayRegisterFastybirdNode(id);
    #endif
}

// -----------------------------------------------------------------------------

uint8_t _gatewayGetNodeAddressToInitialize() {
    for (uint8_t i = 0; i < NODES_GATEWAY_MAX_NODES; i++) {
        if (
            // Check if node has finished addressing procedure
            _gateway_nodes[i].addressing.state == true
            // Check if node is not initialized
            && _gateway_nodes[i].initiliazation.state == false
            && _gateway_nodes[i].packet.waiting_for == GATEWAY_PACKET_NONE
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

/**
 * Send initiliaziation packet to node
 */
void _gatewaySendInitializationPacket(
    const uint8_t id,
    const uint8_t requestState
) {
    char output_content[1];

    // Packet identifier at first postion
    output_content[0] = requestState;

    // Send packet to node
    bool result = _gatewaySendPacket((id + 1), output_content, 1);

    // When successfully sent...
    if (result == true) {
        // ...add mark, that gateway is waiting for reply from node
        _gateway_nodes[id].packet.waiting_for = requestState;
        _gateway_nodes[id].packet.sending_time = millis();

        _gateway_nodes[id].initiliazation.attempts = _gateway_nodes[id].initiliazation.attempts + 1;
    }
}

// -----------------------------------------------------------------------------

/**
 * Send register structure request packet to node
 */
void _gatewaySendRegisterInitializationPacket(
    const uint8_t id,
    const uint8_t requestState,
    const uint8_t start
) {
    char output_content[_gateway_nodes[id].packet.max_length];

    // Packet identifier at first postion
    output_content[0] = requestState;
    output_content[1] = (char) (start >> 8);
    output_content[2] = (char) (start & 0xFF);
    
    // It is based on maximum packed size reduced by packet header (4 bytes)
    uint8_t max_readable_addresses = start + _gateway_nodes[id].packet.max_length - 4;

    switch (requestState)
    {
        case GATEWAY_PACKET_AI_REGISTERS_STRUCTURE:
            if (max_readable_addresses <= _gateway_nodes[id].registers_size[GATEWAY_REGISTER_AI]) {
                output_content[3] = (char) (max_readable_addresses >> 8);
                output_content[4] = (char) (max_readable_addresses & 0xFF);

            } else {
                output_content[3] = (char) (_gateway_nodes[id].registers_size[GATEWAY_REGISTER_AI] >> 8);
                output_content[4] = (char) (_gateway_nodes[id].registers_size[GATEWAY_REGISTER_AI] & 0xFF);
            }
            break;

        case GATEWAY_PACKET_AO_REGISTERS_STRUCTURE:
            if (max_readable_addresses <= _gateway_nodes[id].registers_size[GATEWAY_REGISTER_AO]) {
                output_content[3] = (char) (max_readable_addresses >> 8);
                output_content[4] = (char) (max_readable_addresses & 0xFF);

            } else {
                output_content[3] = (char) (_gateway_nodes[id].registers_size[GATEWAY_REGISTER_AO] >> 8);
                output_content[4] = (char) (_gateway_nodes[id].registers_size[GATEWAY_REGISTER_AO] & 0xFF);
            }
            break;
    }

    // Send packet to node
    bool result = _gatewaySendPacket((id + 1), output_content, 5);

    // When successfully sent...
    if (result == true) {
        // ...add mark, that gateway is waiting for reply from node
        _gateway_nodes[id].packet.waiting_for = requestState;
        _gateway_nodes[id].packet.sending_time = millis();

        _gateway_nodes[id].initiliazation.attempts = _gateway_nodes[id].initiliazation.attempts + 1;
    }
}

// -----------------------------------------------------------------------------

/**
 * Get first not initialized node from the list & try to continue in initiliazation
 */
void _gatewayContinueInNodeInitialization(
    const uint8_t address
) {
    // Convert node address to index
    uint8_t index = address - 1;

    if (
        // Check if node has finished addressing procedure
        _gateway_nodes[index].addressing.state == true
        // Check if node is not initialized
        && _gateway_nodes[index].initiliazation.state == false
        && _gateway_nodes[index].initiliazation.attempts <= NODES_GATEWAY_MAX_INIT_ATTEMPTS
        && _gateway_nodes[index].packet.waiting_for == GATEWAY_PACKET_NONE
    ) {
        switch (_gateway_nodes[index].initiliazation.step)
        {
            case GATEWAY_PACKET_HW_MODEL:
            case GATEWAY_PACKET_HW_MANUFACTURER:
            case GATEWAY_PACKET_HW_VERSION:
            case GATEWAY_PACKET_FW_MODEL:
            case GATEWAY_PACKET_FW_MANUFACTURER:
            case GATEWAY_PACKET_FW_VERSION:
            case GATEWAY_PACKET_REGISTERS_SIZE:
                _gatewaySendInitializationPacket(index, _gateway_nodes[index].initiliazation.step);
                break;

            case GATEWAY_PACKET_AI_REGISTERS_STRUCTURE:
            case GATEWAY_PACKET_AO_REGISTERS_STRUCTURE:
                _gatewaySendRegisterInitializationPacket(index, _gateway_nodes[index].initiliazation.step, 0);
                break;
        }
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
    if (_gatewayIsPacketInGroup(packet_id, gateway_packets_addresing, GATEWAY_PACKET_ADDRESS_MAX)) {
        _gatewayAddressRequestHandler(packet_id, sender_address, payload);

    // Node send reply to request
    } else {
        if (sender_address == PJON_NOT_ASSIGNED) {
            DEBUG_MSG(PSTR("[GATEWAY][ERR] Received packet is without sender address\n"));

            return;
        }

        // Check if gateway is waiting for reply from node (initiliazation sequence)
        if (_gateway_nodes[(sender_address - 1)].packet.waiting_for == GATEWAY_PACKET_NONE) {
            DEBUG_MSG(
                PSTR("[GATEWAY][ERR] Received packet for node with address: %d but gateway is not waiting for packet from this node\n"),
                sender_address
            );

            return;
        }

        // Check if gateway is waiting for reply from node (initiliazation sequence)
        if (_gateway_nodes[(sender_address - 1)].packet.waiting_for != packet_id) {
            DEBUG_MSG(
                PSTR("[GATEWAY][ERR] Received packet: %s for node with address: %d but gateway is waiting for: %s\n"),
                _gatewayPacketName(packet_id).c_str(),
                sender_address,
                _gatewayPacketName(_gateway_nodes[(sender_address - 1)].packet.waiting_for).c_str()
            );

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
        wsOnConnectRegister(_gatewayWSOnConnect);
        wsOnConfigureRegister(_gatewayWSOnConfigure);
        wsOnActionRegister(_gatewayWSOnAction);
    #endif

    _gatewayBootup();

    firmwareRegisterLoop(gatewayLoop);
}

// -----------------------------------------------------------------------------

void gatewayLoop() {
    // If system is flagged unstable we do not init nodes networks
    #if STABILTY_CHECK_ENABLED
        if (!stabiltyCheck()) {
            return;
        }
    #endif

    // Little delay before gateway start
    if (millis() < NODES_GATEWAY_SEARCH_DELAY) {
        return;
    }

    _gatewayCheckPacketsDelays();

    // If some node is not initialized, get its address
    uint8_t node_to_initialize = _gatewayGetNodeAddressToInitialize();

    if ((millis() - NODES_GATEWAY_SEARCH_DELAY) < NODES_GATEWAY_ADDRESSING_TIMEOUT) {
        _gatewaySearchForNodes();

    // Check nodes presence in given interval
    } else if (millis() - _gateway_last_nodes_check > NODES_GATEWAY_NODES_CHECK_INTERVAL) {
        _gateway_last_nodes_check = millis();

        _gatewayCheckNodesPresence();

    // Check if all connected nodes are initialized
    } else if (node_to_initialize != 0) {
        _gatewayContinueInNodeInitialization(node_to_initialize);

    // Continue in nodes registers reading
    } else {
        _gatewayReadNodes();
    }

    _gateway_bus.receive(50000);
    _gateway_bus.update();
}

#endif // NODES_GATEWAY_SUPPORT