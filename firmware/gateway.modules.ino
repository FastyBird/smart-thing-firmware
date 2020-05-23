/*

GATEWAY MODULE - MODULES INTERFACES

Copyright (C) 2018 FastyBird Ltd. <info@fastybird.com>

*/

#if FB_GATEWAY_SUPPORT

bool _gateway_settings_save = false;
bool _gateway_web_config_success = false;

std::vector<uint8_t> * _gateway_web_config_buffer;

uint8_t _gateway_di_register_channel_property_index = INDEX_NONE;
uint8_t _gateway_do_register_channel_property_index = INDEX_NONE;
uint8_t _gateway_ai_register_channel_property_index = INDEX_NONE;
uint8_t _gateway_ao_register_channel_property_index = INDEX_NONE;
uint8_t _gateway_ev_register_channel_property_index = INDEX_NONE;

// -----------------------------------------------------------------------------
// MODULE: SUB-MODULE PRIVATE
// -----------------------------------------------------------------------------

#if FASTYBIRD_SUPPORT && FASTYBIRD_NODES_SUPPORT
    String _gatewayModulesCreateFastyBirdChannelName(
        String channelPrefix,
        uint8_t index
    ) {
        // Create channel name with replacable suffix: somechannel-%d
        char channel_with_suffix[channelPrefix.length() + 5];
        strcpy(channel_with_suffix, channelPrefix.c_str());
        strcat(channel_with_suffix, FASTYBIRD_CHANNEL_ARRAY_SUFFIX);

        char channel_name[strlen(channel_with_suffix) + 5];
        sprintf(channel_name, channel_with_suffix, index);

        return String(channel_name);
    }

// -----------------------------------------------------------------------------

    void _gatewayModulesFastyBirdReportRegisterValue(
        const uint8_t nodeIndex,
        const uint8_t dataRegister,
        const uint8_t address,
        String payload
    ) {
        gateway_node_t gateway_node = gatewayGetNode(nodeIndex);

        uint8_t fb_node_index = fastybirdNodesFindNodeIndex(String(gateway_node.serial_number));

        if (fb_node_index != INDEX_NONE) {
            String channel_name;

            switch (dataRegister)
            {
                case GATEWAY_REGISTER_DI:
                    channel_name = _gatewayModulesCreateFastyBirdChannelName(FASTYBIRD_CHANNEL_BINARY_SENSOR, (address + 1));
                    break;

                case GATEWAY_REGISTER_DO:
                    channel_name = _gatewayModulesCreateFastyBirdChannelName(FASTYBIRD_CHANNEL_BINARY_ACTOR, (address + 1));
                    break;

                case GATEWAY_REGISTER_AI:
                    channel_name = _gatewayModulesCreateFastyBirdChannelName(FASTYBIRD_CHANNEL_ANALOG_SENSOR, (address + 1));
                    break;

                case GATEWAY_REGISTER_AO:
                    channel_name = _gatewayModulesCreateFastyBirdChannelName(FASTYBIRD_CHANNEL_ANALOG_ACTOR, (address + 1));
                    break;

                case GATEWAY_REGISTER_EV:
                    channel_name = _gatewayModulesCreateFastyBirdChannelName(FASTYBIRD_CHANNEL_EVENT, (address + 1));
                    break;

                default:
                    return;
            }

            uint8_t fb_channel_index = fastybirdNodesFindChannelIndex(fb_node_index, channel_name);

            if (fb_channel_index != INDEX_NONE) {
                uint8_t property_index = fastybirdNodesFindChannelPropertyIndex(fb_channel_index, FASTYBIRD_PROPERTY_STATE);

                if (property_index != INDEX_NONE) {
                    DEBUG_MSG(PSTR("[INFO][GATEWAY][MODULES] Sending register value\n"));

                    fastybirdNodesReportChannelPropertyValue(
                        fb_node_index,
                        fb_channel_index,
                        property_index,
                        payload.c_str()
                    );

                } else {
                    fastybird_node_channel_t channel = fastybirdNodesGetChannel(fb_channel_index);

                    DEBUG_MSG(PSTR("[ERR][GATEWAY][MODULES] Channel property for channel: %s was not found\n"), channel.name.c_str());
                }

            } else {
                fastybird_node_t node = fastybirdNodesGetNode(fb_node_index);

                DEBUG_MSG(PSTR("[ERR][GATEWAY][MODULES] Channel for node: %s was not found\n"), node.id);
            }

        } else {
            DEBUG_MSG(PSTR("[ERR][GATEWAY][MODULES] Node was not found\n"));
        }
    }

