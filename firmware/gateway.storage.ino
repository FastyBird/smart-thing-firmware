/*

GATEWAY MODULE - NODES STORAGE

Copyright (C) 2018 FastyBird Ltd. <info@fastybird.com>

*/

#if FB_GATEWAY_SUPPORT

const char * _gateway_storage_config_filename = "gateway.conf";

// -----------------------------------------------------------------------------
// MODULE: SUB-MODULE API
// -----------------------------------------------------------------------------

String gatewayStorageReadConfiguration()
{
    String stored_content = storageReadConfiguration(_gateway_storage_config_filename);

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

void gatewayStorageAddNode(
    const uint8_t nodeIndex
) {
    // Validate node index
    if (nodeIndex >= FB_GATEWAY_MAX_NODES) {
        return;
    }

    DynamicJsonBuffer jsonBuffer;

    JsonArray& registered_nodes = jsonBuffer.parseArray(gatewayStorageReadConfiguration().c_str());

    uint8_t index = 0;

    gateway_node_t node = gatewayGetNode(nodeIndex);

    for (JsonObject& stored_node : registered_nodes) {
        if (stored_node["address"] == (nodeIndex + 1)) {
            registered_nodes.remove(index);
        }

        index++;
    }

    JsonObject& storage_node = registered_nodes.createNestedObject();

    if (!storage_node.success()) {
        DEBUG_MSG(PSTR("[ERR][GATEWAY][STORAGE] Could not create configuration schema for storing.\n"));

        return;
    }

    storage_node["address"] = nodeIndex + 1;
    storage_node["serial_number"] = node.serial_number;

    JsonObject& hardware_info = storage_node.createNestedObject("hardware");

    hardware_info["manufacturer"] = node.hardware.manufacturer;
    hardware_info["model"] = node.hardware.model;
    hardware_info["version"] = node.hardware.version;

    JsonObject& firmware_info = storage_node.createNestedObject("firmware");

    firmware_info["manufacturer"] = node.firmware.manufacturer;
    firmware_info["model"] = node.firmware.model;
    firmware_info["version"] = node.firmware.version;

    storage_node["digital_inputs"] = gatewayRegistersDigitalInputsSize(nodeIndex);
    storage_node["digital_outputs"] = gatewayRegistersDigitalOutputsSize(nodeIndex);
    storage_node["analog_inputs"] = gatewayRegistersAnalogInputsSize(nodeIndex);
    storage_node["analog_outputs"] = gatewayRegistersAnalogOutputsSize(nodeIndex);
    storage_node["event_inputs"] = gatewayRegistersEventInputsSize(nodeIndex);

    String output;

    registered_nodes.printTo(output);

    storageWriteConfiguration(_gateway_storage_config_filename, output);
}

// -----------------------------------------------------------------------------

void gatewayStorageRemoveNode(
    const uint8_t nodeIndex
) {
    DynamicJsonBuffer jsonBuffer;

    JsonArray& registered_nodes = jsonBuffer.parseArray(gatewayStorageReadConfiguration().c_str());

    bool removed = false;
    uint8_t index = 0;

    for (JsonObject& stored_node : registered_nodes) {
        if (stored_node["address"] == (nodeIndex + 1)) {
            registered_nodes.remove(index);

            removed = true;
        }

        index++;
    }

    if (removed) {
        String output;

        registered_nodes.printTo(output);

        storageWriteConfiguration(_gateway_storage_config_filename, output);
    }
}

// -----------------------------------------------------------------------------

bool gatewayStorageRestoreFromJson(
    JsonObject& data
) {
    const char * _device = data["device"];

    if (strcmp(_device, DEVICE) != 0) {
        return false;
    }

    const char * _version = data["version"];

    if (strcmp(_version, FIRMWARE_VERSION) != 0) {
        return false;
    }

    for (auto element : data) {
        if (
            strcmp(element.key, "device") == 0
            || strcmp(element.key, "manufacturer") == 0
            || strcmp(element.key, "version") == 0
            || strcmp(element.key, "backup") == 0
            || strcmp(element.key, "timestamp") == 0
        ) {
            continue;
        }

        // Parse data
    }

    DEBUG_MSG(PSTR("[INFO][GATEWAY][STORAGE] Structure restored successfully\n"));

    return true;
}

// -----------------------------------------------------------------------------
// MODULE: SUB-MODULE CORE
// -----------------------------------------------------------------------------

void gatewayStorageSetup()
{
    DynamicJsonBuffer jsonBuffer;

    JsonArray& registered_nodes = jsonBuffer.parseArray(gatewayStorageReadConfiguration().c_str());

    uint8_t loaded_counter = 0;

    // Load all nodes definition stored in memory
    for (JsonObject& stored_node : registered_nodes) {
        uint8_t nodeIndex = (stored_node["address"].as<unsigned int>() - 1);

        gatewaySetSerialNumber(nodeIndex, stored_node["serial_number"].as<char *>());

        gatewayResetNodeFlags(nodeIndex);

        JsonObject& hardware = stored_node["hardware"];

        gatewaySetHardwareModel(nodeIndex, hardware["model"].as<char *>());
        gatewaySetHardwareManufacturer(nodeIndex, hardware["manufacturer"].as<char *>());
        gatewaySetHardwareVersion(nodeIndex, hardware["version"].as<char *>());

        JsonObject& firmware = stored_node["firmware"];

        gatewaySetFirmwareModel(nodeIndex, firmware["model"].as<char *>());
        gatewaySetFirmwareManufacturer(nodeIndex, firmware["manufacturer"].as<char *>());
        gatewaySetFirmwareVersion(nodeIndex, firmware["version"].as<char *>());

        gatewayAddressingSet(nodeIndex, stored_node["address"].as<unsigned int>());
        gatewayInitializationReset(nodeIndex);
        gatewayCommunicationReset(nodeIndex);
        gatewayRegistersReset(nodeIndex);
        gatewayRegistersResetReading(nodeIndex);

        loaded_counter++;
    }

    DEBUG_MSG(PSTR("[INFO][GATEWAY][STORAGE] Successfylly loaded %d nodes from memory storage\n"), loaded_counter);
}

#endif // FB_GATEWAY_SUPPORT
