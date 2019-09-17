/*

NODES MODULE - MODULES INTERFACES

Copyright (C) 2018 FastyBird s.r.o. <info@fastybird.com>

*/

#if NODES_GATEWAY_SUPPORT

bool _gateway_settings_save = false;
bool _gateway_web_config_success = false;
std::vector<uint8_t> * _gateway_web_config_buffer;

#if FASTYBIRD_SUPPORT
    /**
     * Provide module configuration schema
     */
    void _gatewayReportConfigurationSchema(
        JsonArray& configuration
    ) {
        // TODO: Add module config
    }

// -----------------------------------------------------------------------------

    /**
     * Report module configuration
     */
    void _gatewayReportConfiguration(
        JsonObject& configuration
    ) {
        // TODO: Add module config
    }

// -----------------------------------------------------------------------------

    /**
     * Update module configuration via WS or MQTT etc.
     */
    bool _gatewayUpdateConfiguration(
        JsonObject& configuration
    ) {
        DEBUG_MSG(PSTR("[GATEWAY] Updating module\n"));

        return false;
    }

// -----------------------------------------------------------------------------

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

                // Add register payload callback
                register_property.payloadCallback = ([nodeId](uint8_t id, const char * payload) {
                    // Action to perform
                    if (strcmp(payload, FASTYBIRD_SWITCH_PAYLOAD_TOGGLE) == 0) {
                        gatewayDigitalRegisterStatus(nodeId, id, _gatewayRegistersReadDigitalValue(nodeId, GATEWAY_REGISTER_DO, id) ? false : true);

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

    void _gatewayFastybirdReportDigitalRegisterValue(
        const uint8_t id,
        const uint8_t dataRegister,
        const uint8_t address,
        const uint8_t channel
    ) {
        _fastybirdReportNodeChannelValue(
            _gateway_nodes[id].serial_number,
            channel,
            address,
            _gatewayRegistersReadDigitalValue(id, dataRegister, address) ? FASTYBIRD_SWITCH_PAYLOAD_ON : FASTYBIRD_SWITCH_PAYLOAD_OFF
        );
    }

// -----------------------------------------------------------------------------

    void _gatewayFastybirdReportAnalogRegisterValue(
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

                _gatewayRegistersReadAnalogValue(id, dataRegister, address, uint8_value);

                payload = String(uint8_value);
                break;

            case GATEWAY_DATA_TYPE_UINT16:
                uint16_t uint16_value;

                _gatewayRegistersReadAnalogValue(id, dataRegister, address, uint16_value);

                payload = String(uint16_value);
                break;

            case GATEWAY_DATA_TYPE_UINT32:
                uint32_t uint32_value;

                _gatewayRegistersReadAnalogValue(id, dataRegister, address, uint32_value);

                payload = String(uint32_value);
                break;

            case GATEWAY_DATA_TYPE_INT8:
                int8_t int8_value;

                _gatewayRegistersReadAnalogValue(id, dataRegister, address, int8_value);

                payload = String(int8_value);
                break;

            case GATEWAY_DATA_TYPE_INT16:
                int16_t int16_value;

                _gatewayRegistersReadAnalogValue(id, dataRegister, address, int16_value);

                payload = String(int16_value);
                break;

            case GATEWAY_DATA_TYPE_INT32:
                int32_t int32_value;

                _gatewayRegistersReadAnalogValue(id, dataRegister, address, int32_value);

                payload = String(int32_value);
                break;

            case GATEWAY_DATA_TYPE_FLOAT32:
                float float_value;

                _gatewayRegistersReadAnalogValue(id, dataRegister, address, float_value);

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

    void _gatewayFastybirdReportEventRegisterValue(
        const uint8_t id,
        const uint8_t dataRegister,
        const uint8_t address,
        const uint8_t channel
    ) {
        String payload;

        payload = String(_gatewayRegistersReadEventValue(id, dataRegister, address));

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

        JsonArray& registered_nodes = jsonBuffer.parseArray(_gatewayStorageReadConfiguration().c_str());

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

        request->send(_gateway_web_config_success ? 200 : 400);

        if (_gateway_web_config_success) {
            deferredReset(250, CUSTOM_RESTORE_WEB);
        }
    }

// -----------------------------------------------------------------------------

    void _gatewayOnPostConfigData(
        AsyncWebServerRequest *request,
        String filename,
        size_t index,
        uint8_t * data,
        size_t len,
        bool final
    ) {
        if (!webAuthenticate(request)) {
            return request->requestAuthentication(getIdentifier().c_str());
        }

        // No buffer
        if (final && (index == 0)) {
            DynamicJsonBuffer jsonBuffer;
            JsonObject& root = jsonBuffer.parseObject((char *) data);

            if (root.success()) {
                _gateway_web_config_success = _gatewayStorageRestoreFromJson(root);
            }

            return;
        }

        // Buffer start => reset
        if (index == 0) {
            if (_gateway_web_config_buffer) {
                delete _gateway_web_config_buffer;
            }
        }

        // init buffer if it doesn't exist
        if (!_gateway_web_config_buffer) {
            _gateway_web_config_buffer = new std::vector<uint8_t>();
            _gateway_web_config_success = false;
        }

        // Copy
        if (len > 0) {
            _gateway_web_config_buffer->reserve(_gateway_web_config_buffer->size() + len);
            _gateway_web_config_buffer->insert(_gateway_web_config_buffer->end(), data, data + len);
        }

        // Ending
        if (final) {
            _gateway_web_config_buffer->push_back(0);

            // Parse JSON
            DynamicJsonBuffer jsonBuffer;
            JsonObject& root = jsonBuffer.parseObject((char *) _gateway_web_config_buffer->data());

            if (root.success()) {
                _gateway_web_config_success = _gatewayStorageRestoreFromJson(root);
            }

            delete _gateway_web_config_buffer;
        }
    }
#endif

#endif