// -----------------------------------------------------------------------------

    void _gatewayModulesFastyBirdDoRegisterChannelProperyPayload(
        const uint8_t fbNodeIndex,
        const uint8_t fbChannelIndex,
        const uint8_t fbPropertyIndex,
        const char * payload
    ) {
        // TODO: implement it
    }

// -----------------------------------------------------------------------------

    void _gatewayModulesFastyBirdAoRegisterChannelProperyPayload(
        const uint8_t fbNodeIndex,
        const uint8_t fbChannelIndex,
        const uint8_t fbPropertyIndex,
        const char * payload
    ) {
        // TODO: implement it
    }

// -----------------------------------------------------------------------------

    void _gatewayModulesFastyBirdDiRegisterChannelProperyQuery(
        const uint8_t fbNodeIndex,
        const uint8_t fbChannelIndex,
        const uint8_t fbPropertyIndex
    ) {
        fastybird_node_t fb_node = fastybirdNodesGetNode(fbNodeIndex);

        char stored_sn[15] = GATEWAY_DESCRIPTION_NOT_SET;

        for (uint8_t node_index = 0; node_index < FB_GATEWAY_MAX_NODES; node_index++) {
            gatewayGetSerialNumber(node_index, stored_sn);

            // At first, find gateway node by node serial number
            if (strcmp(stored_sn, fb_node.id) == 0) {
                for (uint8_t address = 0; address < fb_node.channels.size(); address++) {
                    // Transform channel index to gateway node register address
                    if (fb_node.channels[address] == fbChannelIndex) {
                        String value;

                        switch (_gatewayRegistersGetRegisterDataType(node_index, GATEWAY_REGISTER_DI, address))
                        {
                            case GATEWAY_DATA_TYPE_BOOL:
                                bool bool_stored_value;

                                gatewayRegistersReadValue(node_index, GATEWAY_REGISTER_DI, address, bool_stored_value);

                                value = bool_stored_value ? FASTYBIRD_SWITCH_PAYLOAD_ON : FASTYBIRD_SWITCH_PAYLOAD_OFF;
                                break;

                            default:
                                DEBUG_MSG(PSTR("[ERR][GATEWAY][MODULES] Provided unknown data type for reading from register\n"));

                                return;
                        }

                        _gatewayModulesFastyBirdReportRegisterValue(
                            node_index,
                            GATEWAY_REGISTER_DI,
                            address,
                            value
                        );
                    }
                }
            }

            strcpy(stored_sn, GATEWAY_DESCRIPTION_NOT_SET);
        }
    }

// -----------------------------------------------------------------------------

    void _gatewayModulesFastyBirdDoRegisterChannelProperyQuery(
        const uint8_t fbNodeIndex,
        const uint8_t fbChannelIndex,
        const uint8_t fbPropertyIndex
    ) {
        fastybird_node_t fb_node = fastybirdNodesGetNode(fbNodeIndex);

        char stored_sn[15] = GATEWAY_DESCRIPTION_NOT_SET;

        for (uint8_t node_index = 0; node_index < FB_GATEWAY_MAX_NODES; node_index++) {
            gatewayGetSerialNumber(node_index, stored_sn);

            // At first, find gateway node by node serial number
            if (strcmp(stored_sn, fb_node.id) == 0) {
                for (uint8_t address = 0; address < fb_node.channels.size(); address++) {
                    // Transform channel index to gateway node register address
                    if (fb_node.channels[address] == fbChannelIndex) {
                        String value;

                        switch (_gatewayRegistersGetRegisterDataType(node_index, GATEWAY_REGISTER_DO, address))
                        {
                            case GATEWAY_DATA_TYPE_BOOL:
                                bool bool_stored_value;

                                gatewayRegistersReadValue(node_index, GATEWAY_REGISTER_DO, address, bool_stored_value);

                                value = bool_stored_value ? FASTYBIRD_SWITCH_PAYLOAD_ON : FASTYBIRD_SWITCH_PAYLOAD_OFF;
                                break;

                            default:
                                DEBUG_MSG(PSTR("[ERR][GATEWAY][MODULES] Provided unknown data type for reading from register\n"));

                                return;
                        }

                        _gatewayModulesFastyBirdReportRegisterValue(
                            node_index,
                            GATEWAY_REGISTER_DO,
                            address,
                            value
                        );
                    }
                }
            }

            strcpy(stored_sn, GATEWAY_DESCRIPTION_NOT_SET);
        }
    }

