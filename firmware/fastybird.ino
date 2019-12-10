/*

FASTYBIRD MODULE

Copyright (C) 2018 FastyBird s.r.o. <info@fastybird.com>

*/

#if FASTYBIRD_SUPPORT

#if MQTT_SUPPORT
    #include "./fastybird/mqtt_api.h"
#endif

#if FASTYBIRD_GATEWAY_SUPPORT
    #include "./fastybird/nodes.h"
#endif

bool _fastybird_initialized = false;

uint8_t _fastybird_device_advertisement_progress = FASTYBIRD_PUB_CONNECTION;
uint8_t _fastybird_channel_advertisement_progress = FASTYBIRD_PUB_CHANNEL_NAME;
uint8_t _fastybird_channel_property_advertisement_progress = FASTYBIRD_PUB_CHANNEL_PROPERTY_NAME;

std::vector<fastybird_channel_t> _fastybird_channels;
std::vector<fastybird_channel_property_t> _fastybird_channels_properties;

// -----------------------------------------------------------------------------

void _fastybirdInitializeChannels() {
    // Check if channels are initialized
    if (_fastybird_channels.size() == 0) {
        for (uint8_t i = 0; i < FASTYBIRD_MAX_CHANNELS; i++) {
            switch(i) {
                case 0:
                    _fastybird_channels.push_back(fastybird_channel_t {
                        CHANNEL1_NAME,
                        1,
                        false
                    });
                    break;

                case 1:
                    _fastybird_channels.push_back(fastybird_channel_t {
                        CHANNEL2_NAME,
                        1,
                        false
                    });
                    break;

                case 2:
                    _fastybird_channels.push_back(fastybird_channel_t {
                        CHANNEL3_NAME,
                        1,
                        false
                    });
                    break;

                case 3:
                    _fastybird_channels.push_back(fastybird_channel_t {
                        CHANNEL4_NAME,
                        1,
                        false
                    });
                    break;

                case 4:
                    _fastybird_channels.push_back(fastybird_channel_t {
                        CHANNEL5_NAME,
                        1,
                        false
                    });
                    break;

                case 5:
                    _fastybird_channels.push_back(fastybird_channel_t {
                        CHANNEL6_NAME,
                        1,
                        false
                    });
                    break;

                case 6:
                    _fastybird_channels.push_back(fastybird_channel_t {
                        CHANNEL7_NAME,
                        1,
                        false
                    });
                    break;

                case 7:
                    _fastybird_channels.push_back(fastybird_channel_t {
                        CHANNEL8_NAME,
                        1,
                        false
                    });
                    break;
            }
        }
    }
}

// -----------------------------------------------------------------------------
// MODULE PRIVATE
// -----------------------------------------------------------------------------

#if WEB_SUPPORT && WS_SUPPORT
    // WS client is connected
    void _fastybirdWSOnConnect(
        JsonObject& root
    ) {
        JsonArray& modules = root.containsKey("modules") ? root["modules"] : root.createNestedArray("modules");
        JsonObject& module = modules.createNestedObject();

        module["module"] = "fastybird";
        module["visible"] = true;

        // Data container
        JsonObject& data = module.createNestedObject("data");

        data["status"] = fastybirdIsDeviceInitialzed() ? true : false;
    }

// -----------------------------------------------------------------------------

    void _fastybirdWSOnUpdate(
        JsonObject& root
    ) {
        JsonArray& modules = root.containsKey("modules") ? root["modules"] : root.createNestedArray("modules");
        JsonObject& module = modules.createNestedObject();

        module["module"] = "fastybird";

        // Data container
        JsonObject& data = module.createNestedObject("data");

        data["status"] = fastybirdIsDeviceInitialzed() ? true : false;
    }
#endif // WEB_SUPPORT && WS_SUPPORT

// -----------------------------------------------------------------------------

/**
 * Initialize given device channel property
 */
