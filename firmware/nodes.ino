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

unsigned long _gateway_last_heartbeat = 0;

typedef struct {
    byte index;
    String value;
} node_channel_t;

typedef struct {
    byte type;
    byte length;
    bool readable;
    bool writable;
    byte format;
} node_channel_structure_t;

typedef struct {
    String key;
    String value;
} node_setting_t;

typedef struct {
    String manufacturer;
    String model;
    String version;
} node_hardware_t;

typedef struct {
    String manufacturer;
    String model;
    String version;
} node_firmware_t;

typedef struct {
    // Node basic info
    String node;
    byte address;

    // Node initialization statuses
    bool initialized;

    unsigned long heartbeat;
    unsigned long last_ping;
    unsigned long last_updated;

    node_hardware_t hardware;
    node_firmware_t firmware;

    // Node channels
    std::vector<node_channel_structure_t> channels;

    std::vector<node_channel_t> inputs;
    std::vector<node_channel_t> outputs;
    std::vector<node_channel_t> buttons;
    std::vector<node_channel_t> switches;

    std::vector<node_setting_t> settings;
} node_t;

std::vector<node_t> _gateway_nodes;

// -----------------------------------------------------------------------------
// MODULE PRIVATE
// -----------------------------------------------------------------------------

void _nodesCollectNodes(
    JsonArray& container
) {
    for(byte i = 0; i < _gateway_nodes.size(); i++) {
        // Check if node was fully initialized
        if (_gateway_nodes[i].initialized == true) {
            JsonObject& node = container.createNestedObject();

            // Node basic info
            node["node"] = _gateway_nodes[i].node;
            node["address"] = _gateway_nodes[i].address;

            node["heartbeat"] = _gateway_nodes[i].heartbeat;
            node["last_ping"] = _gateway_nodes[i].last_ping;
            node["last_updated"] = _gateway_nodes[i].last_updated;

            JsonObject& hardware = node.createNestedObject("hardware");

            hardware["manufacturer"] = _gateway_nodes[i].hardware.manufacturer;
            hardware["model"] = _gateway_nodes[i].hardware.model;
            hardware["version"] = _gateway_nodes[i].hardware.version;

            JsonObject& firmware = node.createNestedObject("firmware");

            firmware["manufacturer"] = _gateway_nodes[i].firmware.manufacturer;
            firmware["model"] = _gateway_nodes[i].firmware.model;
            firmware["version"] = _gateway_nodes[i].firmware.version;

            // Node channels schema
            JsonArray& node_channels = node.createNestedArray("channels");

            for(byte j = 0; j < _gateway_nodes[i].channels.size(); j++) {
                JsonObject& node_channel = node_channels.createNestedObject();

                JsonObject& node_channel_schema = node_channel.createNestedObject("schema");

                node_channel_schema["length"] = _gateway_nodes[i].channels[j].length;
                node_channel_schema["readable"] = _gateway_nodes[i].channels[j].readable;
                node_channel_schema["writable"] = _gateway_nodes[i].channels[j].writable;

                switch (_gateway_nodes[i].channels[j].type) {
                    case GATEWAY_NODE_CHANNEL_TYPE_BUTTON:
                        node_channel_schema["type"] = "button";
                        break;

                    case GATEWAY_NODE_CHANNEL_TYPE_INPUT:
                        node_channel_schema["type"] = "input";
                        break;

                    case GATEWAY_NODE_CHANNEL_TYPE_OUTPUT:
                        node_channel_schema["type"] = "output";
                        break;

                    case GATEWAY_NODE_CHANNEL_TYPE_SWITCH:
                        node_channel_schema["type"] = "switch";
                        break;

                    default:
                        node_channel_schema["type"] = (char *) NULL;
                        break;
                }

                switch (_gateway_nodes[i].channels[j].format) {
                    case GATEWAY_NODE_CHANNEL_FORMAT_EVENT:
                        node_channel_schema["format"] = "event";
                        break;

                    case GATEWAY_NODE_CHANNEL_FORMAT_DIGITAL:
                        node_channel_schema["format"] = "boolean";
                        break;

                    case GATEWAY_NODE_CHANNEL_FORMAT_ANALOG:
                        node_channel_schema["format"] = "float";
                        break;

                    case GATEWAY_NODE_CHANNEL_FORMAT_NUMERIC:
                        node_channel_schema["format"] = "number";
                        break;

                    default:
                        node_channel_schema["format"] = (char *) NULL;
                        break;
                }

                // Node channels values container
                JsonArray& node_channel_values = node_channel.createNestedArray("values");

                std::vector<node_channel_t> channels;

                if (_gateway_nodes[i].channels[j].type == GATEWAY_NODE_CHANNEL_TYPE_BUTTON) {
                    channels = _gateway_nodes[i].buttons;

                } else if (_gateway_nodes[i].channels[j].type == GATEWAY_NODE_CHANNEL_TYPE_SWITCH) {
                    channels = _gateway_nodes[i].switches;

                } else if (_gateway_nodes[i].channels[j].type == GATEWAY_NODE_CHANNEL_TYPE_INPUT) {
                    channels = _gateway_nodes[i].inputs;

                } else if (_gateway_nodes[i].channels[j].type == GATEWAY_NODE_CHANNEL_TYPE_OUTPUT) {
                    channels = _gateway_nodes[i].outputs;
                }

                for (byte k = 0; k < channels.size(); k++) {
                    if (channels[k].value.length() == 0) {
                        node_channel_values.add((char *) NULL);

                    } else {
                        switch (_gateway_nodes[i].channels[j].format) {
                            case GATEWAY_NODE_CHANNEL_FORMAT_EVENT:
                                node_channel_values.add(channels[k].value);
                                break;

                            case GATEWAY_NODE_CHANNEL_FORMAT_DIGITAL:
                                node_channel_values.add(strcmp(channels[k].value.c_str(), "1") == 0 ? true : false);
                                break;

                            case GATEWAY_NODE_CHANNEL_FORMAT_ANALOG:
                                node_channel_values.add(channels[k].value.toFloat());
                                break;

                            case GATEWAY_NODE_CHANNEL_FORMAT_NUMERIC:
                                node_channel_values.add(channels[k].value.toInt());
                                break;

                            default:
                                node_channel_values.add((char *) NULL);
                                break;
                        }
                    }
                }
            }
        }
    }
}

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
        uint32_t client_id,
        const char * action,
        JsonObject& data
    ) {
        if (
            strcmp(action, "switch") == 0
            && data.containsKey("node")
            && data.containsKey("channel")
            && data.containsKey("index")
            && data.containsKey("value")
        ) {
            for (byte i = 0; i < _gateway_nodes.size(); i++) {
                // Search for node
                if (_gateway_nodes[i].node.compareTo(String(data["node"].as<const char *>())) == 0) {
                    DEBUG_MSG(PSTR("[GATEWAY] Found node to update\n"));

                    // Check if channel & channel index is in valid range
                    if (
                        data["channel"].as<byte>() < _gateway_nodes[i].channels.size()
                        && data["index"].as<byte>() < _gateway_nodes[i].channels[data["channel"].as<byte>()].length
                    ) {
                        DEBUG_MSG(PSTR("[GATEWAY] Channel index to update is ok\n"));

                        // Only writable channels could be updated
                        if (_gateway_nodes[i].channels[data["channel"].as<byte>()].writable) {
                            switch (_gateway_nodes[i].channels[data["channel"].as<byte>()].format)
                            {
                                case GATEWAY_NODE_CHANNEL_FORMAT_ANALOG:
                                    _communicationSendDataToNode(
                                        _gateway_nodes[i].address,
                                        _gateway_nodes[i].channels[data["channel"].as<byte>()].type,
                                        data["index"].as<byte>(),
                                        data["value"].as<float>()
                                    );
                                    break;

                                case GATEWAY_NODE_CHANNEL_FORMAT_DIGITAL:
                                    _communicationSendDataToNode(
                                        _gateway_nodes[i].address,
                                        _gateway_nodes[i].channels[data["channel"].as<byte>()].type,
                                        data["index"].as<byte>(),
                                        data["value"].as<bool>()
                                    );
                                    break;

                                case GATEWAY_NODE_CHANNEL_FORMAT_NUMERIC:
                                    _communicationSendDataToNode(
                                        _gateway_nodes[i].address,
                                        _gateway_nodes[i].channels[data["channel"].as<byte>()].type,
                                        data["index"].as<byte>(),
                                        data["value"].as<int>()
                                    );
                                    break;

                                case GATEWAY_NODE_CHANNEL_FORMAT_EVENT:
                                    _communicationSendDataToNode(
                                        _gateway_nodes[i].address,
                                        _gateway_nodes[i].channels[data["channel"].as<byte>()].type,
                                        data["index"].as<byte>(),
                                        data["value"].as<const char *>()
                                    );
                                    break;
                            }
                        } else {
                            DEBUG_MSG(PSTR("[GATEWAY][ERR] Channel is not writtable\n"));
                        }
                    }
                }
            }

            // Propagate nodes structure to WS clients
            wsSend(_nodesWebSocketUpdate);
/*
            JsonArray& nodes = data["nodes"].as<JsonArray&>();

            for (byte i = 0; i < nodes.size(); i++) {
                JsonObject& node = nodes[i];

                if (node.containsKey("channels")) {
                    JsonArray& channels = node["channels"].as<JsonArray&>();

                    for (byte j = 0; j < channels.size(); j++) {
                        JsonObject& channel = channels[j].as<JsonObject&>();

                        if (channel.containsKey("values")) {
                            JsonArray& values = channel["values"].as<JsonArray&>();

                            for (byte k = 0; k < values.size(); k++) {
                                // Check if channel is writable
                                if (_gateway_nodes[i].channels[j].writable) {
                                    switch (_gateway_nodes[i].channels[j].format)
                                    {
                                        case GATEWAY_NODE_CHANNEL_FORMAT_ANALOG:
                                            _communicationSendDataToNode(_gateway_nodes[i].address, _gateway_nodes[i].channels[j].type, k, values[i].as<float>());
                                            break;

                                        case GATEWAY_NODE_CHANNEL_FORMAT_DIGITAL:
                                            _communicationSendDataToNode(_gateway_nodes[i].address, _gateway_nodes[i].channels[j].type, k, values[i].as<bool>());
                                            break;

                                        case GATEWAY_NODE_CHANNEL_FORMAT_NUMERIC:
                                            _communicationSendDataToNode(_gateway_nodes[i].address, _gateway_nodes[i].channels[j].type, k, values[i].as<int>());
                                            break;

                                        case GATEWAY_NODE_CHANNEL_FORMAT_EVENT:
                                            _communicationSendDataToNode(_gateway_nodes[i].address, _gateway_nodes[i].channels[j].type, k, values[i].as<const char *>());
                                            break;
                                    }
                                }
                            }
                        }
                    }
                }
            }
            */
        }
    }
