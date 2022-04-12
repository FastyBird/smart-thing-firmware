/*

FASTYBIRD MODULE

Copyright (C) 2018 FastyBird s.r.o. <code@fastybird.com>

*/

#if FASTYBIRD_SUPPORT

// Initilalization pointers
uint8_t _fastybird_initialize_property = INDEX_NONE;
uint8_t _fastybird_initialize_channel = INDEX_NONE;
uint8_t _fastybird_initialize_channel_property = INDEX_NONE;

// Initialization step pointers
uint8_t _fastybird_device_advertisement_progress = FASTYBIRD_PUB_DEVICE_INITIALIZE;
uint8_t _fastybird_device_property_advertisement_progress = FASTYBIRD_PUB_PROPERTY_NAME;
uint8_t _fastybird_channel_advertisement_progress = FASTYBIRD_PUB_CHANNEL_NAME;
uint8_t _fastybird_channel_property_advertisement_progress = FASTYBIRD_PUB_PROPERTY_NAME;

// Registered devices
std::vector<fastybird_device_t> _fastybird_devices;
// Registered channels
std::vector<fastybird_channel_t> _fastybird_channels;
// Registered properties
std::vector<fastybird_property_t> _fastybird_properties;
// Registered controls
std::vector<fastybird_control_t> _fastybird_controls;

// Device callbacks - each module could register own callback
std::vector<fastybird_on_connect_callback_t> _fastybird_on_connect_callbacks;

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------

fastybird_device_t fastybirdGetDevice(
    const uint8_t deviceIndex
) {
    return _fastybird_devices[deviceIndex];
}

// -----------------------------------------------------------------------------