bool _fastybirdInitializeChannelProperty(
    uint8_t channelIndex,
    uint8_t propertyIndex
) {
    if (
        channelIndex >= _fastybird_channels.size()
        || propertyIndex >= _fastybird_channels_properties.size()
        || _fastybird_channels_properties[propertyIndex].initialized
    ) {
        return true;
    }

    fastybird_channel_t channel = _fastybird_channels[channelIndex];
    fastybird_channel_property_t property = _fastybird_channels_properties[propertyIndex];

    switch (_fastybird_channel_property_advertisement_progress)
    {
        case FASTYBIRD_PUB_CHANNEL_PROPERTY_NAME:
            if (!_fastybirdPropagateChannelPropertyName(channel, property)) {
                return false;
            }

            _fastybird_channel_property_advertisement_progress = FASTYBIRD_PUB_CHANNEL_PROPERTY_TYPE;
            break;

        case FASTYBIRD_PUB_CHANNEL_PROPERTY_TYPE:
            if (!_fastybirdPropagateChannelPropertyType(channel, property)) {
                return false;
            }

            _fastybird_channel_property_advertisement_progress = FASTYBIRD_PUB_CHANNEL_PROPERTY_SETABLE;
            break;

        case FASTYBIRD_PUB_CHANNEL_PROPERTY_SETABLE:
            if (!_fastybirdPropagateChannelPropertySettable(channel, property)) {
                return false;
            }

            _fastybird_channel_property_advertisement_progress = FASTYBIRD_PUB_CHANNEL_PROPERTY_QUERYABLE;
            break;

        case FASTYBIRD_PUB_CHANNEL_PROPERTY_QUERYABLE:
            if (!_fastybirdPropagateChannelPropertyQueryable(channel, property)) {
                return false;
            }

            _fastybird_channel_property_advertisement_progress = FASTYBIRD_PUB_CHANNEL_PROPERTY_DATA_TYPE;
            break;

        case FASTYBIRD_PUB_CHANNEL_PROPERTY_DATA_TYPE:
            if (!_fastybirdPropagateChannelPropertyDataType(channel, property)) {
                return false;
            }

            _fastybird_channel_property_advertisement_progress = FASTYBIRD_PUB_CHANNEL_PROPERTY_UNITS;
            break;

        case FASTYBIRD_PUB_CHANNEL_PROPERTY_UNITS:
            if (!_fastybirdPropagateChannelPropertyUnit(channel, property)) {
                return false;
            }

            _fastybird_channel_property_advertisement_progress = FASTYBIRD_PUB_CHANNEL_PROPERTY_FORMAT;
            break;

        case FASTYBIRD_PUB_CHANNEL_PROPERTY_FORMAT:
            if (!_fastybirdPropagateChannelPropertyFormat(channel, property)) {
                return false;
            }

            _fastybird_channel_property_advertisement_progress = FASTYBIRD_PUB_CHANNEL_PROPERTY_DONE;
            break;

// -----------------------------------------------------------------------------
// CHANNEL PROPERTY INITIALIZATION IS DONE
// -----------------------------------------------------------------------------

        case FASTYBIRD_PUB_CHANNEL_PROPERTY_DONE:
            return true;
            break;
    }

    return false;
}

// -----------------------------------------------------------------------------

/**
 * Initialize given device channel
 */