// -----------------------------------------------------------------------------

    void _gatewayModulesFastyBirdAiRegisterChannelProperyQuery(
        const uint8_t fbNodeIndex,
        const uint8_t fbChannelIndex,
        const uint8_t fbPropertyIndex
    ) {
        fastybird_node_t fb_node = fastybirdNodesGetNode(fbNodeIndex);

        char stored_sn[15] = GATEWAY_DESCRIPTION_NOT_SET;

        for (uint8_t node_index = 0; node_index < FB_GATEWAY_MAX_NODES; node_index++) {
            gatewayGetSerialNumber(node_index, stored_sn);

            // At first, find gateway node by node serial number
            if (strcmp(stored_sn, fb_node.id) == 0) {
                for (uint8_t address = 0; address < fb_node.channels.size(); address++) {
                    // Transform channel index to gateway node register address
                    if (fb_node.channels[address] == fbChannelIndex) {
                        String value;

                        switch (_gatewayRegistersGetRegisterDataType(node_index, GATEWAY_REGISTER_AI, address))
                        {
                            case GATEWAY_DATA_TYPE_UINT8:
                                uint8_t uint8_stored_value;

                                gatewayRegistersReadValue(node_index, GATEWAY_REGISTER_AI, address, uint8_stored_value);

                                value = String(uint8_stored_value);
                                break;

                            case GATEWAY_DATA_TYPE_UINT16:
                                uint16_t uint16_stored_value;

                                gatewayRegistersReadValue(node_index, GATEWAY_REGISTER_AI, address, uint16_stored_value);

                                value = String(uint16_stored_value);
                                break;

                            case GATEWAY_DATA_TYPE_UINT32:
                                uint32_t uint32_stored_value;

                                gatewayRegistersReadValue(node_index, GATEWAY_REGISTER_AI, address, uint32_stored_value);

                                value = String(uint32_stored_value);
                                break;

                            case GATEWAY_DATA_TYPE_INT8:
                                int8_t int8_stored_value;

                                gatewayRegistersReadValue(node_index, GATEWAY_REGISTER_AI, address, int8_stored_value);

                                value = String(int8_stored_value);
                                break;

                            case GATEWAY_DATA_TYPE_INT16:
                                int16_t int16_stored_value;

                                gatewayRegistersReadValue(node_index, GATEWAY_REGISTER_AI, address, int16_stored_value);

                                value = String(int16_stored_value);
                                break;

                            case GATEWAY_DATA_TYPE_INT32:
                                int32_t int32_stored_value;

                                gatewayRegistersReadValue(node_index, GATEWAY_REGISTER_AI, address, int32_stored_value);

                                value = String(int32_stored_value);
                                break;

                            case GATEWAY_DATA_TYPE_FLOAT32:
                                float float_stored_value;

                                gatewayRegistersReadValue(node_index, GATEWAY_REGISTER_AI, address, float_stored_value);

                                value = String(float_stored_value);
                                break;

                            default:
                                DEBUG_MSG(PSTR("[ERR][GATEWAY][MODULES] Provided unknown data type for writing into register\n"));

                                return;
                        }
                        
                        _gatewayModulesFastyBirdReportRegisterValue(
                            node_index,
                            GATEWAY_REGISTER_AI,
                            address,
                            value
                        );
                    }
                }
            }

            strcpy(stored_sn, GATEWAY_DESCRIPTION_NOT_SET);
        }
    }

