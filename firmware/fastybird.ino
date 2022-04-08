/*

FASTYBIRD MODULE

Copyright (C) 2018 FastyBird Ltd. <info@fastybird.com>

*/

#if FASTYBIRD_SUPPORT

bool _fastybird_initialized = false;

// Initilalization pointers
uint8_t _fastybird_initialize_property = INDEX_NONE;
uint8_t _fastybird_initialize_channel = INDEX_NONE;
uint8_t _fastybird_initialize_channel_property = INDEX_NONE;

// Initialization step pointers
uint8_t _fastybird_device_advertisement_progress = FASTYBIRD_PUB_CONNECTION;
uint8_t _fastybird_device_property_advertisement_progress = FASTYBIRD_PUB_PROPERTY_NAME;
uint8_t _fastybird_channel_advertisement_progress = FASTYBIRD_PUB_CHANNEL_NAME;
uint8_t _fastybird_channel_property_advertisement_progress = FASTYBIRD_PUB_PROPERTY_NAME;

// Registered channels
std::vector<fastybird_channel_t> _fastybird_channels;
// Registered properties
std::vector<fastybird_property_t> _fastybird_properties;

// Device callbacks - each module could register own callback
std::vector<fastybird_on_connect_callback_f> _fastybird_on_connect_callbacks;
std::vector<fastybird_on_report_configuration_schema_callback_f> _fastybird_report_configuration_schema_callbacks;
std::vector<fastybird_on_report_configuration_callback_f> _fastybird_report_configuration_callbacks;
std::vector<fastybird_on_configure_callback_f> _fastybird_on_configure_callbacks;
std::vector<fastybird_on_control_callback_t> _fastybird_on_control_callbacks;

// Channel callbacks - each module could register own callback
std::vector<fastybird_on_report_channel_configuration_schema_callback_f> _fastybird_report_channel_configuration_schema_callbacks;
std::vector<fastybird_on_report_channel_configuration_callback_f> _fastybird_report_channel_configuration_callbacks;
std::vector<fastybird_on_channel_configure_callback_f> _fastybird_on_channel_configure_callbacks;

// -----------------------------------------------------------------------------
// MODULE PRIVATE
// -----------------------------------------------------------------------------

#if FASTYBIRD_MAX_CHANNELS > 0
    void _fastybirdInitializeChannels()
    {
        // Check if channels are initialized
        if (_fastybird_channels.size() == 0) {
            for (uint8_t i = 0; i < FASTYBIRD_MAX_CHANNELS; i++) {
                switch(i) {
                    case 0:
                        _fastybird_channels.push_back(fastybird_channel_t {
                            FASTYBIRD_CHANNEL1_NAME
                        });
                        break;

                    case 1:
                        _fastybird_channels.push_back(fastybird_channel_t {
                            FASTYBIRD_CHANNEL2_NAME
                        });
                        break;

                    case 2:
                        _fastybird_channels.push_back(fastybird_channel_t {
                            FASTYBIRD_CHANNEL3_NAME
                        });
                        break;

                    case 3:
                        _fastybird_channels.push_back(fastybird_channel_t {
                            FASTYBIRD_CHANNEL4_NAME
                        });
                        break;

                    case 4:
                        _fastybird_channels.push_back(fastybird_channel_t {
                            FASTYBIRD_CHANNEL5_NAME
                        });
                        break;

                    case 5:
                        _fastybird_channels.push_back(fastybird_channel_t {
                            FASTYBIRD_CHANNEL6_NAME
                        });
                        break;

                    case 6:
                        _fastybird_channels.push_back(fastybird_channel_t {
                            FASTYBIRD_CHANNEL7_NAME
                        });
                        break;

                    case 7:
                        _fastybird_channels.push_back(fastybird_channel_t {
                            FASTYBIRD_CHANNEL8_NAME
                        });
                        break;
                }
            }
        }
    }
#endif

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