bool _fastybirdInitializeChannel(
    uint8_t channelIndex
) {
    if (
        channelIndex >= _fastybird_channels.size()
        || _fastybird_channels[channelIndex].initialized
    ) {
        return true;
    }

    DynamicJsonBuffer jsonBuffer;

    JsonArray& configurationSchema = jsonBuffer.createArray();

    std::vector<String> channel_properties;
    std::vector<String> channel_controls;

    fastybird_channel_t channel = _fastybird_channels[channelIndex];

    switch (_fastybird_channel_advertisement_progress)
    {
        
// -----------------------------------------------------------------------------
// CHANNEL BASIC INFO
// -----------------------------------------------------------------------------

        case FASTYBIRD_PUB_CHANNEL_NAME:
            if (!_fastybirdPropagateChannelName(channel)) {
                return false;
            }
            
            _fastybird_channel_advertisement_progress = FASTYBIRD_PUB_CHANNEL_ARRAY;
            break;

        case FASTYBIRD_PUB_CHANNEL_ARRAY:
            if (!_fastybirdPropagateChannelSize(channel)) {
                return false;
            }

            _fastybird_channel_advertisement_progress = FASTYBIRD_PUB_CHANNEL_PROPERTIES;
            break;

        case FASTYBIRD_PUB_CHANNEL_PROPERTIES:
            if (channel.properties.size() > 0) {
                for (uint8_t i = 0; i < channel.properties.size(); i++) {
                    uint8_t propertyIndex = channel.properties[i];

                    if (propertyIndex < _fastybird_channels_properties.size()) {
                        channel_properties.push_back(_fastybird_channels_properties[propertyIndex].name);
                    }
                }
            }

            if (channel_properties.size() > 0 && !_fastybirdPropagateChannelProperties(channel, channel_properties)) {
                return false;
            }

            _fastybird_channel_advertisement_progress = FASTYBIRD_PUB_CHANNEL_CONTROL_STRUCTURE;
            break;

        case FASTYBIRD_PUB_CHANNEL_CONTROL_STRUCTURE:
            if (channel.configurationCallback.size()) {
                channel_controls.push_back(FASTYBIRD_CHANNEL_CONTROL_CONFIGURE);
            }

            if (!_fastybirdPropagateChannelControlConfiguration(channel, channel_controls)) {
                return false;
            }

            if (channel.configurationSchemaCallback.size() > 0) {
                _fastybird_channel_advertisement_progress = FASTYBIRD_PUB_CHANNEL_CONFIGURATION_SCHEMA;
 
            } else if (channel.properties.size() > 0) {
                _fastybird_channel_advertisement_progress = FASTYBIRD_PUB_CHANNEL_PROPERTY;

            } else {
                _fastybird_channel_advertisement_progress = FASTYBIRD_PUB_CHANNEL_DONE;
            }
            break;

// -----------------------------------------------------------------------------
// CHANNEL CONTROL CONFIGURATION
// -----------------------------------------------------------------------------

        case FASTYBIRD_PUB_CHANNEL_CONFIGURATION_SCHEMA:
            for (uint8_t i = 0; i < channel.configurationSchemaCallback.size(); i++) {
                (channel.configurationSchemaCallback[i])(configurationSchema);
            }

            if (configurationSchema.size() > 0) {
                if (!_fastybirdPropagateChannelConfigurationSchema(channel, configurationSchema)) {
                    return false;
                }
            }

            if (channel.properties.size() > 0) {
                _fastybird_channel_advertisement_progress = FASTYBIRD_PUB_CHANNEL_PROPERTY;
 
            } else {
                _fastybird_channel_advertisement_progress = FASTYBIRD_PUB_CHANNEL_DONE;
            }
            break;

// -----------------------------------------------------------------------------
// CHANNEL PROPERTIES
// -----------------------------------------------------------------------------

        case FASTYBIRD_PUB_CHANNEL_PROPERTY:
            if (channel.properties.size() > 0) {
                for (uint8_t i = 0; i < channel.properties.size(); i++) {
                    uint8_t propertyIndex = channel.properties[i];

                    if (
                        propertyIndex >= _fastybird_channels_properties.size()
                        || _fastybird_channels_properties[propertyIndex].initialized
                    ) {
                        continue;
                    }

                    if (!_fastybirdInitializeChannelProperty(channelIndex, propertyIndex)) {
                        return false;
                    }

                    _fastybird_channels_properties[propertyIndex].initialized = true;

                    _fastybird_channel_property_advertisement_progress = FASTYBIRD_PUB_CHANNEL_PROPERTY_NAME;
                }
            }

            _fastybird_channel_advertisement_progress = FASTYBIRD_PUB_CHANNEL_DONE;
            break;

// -----------------------------------------------------------------------------
// CHANNEL INITIALIZATION IS DONE
// -----------------------------------------------------------------------------

        case FASTYBIRD_PUB_CHANNEL_DONE:
            return true;
            break;
    }

    return false;
}

