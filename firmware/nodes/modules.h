/*

NODES MODULE - MODULES INTERFACES

Copyright (C) 2018 FastyBird s.r.o. <info@fastybird.com>

*/

#if NODES_GATEWAY_SUPPORT

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

        JsonObject& addressing = node.createNestedObject("addressing");

        addressing["address"] = id + 1;
        addressing["addressed"] = _gateway_nodes[id].addressing.state;
        addressing["lost"] = _gateway_nodes[id].addressing.lost;

        JsonObject& initiliazation = node.createNestedObject("initiliazation");

        initiliazation["initialized"] = _gateway_nodes[id].initiliazation.state;

        JsonObject& searching = node.createNestedObject("searching");

        searching["state"] = _gateway_nodes[id].searching.state;

        // Node basic info
        node["node"] = String(_gateway_nodes[id].serial_number);

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

        JsonArray& event_inputs_register = registers.createNestedArray("event_inputs");

        for (uint8_t j = 0; j < _gateway_nodes[id].registers_size[GATEWAY_REGISTER_EV]; j++) {
            JsonObject& ev_register = event_inputs_register.createNestedObject();

            ev_register["data_type"] = "u1";
            ev_register["value"] = _gatewayReadEventRegisterValue(id, GATEWAY_REGISTER_EV, j);
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
        
        // Data container
        JsonObject& data = module.createNestedObject("data");

        // Nodes container
        JsonArray& nodes = data.createNestedArray("nodes");

        for (uint8_t i = 0; i < NODES_GATEWAY_MAX_NODES; i++) {
            JsonObject& node = nodes.createNestedObject();

            _gatewayCollectNode(node, i);
        }

        // Configuration container
        JsonObject& configuration = module.createNestedObject("config");

        // Configuration values container
        JsonObject& configuration_values = configuration.createNestedObject("values");

        // Configuration schema container
        JsonArray& configuration_schema = configuration.createNestedArray("schema");
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
        // Write to node register
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
                            gatewayDigitalRegisterStatus(i, data["address"].as<uint8_t>(), data["value"].as<bool>());

                        } else {
                            DEBUG_MSG(PSTR("[GATEWAY][ERR] Register address: %d is out of range: 0 - %d\n"), data["address"].as<uint8_t>(), _gateway_nodes[i].registers_size[GATEWAY_REGISTER_DO]);
                        }

                    } else if (strcmp("analog_output", data["register"].as<const char *>()) == 0) {
                        if (data["address"].as<uint8_t>() < _gateway_nodes[i].registers_size[GATEWAY_REGISTER_AO]) {
                            // TODO: implement it
                            // TODO: _gatewayRequestWritingSingleAnalogRegister(i, data["address"].as<uint8_t>(), data["value"].as<float>());

                        } else {
                            DEBUG_MSG(PSTR("[GATEWAY][ERR] Register address: %d is out of range: 0 - %d\n"), data["address"].as<uint8_t>(), _gateway_nodes[i].registers_size[GATEWAY_REGISTER_AO]);
                        }
                    }
                }
            }

        // Scan for new unaddressed nodes
        } else if (strcmp(action, "scan") == 0) {
            _gateway_nodes_scan_client_id = clientId;

            _gatewaySearchNewNodesStart();

        // Remove node
        } else if (
            strcmp(action, "remove") == 0
            && data.containsKey("node")
        ) {
            for (uint8_t i = 0; i < NODES_GATEWAY_MAX_NODES; i++) {
                // Search for node
                if (strcmp(_gateway_nodes[i].serial_number, data["node"].as<const char *>()) == 0) {
                    DEBUG_MSG(PSTR("[GATEWAY] Found node to remove\n"));

                    _gatewayRemoveNodeFromStorage(i);

                    deferredReset(250, CUSTOM_RESET_WEB);
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
            FASTYBIRD_PROPERTY_STATE
        };

        uint8_t nodeId = id;

        switch (dataRegister)
        {
            case GATEWAY_REGISTER_DI:
                register_property.settable = false;
                register_property.queryable = false;
                register_property.dataType = FASTYBIRD_PROPERTY_DATA_TYPE_ENUM;

                char di_format[30];

                strcpy(di_format, String(FASTYBIRD_SWITCH_PAYLOAD_ON).c_str());
                strcat(di_format, ",");
                strcat(di_format, String(FASTYBIRD_SWITCH_PAYLOAD_OFF).c_str());

                register_property.format = String(di_format);

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
                register_property.queryable = false;
                register_property.dataType = FASTYBIRD_PROPERTY_DATA_TYPE_ENUM;

                char do_format[30];

                strcpy(do_format, String(FASTYBIRD_SWITCH_PAYLOAD_ON).c_str());
                strcat(do_format, ",");
                strcat(do_format, String(FASTYBIRD_SWITCH_PAYLOAD_OFF).c_str());
                strcat(do_format, ",");
                strcat(do_format, String(FASTYBIRD_SWITCH_PAYLOAD_TOGGLE).c_str());

                register_property.format = String(do_format);

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

                // Add register payload callback
                register_property.payloadCallback = ([nodeId](uint8_t id, const char * payload) {
                    // Action to perform
                    if (strcmp(payload, FASTYBIRD_SWITCH_PAYLOAD_TOGGLE) == 0) {
                        gatewayDigitalRegisterStatus(nodeId, id, _gatewayReadDigitalRegisterValue(nodeId, GATEWAY_REGISTER_DO, id) ? false : true);

                    } else {
                        gatewayDigitalRegisterStatus(nodeId, id, strcmp(payload, FASTYBIRD_SWITCH_PAYLOAD_ON) == 0);
                    }
                });
                break;

            case GATEWAY_REGISTER_AI:
                register_property.settable = false;
                register_property.queryable = false;
                register_property.dataType = FASTYBIRD_PROPERTY_DATA_TYPE_FLOAT;
                break;

            case GATEWAY_REGISTER_AO:
                register_property.settable = true;
                register_property.queryable = false;
                register_property.dataType = FASTYBIRD_PROPERTY_DATA_TYPE_FLOAT;

                // Add register payload callback
                register_property.payloadCallback = ([nodeId](uint8_t id, const char * payload) {
                    // TODO: implement it
                    // TODO: _gatewayRequestWritingSingleAnalogRegister(nodeId, id, payload);
                });
                break;

            case GATEWAY_REGISTER_EV:
                register_property.settable = false;
                register_property.queryable = false;
                register_property.dataType = FASTYBIRD_PROPERTY_DATA_TYPE_INTEGER;
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
                FASTYBIRD_CHANNEL_BINARY_SENSOR,
                FASTYBIRD_CHANNEL_BINARY_SENSOR,
                _gateway_nodes[id].registers_size[dataRegister],
                false,
                false,
                false
            };

        } else if (dataRegister == GATEWAY_REGISTER_DO) {
            register_channel = {
                FASTYBIRD_CHANNEL_BINARY_ACTOR,
                FASTYBIRD_CHANNEL_BINARY_ACTOR,
                _gateway_nodes[id].registers_size[dataRegister],
                false,
                false,
                false
            };

        } else if (dataRegister == GATEWAY_REGISTER_AI) {
            register_channel = {
                FASTYBIRD_CHANNEL_ANALOG_SENSOR,
                FASTYBIRD_CHANNEL_ANALOG_SENSOR,
                _gateway_nodes[id].registers_size[dataRegister],
                false,
                false,
                false
            };

        } else if (dataRegister == GATEWAY_REGISTER_AO) {
            register_channel = {
                FASTYBIRD_CHANNEL_ANALOG_ACTOR,
                FASTYBIRD_CHANNEL_ANALOG_ACTOR,
                _gateway_nodes[id].registers_size[dataRegister],
                false,
                false,
                false
            };

        } else if (dataRegister == GATEWAY_REGISTER_EV) {
            register_channel = {
                FASTYBIRD_CHANNEL_EVENT,
                FASTYBIRD_CHANNEL_EVENT,
                _gateway_nodes[id].registers_size[dataRegister],
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
        register_node.channels.push_back(_gatewayFastybirdGetChannelStructure(id, GATEWAY_REGISTER_EV));

        // TODO: implement it
        // TODO: Node configure support
        //for (uint8_t i = 0; i < _gateway_nodes[nodeIndex].settings.size(); i++) {
        //    register_node.settings.push_back({
        //        _gateway_nodes[nodeIndex].settings[i].key.c_str(),
        //        _gateway_nodes[nodeIndex].settings[i].value.c_str()
        //    });
        //}

        fastybirdRegisterNode(register_node);
    }

// -----------------------------------------------------------------------------

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

// -----------------------------------------------------------------------------

    void _gatewayReportEventRegisterValue(
        const uint8_t id,
        const uint8_t dataRegister,
        const uint8_t address,
        const uint8_t channel
    ) {
        String payload;

        payload = String(_gatewayReadEventRegisterValue(id, dataRegister, address));

        _fastybirdReportNodeChannelValue(
            _gateway_nodes[id].serial_number,
            channel,
            address,
            payload.c_str()
        );
    }
#endif

// -----------------------------------------------------------------------------

#if WEB_SUPPORT
    void _gatewayOnGetConfig(
        AsyncWebServerRequest * request
    ) {
        webLog(request);

        if (!webAuthenticate(request)) {
            return request->requestAuthentication(getIdentifier().c_str());
        }

        AsyncResponseStream * response = request->beginResponseStream("text/json");

        char buffer[100];

        snprintf_P(buffer, sizeof(buffer), PSTR("attachment; filename=\"%s-gateway-backup.json\""), (char *) getIdentifier().c_str());

        response->addHeader("Content-Disposition", buffer);
        response->addHeader("X-XSS-Protection", "1; mode=block");
        response->addHeader("X-Content-Type-Options", "nosniff");
        response->addHeader("X-Frame-Options", "deny");

        response->printf("{\n\"thing\": \"%s\"", THING);
        response->printf(",\n\"manufacturer\": \"%s\"", FIRMWARE_MANUFACTURER);
        response->printf(",\n\"version\": \"%s\"", FIRMWARE_VERSION);

        #if NTP_SUPPORT
            response->printf(",\n\"timestamp\": \"%s\"", ntpDateTime().c_str());
        #endif

        DynamicJsonBuffer jsonBuffer;

        JsonArray& registered_nodes = jsonBuffer.parseArray(_gatewayReadStoredConfiguration().c_str());

        String output;

        registered_nodes.printTo(output);

        response->printf(",\n\"gateway\": %s", output.c_str());

        response->printf("\n}");

        request->send(response);
    }

// -----------------------------------------------------------------------------

    void _gatewayOnPostConfig(
        AsyncWebServerRequest * request
    ) {
        webLog(request);

        if (!webAuthenticate(request)) {
            return request->requestAuthentication(getIdentifier().c_str());
        }

        bool gateway_web_config_success = false;

        int params = request->params();

        for (uint8_t i = 0; i < params; i++) {
            AsyncWebParameter* p = request->getParam(i);
            
            if (p->isFile()) {
                DynamicJsonBuffer jsonBuffer;

                JsonObject& root = jsonBuffer.parseObject(p->value().c_str());

                if (root.success()) {
                    gateway_web_config_success = _gatewayRestoreStorageFromJson(root);
                }
            }
        }

        request->send(gateway_web_config_success ? 200 : 400);
    }
#endif

#endif