// -----------------------------------------------------------------------------

    void _gatewayModulesFastyBirdAoRegisterChannelProperyQuery(
        const uint8_t fbNodeIndex,
        const uint8_t fbChannelIndex,
        const uint8_t fbPropertyIndex
    ) {
        fastybird_node_t fb_node = fastybirdNodesGetNode(fbNodeIndex);

        char stored_sn[15] = GATEWAY_DESCRIPTION_NOT_SET;

        for (uint8_t node_index = 0; node_index < FB_GATEWAY_MAX_NODES; node_index++) {
            gatewayGetSerialNumber(node_index, stored_sn);

            // At first, find gateway node by node serial number
            if (strcmp(stored_sn, fb_node.id) == 0) {
                for (uint8_t address = 0; address < fb_node.channels.size(); address++) {
                    // Transform channel index to gateway node register address
                    if (fb_node.channels[address] == fbChannelIndex) {
                        String value;

                        switch (_gatewayRegistersGetRegisterDataType(node_index, GATEWAY_REGISTER_AO, address))
                        {
                            case GATEWAY_DATA_TYPE_UINT8:
                                uint8_t uint8_stored_value;

                                gatewayRegistersReadValue(node_index, GATEWAY_REGISTER_AO, address, uint8_stored_value);

                                value = String(uint8_stored_value);
                                break;

                            case GATEWAY_DATA_TYPE_UINT16:
                                uint16_t uint16_stored_value;

                                gatewayRegistersReadValue(node_index, GATEWAY_REGISTER_AO, address, uint16_stored_value);

                                value = String(uint16_stored_value);
                                break;

                            case GATEWAY_DATA_TYPE_UINT32:
                                uint32_t uint32_stored_value;

                                gatewayRegistersReadValue(node_index, GATEWAY_REGISTER_AO, address, uint32_stored_value);

                                value = String(uint32_stored_value);
                                break;

                            case GATEWAY_DATA_TYPE_INT8:
                                int8_t int8_stored_value;

                                gatewayRegistersReadValue(node_index, GATEWAY_REGISTER_AO, address, int8_stored_value);

                                value = String(int8_stored_value);
                                break;

                            case GATEWAY_DATA_TYPE_INT16:
                                int16_t int16_stored_value;

                                gatewayRegistersReadValue(node_index, GATEWAY_REGISTER_AO, address, int16_stored_value);

                                value = String(int16_stored_value);
                                break;

                            case GATEWAY_DATA_TYPE_INT32:
                                int32_t int32_stored_value;

                                gatewayRegistersReadValue(node_index, GATEWAY_REGISTER_AO, address, int32_stored_value);

                                value = String(int32_stored_value);
                                break;

                            case GATEWAY_DATA_TYPE_FLOAT32:
                                float float_stored_value;

                                gatewayRegistersReadValue(node_index, GATEWAY_REGISTER_AO, address, float_stored_value);

                                value = String(float_stored_value);
                                break;

                            default:
                                DEBUG_MSG(PSTR("[ERR][GATEWAY][MODULES] Provided unknown data type for writing into register\n"));

                                return;
                        }
                        
                        _gatewayModulesFastyBirdReportRegisterValue(
                            node_index,
                            GATEWAY_REGISTER_AO,
                            address,
                            value
                        );
                    }
                }
            }

            strcpy(stored_sn, GATEWAY_DESCRIPTION_NOT_SET);
        }
    }

// -----------------------------------------------------------------------------

    void _gatewayModulesFastyBirdEvRegisterChannelProperyQuery(
        const uint8_t fbNodeIndex,
        const uint8_t fbChannelIndex,
        const uint8_t fbPropertyIndex
    ) {
        fastybird_node_t fb_node = fastybirdNodesGetNode(fbNodeIndex);

        char stored_sn[15] = GATEWAY_DESCRIPTION_NOT_SET;

        for (uint8_t node_index = 0; node_index < FB_GATEWAY_MAX_NODES; node_index++) {
            gatewayGetSerialNumber(node_index, stored_sn);

            // At first, find gateway node by node serial number
            if (strcmp(stored_sn, fb_node.id) == 0) {
                for (uint8_t address = 0; address < fb_node.channels.size(); address++) {
                    // Transform channel index to gateway node register address
                    if (fb_node.channels[address] == fbChannelIndex) {
                        String value;

                        switch (_gatewayRegistersGetRegisterDataType(node_index, GATEWAY_REGISTER_EV, address))
                        {
                            case GATEWAY_DATA_TYPE_UINT8:
                                uint8_t uint8_stored_value;

                                gatewayRegistersReadValue(node_index, GATEWAY_REGISTER_EV, address, uint8_stored_value);

                                value = String(uint8_stored_value);
                                break;

                            default:
                                DEBUG_MSG(PSTR("[ERR][GATEWAY][MODULES] Provided unknown data type for writing into register\n"));

                                return;
                        }

                        _gatewayModulesFastyBirdReportRegisterValue(
                            node_index,
                            GATEWAY_REGISTER_EV,
                            address,
                            value
                        );
                    }
                }
            }

            strcpy(stored_sn, GATEWAY_DESCRIPTION_NOT_SET);
        }
    }