// -----------------------------------------------------------------------------

/**
 * Initilialize device to broker
 */
void _fastybirdInitializeSystem() {
    DynamicJsonBuffer jsonBuffer;

    JsonArray& configurationSchema = jsonBuffer.createArray();

    std::vector<String> device_properties;
    std::vector<String> device_controls;

    switch (_fastybird_device_advertisement_progress)
    {
        case FASTYBIRD_PUB_CONNECTION:
            // Notify cloud broker that device is sending initialization sequences
            if (!_fastybirdPropagateDeviceProperty(FASTYBIRD_PROPERTY_STATE, FASTYBIRD_STATUS_INIT)) {
                return;
            }
 
            // Collect all device default properties...
            device_properties.push_back(FASTYBIRD_PROPERTY_INTERVAL);
            device_properties.push_back(FASTYBIRD_PROPERTY_UPTIME);
            device_properties.push_back(FASTYBIRD_PROPERTY_FREE_HEAP);
            device_properties.push_back(FASTYBIRD_PROPERTY_CPU_LOAD);
            device_properties.push_back(FASTYBIRD_PROPERTY_STATE);

            #if WIFI_SUPPORT
                device_properties.push_back(FASTYBIRD_PROPERTY_IP_ADDRESS);
            #endif

            #if ADC_MODE_VALUE == ADC_VCC
                device_properties.push_back(FASTYBIRD_PROPERTY_VCC);
            #endif

            #if WIFI_SUPPORT
                device_properties.push_back(FASTYBIRD_PROPERTY_SSID);
                device_properties.push_back(FASTYBIRD_PROPERTY_RSSI);
            #endif
 
            // ...and pass them to the broker
            if (!_fastybirdPropagateDevicePropertiesStructure(device_properties)) {
                return;
            }

            // For device with wifi support notify its IP address
            #if WIFI_SUPPORT
                if (!_fastybirdPropagateDeviceProperty(FASTYBIRD_PROPERTY_IP_ADDRESS, getIP().c_str())) {
                    return;
                }
            #endif

            // Heartbeat interval
            if (!_fastybirdPropagateDeviceProperty(FASTYBIRD_PROPERTY_INTERVAL, String(HEARTBEAT_INTERVAL / 1000).c_str())) {
                return;
            }

            _fastybird_device_advertisement_progress = FASTYBIRD_PUB_NAME;
            break;

        case FASTYBIRD_PUB_NAME:
            if (!_fastybirdPropagateDeviceName(getIdentifier().c_str())) {
                return;
            }

            _fastybird_device_advertisement_progress = FASTYBIRD_PUB_HARDWARE;
            break;

        // Describe device hardware details to cloud broker
        case FASTYBIRD_PUB_HARDWARE:
            #if WIFI_SUPPORT
                if (!_fastybirdPropagateDeviceHardwareField(FASTYBIRD_HARDWARE_MAC_ADDRESS, WiFi.macAddress().c_str())) {
                    return;
                }
            #endif

            if (!_fastybirdPropagateDeviceHardwareManufacturer(MANUFACTURER)) {
                return;
            }

            if (!_fastybirdPropagateDeviceHardwareModelName(DEVICE)) {
                return;
            }

            if (!_fastybirdPropagateDeviceHardwareVersion(HARWARE_VERSION)) {
                return;
            }

            _fastybird_device_advertisement_progress = FASTYBIRD_PUB_FIRMWARE;
            break;

        // Describe device firmware details to cloud broker
        case FASTYBIRD_PUB_FIRMWARE:
            if (!_fastybirdPropagateDeviceFirmwareManufacturer(FIRMWARE_MANUFACTURER)) {
                return;
            }

            if (!_fastybirdPropagateDeviceFirmwareName(FIRMWARE_MANUFACTURER)) {
                return;
            }

            if (!_fastybirdPropagateDeviceFirmwareVersion(FIRMWARE_VERSION)) {
                return;
            }

            _fastybird_device_advertisement_progress = FASTYBIRD_PUB_CHANNELS;
            break;

        case FASTYBIRD_PUB_CHANNELS:
            if (!_fastybirdPropagateDeviceChannels(_fastybird_channels)) {
                return;
            }

            _fastybird_device_advertisement_progress = FASTYBIRD_PUB_CONTROL_STRUCTURE;
            break;

        case FASTYBIRD_PUB_CONTROL_STRUCTURE:
            for (uint8_t i = 0; i < _fastybird_on_control_callbacks.size(); i++) {
                device_controls.push_back(_fastybird_on_control_callbacks[i].controlName);
            }

            if (!_fastybirdPropagateDeviceControlConfiguration(device_controls)) {
                return;
            }

            _fastybird_device_advertisement_progress = FASTYBIRD_PUB_CONFIGURATION_SCHEMA;
            break;

        case FASTYBIRD_PUB_CONFIGURATION_SCHEMA:
            for (uint8_t i = 0; i < _fastybird_report_configuration_schema_callbacks.size(); i++) {
                (_fastybird_report_configuration_schema_callbacks[i])(configurationSchema);
            }

            if (configurationSchema.size() > 0) {
                if (!_fastybirdPropagateDeviceConfigurationSchema(configurationSchema)) {
                    return;
                }
            }

            _fastybird_device_advertisement_progress = FASTYBIRD_PUB_INITIALIZE_CHANNELS;
            break;

        case FASTYBIRD_PUB_INITIALIZE_CHANNELS:
            if (_fastybird_channels.size() > 0) {
                for (uint8_t i = 0; i < _fastybird_channels.size(); i++) {
                    if (_fastybird_channels[i].initialized) {
                        continue;
                    }

                    if (!_fastybirdInitializeChannel(i)) {
                        return;
                    }

                    _fastybird_channels[i].initialized = true;

                    _fastybird_channel_advertisement_progress = FASTYBIRD_PUB_CHANNEL_NAME;

                    return;
                }
            }

            _fastybird_device_advertisement_progress = FASTYBIRD_PUB_READY;
            break;

        case FASTYBIRD_PUB_READY:
            if (!_fastybirdPropagateDeviceProperty(FASTYBIRD_PROPERTY_STATE, FASTYBIRD_STATUS_READY)) {
                return;
            }

            _fastybird_device_advertisement_progress = FASTYBIRD_PUB_CONFIGURATION;
            break;

        case FASTYBIRD_PUB_CONFIGURATION:
            if (!fastybirdReportConfiguration()) {
                return;
            }

            _fastybird_device_advertisement_progress = FASTYBIRD_PUB_CHANNELS_CONFIGURATION;
            break;

        case FASTYBIRD_PUB_CHANNELS_CONFIGURATION:
            if (_fastybird_channels.size() > 0) {
                for (uint8_t i = 0; i < _fastybird_channels.size(); i++) {
                    if (_fastybird_channels[i].configurationCallback.size() > 0) {
                        if (!fastybirdReportChannelConfiguration(i)) {
                            return;
                        }
                    }
                }
            }

            _fastybird_device_advertisement_progress = FASTYBIRD_PUB_HEARTBEAT;
            break;

        case FASTYBIRD_PUB_HEARTBEAT:
            _fastybird_initialized = true;

            #if WEB_SUPPORT && WS_SUPPORT
                wsSend(_fastybirdWSOnUpdate);
            #endif
            
            for (uint8_t i = 0; i < _fastybird_on_connect_callbacks.size(); i++) {
                (_fastybird_on_connect_callbacks[i])();
            }
            break;
    }
}