uint8_t fastybirdFindDeviceIndex(
    const char * name
) {
    for (uint8_t i = 0; i < _fastybird_devices.size(); i++) {
        if (strcmp(_fastybird_devices[i].name, name) == 0) {
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

uint8_t fastybirdFindChannelIndex(
    const uint8_t deviceIndex,
    const char * name
) {
    if (deviceIndex >= _fastybird_devices.size()) {
        return INDEX_NONE;
    }

    fastybird_device_t device = fastybirdGetDevice(deviceIndex);

    for (uint8_t i = 0; i < device.channels.size(); i++) {
        uint8_t channelIndex = device.channels[i];

        if (
            channelIndex < _fastybird_channels.size()
            && strcmp(_fastybird_channels[channelIndex].name, name) == 0
        ) {
            return channelIndex;
        }
    }

    return INDEX_NONE;
}

// -----------------------------------------------------------------------------

fastybird_property_t fastybirdGetProperty(
    const uint8_t propertyIndex
) {
    return _fastybird_properties[propertyIndex];
}

// -----------------------------------------------------------------------------

uint8_t fastybirdFindDevicePropertyIndex(
    const uint8_t deviceIndex,
    const char * name
) {
    if (deviceIndex >= _fastybird_devices.size()) {
        return INDEX_NONE;
    }

    fastybird_device_t device = fastybirdGetDevice(deviceIndex);

    for (uint8_t i = 0; i < device.properties.size(); i++) {
        uint8_t propertyIndex = device.properties[i];

        if (
            propertyIndex < _fastybird_properties.size()
            && strcmp(_fastybird_properties[propertyIndex].name, name) == 0
        ) {
            return propertyIndex;
        }
    }

    return INDEX_NONE;
}

// -----------------------------------------------------------------------------

uint8_t fastybirdFindChannelPropertyIndex(
    const uint8_t deviceIndex,
    const uint8_t channelIndex,
    const char * name
) {
    if (deviceIndex >= _fastybird_devices.size()) {
        return INDEX_NONE;
    }

    fastybird_device_t device = fastybirdGetDevice(deviceIndex);

    if (channelIndex >= device.channels.size()) {
        return INDEX_NONE;
    }

    fastybird_channel_t channel = fastybirdGetChannel(channelIndex);

    for (uint8_t i = 0; i < channel.properties.size(); i++) {
        uint8_t propertyIndex = channel.properties[i];

        if (
            propertyIndex < _fastybird_properties.size()
            && strcmp(_fastybird_properties[propertyIndex].name, name) == 0
        ) {
            return propertyIndex;
        }
    }

    return INDEX_NONE;
}

// -----------------------------------------------------------------------------

fastybird_control_t fastybirdGetControl(
    const uint8_t controlIndex
) {
    return _fastybird_controls[controlIndex];
}

// -----------------------------------------------------------------------------

uint8_t fastybirdFindDeviceControlIndex(
    const uint8_t deviceIndex,
    const char * name
) {
    if (deviceIndex >= _fastybird_devices.size()) {
        return INDEX_NONE;
    }

    fastybird_device_t device = fastybirdGetDevice(deviceIndex);

    for (uint8_t i = 0; i < device.controls.size(); i++) {
        uint8_t controlIndex = device.controls[i];

        if (
            controlIndex < _fastybird_controls.size()
            && strcmp(_fastybird_controls[controlIndex].name, name) == 0
        ) {
            return controlIndex;
        }
    }

    return INDEX_NONE;
}

// -----------------------------------------------------------------------------

uint8_t fastybirdFindChannelControlIndex(
    const uint8_t deviceIndex,
    const uint8_t channelIndex,
    const char * name
) {
    if (deviceIndex >= _fastybird_devices.size()) {
        return INDEX_NONE;
    }

    fastybird_device_t device = fastybirdGetDevice(deviceIndex);

    if (channelIndex >= device.channels.size()) {
        return INDEX_NONE;
    }

    fastybird_channel_t channel = fastybirdGetChannel(channelIndex);

    for (uint8_t i = 0; i < channel.controls.size(); i++) {
        uint8_t controlIndex = channel.controls[i];

        if (
            controlIndex < _fastybird_controls.size()
            && strcmp(_fastybird_controls[controlIndex].name, name) == 0
        ) {
            return controlIndex;
        }
    }

    return INDEX_NONE;
}

// -----------------------------------------------------------------------------
// MODULE PRIVATE
// -----------------------------------------------------------------------------

void _fastybirdInitialize()
{
    char buff[20];

    snprintf_P(buff, sizeof(buff), PSTR("%08X"), ESP.getChipId());

    String serial_no = String(buff);

    serial_no.toLowerCase();

    _fastybird_devices.push_back({
        strdup(serial_no.c_str()),
        HARDWARE_MODEL,
        HARDWARE_MANUFACTURER,
        HARDWARE_VERSION,
        FIRMWARE_MANUFACTURER,
        FIRMWARE_VERSION,
        false
    });

    uint8_t propertyIndex;

    // Create device property structure
    propertyIndex = fastybirdRegisterProperty(
        FASTYBIRD_PROPERTY_INTERVAL,
        FASTYBIRD_PROPERTY_DATA_TYPE_INTEGER,
        "",
        ""
    );

    // Register property to device
    fastybirdMapPropertyToDevice(
        FASTYBIRD_MAIN_DEVICE_INDEX,
        propertyIndex
    );

    // Create device property structure
    propertyIndex = fastybirdRegisterProperty(
        FASTYBIRD_PROPERTY_UPTIME,
        FASTYBIRD_PROPERTY_DATA_TYPE_INTEGER,
        "",
        ""
    );

    // Register property to device
    fastybirdMapPropertyToDevice(
        FASTYBIRD_MAIN_DEVICE_INDEX,
        propertyIndex
    );

    // Create device property structure
    propertyIndex = fastybirdRegisterProperty(
        FASTYBIRD_PROPERTY_FREE_HEAP,
        FASTYBIRD_PROPERTY_DATA_TYPE_INTEGER,
        "",
        ""
    );

    // Register property to device
    fastybirdMapPropertyToDevice(
        FASTYBIRD_MAIN_DEVICE_INDEX,
        propertyIndex
    );

    // Create device property structure
    propertyIndex = fastybirdRegisterProperty(
        FASTYBIRD_PROPERTY_CPU_LOAD,
        FASTYBIRD_PROPERTY_DATA_TYPE_INTEGER,
        "",
        ""
    );

    // Register property to device
    fastybirdMapPropertyToDevice(
        FASTYBIRD_MAIN_DEVICE_INDEX,
        propertyIndex
    );

    #if WIFI_SUPPORT
        // Create device property structure
        propertyIndex = fastybirdRegisterProperty(
            FASTYBIRD_PROPERTY_IP_ADDRESS,
            FASTYBIRD_PROPERTY_DATA_TYPE_STRING,
            "",
            ""
        );

        // Register property to device
        fastybirdMapPropertyToDevice(
            FASTYBIRD_MAIN_DEVICE_INDEX,
            propertyIndex
        );

        // Create device property structure
        propertyIndex = fastybirdRegisterProperty(
            FASTYBIRD_PROPERTY_SSID,
            FASTYBIRD_PROPERTY_DATA_TYPE_STRING,
            "",
            ""
        );

        // Register property to device
        fastybirdMapPropertyToDevice(
            FASTYBIRD_MAIN_DEVICE_INDEX,
            propertyIndex
        );

        // Create device property structure
        propertyIndex = fastybirdRegisterProperty(
            FASTYBIRD_PROPERTY_RSSI,
            FASTYBIRD_PROPERTY_DATA_TYPE_INTEGER,
            "",
            ""
        );

        // Register property to device
        fastybirdMapPropertyToDevice(
            FASTYBIRD_MAIN_DEVICE_INDEX,
            propertyIndex
        );
    #endif

    #if ADC_MODE_VALUE == ADC_VCC
        // Create device property structure
        propertyIndex = fastybirdRegisterProperty(
            FASTYBIRD_PROPERTY_VCC,
            FASTYBIRD_PROPERTY_DATA_TYPE_INTEGER,
            "",
            ""
        );

        // Register property to device
        fastybirdMapPropertyToDevice(
            FASTYBIRD_MAIN_DEVICE_INDEX,
            propertyIndex
        );
    #endif

    char channelName[50];

    for (uint8_t i = 0; i < FASTYBIRD_MAX_CHANNELS; i++) {
        strcpy(channelName, FASTYBIRD_CHANNEL_DEFAULT);
        strcat(channelName, "-");
        itoa((i + 1), channelName + strlen(channelName), 10);

        fastybirdRegisterChannel(channelName);

        fastybirdMapChannelToDevice(FASTYBIRD_MAIN_DEVICE_INDEX, (_fastybird_channels.size() - 1));
    }
}

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

void _fastybirdSystemOnHeartbeat()
{
    fastybird_device_t device = fastybirdGetDevice(FASTYBIRD_MAIN_DEVICE_INDEX);

    char buf[20];

    fastybirdApiReportDevicePropertyValue(
        device.name,
        FASTYBIRD_PROPERTY_INTERVAL,
        itoa(HEARTBEAT_INTERVAL / 1000, buf, 10)
    );

    fastybirdApiReportDevicePropertyValue(
        device.name,
        FASTYBIRD_PROPERTY_FREE_HEAP,
        itoa(getFreeHeap(), buf, 10)
    );

    fastybirdApiReportDevicePropertyValue(
        device.name,
        FASTYBIRD_PROPERTY_UPTIME,
        itoa(getUptime(), buf, 10)
    );

    #if WIFI_SUPPORT
        fastybirdApiReportDevicePropertyValue(
            device.name,
            FASTYBIRD_PROPERTY_IP_ADDRESS,
            getIP().c_str()
        );

        fastybirdApiReportDevicePropertyValue(
            device.name,
            FASTYBIRD_PROPERTY_RSSI,
            itoa(WiFi.RSSI(), buf, 10)
        );

        fastybirdApiReportDevicePropertyValue(
            device.name,
            FASTYBIRD_PROPERTY_SSID,
            getNetwork().c_str()
        );
    #endif

    fastybirdApiReportDevicePropertyValue(
        device.name,
        FASTYBIRD_PROPERTY_CPU_LOAD,
        itoa(systemLoadAverage(), buf, 10)
    );

    #if ADC_MODE_VALUE == ADC_VCC
        fastybirdApiReportDevicePropertyValue(
            device.name,
            FASTYBIRD_PROPERTY_VCC,
            itoa(ESP.getVcc(), buf, 10)
        );
    #endif
}

// -----------------------------------------------------------------------------

/**
 * Initialize given device channel property
 */
bool _fastybirdInitializeChannelProperty(
    const uint8_t deviceIndex,
    const uint8_t channelIndex,
    const uint8_t propertyIndex
) {
    fastybird_device_t device = fastybirdGetDevice(deviceIndex);
    fastybird_channel_t channel = fastybirdGetChannel(channelIndex);
    fastybird_property_t property = fastybirdGetProperty(propertyIndex);

    switch (_fastybird_channel_property_advertisement_progress)
    {
        case FASTYBIRD_PUB_PROPERTY_NAME:
            if (
                !fastybirdApiPropagateChannelPropertyName(
                    device.name,
                    channel.name,
                    property.name,
                    property.name
                )
            ) {
                return false;
            }

            _fastybird_channel_property_advertisement_progress = FASTYBIRD_PUB_PROPERTY_SETABLE;
            break;

        case FASTYBIRD_PUB_PROPERTY_SETABLE:
            if (
                !fastybirdApiPropagateChannelPropertySettable(
                    device.name,
                    channel.name,
                    property.name,
                    property.settable
                )
            ) {
                return false;
            }

            _fastybird_channel_property_advertisement_progress = FASTYBIRD_PUB_PROPERTY_QUERYABLE;
            break;

        case FASTYBIRD_PUB_PROPERTY_QUERYABLE:
            if (
                !fastybirdApiPropagateChannelPropertyQueryable(
                    device.name,
                    channel.name,
                    property.name,
                    property.queryable
                )
            ) {
                return false;
            }

            _fastybird_channel_property_advertisement_progress = FASTYBIRD_PUB_PROPERTY_DATA_TYPE;
            break;

        case FASTYBIRD_PUB_PROPERTY_DATA_TYPE:
            if (
                !fastybirdApiPropagateChannelPropertyDataType(
                    device.name,
                    channel.name,
                    property.name,
                    property.dataType
                )
            ) {
                return false;
            }

            _fastybird_channel_property_advertisement_progress = FASTYBIRD_PUB_PROPERTY_UNIT;
            break;

        case FASTYBIRD_PUB_PROPERTY_UNIT:
            if (
                !fastybirdApiPropagateChannelPropertyUnit(
                    device.name,
                    channel.name,
                    property.name,
                    property.unit
                )
            ) {
                return false;
            }

            _fastybird_channel_property_advertisement_progress = FASTYBIRD_PUB_PROPERTY_FORMAT;
            break;

        case FASTYBIRD_PUB_PROPERTY_FORMAT:
            if (
                !fastybirdApiPropagateChannelPropertyFormat(
                    device.name,
                    channel.name,
                    property.name,
                    property.format
                )
            ) {
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
    const uint8_t deviceIndex,
    const uint8_t channelIndex
) {
    fastybird_device_t device = fastybirdGetDevice(deviceIndex);
    fastybird_channel_t channel = fastybirdGetChannel(channelIndex);

    std::vector<String> properties;
    std::vector<String> controls;

    switch (_fastybird_channel_advertisement_progress)
    {
            
// -----------------------------------------------------------------------------
// CHANNEL BASIC INFO
// -----------------------------------------------------------------------------

        case FASTYBIRD_PUB_CHANNEL_NAME:
            if (
                !fastybirdApiPropagateChannelName(
                    device.name,
                    channel.name,
                    channel.name
                )
            ) {
                return false;
            }
            
            _fastybird_channel_advertisement_progress = FASTYBIRD_PUB_CHANNEL_PROPERTIES;
            break;

        case FASTYBIRD_PUB_CHANNEL_PROPERTIES:
            for (uint8_t i = 0; i < channel.properties.size(); i++) {
                if (channel.properties[i] < _fastybird_properties.size()) {
                    properties.push_back(_fastybird_properties[channel.properties[i]].name);
                }
            }

            if (
                !fastybirdApiPropagateChannelPropertiesNames(
                    device.name,
                    channel.name,
                    properties
                )
            ) {
                return false;
            }

            _fastybird_channel_advertisement_progress = FASTYBIRD_PUB_CHANNEL_CONTROLS;
            break;

        case FASTYBIRD_PUB_CHANNEL_CONTROLS:
            for (uint8_t i = 0; i < channel.controls.size(); i++) {
                if (channel.controls[i] < _fastybird_controls.size()) {
                    controls.push_back(_fastybird_controls[channel.controls[i]].name);
                }
            }

            if (
                !fastybirdApiPropagateChannelControlsNames(
                    device.name,
                    channel.name,
                    controls
                )
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
                    if (!_fastybirdInitializeChannelProperty(deviceIndex, channelIndex, propertyIndex)) {
                        return false;
                    }

                    // Property was fully initialized
                    _fastybird_channel_property_advertisement_progress = FASTYBIRD_PUB_PROPERTY_NAME;

                    // Search for new property index
                    for (uint8_t i = 0; i < channel.properties.size(); i++) {
                        // Search for property index position in collection...
                        if (channel.properties[i] == propertyIndex) {
                            // Check if there is left any uninitialized property...
                            if ((uint8_t) (i + 1) < channel.properties.size()) {
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

// -----------------------------------------------------------------------------

/**
 * Initilialize device to broker
 */
void _fastybirdInitializeDevice(
    const uint8_t deviceIndex
) {
    fastybird_device_t device = fastybirdGetDevice(deviceIndex);

    std::vector<String> channels;
    std::vector<String> properties;
    std::vector<String> controls;

    switch (_fastybird_device_advertisement_progress)
    {
        case FASTYBIRD_PUB_DEVICE_INITIALIZE:
            // Notify broker that device is sending initialization sequences
            if (
                !fastybirdApiReportDeviceState(
                    device.name,
                    FASTYBIRD_STATUS_INIT
                )
            ) {
                return;
            }

            _fastybird_device_advertisement_progress = FASTYBIRD_PUB_DEVICE_NAME;
            break;

        case FASTYBIRD_PUB_DEVICE_NAME:
            if (
                !fastybirdApiPropagateDeviceName(
                    device.name,
                    getIdentifier().c_str()
                )
            ) {
                return;
            }

            _fastybird_device_advertisement_progress = FASTYBIRD_PUB_DEVICE_HARDWARE;
            break;

        case FASTYBIRD_PUB_DEVICE_HARDWARE:
            if (
                !fastybirdApiPropagateDeviceHardwareField(
                    device.name,
                    FASTYBIRD_HARDWARE_MANUFACTURER,
                    device.hardware_manufacturer
                )
            ) {
                return;
            }

            if (
                !fastybirdApiPropagateDeviceHardwareField(
                    device.name,
                    FASTYBIRD_HARDWARE_MODEL,
                    device.hardware_model
                )
            ) {
                return;
            }

            if (
                !fastybirdApiPropagateDeviceHardwareField(
                    device.name,
                    FASTYBIRD_HARDWARE_VERSION,
                    device.hardware_version
                )
            ) {
                return;
            }

            #if WIFI_SUPPORT
                if (
                    !fastybirdApiPropagateDeviceHardwareField(
                        device.name,
                        FASTYBIRD_HARDWARE_MAC_ADDRESS,
                        WiFi.macAddress().c_str()
                    )
                ) {
                    return;
                }
            #endif

            _fastybird_device_advertisement_progress = FASTYBIRD_PUB_DEVICE_FIRMWARE;
            break;

        case FASTYBIRD_PUB_DEVICE_FIRMWARE:
            if (
                !fastybirdApiPropagateDeviceFirmwareField(
                    device.name,
                    FASTYBIRD_FIRMWARE_MANUFACTURER,
                    device.firmware_manufacturer
                )
            ) {
                return;
            }

            if (
                !fastybirdApiPropagateDeviceFirmwareField(
                    device.name,
                    FASTYBIRD_FIRMWARE_VERSION,
                    device.firmware_version
                )
            ) {
                return;
            }

            _fastybird_device_advertisement_progress = FASTYBIRD_PUB_DEVICE_PROPERTIES;
            break;

        case FASTYBIRD_PUB_DEVICE_PROPERTIES:
            for (uint8_t i = 0; i < device.properties.size(); i++) {
                if (device.properties[i] < _fastybird_properties.size()) {
                    properties.push_back(_fastybird_properties[device.properties[i]].name);
                }
            }

            if (
                !fastybirdApiPropagateDevicePropertiesNames(
                    device.name,
                    properties
                )
            ) {
                return;
            }

            _fastybird_device_advertisement_progress = FASTYBIRD_PUB_DEVICE_CONTROLS;
            break;

        case FASTYBIRD_PUB_DEVICE_CONTROLS:
            for (uint8_t i = 0; i < device.controls.size(); i++) {
                if (device.controls[i] < _fastybird_controls.size()) {
                    controls.push_back(_fastybird_controls[device.controls[i]].name);
                }
            }

            if (
                !fastybirdApiPropagateDeviceControlsNames(
                    device.name,
                    controls
                )
            ) {
                return;
            }

            _fastybird_device_advertisement_progress = FASTYBIRD_PUB_DEVICE_CHANNELS;
            break;

        case FASTYBIRD_PUB_DEVICE_CHANNELS:
            for (uint8_t i = 0; i < device.channels.size(); i++) {
                if (device.channels[i] < _fastybird_channels.size()) {
                    channels.push_back(_fastybird_channels[device.channels[i]].name);
                }
            }

            if (
                !fastybirdApiPropagateDeviceChannelsNames(
                    device.name,
                    channels
                )
            ) {
                return;
            }

            if (device.properties.size() > 0) {
                _fastybird_device_advertisement_progress = FASTYBIRD_PUB_DEVICE_PROPERTY;
            } else if (device.channels.size() > 0) {
                _fastybird_device_advertisement_progress = FASTYBIRD_PUB_DEVICE_CHANNEL;
            } else {
                _fastybird_device_advertisement_progress = FASTYBIRD_PUB_DEVICE_READY;
            }
            break;

        case FASTYBIRD_PUB_DEVICE_PROPERTY:
            if (device.channels.size() > 0) {
                _fastybird_device_advertisement_progress = FASTYBIRD_PUB_DEVICE_CHANNEL;
            } else {
                _fastybird_device_advertisement_progress = FASTYBIRD_PUB_DEVICE_READY;
            }
            break;

        case FASTYBIRD_PUB_DEVICE_CHANNEL:
            if (device.channels.size() > 0) {
                // Take channels pointer
                uint8_t channelIndex = _fastybird_initialize_channel;

                // No channels were initialized...
                if (channelIndex == INDEX_NONE) {
                    // ...pick first from device channels collection
                    channelIndex = device.channels[0];

                    // Update channel pointer
                    _fastybird_initialize_channel = channelIndex;
                }

                // Check if channel index is in valid range
                if (channelIndex < _fastybird_channels.size()) {
                    // Process channel initialization
                    if (!_fastybirdInitializeChannel(deviceIndex, channelIndex)) {
                        return;
                    }

                    // Channel was fully initialized
                    _fastybird_channel_advertisement_progress = FASTYBIRD_PUB_CHANNEL_NAME;

                    // Search for new channel index
                    for (uint8_t i = 0; i < _fastybird_channels.size(); i++) {
                        // Search for channel index position in collection...
                        if (device.channels[i] == channelIndex) {
                            // Check if there is left any uninitialized channel...
                            if ((uint8_t) (i + 1) < _fastybird_channels.size()) {
                                // ...if yes, update pointer
                                _fastybird_initialize_channel = device.channels[i + 1];

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
            _fastybird_device_advertisement_progress = FASTYBIRD_PUB_DEVICE_READY;
            break;

        case FASTYBIRD_PUB_DEVICE_READY:
            if (
                !fastybirdApiReportDeviceState(
                    device.name,
                    FASTYBIRD_STATUS_READY
                )
            ) {
                return;
            }

            _fastybird_devices[deviceIndex].initialized = true;

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
    fastybird_on_connect_callback_t callback
) {
    _fastybird_on_connect_callbacks.push_back(callback);
}

// -----------------------------------------------------------------------------

void fastybirdOnControlRegister(
    fastybird_controls_process_call_t callback,
    const char * control
) {
    _fastybird_controls.push_back({
        strdup(control),
        callback,
    });

    uint8_t controlIndex = (_fastybird_controls.size() - 1);

    _fastybird_devices[FASTYBIRD_MAIN_DEVICE_INDEX].controls.push_back(controlIndex);
}

// -----------------------------------------------------------------------------

void fastybirdResetDeviceInitialization()
{
    for (uint8_t i = 0; i < _fastybird_devices.size(); i++) {
        _fastybird_devices[i].initialized = false;
    }

    _fastybird_device_advertisement_progress = FASTYBIRD_PUB_DEVICE_INITIALIZE;
    _fastybird_device_property_advertisement_progress = FASTYBIRD_PUB_PROPERTY_NAME;
    _fastybird_channel_advertisement_progress = FASTYBIRD_PUB_CHANNEL_NAME;
    _fastybird_channel_property_advertisement_progress = FASTYBIRD_PUB_PROPERTY_NAME;

    #if WEB_SUPPORT && WS_SUPPORT
        wsSend(_fastybirdWSOnUpdate);
    #endif
}

// -----------------------------------------------------------------------------

bool fastybirdIsDeviceInitialzed(
    const uint8_t deviceIndex
) {
    if (deviceIndex >= _fastybird_devices.size()) {
        return false;
    }

    return _fastybird_devices[deviceIndex].initialized;
}

// -----------------------------------------------------------------------------

bool fastybirdIsDeviceInitialzed()
{
    return fastybirdIsDeviceInitialzed(FASTYBIRD_MAIN_DEVICE_INDEX);
}

// -----------------------------------------------------------------------------
// MODULE API - CHANNELS
// -----------------------------------------------------------------------------

uint8_t fastybirdRegisterChannel(
    const char * name
) {
    fastybird_channel_t channel = {
        strdup(name)
    };

    _fastybird_channels.push_back(channel);

    return (_fastybird_channels.size() - 1);
}

// -----------------------------------------------------------------------------

bool fastybirdMapChannelToDevice(
    const uint8_t deviceIndex,
    const uint8_t channelIndex
) {
    // Basic indexes validation
    if (
        deviceIndex >= _fastybird_devices.size()
        || channelIndex >= _fastybird_channels.size()
    ) {
        return false;
    }

    fastybird_device_t device = fastybirdGetDevice(deviceIndex);

    // Check if channel is mapped to device or not
    for (uint8_t i = 0; i < device.channels.size(); i++) {
        if (device.channels[i] == channelIndex) {
            return true;
        }
    }

    fastybird_channel_t channel = fastybirdGetChannel(channelIndex);

    DEBUG_MSG(
        PSTR("[INFO][FASTYBIRD] Map channel: %s to device: %s\n"),
        device.name,
        channel.name
    );

    _fastybird_devices[deviceIndex].channels.push_back(channelIndex);

    return true;
}

// -----------------------------------------------------------------------------
// MODULE API - PROPERTIES
// -----------------------------------------------------------------------------

uint8_t fastybirdRegisterProperty(
    const char * name,
    const char * dataType,
    const char * units,
    const char * format
) {
    fastybird_property_t property = {
        strdup(name),
        false,
        false,
        strdup(dataType),
        strdup(units),
        strdup(format)
    };

    _fastybird_properties.push_back(property);

    return (_fastybird_properties.size() - 1);
}

// -----------------------------------------------------------------------------

uint8_t fastybirdRegisterProperty(
    const char * name,
    const char * dataType,
    const char * units,
    const char * format,
    fastybird_properties_process_set_t setCallback
) {
    uint8_t propertyIndex = fastybirdRegisterProperty(
        name,
        dataType,
        units,
        format
    );

    _fastybird_properties[propertyIndex].settable = true;
    _fastybird_properties[propertyIndex].set_callback = setCallback;

    return propertyIndex;
}

// -----------------------------------------------------------------------------

uint8_t fastybirdRegisterProperty(
    const char * name,
    const char * dataType,
    const char * units,
    const char * format,
    fastybird_properties_process_query_t queryCallback
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
    const char * name,
    const char * dataType,
    const char * units,
    const char * format,
    fastybird_properties_process_set_t setCallback,
    fastybird_properties_process_query_t queryCallback
) {
    uint8_t propertyIndex = fastybirdRegisterProperty(
        name,
        dataType,
        units,
        format
    );

    _fastybird_properties[propertyIndex].settable = true;
    _fastybird_properties[propertyIndex].set_callback = setCallback;

    _fastybird_properties[propertyIndex].queryable = true;
    _fastybird_properties[propertyIndex].query_callback = queryCallback;

    return propertyIndex;
}

// -----------------------------------------------------------------------------

bool fastybirdMapPropertyToDevice(
    const uint8_t deviceIndex,
    const uint8_t propertyIndex
) {
    // Basic indexes validation
    if (
        deviceIndex >= _fastybird_devices.size()
        || propertyIndex >= _fastybird_properties.size()
    ) {
        return false;
    }

    fastybird_device_t device = fastybirdGetDevice(deviceIndex);

    // Check if property is mapped to device or not
    for (uint8_t i = 0; i < device.properties.size(); i++) {
        if (device.properties[i] == propertyIndex) {
            return true;
        }
    }

    fastybird_property_t property = fastybirdGetProperty(propertyIndex);

    DEBUG_MSG(
        PSTR("[INFO][FASTYBIRD] Map property: %s to device: %s\n"),
        property.name,
        device.name
    );

    _fastybird_devices[deviceIndex].properties.push_back(propertyIndex);

    return true;
}

// -----------------------------------------------------------------------------

bool fastybirdMapPropertyToChannel(
    const uint8_t deviceIndex,
    const uint8_t channelIndex,
    const uint8_t propertyIndex
) {
    // Basic indexes validation
    if (
        deviceIndex >= _fastybird_devices.size()
        || channelIndex >= _fastybird_channels.size()
        || propertyIndex >= _fastybird_properties.size()
    ) {
        return false;
    }

    fastybird_device_t device = fastybirdGetDevice(deviceIndex);

    bool validChannelIndex = false;

    // Validate channel index against device channels
    for (uint8_t i = 0; i < device.channels.size(); i++) {
        if (device.channels[i] == channelIndex) {
            validChannelIndex = true;

            break;
        }
    }

    if (validChannelIndex == false) {
        return false;
    }

    fastybird_channel_t channel = fastybirdGetChannel(channelIndex);

    // Check if property is mapped to channel or not
    for (uint8_t i = 0; i < channel.properties.size(); i++) {
        if (channel.properties[i] == propertyIndex) {
            return true;
        }
    }

    fastybird_property_t property = fastybirdGetProperty(propertyIndex);

    DEBUG_MSG(
        PSTR("[INFO][FASTYBIRD] Map property: %s to channel: %s and device: %s\n"),
        property.name,
        channel.name,
        device.name
    );

    _fastybird_channels[channelIndex].properties.push_back(propertyIndex);

    return true;
}

// -----------------------------------------------------------------------------

bool fastybirdReportChannelPropertyValue(
    const uint8_t deviceIndex,
    const uint8_t channelIndex,
    const uint8_t propertyIndex,
    const char * payload
) {
    // Basic indexes validation
    if (
        deviceIndex >= _fastybird_devices.size()
        || channelIndex >= _fastybird_channels.size()
        || propertyIndex >= _fastybird_properties.size()
    ) {
        return false;
    }

    fastybird_device_t device = fastybirdGetDevice(deviceIndex);

    bool validChannelIndex = false;

    // Validate channel index against device channels
    for (uint8_t i = 0; i < device.channels.size(); i++) {
        if (device.channels[i] == channelIndex) {
            validChannelIndex = true;

            break;
        }
    }

    if (validChannelIndex == false) {
        return false;
    }

    fastybird_channel_t channel = fastybirdGetChannel(channelIndex);

    bool validPropertyIndex = false;

    // Validate property index against channel properties
    for (uint8_t i = 0; i < channel.properties.size(); i++) {
        if (channel.properties[i] == propertyIndex) {
            validPropertyIndex = true;

            break;
        }
    }

    if (validPropertyIndex == false) {
        return false;
    }

    fastybird_property_t property = fastybirdGetProperty(propertyIndex);

    return fastybirdApiReportChannelPropertyValue(
        device.name,
        channel.name,
        property.name,
        payload
    );
}

// -----------------------------------------------------------------------------
// MODULE CORE
// -----------------------------------------------------------------------------

void fastybirdSetup()
{
    // Initialize all configured channels
    _fastybirdInitialize();

    // Initialize API setup
    fastybirdApiSetup();

    #if WEB_SUPPORT && WS_SUPPORT
        wsOnConnectRegister(_fastybirdWSOnConnect);
        wsOnUpdateRegister(_fastybirdWSOnUpdate);
    #endif

    systemOnHeartbeatRegister(_fastybirdSystemOnHeartbeat);

    fastybirdOnControlRegister(
        [](const uint8_t controlIndex, const char * payload) {
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
        if (fastybirdIsDeviceInitialzed(FASTYBIRD_MAIN_DEVICE_INDEX) == false) {
            static uint32_t last_init_call = 0;

            if (last_init_call == 0 || (millis() - last_init_call > 500)) {
                last_init_call = millis();

                _fastybirdInitializeDevice(FASTYBIRD_MAIN_DEVICE_INDEX);
            }
        }
    }
}

#endif // FASTYBIRD_SUPPORT