// -----------------------------------------------------------------------------

    void _gatewayModulesInitializeFastyBirdChannelProperties()
    {
        // DI REGISTER
        //////////////

        char di_format[30];

        strcpy(di_format, FASTYBIRD_SWITCH_PAYLOAD_ON);
        strcat(di_format, ",");
        strcat(di_format, FASTYBIRD_SWITCH_PAYLOAD_OFF);

        // Register channel property to collection
        _gateway_di_register_channel_property_index = fastybirdNodesRegisterNodeProperty(
            FASTYBIRD_PROPERTY_STATE,
            FASTYBIRD_PROPERTY_DATA_TYPE_ENUM,
            "",
            di_format,
            _gatewayModulesFastyBirdDiRegisterChannelProperyQuery
        );

        // DO REGISTER
        //////////////

        char do_format[30];

        strcpy(do_format, FASTYBIRD_SWITCH_PAYLOAD_ON);
        strcat(do_format, ",");
        strcat(do_format, FASTYBIRD_SWITCH_PAYLOAD_OFF);
        strcat(do_format, ",");
        strcat(do_format, FASTYBIRD_SWITCH_PAYLOAD_TOGGLE);

        // Register channel property to collection
        _gateway_do_register_channel_property_index = fastybirdNodesRegisterNodeProperty(
            FASTYBIRD_PROPERTY_STATE,
            FASTYBIRD_PROPERTY_DATA_TYPE_ENUM,
            "",
            di_format,
            _gatewayModulesFastyBirdDoRegisterChannelProperyPayload,
            _gatewayModulesFastyBirdDoRegisterChannelProperyQuery
        );

        // AI REGISTER
        //////////////
        
        // Register channel property to collection
        _gateway_ai_register_channel_property_index = fastybirdNodesRegisterNodeProperty(
            FASTYBIRD_PROPERTY_STATE,
            FASTYBIRD_PROPERTY_DATA_TYPE_FLOAT,
            "",
            "",
            _gatewayModulesFastyBirdAiRegisterChannelProperyQuery
        );

        // AO REGISTER
        //////////////

        // Register channel property to collection
        _gateway_ao_register_channel_property_index = fastybirdNodesRegisterNodeProperty(
            FASTYBIRD_PROPERTY_STATE,
            FASTYBIRD_PROPERTY_DATA_TYPE_FLOAT,
            "",
            "",
            _gatewayModulesFastyBirdAoRegisterChannelProperyPayload,
            _gatewayModulesFastyBirdAoRegisterChannelProperyQuery
        );

        // EV REGISTER
        //////////////

        // Register channel property to collection
        _gateway_ev_register_channel_property_index = fastybirdNodesRegisterNodeProperty(
            FASTYBIRD_PROPERTY_STATE,
            FASTYBIRD_PROPERTY_DATA_TYPE_INTEGER,
            "",
            "",
            _gatewayModulesFastyBirdEvRegisterChannelProperyQuery
        );
    }