// -----------------------------------------------------------------------------

bool fastybirdReportConfiguration() {
    DynamicJsonBuffer jsonBuffer;

    JsonObject& configuration = jsonBuffer.createObject();

    for (uint8_t i = 0; i < _fastybird_report_configuration_callbacks.size(); i++) {
        (_fastybird_report_configuration_callbacks[i])(configuration);
    }

    if (configuration.size() > 0) {
        return _fastybirdPropagateDeviceConfiguration(configuration);
    }

    return true;
}

// -----------------------------------------------------------------------------

bool fastybirdReportChannelConfiguration(
    uint8_t id
) {
    // Check channel
    if (id >= _fastybird_channels.size()) {
        return false;
    }

    DynamicJsonBuffer jsonBuffer;

    JsonObject& configuration = jsonBuffer.createObject();

    for (uint8_t i = 0; i < _fastybird_channels[id].configurationCallback.size(); i++) {
        (_fastybird_channels[id].configurationCallback[i])(configuration);
    }

    if (configuration.size() > 0) {
        return _fastybirdPropagateChannelConfiguration(_fastybird_channels[id], 0, configuration);
    }

    return true;
}

// -----------------------------------------------------------------------------

bool fastybirdReportChannelPropertyValue(
    const uint8_t channel,
    String property,
    const char * payload
) {
    // Convert channel number to channel index
    uint8_t id = (channel - 1);

    // Check channel
    if (id >= _fastybird_channels.size()) {
        return false;
    }

    for (uint8_t i = 0; i < _fastybird_channels_properties.size(); i++) {
        // Search for given property
        if (property.equals(_fastybird_channels_properties[i].name)) {
            return _fastybirdPropagateChannelValue(
                _fastybird_channels[id],
                _fastybird_channels_properties[i],
                payload
            );
        }
    }
}

