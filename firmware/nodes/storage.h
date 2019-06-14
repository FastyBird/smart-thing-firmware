/*

NODES MODULE - MODULE STORAGE

Copyright (C) 2018 FastyBird Ltd. <info@fastybird.com>

*/

#if NODES_GATEWAY_SUPPORT

// -----------------------------------------------------------------------------
// MODULE PRIVATE
// -----------------------------------------------------------------------------

String _gatewayReadStoredConfiguration() {
    String stored_content = storageReadConfiguration(_gateway_config_filename);

    if (strcmp(stored_content.c_str(), "") == 0) {
        stored_content = String("[]");
    }

    DynamicJsonBuffer jsonBuffer;

    JsonArray& registered_nodes = jsonBuffer.parseArray(stored_content.c_str());

    if (!registered_nodes.success()) {
        return String("[]");
    }

    return stored_content;
}

// -----------------------------------------------------------------------------

void _gatewayAddNodeToStorage(
    const uint8_t id
) {
    DynamicJsonBuffer jsonBuffer;

    JsonArray& registered_nodes = jsonBuffer.parseArray(_gatewayReadStoredConfiguration().c_str());

    uint8_t index = 0;

    for (JsonObject& stored_node : registered_nodes) {
        if (stored_node["address"] == (id + 1)) {
            registered_nodes.remove(index);
        }

        index++;
    }

    JsonObject& node = registered_nodes.createNestedObject();

    if (!node.success()) {
        DEBUG_MSG(PSTR("[GATEWAY][ERR] Could not create configuration schema for storing.\n"));

        return;
    }

    node["address"] = id + 1;
    node["serial_number"] = _gateway_nodes[id].serial_number;

    JsonObject& hardware_info = node.createNestedObject("hardware");

    hardware_info["manufacturer"] = _gateway_nodes[id].hardware.manufacturer;
    hardware_info["model"] = _gateway_nodes[id].hardware.model;
    hardware_info["version"] = _gateway_nodes[id].hardware.version;

    JsonObject& firmware_info = node.createNestedObject("firmware");

    firmware_info["manufacturer"] = _gateway_nodes[id].firmware.manufacturer;
    firmware_info["model"] = _gateway_nodes[id].firmware.model;
    firmware_info["version"] = _gateway_nodes[id].firmware.version;

    node["digital_inputs"] = _gateway_nodes[id].registers_size[GATEWAY_REGISTER_DI];
    node["digital_outputs"] = _gateway_nodes[id].registers_size[GATEWAY_REGISTER_DO];
    node["analog_inputs"] = _gateway_nodes[id].registers_size[GATEWAY_REGISTER_AI];
    node["analog_outputs"] = _gateway_nodes[id].registers_size[GATEWAY_REGISTER_AO];
    node["event_inputs"] = _gateway_nodes[id].registers_size[GATEWAY_REGISTER_EV];

    String output;

    registered_nodes.printTo(output);

    storageWriteConfiguration(_gateway_config_filename, output);
}

// -----------------------------------------------------------------------------

void _gatewayRemoveNodeFromStorage(
    const uint8_t id
) {
    DynamicJsonBuffer jsonBuffer;

    JsonArray& registered_nodes = jsonBuffer.parseArray(_gatewayReadStoredConfiguration().c_str());

    bool removed = false;
    uint8_t index = 0;

    for (JsonObject& stored_node : registered_nodes) {
        if (stored_node["address"] == (id + 1)) {
            registered_nodes.remove(index);

            removed = true;
        }

        index++;
    }

    if (removed) {
        String output;

        registered_nodes.printTo(output);

        storageWriteConfiguration(_gateway_config_filename, output);
    }
}

// -----------------------------------------------------------------------------

bool _gatewayRestoreStorageFromJson(
    JsonObject& data
) {
    const char * _thing = data["thing"];

    if (strcmp(_thing, THING) != 0) {
        return false;
    }

    const char * _version = data["version"];

    if (strcmp(_version, FIRMWARE_VERSION) != 0) {
        return false;
    }

    for (auto element : data) {
        if (strcmp(element.key, "thing") == 0) {
            continue;
        }

        if (strcmp(element.key, "version") == 0) {
            continue;
        }

        // Parse gateway structure
    }

    DEBUG_MSG(PSTR("[GATEWAY] Structure restored successfully\n"));

    return true;
}

// -----------------------------------------------------------------------------

void _gatewayRestoreStorageFromMemory() {
    DynamicJsonBuffer jsonBuffer;

    JsonArray& registered_nodes = jsonBuffer.parseArray(_gatewayReadStoredConfiguration().c_str());

    for (JsonObject& stored_node : registered_nodes) {
        uint8_t id = (stored_node["address"].as<unsigned int>() - 1);

        strncpy(_gateway_nodes[id].serial_number, stored_node["serial_number"].as<char *>(), (uint8_t) strlen(stored_node["serial_number"].as<char *>()));

        _gateway_nodes[id].addressing.state = false;
        _gateway_nodes[id].addressing.lost = 0;

        _gateway_nodes[id].searching.state = false;
        _gateway_nodes[id].searching.registration = 0;

        JsonObject& hardware = stored_node["hardware"];

        strncpy(_gateway_nodes[id].hardware.manufacturer, hardware["manufacturer"].as<char *>(), (uint8_t) strlen(hardware["manufacturer"].as<char *>()));
        strncpy(_gateway_nodes[id].hardware.model, hardware["model"].as<char *>(), (uint8_t) strlen(hardware["model"].as<char *>()));
        strncpy(_gateway_nodes[id].hardware.version, hardware["version"].as<char *>(), (uint8_t) strlen(hardware["version"].as<char *>()));

        JsonObject& firmware = stored_node["firmware"];

        strncpy(_gateway_nodes[id].firmware.manufacturer, firmware["manufacturer"].as<char *>(), (uint8_t) strlen(firmware["manufacturer"].as<char *>()));
        strncpy(_gateway_nodes[id].firmware.model, firmware["model"].as<char *>(), (uint8_t) strlen(firmware["model"].as<char *>()));
        strncpy(_gateway_nodes[id].firmware.version, firmware["version"].as<char *>(), (uint8_t) strlen(firmware["version"].as<char *>()));

        _gateway_nodes[id].registers_size[GATEWAY_REGISTER_DI] = stored_node["digital_inputs"].as<unsigned int>();
        _gateway_nodes[id].registers_size[GATEWAY_REGISTER_DO] = stored_node["digital_outputs"].as<unsigned int>();
        _gateway_nodes[id].registers_size[GATEWAY_REGISTER_AI] = stored_node["analog_inputs"].as<unsigned int>();
        _gateway_nodes[id].registers_size[GATEWAY_REGISTER_AO] = stored_node["analog_outputs"].as<unsigned int>();
        _gateway_nodes[id].registers_size[GATEWAY_REGISTER_AI] = stored_node["event_inputs"].as<unsigned int>();
    }
}

#endif