// -----------------------------------------------------------------------------

    void _gatewayModulesInitializeFastyBirdNode(
        const uint8_t nodeIndex
    ) {
        gateway_node_t gatewayNode = gatewayGetNode(nodeIndex);

        char stored_sn[15] = GATEWAY_DESCRIPTION_NOT_SET;

        char hw_manufacturer[20] = GATEWAY_DESCRIPTION_NOT_SET;
        char hw_model[20] = GATEWAY_DESCRIPTION_NOT_SET;
        char hw_version[20] = GATEWAY_DESCRIPTION_NOT_SET;

        char fw_manufacturer[20] = GATEWAY_DESCRIPTION_NOT_SET;
        char fw_model[20] = GATEWAY_DESCRIPTION_NOT_SET;
        char fw_version[20] = GATEWAY_DESCRIPTION_NOT_SET;

        gatewayGetSerialNumber(nodeIndex, stored_sn);

        gatewayGetHardwareManufacturer(nodeIndex, hw_manufacturer);
        gatewayGetHardwareModel(nodeIndex, hw_model);
        gatewayGetHardwareVersion(nodeIndex, hw_version);

        gatewayGetFirmwareManufacturer(nodeIndex, fw_manufacturer);
        gatewayGetFirmwareModel(nodeIndex, fw_model);
        gatewayGetFirmwareVersion(nodeIndex, fw_version);

        const uint8_t fb_node_index = fastybirdNodesRegisterNode(
            stored_sn,
            hw_model,
            hw_version,
            hw_manufacturer,
            fw_model,
            fw_version,
            fw_manufacturer,
            gatewayNode.ready
        );

        uint8_t fb_channel_index = INDEX_NONE;

        // DI registers channel
        if (gatewayRegistersDigitalInputsSize(nodeIndex) > 0) {
            for (uint8_t i = 0; i < gatewayRegistersDigitalInputsSize(nodeIndex); i++) {
                // Register channel to collection
                fb_channel_index = fastybirdNodesRegisterNodeChannel(
                    _gatewayModulesCreateFastyBirdChannelName(FASTYBIRD_CHANNEL_BINARY_SENSOR, (i + 1)).c_str()
                );

                // Mapp all together
                fastybirdNodesMapChannelToNode(fb_node_index, fb_channel_index);
                fastybirdNodesMapPropertyToChannel(fb_channel_index, _gateway_di_register_channel_property_index);
            }
        }

        // DO registers channel
        if (gatewayRegistersDigitalOutputsSize(nodeIndex) > 0) {
            for (uint8_t i = 0; i < gatewayRegistersDigitalOutputsSize(nodeIndex); i++) {
                // Register channel to collection
                fb_channel_index = fastybirdNodesRegisterNodeChannel(
                    _gatewayModulesCreateFastyBirdChannelName(FASTYBIRD_CHANNEL_BINARY_ACTOR, (i + 1)).c_str()
                );

                // Mapp all together
                fastybirdNodesMapChannelToNode(fb_node_index, fb_channel_index);
                fastybirdNodesMapPropertyToChannel(fb_channel_index, _gateway_do_register_channel_property_index);
            }
        }

        // AI registers channel
        if (gatewayRegistersAnalogInputsSize(nodeIndex) > 0) {
            for (uint8_t i = 0; i < gatewayRegistersAnalogInputsSize(nodeIndex); i++) {
                // Register channel to collection
                fb_channel_index = fastybirdNodesRegisterNodeChannel(
                    _gatewayModulesCreateFastyBirdChannelName(FASTYBIRD_CHANNEL_ANALOG_SENSOR, (i + 1)).c_str()
                );

                // Mapp all together
                fastybirdNodesMapChannelToNode(fb_node_index, fb_channel_index);
                fastybirdNodesMapPropertyToChannel(fb_channel_index, _gateway_ai_register_channel_property_index);
            }
        }

        // AO registers channel
        if (gatewayRegistersAnalogOutputsSize(nodeIndex) > 0) {
            for (uint8_t i = 0; i < gatewayRegistersAnalogOutputsSize(nodeIndex); i++) {
                // Register channel to collection
                fb_channel_index = fastybirdNodesRegisterNodeChannel(
                    _gatewayModulesCreateFastyBirdChannelName(FASTYBIRD_CHANNEL_ANALOG_ACTOR, (i + 1)).c_str()
                );

                // Mapp all together
                fastybirdNodesMapChannelToNode(fb_node_index, fb_channel_index);
                fastybirdNodesMapPropertyToChannel(fb_channel_index, _gateway_ao_register_channel_property_index);
            }
        }

        // EV register channel
        if (gatewayRegistersEventInputsSize(nodeIndex) > 0) {
            for (uint8_t i = 0; i < gatewayRegistersEventInputsSize(nodeIndex); i++) {
                // Register channel to collection
                fb_channel_index = fastybirdNodesRegisterNodeChannel(
                    _gatewayModulesCreateFastyBirdChannelName(FASTYBIRD_CHANNEL_EVENT, (i + 1)).c_str()
                );

                // Mapp all together
                fastybirdNodesMapChannelToNode(fb_node_index, fb_channel_index);
                fastybirdNodesMapPropertyToChannel(fb_channel_index, _gateway_ev_register_channel_property_index);
            }
        }
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

        response->printf("{\n\"device\": \"%s\"", DEVICE);
        response->printf(",\n\"manufacturer\": \"%s\"", FIRMWARE_MANUFACTURER);
        response->printf(",\n\"version\": \"%s\"", FIRMWARE_VERSION);

        DynamicJsonBuffer jsonBuffer;

        JsonArray& registered_nodes = jsonBuffer.parseArray(gatewayStorageReadConfiguration().c_str());

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
                _gateway_web_config_success = gatewayStorageRestoreFromJson(root);
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
                _gateway_web_config_success = gatewayStorageRestoreFromJson(root);
            }

            delete _gateway_web_config_buffer;
        }
    }