// -----------------------------------------------------------------------------

void fastybirdResetDeviceInitialization() {
    _fastybird_initialized = false;
    _fastybird_device_advertisement_progress = FASTYBIRD_PUB_CONNECTION;

    for (uint8_t i = 0; i < _fastybird_channels.size(); i++) {
        _fastybird_channels[i].initialized = false;
    }

    for (uint8_t j = 0; j < _fastybird_channels_properties.size(); j++) {
        _fastybird_channels_properties[j].initialized = false;
    }

    _fastybird_channel_advertisement_progress = FASTYBIRD_PUB_CHANNEL_NAME;
    _fastybird_channel_property_advertisement_progress = FASTYBIRD_PUB_CHANNEL_PROPERTY_NAME;

    #if WEB_SUPPORT && WS_SUPPORT
        wsSend(_fastybirdWSOnUpdate);
    #endif
    
    _fastybirdBeforeInitialization();
}

// -----------------------------------------------------------------------------

bool fastybirdIsDeviceInitialzed() {
    return _fastybird_initialized;
}

// -----------------------------------------------------------------------------

/**
 * Map property to channel
 */
uint8_t fastybirdRegisterPropertyToChannel(
    uint8_t channel,
    uint8_t propertyIndex
) {
    _fastybirdInitializeChannels();

    // Convert channel number to channel index
    uint8_t id = (channel - 1);

    // Check channel
    if (id >= _fastybird_channels.size()) {
        return false;
    }

    _fastybird_channels[id].properties.push_back(propertyIndex);
}

// -----------------------------------------------------------------------------

/**
 * Map configuration schema callback to channel
 */
uint8_t fastybirdRegisterConfigurationSchemaToChannel(
    uint8_t channel,
    fastybird_report_channel_confiuration_schema_callback_f callback
) {
    _fastybirdInitializeChannels();

    // Convert channel number to channel index
    uint8_t id = (channel - 1);

    // Check channel
    if (id >= _fastybird_channels.size()) {
        return false;
    }

    _fastybird_channels[id].configurationSchemaCallback.push_back(callback);
}