#endif

// -----------------------------------------------------------------------------

#if FASTYBIRD_SUPPORT
    void _gatewayRegisterFastybirdNode(
        byte nodeIndex
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

        for (byte i = 0; i < _gateway_nodes[nodeIndex].channels.size(); i++) {
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

        for (byte i = 0; i < _gateway_nodes[nodeIndex].settings.size(); i++) {
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

bool _gatewayIsKnownNode(
    byte senderId,
    byte packetId
) {
    int node_index = _gatewayFindNodeIndexByAddress(senderId);

    if (node_index != -1) {
        return true;
    }
    
    DEBUG_MSG(PSTR("[GATEWAY][ERR] Received packet: %s for not initailized node: %d\n"), _gatewayPacketName(packetId).c_str(), senderId);

    _gatewayReplyPacket(GATEWAY_PACKET_WHO_ARE_YOU);

    return false;
}

// -----------------------------------------------------------------------------

void _gatewayReceiveHandler(
    uint8_t * payload,
    uint16_t length,
    const PJON_Packet_Info &packetInfo
) {
    char local_payload[length];

    for (uint16_t i = 0; i < length; i++) {
        local_payload[i] = (char) payload[i];
    }

    //DEBUG_MSG(PSTR("[GATEWAY] Received data: %s\n"), (char) (payload[0]));
    //DEBUG_MSG(PSTR("[GATEWAY] Received data: %d\n"), (uint16_t)(payload[1] << 8) | (uint16_t)(payload[2] & 0xFF));
    DEBUG_MSG(PSTR("[GATEWAY] Received data: %d\n"), (byte) (payload[0]));
    DEBUG_MSG(PSTR("[GATEWAY] Received data: %s\n"), (char) (payload[1]));
    DEBUG_MSG(PSTR("[GATEWAY] Received data: %s\n"), (char) (payload[2]));
    //DEBUG_MSG(PSTR("[GATEWAY] Received data: %s\n"), (const char *) payload[1]);

    DynamicJsonBuffer jsonBuffer;
    
    JsonObject& parsed_packet = jsonBuffer.parseObject(local_payload);

    if (!parsed_packet.success()) {
        DEBUG_MSG(PSTR("[GATEWAY][ERR] Error parsing packet payload data\n"));

        return;
    }

    DEBUG_MSG(PSTR("[GATEWAY] Successfully parsed packed payload\n"));

    // Check if packet identifier is present
    if (parsed_packet.containsKey("P") == false)  {
        DEBUG_MSG(PSTR("[GATEWAY][ERR] Missing packet identifier\n"));

        return;
    }

    byte sender_id = 0;

    // Get sender address from header
    if (packetInfo.header & PJON_TX_INFO_BIT) {
        sender_id = packetInfo.sender_id;
    }

    if (sender_id == 0) {
        DEBUG_MSG(PSTR("[GATEWAY][ERR] Received packet is without sender address\n"));

        _gatewayReplyPacket(GATEWAY_PACKET_UNKNOWN);

        return;
    }

    byte packet_id = parsed_packet["P"].as<byte>();

    // Check if node is allready in collection
    int node_index = _gatewayFindNodeIndexByAddress(sender_id);

    DEBUG_MSG(PSTR("[GATEWAY] Received packet: %s for node with identifier: %d\n"), _gatewayPacketName(packet_id).c_str(), sender_id);

    switch (packet_id) {
        // Node is searching for master
        case GATEWAY_PACKET_MASTER_LOOKUP:
            // Send packet acceptation to node
            _gatewayAcceptPacket(GATEWAY_PACKET_MASTER_LOOKUP);
            break;

        // Node is sending hearbeat data request. Master have to respond with PONG
        case GATEWAY_PACKET_HEARTBEAT:
            if (_gatewayIsKnownNode(sender_id, packet_id)) {
                // U    = Node uptime
                if (parsed_packet.containsKey("U"))  {
                    // Send packet acceptation to node
                    _gatewayReplyPacket(GATEWAY_PACKET_PONG);

                    _gateway_nodes[node_index].heartbeat = parsed_packet["U"].as<unsigned long>();

                } else {
                    _gatewayReplyPacket(GATEWAY_PACKET_ERROR);

                    DEBUG_MSG(PSTR("[GATEWAY][ERR] Invalid HEARTBEAT packet structure.\n"));
                }

            } else {
                DEBUG_MSG(PSTR("[GATEWAY][ERR] Received HEARTBEAT from unknown node: %d\n"), sender_id);
            }
            break;

        // Node is sending PING request. Master have to respond with PONG
        case GATEWAY_PACKET_PING:
            if (_gatewayIsKnownNode(sender_id, packet_id)) {
                // Send packet acceptation to node
                _gatewayReplyPacket(GATEWAY_PACKET_PONG);

                _gateway_nodes[node_index].last_ping = millis();

            } else {
                DEBUG_MSG(PSTR("[GATEWAY][ERR] Received ping from unknown node: %d\n"), sender_id);
            }
            break;

        // Node is sending initialization request
        case GATEWAY_PACKET_INIT_START:
            // Check for valid structure
            // N    = Node serial number
            if (parsed_packet.containsKey("N"))  {
                // Send packet acceptation to node
                _gatewayAcceptPacket(packet_id);

                DEBUG_MSG(PSTR("[GATEWAY] Starting initialization of node\n"));
                DEBUG_MSG(PSTR("[GATEWAY]     Node: %s\n"), parsed_packet["N"].as<char *>());

                // If node is already collection...
                if (node_index != -1) {
                    // ...update it
                    _gateway_nodes[node_index].node = String(parsed_packet["N"].as<char *>());
                    _gateway_nodes[node_index].initialized = false;
                    _gateway_nodes[node_index].heartbeat = 0;
                    _gateway_nodes[node_index].last_ping = 0;
                    _gateway_nodes[node_index].last_updated = millis();

                    _gateway_nodes[node_index].channels.clear();

                    _gateway_nodes[node_index].inputs.clear();
                    _gateway_nodes[node_index].outputs.clear();
                    _gateway_nodes[node_index].buttons.clear();
                    _gateway_nodes[node_index].switches.clear();

                    _gateway_nodes[node_index].settings.clear();

                } else {
                    // ...if not, create it
                    _gateway_nodes.push_back((node_t) {
                        String(parsed_packet["N"].as<char *>()),    // Node type
                        sender_id,                                  // Node network address
                        false,                                      // Not finished initialization flag
                        0,                                          // Node heartbeat set
                        0,                                          // Node ping set
                        millis()                                    // Record update timestamp
                    });
                }

            } else {
                _gatewayReplyPacket(GATEWAY_PACKET_ERROR);

                DEBUG_MSG(PSTR("[GATEWAY][ERR] Invalid NODE INIT packet structure.\n"));
            }

            break;

        // Node is sending hardware details request
        case GATEWAY_PACKET_HARDWARE_INFO:
            if (_gatewayIsKnownNode(sender_id, packet_id)) {
                // If node has finished initialization, hardware structure can not be changed
                if (_gateway_nodes[node_index].initialized) {
                    _gatewayReplyPacket(GATEWAY_PACKET_ERROR);

                    DEBUG_MSG(PSTR("[GATEWAY][ERR] Initialized node: %d could not be updated\n"), sender_id);

                // Check for valid structure
                // M = Manufacturer
                // T = Model/Type
                // V = Version
                } else if (parsed_packet.containsKey("M") && parsed_packet.containsKey("T") && parsed_packet.containsKey("V"))  {
                    DEBUG_MSG(PSTR("[GATEWAY] Configuring settings for node: %d\n"), sender_id);

                    _gatewayAcceptPacket(packet_id);

                    // Record last update timestamp
                    _gateway_nodes[node_index].last_updated = millis();

                    _gateway_nodes[node_index].hardware = (node_hardware_t) {
                        String(parsed_packet["M"].as<char *>()),
                        String(parsed_packet["T"].as<char *>()),
                        String(parsed_packet["V"].as<char *>())
                    };

                } else {
                    _gatewayReplyPacket(GATEWAY_PACKET_ERROR);

                    DEBUG_MSG(PSTR("[GATEWAY][ERR] Invalid HARDWARE packet structure.\n"));
                }
            }
            break;

        case GATEWAY_PACKET_FIRMWARE_INFO:
            if (_gatewayIsKnownNode(sender_id, packet_id)) {
                // If node has finished initialization, firmware structure can not be changed
                if (_gateway_nodes[node_index].initialized) {
                    _gatewayReplyPacket(GATEWAY_PACKET_ERROR);

                    DEBUG_MSG(PSTR("[GATEWAY][ERR] Initialized node: %d could not be updated\n"), sender_id);

                // Check for valid structure
                // M = Manufacturer
                // T = Model/Type
                // V = Version
                } else if (parsed_packet.containsKey("M") && parsed_packet.containsKey("T") && parsed_packet.containsKey("V"))  {
                    DEBUG_MSG(PSTR("[GATEWAY] Configuring settings for node: %d\n"), sender_id);

                    _gatewayAcceptPacket(packet_id);

                    // Record last update timestamp
                    _gateway_nodes[node_index].last_updated = millis();

                    _gateway_nodes[node_index].firmware = (node_firmware_t) {
                        String(parsed_packet["M"].as<char *>()),
                        String(parsed_packet["T"].as<char *>()),
                        String(parsed_packet["V"].as<char *>())
                    };

                } else {
                    _gatewayReplyPacket(GATEWAY_PACKET_ERROR);

                    DEBUG_MSG(PSTR("[GATEWAY][ERR] Invalid FIRMWARE packet structure.\n"));
                }
            }
            break;

        // Node sent settings schema
        case GATEWAY_PACKET_SETTINGS_SCHEMA:
            if (_gatewayIsKnownNode(sender_id, packet_id)) {
                // If node has finished initialization, channel structure can not be changed
                if (_gateway_nodes[node_index].initialized) {
                    _gatewayReplyPacket(GATEWAY_PACKET_ERROR);

                    DEBUG_MSG(PSTR("[GATEWAY][ERR] Initialized node: %d could not be updated\n"), sender_id);

                // Check for valid structure
                // A = Attribute name
                // T = Attribute type (Number|Select|Boolean)
                // V = Attribute value
                } else if (parsed_packet.containsKey("A") && parsed_packet.containsKey("T") && parsed_packet.containsKey("V"))  {
                    DEBUG_MSG(PSTR("[GATEWAY] Configuring settings for node: %d\n"), sender_id);

                    _gatewayAcceptPacket(packet_id);

                    // Record last update timestamp
                    _gateway_nodes[node_index].last_updated = millis();

                    // TODO: finish implementation...

                } else {
                    _gatewayReplyPacket(GATEWAY_PACKET_ERROR);

                    DEBUG_MSG(PSTR("[GATEWAY][ERR] Invalid SETTINGS SCHEMA packet structure.\n"));
                }
            }
            break;

        // Node sent channels schema
        case GATEWAY_PACKET_CHANNELS_SCHEMA:
            if (_gatewayIsKnownNode(sender_id, packet_id)) {
                // If node has finished initialization, channel structure can not be changed
                if (_gateway_nodes[node_index].initialized) {
                    _gatewayReplyPacket(GATEWAY_PACKET_ERROR);

                    DEBUG_MSG(PSTR("[GATEWAY][ERR] Initialized node: %d could not be updated\n"), sender_id);

                // Check for valid structure
                // T    = Channel type              => (button|input|output|switch)
                // C    = Number of channels        => number
                // I    = Channel is readable       => true|false
                // O    = Channel is writtable      => true|false
                // F    = Channel data type format  => float|boolean|integer|char
                } else if (
                    parsed_packet.containsKey("T")
                    && parsed_packet.containsKey("C")
                    && parsed_packet.containsKey("I")
                    && parsed_packet.containsKey("O")
                    && parsed_packet.containsKey("F")
                ) {
                    DEBUG_MSG(PSTR("[GATEWAY] Configuring channels for node: %d\n"), sender_id);

                    _gatewayAcceptPacket(packet_id);

                    // Record last update timestamp
                    _gateway_nodes[node_index].last_updated = millis();

                    bool is_valid = true;

                    if (
                        parsed_packet["T"].as<byte>() != GATEWAY_NODE_CHANNEL_TYPE_BUTTON
                        && parsed_packet["T"].as<byte>() != GATEWAY_NODE_CHANNEL_TYPE_INPUT
                        && parsed_packet["T"].as<byte>() != GATEWAY_NODE_CHANNEL_TYPE_OUTPUT
                        && parsed_packet["T"].as<byte>() != GATEWAY_NODE_CHANNEL_TYPE_SWITCH
                    ) {
                        is_valid = false;
                    }

                    if (
                        parsed_packet["F"].as<byte>() != GATEWAY_NODE_CHANNEL_FORMAT_ANALOG
                        && parsed_packet["F"].as<byte>() != GATEWAY_NODE_CHANNEL_FORMAT_DIGITAL
                        && parsed_packet["F"].as<byte>() != GATEWAY_NODE_CHANNEL_FORMAT_NUMERIC
                        && parsed_packet["F"].as<byte>() != GATEWAY_NODE_CHANNEL_FORMAT_EVENT
                    ) {
                        is_valid = false;
                    }

                    if (is_valid == true) {
                        // Store channel schema
                        _gateway_nodes[node_index].channels.push_back((node_channel_structure_t) {
                            parsed_packet["T"].as<byte>(),
                            parsed_packet["C"].as<byte>(),
                            parsed_packet.containsKey("I") && (strcmp(parsed_packet["I"].as<char *>(), "T") == 0 || parsed_packet["I"].as<bool>()) ? true : false,
                            parsed_packet.containsKey("O") && (strcmp(parsed_packet["O"].as<char *>(), "T") == 0 || parsed_packet["O"].as<bool>()) ? true : false,
                            parsed_packet["F"].as<byte>()
                        });

                        // Init node channels collection
                        for (byte i = 0; i < parsed_packet["C"].as<byte>(); i++) {
                            switch (parsed_packet["T"].as<byte>())
                            {
                                case GATEWAY_NODE_CHANNEL_TYPE_BUTTON:
                                    _gateway_nodes[node_index].buttons.push_back((node_channel_t) {i, ""});
                                    break;

                                case GATEWAY_NODE_CHANNEL_TYPE_INPUT:
                                    _gateway_nodes[node_index].inputs.push_back((node_channel_t) {i, ""});
                                    break;

                                case GATEWAY_NODE_CHANNEL_TYPE_OUTPUT:
                                    _gateway_nodes[node_index].outputs.push_back((node_channel_t) {i, ""});
                                    break;

                                case GATEWAY_NODE_CHANNEL_TYPE_SWITCH:
                                    _gateway_nodes[node_index].switches.push_back((node_channel_t) {i, ""});
                                    break;
                            }
                        }

                    } else {
                        DEBUG_MSG(PSTR("[GATEWAY][ERR] Invalid value in CHANNEL SCHEMA packet.\n"));
                    }

                } else {
                    _gatewayReplyPacket(GATEWAY_PACKET_ERROR);

                    DEBUG_MSG(PSTR("[GATEWAY][ERR] Invalid CHANNEL SCHEMA packet structure.\n"));
                }
            }
            break;

        // Node sent last initialization packed
        case GATEWAY_PACKET_INIT_END:
            if (_gatewayIsKnownNode(sender_id, packet_id)) {
                // Send packet acceptation to node
                _gatewayAcceptPacket(packet_id);

                _gateway_nodes[node_index].initialized = true;
                _gateway_nodes[node_index].last_updated = millis();

                #if WEB_SUPPORT && WS_SUPPORT
                    // Propagate nodes structure to WS clients
                    wsSend(_nodesWebSocketUpdate);
                #endif

                #if FASTYBIRD_SUPPORT
                    _gatewayRegisterFastybirdNode(node_index);
                #endif
            }
            break;

        // Node is sending data
        case GATEWAY_PACKET_DATA:
            if (_gatewayIsKnownNode(sender_id, packet_id)) {
                // Check for valid structure
                // T    = Channel type              => (button|input|output|switch)
                // I    = Channel index             => number
                // V    = Channel value             => float|boolean|integer|char
                if (parsed_packet.containsKey("T") && parsed_packet.containsKey("I") && parsed_packet.containsKey("V"))  {
                    switch (parsed_packet["T"].as<byte>())
                    {
                        // Received button event
                        case GATEWAY_NODE_CHANNEL_TYPE_BUTTON:
                            if (parsed_packet["I"].as<byte>() >= _gateway_nodes[node_index].buttons.size()) {
                                _gatewayReplyPacket(GATEWAY_PACKET_ERROR);

                                DEBUG_MSG(PSTR("[GATEWAY][ERR] Received node: %d channel value for unknown node channel with index: %d\n"), _gateway_nodes[node_index].address, parsed_packet["I"].as<byte>());
                                break;
                            }

                            _gatewayAcceptPacket(packet_id);

                            // Store received value into collection
                            _gateway_nodes[node_index].buttons[parsed_packet["I"].as<byte>()].value = String(parsed_packet["V"].as<const char *>());

                            DEBUG_MSG(PSTR("[GATEWAY] Received value: %s for channel: %d of node: %d\n"), parsed_packet["V"].as<const char *>(), parsed_packet["I"].as<byte>(), _gateway_nodes[node_index].address);

                            #if WEB_SUPPORT && WS_SUPPORT
                                wsSend(_nodesWebSocketUpdate);
                            #endif
                            break;

                        // Received digital input value
                        case GATEWAY_NODE_CHANNEL_TYPE_INPUT:
                            break;

                        // Received analog input value
                        case GATEWAY_NODE_CHANNEL_TYPE_OUTPUT:
                            break;

                        // Received switch status value
                        case GATEWAY_NODE_CHANNEL_TYPE_SWITCH:
                            if (parsed_packet["I"].as<byte>() >= _gateway_nodes[node_index].switches.size()) {
                                _gatewayReplyPacket(GATEWAY_PACKET_ERROR);

                                DEBUG_MSG(PSTR("[GATEWAY][ERR] Received node: %d channel value for unknown node channel with index: %d\n"), _gateway_nodes[node_index].address, parsed_packet["I"].as<byte>());
                                break;
                            }

                            _gatewayAcceptPacket(packet_id);

                            // Store received value into collection
                            _gateway_nodes[node_index].switches[parsed_packet["I"].as<byte>()].value = String(parsed_packet["V"].as<const char *>());

                            DEBUG_MSG(PSTR("[GATEWAY] Received value: %s for channel: %d of node: %d\n"), parsed_packet["V"].as<const char *>(), parsed_packet["I"].as<byte>(), _gateway_nodes[node_index].address);

                            #if WEB_SUPPORT && WS_SUPPORT
                                wsSend(_nodesWebSocketUpdate);
                            #endif
                            break;
                    
                        // Unknown channel type
                        default:
                            _gatewayReplyPacket(GATEWAY_PACKET_ERROR);

                            DEBUG_MSG(PSTR("[GATEWAY][ERR] Received unknown channel type for node: \n"), _gateway_nodes[node_index].address);
                            break;
                    }
                }
            }
            break;

        case GATEWAY_PACKET_UNKNOWN:
        default:
            DEBUG_MSG(PSTR("[GATEWAY] Received UNKNOWN packet from node: %d\n"), sender_id);

            _gatewayReplyPacket(GATEWAY_PACKET_UNKNOWN);
            break;
    }
}

// -----------------------------------------------------------------------------

void _gatewayErrorHandler(
    uint8_t code,
    uint16_t data,
    void *customPointer
) {
    if (code == PJON_CONNECTION_LOST) {
        DEBUG_MSG(PSTR("[GATEWAY][ERR] Connection lost with slave\n"));

    } else if (code == PJON_ID_ACQUISITION_FAIL) {
        if (data == PJON_ID_ACQUIRE) {
            DEBUG_MSG(PSTR("[GATEWAY][ERR] PJON error: multi-master addressing failed.\n"));

        } else if (data == PJON_ID_CONFIRM) {
            DEBUG_MSG(PSTR("[GATEWAY][ERR] PJON error: master-slave id confirmation failed.\n"));

        } else if (data == PJON_ID_NEGATE) {
            DEBUG_MSG(PSTR("[GATEWAY][ERR] PJON error: master-slave id release failed.\n"));

        } else if(data == PJON_ID_REQUEST) {
            DEBUG_MSG(PSTR("[GATEWAY][ERR] PJON error: master-slave id request failed.\n"));
        }
    }
}

// -----------------------------------------------------------------------------

void _communicationSendDataToNode(
    byte address,
    byte channelType,
    byte channel,
    float value
) {
    // Prepare output buffer for new packet
    char output_buffer[PJON_PACKET_MAX_LENGTH];

    sprintf(output_buffer, "{\"P\":%d, \"T\":%d, \"C\":%d, \"V\":%.4f}", GATEWAY_PACKET_DATA, channelType, channel, value);

    _gatewaySendPacket(address, GATEWAY_PACKET_DATA, output_buffer);
}

// -----------------------------------------------------------------------------

void _communicationSendDataToNode(
    byte address,
    byte channelType,
    byte channel,
    bool value
) {
    // Prepare output buffer for new packet
    char output_buffer[PJON_PACKET_MAX_LENGTH];

    sprintf(output_buffer, "{\"P\":%d, \"T\":%d, \"C\":%d, \"V\":%d}", GATEWAY_PACKET_DATA, channelType, channel, value ? 1 : 0);

    _gatewaySendPacket(address, GATEWAY_PACKET_DATA, output_buffer);
}

// -----------------------------------------------------------------------------

void _communicationSendDataToNode(
    byte address,
    byte channelType,
    byte channel,
    int value
) {
    // Prepare output buffer for new packet
    char output_buffer[PJON_PACKET_MAX_LENGTH];

    sprintf(output_buffer, "{\"P\":%d, \"T\":%d, \"C\":%d, \"V\":%d}", GATEWAY_PACKET_DATA, channelType, channel, value);

    _gatewaySendPacket(address, GATEWAY_PACKET_DATA, output_buffer);
}

// -----------------------------------------------------------------------------

void _communicationSendDataToNode(
    byte address,
    byte channelType,
    byte channel,
    const char * value
) {
    // Prepare output buffer for new packet
    char output_buffer[PJON_PACKET_MAX_LENGTH];

    sprintf(output_buffer, "{\"P\":%d, \"T\":%d, \"C\":%d, \"V\":%s}", GATEWAY_PACKET_DATA, channelType, channel, value);

    _gatewaySendPacket(address, GATEWAY_PACKET_DATA, output_buffer);
}

// -----------------------------------------------------------------------------

String _gatewayPacketName(
    byte packet
) {
    char buffer[40] = {0};

    if (packet < GATEWAY_PACKETS_MAX) {
        strncpy_P(buffer, gateway_packet_string[packet], sizeof(buffer));
    }

    return String(buffer);
}

// -----------------------------------------------------------------------------

int _gatewayFindNodeIndexByAddress(
    byte senderAddress
) {
    for (byte i = 0; i < _gateway_nodes.size(); i++) {
        if (_gateway_nodes[i].address == senderAddress) {
            return i;
        }
    }

    return -1;
}

// -----------------------------------------------------------------------------

bool _gatewayAcceptPacket(
    byte packet
) {
    // Reply packet payload container
    char output_buffer[PJON_PACKET_MAX_LENGTH];

    sprintf(output_buffer, "{\"P\":%d, \"A\":%d}", GATEWAY_PACKET_ACCEPTED, packet);

    _gatewayReplyPacket(packet, output_buffer);
}

// -----------------------------------------------------------------------------

bool _gatewayReplyPacket(
    byte packet
) {
    // Reply packet payload container
    char output_buffer[PJON_PACKET_MAX_LENGTH];

    sprintf(output_buffer, "{\"P\":%d}", packet);

    _gatewayReplyPacket(packet, output_buffer);
}

// -----------------------------------------------------------------------------

bool _gatewayReplyPacket(
    byte packet,
    char * payload
) {
    DEBUG_MSG(PSTR("[GATEWAY] Preparing reply packet: %s\n"), _gatewayPacketName(packet).c_str());

    _gateway_bus.reply(payload, strlen(payload));
}

// -----------------------------------------------------------------------------

bool _gatewaySendPacket(
    byte address,
    byte packet,
    char * payload
) {
    DEBUG_MSG(PSTR("[GATEWAY] Preparing packet: %s for node: %d\n"), _gatewayPacketName(packet).c_str(), address);

    uint16_t result = _gateway_bus.send_packet(
        address,            // Master address
        payload,            // Content
        strlen(payload),    // Content length
        _gateway_bus.config,// Header
        packet              // Packet identifier
    );

    if (result != PJON_ACK) {
        if (result == PJON_BUSY ) {
            DEBUG_MSG(PSTR("[GATEWAY][ERR] Sending packet failed, bus is busy\n"));

        } else if (result == PJON_FAIL) {
            DEBUG_MSG(PSTR("[GATEWAY][ERR] Sending packet failed\n"));

        } else {
            DEBUG_MSG(PSTR("[GATEWAY][ERR] Sending packet failed, unknonw error\n"));
        }

        return false;
    }

    DEBUG_MSG(PSTR("[GATEWAY] Sending packet sucess\n"));

    return true;
}

// -----------------------------------------------------------------------------
// MODULE CORE
// -----------------------------------------------------------------------------

void gatewaySetup() {
    _gateway_serial_bus.begin(SERIAL_BAUDRATE);

    _gateway_bus.set_packet_id(true);

    _gateway_bus.strategy.set_serial(&_gateway_serial_bus);

    // Set asynchronous acknowledgement response, avoid sync
    //_gateway_bus.set_synchronous_acknowledge(false);
    
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
    if (_gateway_last_heartbeat == 0 || ((millis() - _gateway_last_heartbeat) > 3000)) {
        if (_gateway_last_heartbeat != 0) {
            // Prepare output buffer for new packet
            char output_buffer[PJON_PACKET_MAX_LENGTH];
            output_buffer[0] = 4;

            _gatewaySendPacket(4, 2, output_buffer);
        }

        _gateway_last_heartbeat = millis();
    }

    _gateway_bus.receive(50000);
    _gateway_bus.update();
}

#endif // NODES_GATEWAY_SUPPORT