#endif

// -----------------------------------------------------------------------------
// MODULE: SUB-MODULE API
// -----------------------------------------------------------------------------

void gatewayModulesNodeInitialized(
    const uint8_t nodeIndex
) {
    #if FASTYBIRD_SUPPORT && FASTYBIRD_NODES_SUPPORT
        _gatewayModulesInitializeFastyBirdNode(nodeIndex);
    #endif
}

// -----------------------------------------------------------------------------

void gatewayModulesNodeIsLost(
    const uint8_t nodeIndex
) {
    #if FASTYBIRD_SUPPORT && FASTYBIRD_NODES_SUPPORT
        gateway_node_t gateway_node = gatewayGetNode(nodeIndex);

        uint8_t fb_node_index = fastybirdNodesFindNodeIndex(String(gateway_node.serial_number));

        if (fb_node_index != INDEX_NONE) {
            fastybirdNodesNodeIsLost(fb_node_index);
        }
    #endif
}

// -----------------------------------------------------------------------------

void gatewayModulesNodeIsAlive(
    const uint8_t nodeIndex
) {
    // Nothing to do here for now
}

// -----------------------------------------------------------------------------

void gatewayModulesNodeIsReady(
    const uint8_t nodeIndex
) {
    #if FASTYBIRD_SUPPORT && FASTYBIRD_NODES_SUPPORT
        gateway_node_t gateway_node = gatewayGetNode(nodeIndex);

        uint8_t fb_node_index = fastybirdNodesFindNodeIndex(String(gateway_node.serial_number));

        if (fb_node_index != INDEX_NONE) {
            fastybirdNodesNodeIsReady(fb_node_index);
        }
    #endif
}

// -----------------------------------------------------------------------------

/**
 * Notify other modules that register value was changed
 */
void gatewayModulesRegisterValueUpdated(
    const uint8_t nodeIndex,
    const uint8_t dataRegister,
    const uint8_t address,
    const bool payload
) {
    #if FASTYBIRD_SUPPORT && FASTYBIRD_NODES_SUPPORT
        _gatewayModulesFastyBirdReportRegisterValue(
            nodeIndex,
            dataRegister,
            address,
            payload ? FASTYBIRD_SWITCH_PAYLOAD_ON : FASTYBIRD_SWITCH_PAYLOAD_OFF
        );
    #endif
}

// -----------------------------------------------------------------------------

void gatewayModulesRegisterValueUpdated(
    const uint8_t nodeIndex,
    const uint8_t dataRegister,
    const uint8_t address,
    const uint8_t payload
) {
    #if FASTYBIRD_SUPPORT && FASTYBIRD_NODES_SUPPORT
        _gatewayModulesFastyBirdReportRegisterValue(
            nodeIndex,
            dataRegister,
            address,
            String(payload)
        );
    #endif
}

// -----------------------------------------------------------------------------