// -----------------------------------------------------------------------------

/**
 * Map configuration callback to channel
 */
uint8_t fastybirdRegisterConfigurationToChannel(
    uint8_t channel,
    fastybird_report_channel_configuration_callback_f callback
) {
    _fastybirdInitializeChannels();

    // Convert channel number to channel index
    uint8_t id = (channel - 1);

    // Check channel
    if (id >= _fastybird_channels.size()) {
        return false;
    }

    _fastybird_channels[id].configurationCallback.push_back(callback);
}

// -----------------------------------------------------------------------------

/**
 * Map configure callback to channel
 */
uint8_t fastybirdRegisterConfigureActionToChannel(
    uint8_t channel,
    fastybird_on_channel_configure_callback_f callback
) {
    _fastybirdInitializeChannels();

    // Convert channel number to channel index
    uint8_t id = (channel - 1);

    // Check channel
    if (id >= _fastybird_channels.size()) {
        return false;
    }

    _fastybird_channels[id].configureCallback.push_back(callback);
}

// -----------------------------------------------------------------------------

/**
 * Register module property to properties container
 */
uint8_t fastybirdRegisterChannelProperty(
    fastybird_channel_property_t property
) {
    _fastybird_channels_properties.push_back(property);

    return (_fastybird_channels_properties.size() - 1);
}

// -----------------------------------------------------------------------------
// MODULE CORE
// -----------------------------------------------------------------------------

void fastybirdSetup() {
    _fastybirdApiSetup();

    #if FASTYBIRD_GATEWAY_SUPPORT
        _fastybirdNodeSetup();
    #endif

    #if WEB_SUPPORT && WS_SUPPORT
        wsOnConnectRegister(_fastybirdWSOnConnect);
        wsOnUpdateRegister(_fastybirdWSOnUpdate);
    #endif

    systemOnHeartbeatRegister(_fastybirdOnHeartbeat);
    
    fastybirdOnControlRegister(
        [](const char * payload) {
            DynamicJsonBuffer jsonBuffer;

            // Parse payload
            JsonObject& root = jsonBuffer.parseObject(payload);

            if (root.success()) {
                DEBUG_MSG(PSTR("[FASTYBIRD] Sending configuration to modules\n"));

                for (uint8_t i = 0; i < _fastybird_on_configure_callbacks.size(); i++) {
                    (_fastybird_on_configure_callbacks[i])(root);
                }

                DEBUG_MSG(PSTR("[FASTYBIRD] Changes were saved\n"));

                #if WEB_SUPPORT && WS_SUPPORT
                    wsReportConfiguration();
                #endif

                // Report back updated configuration
                fastybirdReportConfiguration();
                
            } else {
                DEBUG_MSG(PSTR("[FASTYBIRD] Received payload is not in valid JSON format\n"));
            }
        },
        FASTYBIRD_DEVICE_CONTROL_CONFIGURE
    );

    fastybirdOnControlRegister(
        [](const char * payload) {
            DEBUG_MSG(PSTR("[FASTYBIRD] Requested reset action\n"));

            #if WEB_SUPPORT && WS_SUPPORT
                // Send notification to all clients
                wsSend_P(PSTR("{\"doAction\": \"reload\", \"reason\": \"reset\"}"));
            #endif
            
            deferredReset(250, CUSTOM_RESET_BROKER);
        },
        FASTYBIRD_DEVICE_CONTROL_REBOOT
    );

    // Register firmware callbacks
    firmwareRegisterLoop(fastybirdLoop);
}

// -----------------------------------------------------------------------------

void fastybirdLoop() {
    if (_fastybirdIsApiReady()) {
        if (_fastybird_initialized == false) {
            static uint32_t last_init_call = 0;

            if (last_init_call == 0 || (millis() - last_init_call > 500)) {
                last_init_call = millis();

                _fastybirdInitializeSystem();
            }
        }
    }
}

#endif // FASTYBIRD_SUPPORT