#if FASTYBIRD_MAX_CHANNELS > 0
    /**
     * Initialize given device channel property
     */
    bool _fastybirdInitializeChannelProperty(
        uint8_t channelIndex,
        uint8_t propertyIndex
    ) {
        if (
            channelIndex >= FASTYBIRD_MAX_CHANNELS
            || propertyIndex >= _fastybird_properties.size()
        ) {
            return true;
        }

        fastybird_channel_t channel = _fastybird_channels[channelIndex];
        fastybird_property_t property = _fastybird_properties[propertyIndex];

        switch (_fastybird_channel_property_advertisement_progress)
        {
            case FASTYBIRD_PUB_PROPERTY_NAME:
                if (!fastybirdApiPropagateChannelPropertyName(channel.name.c_str(), property.name.c_str(), property.name.c_str())) {
                    return false;
                }

                _fastybird_channel_property_advertisement_progress = FASTYBIRD_PUB_PROPERTY_SETABLE;
                break;

            case FASTYBIRD_PUB_PROPERTY_SETABLE:
                if (!fastybirdApiPropagateChannelPropertySettable(channel.name.c_str(), property.name.c_str(), property.settable)) {
                    return false;
                }

                _fastybird_channel_property_advertisement_progress = FASTYBIRD_PUB_PROPERTY_QUERYABLE;
                break;

            case FASTYBIRD_PUB_PROPERTY_QUERYABLE:
                if (!fastybirdApiPropagateChannelPropertyQueryable(channel.name.c_str(), property.name.c_str(), property.queryable)) {
                    return false;
                }

                _fastybird_channel_property_advertisement_progress = FASTYBIRD_PUB_PROPERTY_DATA_TYPE;
                break;

            case FASTYBIRD_PUB_PROPERTY_DATA_TYPE:
                if (!fastybirdApiPropagateChannelPropertyDataType(channel.name.c_str(), property.name.c_str(), property.datatype.c_str())) {
                    return false;
                }

                _fastybird_channel_property_advertisement_progress = FASTYBIRD_PUB_PROPERTY_UNIT;
                break;

            case FASTYBIRD_PUB_PROPERTY_UNIT:
                if (!fastybirdApiPropagateChannelPropertyUnit(channel.name.c_str(), property.name.c_str(), property.unit.c_str())) {
                    return false;
                }

                _fastybird_channel_property_advertisement_progress = FASTYBIRD_PUB_PROPERTY_FORMAT;
                break;

            case FASTYBIRD_PUB_PROPERTY_FORMAT:
                if (!fastybirdApiPropagateChannelPropertyFormat(channel.name.c_str(), property.name.c_str(), property.format.c_str())) {
                    return false;
                }

                _fastybird_channel_property_advertisement_progress = FASTYBIRD_PUB_PROPERTY_DONE;
                break;

// -----------------------------------------------------------------------------
// CHANNEL PROPERTY INITIALIZATION IS DONE
// -----------------------------------------------------------------------------

            case FASTYBIRD_PUB_PROPERTY_DONE:
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
        const uint8_t channelIndex
    ) {
        if (
            channelIndex >= FASTYBIRD_MAX_CHANNELS
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
                if (!fastybirdApiPropagateChannelName(channel.name.c_str(), channel.name.c_str())) {
                    return false;
                }
                
                _fastybird_channel_advertisement_progress = FASTYBIRD_PUB_CHANNEL_PROPERTIES;
                break;

            case FASTYBIRD_PUB_CHANNEL_PROPERTIES:
                for (uint8_t i = 0; i < channel.properties.size(); i++) {
                    if (channel.properties[i] < _fastybird_properties.size()) {
                        channel_properties.push_back(_fastybird_properties[channel.properties[i]].name);
                    }
                }

                if (!fastybirdApiPropagateChannelPropertiesNames(channel.name.c_str(), channel_properties)) {
                    return false;
                }

                _fastybird_channel_advertisement_progress = FASTYBIRD_PUB_CHANNEL_CONTROL_STRUCTURE;
                break;

            case FASTYBIRD_PUB_CHANNEL_CONTROL_STRUCTURE:
                if (channel.configurationCallbacks.size()) {
                    channel_controls.push_back(FASTYBIRD_CHANNEL_CONTROL_CONFIGURE);
                }

                if (!fastybirdApiPropagateChannelControlNames(channel.name.c_str(), channel_controls)) {
                    return false;
                }

                if (channel.configurationSchemaCallbacks.size() > 0) {
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
                fastybirdCalldReportChannelConfigurationSchema(channelIndex, configurationSchema);

                if (
                    configurationSchema.size() > 0
                    && !fastybirdApiPropagateChannelConfigurationSchema(channel.name.c_str(), configurationSchema)
                ) {
                    return false;
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
                    // Take properties pointer
                    uint8_t propertyIndex = _fastybird_initialize_channel_property;

                    // No properties were initialized...
                    if (propertyIndex == INDEX_NONE) {
                        // ...pick first from channel properties collection
                        propertyIndex = channel.properties[0];

                        // Update property pointer
                        _fastybird_initialize_channel_property = propertyIndex;
                    }

                    // Check if property index is in valid range
                    if (propertyIndex < _fastybird_properties.size()) {
                        // Process property initialization
                        if (!_fastybirdInitializeChannelProperty(channelIndex, propertyIndex)) {
                            return false;
                        }

                        // Property was fully initialized
                        _fastybird_channel_property_advertisement_progress = FASTYBIRD_PUB_PROPERTY_NAME;

                        // Search for new property index
                        for (uint8_t i = 0; i < channel.properties.size(); i++) {
                            // Search for property index position in collection...
                            if (channel.properties[i] == propertyIndex) {
                                // Check if there is left any uninitialized property...
                                if ((i + 1) < channel.properties.size()) {
                                    // ...if yes, update pointer
                                    _fastybird_initialize_channel_property = channel.properties[i + 1];

                                    return false;
                                }

                                // ...if not, finish properties initialization
                            }
                        }
                    }
                }

                // Reset property initialize step pointer
                _fastybird_channel_property_advertisement_progress = FASTYBIRD_PUB_PROPERTY_NAME;

                // Reset properties process pointer
                _fastybird_initialize_channel_property = INDEX_NONE;

                // Move to the next step
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
#endif

// -----------------------------------------------------------------------------

/**
 * Initilialize device to broker
 */
void _fastybirdInitializeDevice()
{
    DynamicJsonBuffer jsonBuffer;

    JsonArray& configurationSchema = jsonBuffer.createArray();

    std::vector<String> channels;
    std::vector<String> properties;
    std::vector<String> controls;

    char serial_no[20];

    switch (_fastybird_device_advertisement_progress)
    {
        case FASTYBIRD_PUB_CONNECTION:
            // Notify broker that device is sending initialization sequences
            if (!fastybirdApiPropagateDeviceState(FASTYBIRD_STATUS_INIT)) {
                return;
            }
 
            // Collect all device default properties...
            properties.push_back(FASTYBIRD_PROPERTY_INTERVAL);
            properties.push_back(FASTYBIRD_PROPERTY_UPTIME);
            properties.push_back(FASTYBIRD_PROPERTY_FREE_HEAP);
            properties.push_back(FASTYBIRD_PROPERTY_CPU_LOAD);

            #if WIFI_SUPPORT
                properties.push_back(FASTYBIRD_PROPERTY_IP_ADDRESS);
            #endif

            #if ADC_MODE_VALUE == ADC_VCC
                properties.push_back(FASTYBIRD_PROPERTY_VCC);
            #endif

            #if WIFI_SUPPORT
                properties.push_back(FASTYBIRD_PROPERTY_SSID);
                properties.push_back(FASTYBIRD_PROPERTY_RSSI);
            #endif
 
            // ...and pass them to the broker
            if (!fastybirdApiPropagateDevicePropertiesNames(properties)) {
                return;
            }

            // For device with wifi support notify its IP address
            #if WIFI_SUPPORT
                if (!fastybirdApiPropagateDevicePropertyValue(FASTYBIRD_PROPERTY_IP_ADDRESS, getIP().c_str())) {
                    return;
                }
            #endif

            // Heartbeat interval
            if (!fastybirdApiPropagateDevicePropertyValue(FASTYBIRD_PROPERTY_INTERVAL, String(HEARTBEAT_INTERVAL / 1000).c_str())) {
                return;
            }

            _fastybird_device_advertisement_progress = FASTYBIRD_PUB_NAME;
            break;

        case FASTYBIRD_PUB_NAME:
            if (!fastybirdApiPropagateDeviceName(getIdentifier().c_str())) {
                return;
            }

            _fastybird_device_advertisement_progress = FASTYBIRD_PUB_HARDWARE;
            break;

        // Describe device hardware details to cloud broker
        case FASTYBIRD_PUB_HARDWARE:
            #if WIFI_SUPPORT
                if (!fastybirdApiPropagateDeviceHardwareField(FASTYBIRD_HARDWARE_MAC_ADDRESS, WiFi.macAddress().c_str())) {
                    return;
                }
            #endif

            if (!fastybirdApiPropagateDeviceHardwareManufacturer(MANUFACTURER)) {
                return;
            }

            if (!fastybirdApiPropagateDeviceHardwareModelName(DEVICE)) {
                return;
            }

            if (!fastybirdApiPropagateDeviceHardwareVersion(HARWARE_VERSION)) {
                return;
            }

            snprintf_P(serial_no, sizeof(serial_no), PSTR("%08X"), ESP.getChipId());

            if (!fastybirdApiPropagateDeviceHardwareSerialNumber(serial_no)) {
                return;
            }

            _fastybird_device_advertisement_progress = FASTYBIRD_PUB_FIRMWARE;
            break;

        // Describe device firmware details to cloud broker
        case FASTYBIRD_PUB_FIRMWARE:
            if (!fastybirdApiPropagateDeviceFirmwareManufacturer(FIRMWARE_MANUFACTURER)) {
                return;
            }

            if (!fastybirdApiPropagateDeviceFirmwareName(FIRMWARE_MANUFACTURER)) {
                return;
            }

            if (!fastybirdApiPropagateDeviceFirmwareVersion(FIRMWARE_VERSION)) {
                return;
            }

            _fastybird_device_advertisement_progress = FASTYBIRD_PUB_CHANNELS;
            break;

        case FASTYBIRD_PUB_CHANNELS:
            for (uint8_t i = 0; i < FASTYBIRD_MAX_CHANNELS; i++) {
                channels.push_back(_fastybird_channels[i].name);
            }

            if (!fastybirdApiPropagateDeviceChannelsNames(channels)) {
                return;
            }

            _fastybird_device_advertisement_progress = FASTYBIRD_PUB_CONTROL_STRUCTURE;
            break;

        case FASTYBIRD_PUB_CONTROL_STRUCTURE:
            for (uint8_t i = 0; i < _fastybird_on_control_callbacks.size(); i++) {
                controls.push_back(_fastybird_on_control_callbacks[i].controlName);
            }

            if (!fastybirdApiPropagateDeviceControlNames(controls)) {
                return;
            }

            _fastybird_device_advertisement_progress = FASTYBIRD_PUB_CONFIGURATION_SCHEMA;
            break;

        case FASTYBIRD_PUB_CONFIGURATION_SCHEMA:
            for (uint8_t i = 0; i < _fastybird_report_configuration_schema_callbacks.size(); i++) {
                (_fastybird_report_configuration_schema_callbacks[i])(configurationSchema);
            }

            if (
                configurationSchema.size() > 0
                && !fastybirdApiPropagateDeviceConfigurationSchema(configurationSchema)
            ) {
                return;
            }

            #if FASTYBIRD_MAX_CHANNELS > 0
                _fastybird_device_advertisement_progress = FASTYBIRD_PUB_INITIALIZE_CHANNELS;
            #else
                _fastybird_device_advertisement_progress = FASTYBIRD_PUB_READY;
            #endif
            break;

        #if FASTYBIRD_MAX_CHANNELS > 0
            case FASTYBIRD_PUB_INITIALIZE_CHANNELS:
                if (FASTYBIRD_MAX_CHANNELS > 0) {
                    // Take channels pointer
                    uint8_t channelIndex = _fastybird_initialize_channel;

                    // No channels were initialized...
                    if (channelIndex == INDEX_NONE) {
                        // ...pick first from device channels collection
                        channelIndex = 0;

                        // Update channel pointer
                        _fastybird_initialize_channel = channelIndex;
                    }

                    // Check if channel index is in valid range
                    if (channelIndex < FASTYBIRD_MAX_CHANNELS) {
                        // Process channel initialization
                        if (!_fastybirdInitializeChannel(channelIndex)) {
                            return;
                        }

                        // Channel was fully initialized
                        _fastybird_channel_advertisement_progress = FASTYBIRD_PUB_CHANNEL_NAME;

                        // Search for new channel index
                        for (uint8_t i = 0; i < FASTYBIRD_MAX_CHANNELS; i++) {
                            // Search for channel index position in collection...
                            if (i == channelIndex) {
                                // Check if there is left any uninitialized channel...
                                if ((i + 1) < FASTYBIRD_MAX_CHANNELS) {
                                    // ...if yes, update pointer
                                    _fastybird_initialize_channel = (i + 1);

                                    return;
                                }

                                // ...if not, finish channels initialization
                            }
                        }
                    }
                }

                // Reset channel initialize step pointer
                _fastybird_channel_advertisement_progress = FASTYBIRD_PUB_CHANNEL_NAME;

                // Reset channels process pointer
                _fastybird_initialize_channel = INDEX_NONE;

                // Move to the next step
                _fastybird_device_advertisement_progress = FASTYBIRD_PUB_READY;
                break;
        #endif

        case FASTYBIRD_PUB_READY:
            if (!fastybirdApiPropagateDeviceState(FASTYBIRD_STATUS_READY)) {
                return;
            }

            _fastybird_device_advertisement_progress = FASTYBIRD_PUB_CONFIGURATION;
            break;

        case FASTYBIRD_PUB_CONFIGURATION:
            if (!fastybirdReportConfiguration()) {
                return;
            }

            #if FASTYBIRD_MAX_CHANNELS > 0
                _fastybird_device_advertisement_progress = FASTYBIRD_PUB_CHANNELS_CONFIGURATION;
            #else
                _fastybird_device_advertisement_progress = FASTYBIRD_PUB_HEARTBEAT;
            #endif
            break;

        #if FASTYBIRD_MAX_CHANNELS > 0
            case FASTYBIRD_PUB_CHANNELS_CONFIGURATION:
                if (FASTYBIRD_MAX_CHANNELS > 0) {
                    for (uint8_t i = 0; i < FASTYBIRD_MAX_CHANNELS; i++) {
                        if (_fastybird_channels[i].configurationCallbacks.size() > 0) {
                            DynamicJsonBuffer jsonBuffer;

                            JsonObject& configuration = jsonBuffer.createObject();

                            fastybirdCallReportChannelConfiguration(i, configuration);

                            if (
                                configuration.size() > 0
                                && !fastybirdApiPropagateChannelConfiguration(_fastybird_channels[i].name.c_str(), configuration)
                            ) {
                                return;
                            }
                        }
                    }
                }

                _fastybird_device_advertisement_progress = FASTYBIRD_PUB_HEARTBEAT;
                break;
        #endif

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
// MODULE API
// -----------------------------------------------------------------------------

void fastybirdOnConnectRegister(
    fastybird_on_connect_callback_f callback
) {
    _fastybird_on_connect_callbacks.push_back(callback);
}

// -----------------------------------------------------------------------------

void fastybirdOnConfigureRegister(
    fastybird_on_configure_callback_f callback
) {
    _fastybird_on_configure_callbacks.push_back(callback);
}

// -----------------------------------------------------------------------------

void fastybirdOnControlRegister(
    fastybird_control_callback_f callback,
    String control
) {
    _fastybird_on_control_callbacks.push_back((fastybird_on_control_callback_t) {
        callback,
        control
    });
}

// -----------------------------------------------------------------------------

void fastybirdCallOnControlRegister(
    String action,
    const char * payload
) {
    for (uint8_t i = 0; i < _fastybird_on_control_callbacks.size(); i++) {
        if (_fastybird_on_control_callbacks[i].controlName.equals(action)) {
            _fastybird_on_control_callbacks[i].callback(payload);
        }
    }
}

// -----------------------------------------------------------------------------

void fastybirdReportConfigurationSchemaRegister(
    fastybird_on_report_configuration_schema_callback_f callback
) {
    _fastybird_report_configuration_schema_callbacks.push_back(callback);
}

// -----------------------------------------------------------------------------

void fastybirdReportConfigurationRegister(
    fastybird_on_report_configuration_callback_f callback
) {
    _fastybird_report_configuration_callbacks.push_back(callback);
}

// -----------------------------------------------------------------------------

String fastybirdDeviceIdentifier()
{
    char buffer[20];

    snprintf_P(buffer, sizeof(buffer), PSTR("%08X"), ESP.getChipId());

    String serial_no = String(buffer);

    serial_no.toLowerCase();
    
    return serial_no;
}

// -----------------------------------------------------------------------------

void fastybirdResetDeviceInitialization()
{
    _fastybird_initialized = false;
    _fastybird_device_advertisement_progress = FASTYBIRD_PUB_CONNECTION;
    _fastybird_device_property_advertisement_progress = FASTYBIRD_PUB_PROPERTY_NAME;
    _fastybird_channel_advertisement_progress = FASTYBIRD_PUB_CHANNEL_NAME;
    _fastybird_channel_property_advertisement_progress = FASTYBIRD_PUB_PROPERTY_NAME;

    #if WEB_SUPPORT && WS_SUPPORT
        wsSend(_fastybirdWSOnUpdate);
    #endif
    
    fastybirdApiReset();
}

// -----------------------------------------------------------------------------

bool fastybirdIsDeviceInitialzed()
{
    return _fastybird_initialized;
}

// -----------------------------------------------------------------------------

bool fastybirdReportConfiguration()
{
    DynamicJsonBuffer jsonBuffer;

    JsonObject& configuration = jsonBuffer.createObject();

    for (uint8_t i = 0; i < _fastybird_report_configuration_callbacks.size(); i++) {
        (_fastybird_report_configuration_callbacks[i])(configuration);
    }

    if (configuration.size() > 0) {
        return fastybirdApiPropagateDeviceConfiguration(configuration);
    }

    return true;
}

// -----------------------------------------------------------------------------
// MODULE API - CHANNEL
// -----------------------------------------------------------------------------

bool fastybirdReportChannelConfigurationSchemaRegister(
    uint8_t channelIndex,
    fastybird_on_report_channel_configuration_schema_callback_f callback
) {
    // Check channel
    if (channelIndex >= FASTYBIRD_MAX_CHANNELS) {
        return false;
    }

    _fastybird_report_channel_configuration_schema_callbacks.push_back(callback);

    uint8_t schemaCallbackIndex = (_fastybird_report_channel_configuration_schema_callbacks.size() - 1);

    _fastybird_channels[channelIndex].configurationSchemaCallbacks.push_back(schemaCallbackIndex);

    return true;
}

// -----------------------------------------------------------------------------

void fastybirdCalldReportChannelConfigurationSchema(
    const uint8_t channelIndex,
    JsonArray& schema
) {
    for (uint8_t i = 0; i < _fastybird_channels[channelIndex].configurationSchemaCallbacks.size(); i++) {
        if (i < _fastybird_report_channel_configuration_schema_callbacks.size()) {
            _fastybird_report_channel_configuration_schema_callbacks[i](channelIndex, schema);
        }
    }
}

// -----------------------------------------------------------------------------

bool fastybirdReportChannelConfigurationRegister(
    const uint8_t channelIndex,
    fastybird_on_report_channel_configuration_callback_f callback
) {
    // Check channel
    if (channelIndex >= FASTYBIRD_MAX_CHANNELS) {
        return false;
    }

    _fastybird_report_channel_configuration_callbacks.push_back(callback);

    uint8_t configurationIndex = (_fastybird_report_channel_configuration_callbacks.size() - 1);

    _fastybird_channels[channelIndex].configurationCallbacks.push_back(configurationIndex);

    return true;
}

// -----------------------------------------------------------------------------

void fastybirdCallReportChannelConfiguration(
    const uint8_t channelIndex,
    JsonObject& configuration
) {
    for (uint8_t i = 0; i < _fastybird_channels[channelIndex].configurationCallbacks.size(); i++) {
        if (i < _fastybird_report_channel_configuration_callbacks.size()) {
            _fastybird_report_channel_configuration_callbacks[i](channelIndex, configuration);
        }
    }
}

// -----------------------------------------------------------------------------

bool fastybirdOnChannelConfigureRegister(
    const uint8_t channelIndex,
    fastybird_on_channel_configure_callback_f callback
) {
    // Check channel
    if (channelIndex >= FASTYBIRD_MAX_CHANNELS) {
        return false;
    }

    _fastybird_on_channel_configure_callbacks.push_back(callback);

    uint8_t configurationIndex = (_fastybird_on_channel_configure_callbacks.size() - 1);

    _fastybird_channels[channelIndex].configureCallbacks.push_back(configurationIndex);

    return true;
}

// -----------------------------------------------------------------------------

void fastybirdCallOnChannelConfigure(
    const uint8_t channelIndex,
    JsonObject& data
) {
    for (uint8_t i = 0; i < _fastybird_channels[channelIndex].configureCallbacks.size(); i++) {
        if (i < _fastybird_on_channel_configure_callbacks.size()) {
            _fastybird_on_channel_configure_callbacks[i](channelIndex, data);
        }
    }
}

// -----------------------------------------------------------------------------

uint8_t fastybirdFindChannelIndex(
    String name
) {
    for (uint8_t i = 0; i < FASTYBIRD_MAX_CHANNELS; i++) {
        if (_fastybird_channels[i].name.equals(name)) {
            return i;
        }
    }

    return INDEX_NONE;
}

// -----------------------------------------------------------------------------

fastybird_channel_t fastybirdGetChannel(
    const uint8_t channelIndex
) {
    return _fastybird_channels[channelIndex];
}

// -----------------------------------------------------------------------------
// CHANNELS PROPERTIES API
// -----------------------------------------------------------------------------

uint8_t fastybirdRegisterProperty(
    const char name[],
    const char dataType[],
    const char units[],
    const char format[]
) {
    fastybird_property_t property = {
        String(name),
        false,
        false,
        String(dataType),
        String(units),
        String(format)
    };

    _fastybird_properties.push_back(property);

    return (_fastybird_properties.size() - 1);
}

// -----------------------------------------------------------------------------

uint8_t fastybirdRegisterProperty(
    const char name[],
    const char dataType[],
    const char units[],
    const char format[],
    fastybird_properties_process_payload_f payloadCallback
) {
    uint8_t propertyIndex = fastybirdRegisterProperty(
        name,
        dataType,
        units,
        format
    );

    _fastybird_properties[propertyIndex].settable = true;
    _fastybird_properties[propertyIndex].payload_callback = payloadCallback;

    return propertyIndex;
}

// -----------------------------------------------------------------------------

uint8_t fastybirdRegisterProperty(
    const char name[],
    const char dataType[],
    const char units[],
    const char format[],
    fastybird_properties_process_query_f queryCallback
) {
    uint8_t propertyIndex = fastybirdRegisterProperty(
        name,
        dataType,
        units,
        format
    );

    _fastybird_properties[propertyIndex].queryable = true;
    _fastybird_properties[propertyIndex].query_callback = queryCallback;

    return propertyIndex;
}

// -----------------------------------------------------------------------------

uint8_t fastybirdRegisterProperty(
    const char name[],
    const char dataType[],
    const char units[],
    const char format[],
    fastybird_properties_process_payload_f payloadCallback,
    fastybird_properties_process_query_f queryCallback
) {
    uint8_t propertyIndex = fastybirdRegisterProperty(
        name,
        dataType,
        units,
        format
    );

    _fastybird_properties[propertyIndex].settable = true;
    _fastybird_properties[propertyIndex].payload_callback = payloadCallback;

    _fastybird_properties[propertyIndex].queryable = true;
    _fastybird_properties[propertyIndex].query_callback = queryCallback;

    return propertyIndex;
}

// -----------------------------------------------------------------------------

uint8_t fastybirdFindChannelPropertyIndex(
    const uint8_t channelIndex,
    String name
) {
    if (channelIndex >= FASTYBIRD_MAX_CHANNELS) {
        return INDEX_NONE;
    }

    fastybird_channel_t channel = _fastybird_channels[channelIndex];

    for (uint8_t i = 0; i < channel.properties.size(); i++) {
        if (_fastybird_properties[channel.properties[i]].name.equals(name)) {
            return channel.properties[i];
        }
    }

    return INDEX_NONE;
}

// -----------------------------------------------------------------------------

bool fastybirdMapPropertyToChannel(
    const uint8_t channelIndex,
    const uint8_t propertyIndex
) {
    if (
        channelIndex >= FASTYBIRD_MAX_CHANNELS
        || propertyIndex >= _fastybird_properties.size()
    ) {
        return false;
    }

    for (uint8_t i = 0; i < _fastybird_channels[channelIndex].properties.size(); i++) {
        // Check if property is not mapped with channel
        if (propertyIndex == _fastybird_channels[channelIndex].properties[i]) {
            return true;
        }
    }

    DEBUG_MSG(
        PSTR("[INFO][FASTYBIRD] Map property: %s to channel: %s\n"),
        _fastybird_properties[propertyIndex].name.c_str(),
        _fastybird_channels[channelIndex].name.c_str()
    );

    _fastybird_channels[channelIndex].properties.push_back(propertyIndex);

    return true;
}

// -----------------------------------------------------------------------------

fastybird_property_t fastybirdGetProperty(
    const uint8_t propertyIndex
) {
    return _fastybird_properties[propertyIndex];
}

// -----------------------------------------------------------------------------

bool fastybirdReportChannelPropertyValue(
    const uint8_t channelIndex,
    const uint8_t propertyIndex,
    const char * payload
) {
    // Check channel
    if (
        channelIndex >= FASTYBIRD_MAX_CHANNELS
        || propertyIndex >= _fastybird_properties.size()
    ) {
        return false;
    }

    fastybird_channel_t channel = _fastybird_channels[channelIndex];
    fastybird_property_t property = _fastybird_properties[propertyIndex];

    return fastybirdApiPropagateChannelPropertyValue(
        channel.name.c_str(),
        property.name.c_str(),
        payload
    );
}

// -----------------------------------------------------------------------------
// MODULE CORE
// -----------------------------------------------------------------------------

void fastybirdSetup()
{
    #if FASTYBIRD_MAX_CHANNELS > 0
        // Initialize all configured channels
        _fastybirdInitializeChannels();
    #endif

    // Initialize API setup
    fastybirdApiSetup();

    #if WEB_SUPPORT && WS_SUPPORT
        wsOnConnectRegister(_fastybirdWSOnConnect);
        wsOnUpdateRegister(_fastybirdWSOnUpdate);
    #endif

    systemOnHeartbeatRegister(fastybirdApiOnHeartbeat);
    
    fastybirdOnControlRegister(
        [](const char * payload) {
            DynamicJsonBuffer jsonBuffer;

            // Parse payload
            JsonObject& root = jsonBuffer.parseObject(payload);

            if (root.success()) {
                DEBUG_MSG(PSTR("[INFO][FASTYBIRD] Sending configuration to modules\n"));

                for (uint8_t i = 0; i < _fastybird_on_configure_callbacks.size(); i++) {
                    (_fastybird_on_configure_callbacks[i])(root);
                }

                DEBUG_MSG(PSTR("[INFO][FASTYBIRD] Changes were saved\n"));

                #if WEB_SUPPORT && WS_SUPPORT
                    wsReportConfiguration();
                #endif

                // Report back updated configuration
                fastybirdReportConfiguration();
                
            } else {
                DEBUG_MSG(PSTR("[INFO][FASTYBIRD] Received payload is not in valid JSON format\n"));
            }
        },
        FASTYBIRD_DEVICE_CONTROL_CONFIGURE
    );

    fastybirdOnControlRegister(
        [](const char * payload) {
            DEBUG_MSG(PSTR("[INFO][FASTYBIRD] Requested reset action\n"));

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

void fastybirdLoop()
{
    if (fastybirdApiIsReady()) {
        if (_fastybird_initialized == false) {
            static uint32_t last_init_call = 0;

            if (last_init_call == 0 || (millis() - last_init_call > 500)) {
                last_init_call = millis();

                _fastybirdInitializeDevice();
            }
        }
    }
}

#endif // FASTYBIRD_SUPPORT