void gatewayModulesRegisterValueUpdated(
    const uint8_t nodeIndex,
    const uint8_t dataRegister,
    const uint8_t address,
    const uint16_t payload
) {
    #if FASTYBIRD_SUPPORT && FASTYBIRD_NODES_SUPPORT
        _gatewayModulesFastyBirdReportRegisterValue(
            nodeIndex,
            dataRegister,
            address,
            String(payload)
        );
    #endif
}

// -----------------------------------------------------------------------------

void gatewayModulesRegisterValueUpdated(
    const uint8_t nodeIndex,
    const uint8_t dataRegister,
    const uint8_t address,
    const uint32_t payload
) {
    #if FASTYBIRD_SUPPORT && FASTYBIRD_NODES_SUPPORT
        _gatewayModulesFastyBirdReportRegisterValue(
            nodeIndex,
            dataRegister,
            address,
            String(payload)
        );
    #endif
}

// -----------------------------------------------------------------------------

void gatewayModulesRegisterValueUpdated(
    const uint8_t nodeIndex,
    const uint8_t dataRegister,
    const uint8_t address,
    const int8_t payload
) {
    #if FASTYBIRD_SUPPORT && FASTYBIRD_NODES_SUPPORT
        _gatewayModulesFastyBirdReportRegisterValue(
            nodeIndex,
            dataRegister,
            address,
            String(payload)
        );
    #endif
}

// -----------------------------------------------------------------------------

void gatewayModulesRegisterValueUpdated(
    const uint8_t nodeIndex,
    const uint8_t dataRegister,
    const uint8_t address,
    const int16_t payload
) {
    #if FASTYBIRD_SUPPORT && FASTYBIRD_NODES_SUPPORT
        _gatewayModulesFastyBirdReportRegisterValue(
            nodeIndex,
            dataRegister,
            address,
            String(payload)
        );
    #endif
}

// -----------------------------------------------------------------------------

void gatewayModulesRegisterValueUpdated(
    const uint8_t nodeIndex,
    const uint8_t dataRegister,
    const uint8_t address,
    const int32_t payload
) {
    #if FASTYBIRD_SUPPORT && FASTYBIRD_NODES_SUPPORT
        _gatewayModulesFastyBirdReportRegisterValue(
            nodeIndex,
            dataRegister,
            address,
            String(payload)
        );
    #endif
}

// -----------------------------------------------------------------------------

void gatewayModulesRegisterValueUpdated(
    const uint8_t nodeIndex,
    const uint8_t dataRegister,
    const uint8_t address,
    const float payload
) {
    #if FASTYBIRD_SUPPORT && FASTYBIRD_NODES_SUPPORT
        _gatewayModulesFastyBirdReportRegisterValue(
            nodeIndex,
            dataRegister,
            address,
            String(payload)
        );
    #endif
}

// -----------------------------------------------------------------------------
// MODULE: SUB-MODULE CORE
// -----------------------------------------------------------------------------

void gatewayModulesSetup()
{
    #if WEqwB_SUPPORT
        webServer()->on(FB_GATEWAY_WEB_API_CONFIGURATION, HTTP_GET, _gatewayOnGetConfig);
        webServer()->on(FB_GATEWAY_WEB_API_CONFIGURATION, HTTP_POST, _gatewayOnPostConfig, _gatewayOnPostConfigData);
    #endif

    #if FASTYBIRD_SUPPORT
        #if FASTYBIRD_NODES_SUPPORT
        _gatewayModulesInitializeFastyBirdChannelProperties();
        #endif

        fastybirdOnControlRegister(
            [](const char * payload) {
                DEBUG_MSG(PSTR("[INFO][GATEWAY][MODULES] Searching for new nodes\n"));

                gatewayAddressingStartSearching();
            },
            FASTYBIRD_DEVICE_CONTROL_SEARCH_FOR_NODES
        );

        fastybirdOnControlRegister(
            [](const char * payload) {
                DEBUG_MSG(PSTR("[INFO][GATEWAY][MODULES] Disconnection selected node\n"));

                // TODO: implement
            },
            FASTYBIRD_DEVICE_CONTROL_DISCONNECT_NODE
        );
    #endif
}

#endif // FB_GATEWAY_SUPPORT
