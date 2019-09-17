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

uint8_t _fastybird_thing_advertisement_progress = FASTYBIRD_PUB_CONNECTION;
uint8_t _fastybird_channel_advertisement_progress = FASTYBIRD_PUB_CHANNEL_NAME;
uint8_t _fastybird_channel_property_advertisement_progress = FASTYBIRD_PUB_CHANNEL_PROPERTY_NAME;

std::vector<fastybird_channel_t> _fastybird_channels;

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

        data["status"] = fastybirdIsThingInitialzed() ? true : false;
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

        data["status"] = fastybirdIsThingInitialzed() ? true : false;
    }
#endif // WEB_SUPPORT && WS_SUPPORT

// -----------------------------------------------------------------------------

/**
 * Initialize given thing channel property
 */
bool _fastybirdInitializeChannelProperty(
    fastybird_channel_t channelStructure,
    fastybird_channel_property_t propertyStructure
) {
    if (propertyStructure.initialized) {
        return true;
    }

    switch (_fastybird_channel_property_advertisement_progress)
    {    
        case FASTYBIRD_PUB_CHANNEL_PROPERTY_NAME:
            if (!_fastybirdPropagateChannelPropertyName(channelStructure, propertyStructure)) {
                return false;
            }

            _fastybird_channel_property_advertisement_progress = FASTYBIRD_PUB_CHANNEL_PROPERTY_SETABLE;
            break;

        case FASTYBIRD_PUB_CHANNEL_PROPERTY_SETABLE:
            if (!_fastybirdPropagateChannelPropertySettable(channelStructure, propertyStructure)) {
                return false;
            }

            _fastybird_channel_property_advertisement_progress = FASTYBIRD_PUB_CHANNEL_PROPERTY_QUERYABLE;
            break;

        case FASTYBIRD_PUB_CHANNEL_PROPERTY_QUERYABLE:
            if (!_fastybirdPropagateChannelPropertyQueryable(channelStructure, propertyStructure)) {
                return false;
            }

            _fastybird_channel_property_advertisement_progress = FASTYBIRD_PUB_CHANNEL_PROPERTY_DATA_TYPE;
            break;

        case FASTYBIRD_PUB_CHANNEL_PROPERTY_DATA_TYPE:
            if (!_fastybirdPropagateChannelPropertyDataType(channelStructure, propertyStructure)) {
                return false;
            }

            _fastybird_channel_property_advertisement_progress = FASTYBIRD_PUB_CHANNEL_PROPERTY_UNITS;
            break;

        case FASTYBIRD_PUB_CHANNEL_PROPERTY_UNITS:
            if (!_fastybirdPropagateChannelPropertyUnits(channelStructure, propertyStructure)) {
                return false;
            }

            _fastybird_channel_property_advertisement_progress = FASTYBIRD_PUB_CHANNEL_PROPERTY_FORMAT;
            break;

        case FASTYBIRD_PUB_CHANNEL_PROPERTY_FORMAT:
            if (!_fastybirdPropagateChannelPropertyFormat(channelStructure, propertyStructure)) {
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
 * Initialize given thing channel
 */
bool _fastybirdInitializeChannel(
    uint8_t channelIndex
) {
    if (_fastybird_channels[channelIndex].initialized) {
        return true;
    }

    std::vector<String> channel_controls;

    switch (_fastybird_channel_advertisement_progress)
    {
        
// -----------------------------------------------------------------------------
// CHANNEL BASIC INFO
// -----------------------------------------------------------------------------

        case FASTYBIRD_PUB_CHANNEL_NAME:
            if (!_fastybirdPropagateChannelName(_fastybird_channels[channelIndex], _fastybird_channels[channelIndex].name.c_str())) {
                return false;
            }
            
            _fastybird_channel_advertisement_progress = FASTYBIRD_PUB_CHANNEL_TYPE;
            break;

        case FASTYBIRD_PUB_CHANNEL_TYPE:
            if (!_fastybirdPropagateChannelType(_fastybird_channels[channelIndex], _fastybird_channels[channelIndex].type.c_str())) {
                return false;
            }

            _fastybird_channel_advertisement_progress = FASTYBIRD_PUB_CHANNEL_ARRAY;
            break;

        case FASTYBIRD_PUB_CHANNEL_ARRAY:
            if (!_fastybirdPropagateChannelSize(_fastybird_channels[channelIndex], _fastybird_channels[channelIndex].length)) {
                return false;
            }

            _fastybird_channel_advertisement_progress = FASTYBIRD_PUB_CHANNEL_PROPERTIES;
            break;

        case FASTYBIRD_PUB_CHANNEL_PROPERTIES:
            if (!_fastybirdPropagateChannelProperties(_fastybird_channels[channelIndex], _fastybird_channels[channelIndex].properties)) {
                return false;
            }

            _fastybird_channel_advertisement_progress = FASTYBIRD_PUB_CHANNEL_CONTROL_STRUCTURE;
            break;

        case FASTYBIRD_PUB_CHANNEL_CONTROL_STRUCTURE:
            if (_fastybird_channels[channelIndex].isConfigurable) {
                channel_controls.push_back(FASTYBIRD_CHANNEL_CONTROL_CONFIGURE);
            }

            #if SCHEDULER_SUPPORT
                if (_fastybird_channels[channelIndex].hasScheduler) {
                    channel_controls.push_back(FASTYBIRD_CHANNEL_CONTROL_SCHEDULE);
                }
            #endif

            if (!_fastybirdPropagateChannelControlConfiguration(_fastybird_channels[channelIndex], channel_controls)) {
                return false;
            }

            if (_fastybird_channels[channelIndex].properties.size() > 0) {
                _fastybird_channel_advertisement_progress = FASTYBIRD_PUB_CHANNEL_PROPERTY;

            } else if (_fastybird_channels[channelIndex].isConfigurable) {
                _fastybird_channel_advertisement_progress = FASTYBIRD_PUB_CHANNEL_CONFIGURATION_SCHEMA;
 
            } else {
                _fastybird_channel_advertisement_progress = FASTYBIRD_PUB_CHANNEL_DONE;
            }
            break;

// -----------------------------------------------------------------------------
// CHANNEL PROPERTIES
// -----------------------------------------------------------------------------

        case FASTYBIRD_PUB_CHANNEL_PROPERTY:
            if (_fastybird_channels[channelIndex].properties.size() > 0) {
                for (uint8_t i = 0; i < _fastybird_channels[channelIndex].properties.size(); i++) {
                    if (_fastybird_channels[channelIndex].properties[i].initialized) {
                        continue;
                    }

                    if (!_fastybirdInitializeChannelProperty(_fastybird_channels[channelIndex], _fastybird_channels[channelIndex].properties[i])) {
                        return false;
                    }

                    _fastybird_channels[channelIndex].properties[i].initialized = true;

                    _fastybird_channel_property_advertisement_progress = FASTYBIRD_PUB_CHANNEL_PROPERTY_NAME;
                }
            }

            if (_fastybird_channels[channelIndex].isConfigurable) {
                _fastybird_channel_advertisement_progress = FASTYBIRD_PUB_CHANNEL_CONFIGURATION_SCHEMA;
 
            } else {
                _fastybird_channel_advertisement_progress = FASTYBIRD_PUB_CHANNEL_DONE;
            }
            break;

// -----------------------------------------------------------------------------
// CHANNEL CONTROL CONFIGURATION
// -----------------------------------------------------------------------------

        case FASTYBIRD_PUB_CHANNEL_CONFIGURATION_SCHEMA:
            if (_fastybird_channels[channelIndex].isConfigurable) {
                if (!_fastybirdPropagateChannelConfigurationSchema(_fastybird_channels[channelIndex], _fastybird_channels[channelIndex].configurationSchema)) {
                    return false;
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
 * Initilialize thing to broker
 */
void _fastybirdInitializeSystem() {
    DynamicJsonBuffer jsonBuffer;

    JsonArray& configurationSchema = jsonBuffer.createArray();

    std::vector<String> thing_properties;
    std::vector<String> thing_controls;

    switch (_fastybird_thing_advertisement_progress)
    {
        case FASTYBIRD_PUB_CONNECTION:
            // Notify cloud broker that thing is sending initialization sequences
            if (!_fastybirdPropagateThingProperty(FASTYBIRD_PROPERTY_STATE, FASTYBIRD_STATUS_INIT)) {
                return;
            }
 
            // Collect all thing default properties...
            thing_properties.push_back(FASTYBIRD_PROPERTY_INTERVAL);
            thing_properties.push_back(FASTYBIRD_PROPERTY_UPTIME);
            thing_properties.push_back(FASTYBIRD_PROPERTY_FREE_HEAP);
            thing_properties.push_back(FASTYBIRD_PROPERTY_CPU_LOAD);
            thing_properties.push_back(FASTYBIRD_PROPERTY_STATE);

            #if WIFI_SUPPORT
                thing_properties.push_back(FASTYBIRD_PROPERTY_IP_ADDRESS);
            #endif

            #if ADC_MODE_VALUE == ADC_VCC
                thing_properties.push_back(FASTYBIRD_PROPERTY_VCC);
            #endif

            #if WIFI_SUPPORT
                thing_properties.push_back(FASTYBIRD_PROPERTY_SSID);
                thing_properties.push_back(FASTYBIRD_PROPERTY_RSSI);
            #endif
 
            // ...and pass them to the broker
            if (!_fastybirdPropagateThingPropertiesStructure(thing_properties)) {
                return;
            }

            // For thing with wifi support notify its IP address
            #if WIFI_SUPPORT
                if (!_fastybirdPropagateThingProperty(FASTYBIRD_PROPERTY_IP_ADDRESS, getIP().c_str())) {
                    return;
                }
            #endif

            // Heartbeat interval
            if (!_fastybirdPropagateThingProperty(FASTYBIRD_PROPERTY_INTERVAL, String(HEARTBEAT_INTERVAL / 1000).c_str())) {
                return;
            }

            _fastybird_thing_advertisement_progress = FASTYBIRD_PUB_NAME;
            break;

        case FASTYBIRD_PUB_NAME:
            if (!_fastybirdPropagateThingName(getIdentifier().c_str())) {
                return;
            }

            _fastybird_thing_advertisement_progress = FASTYBIRD_PUB_HARDWARE;
            break;

        // Describe thing hardware details to cloud broker
        case FASTYBIRD_PUB_HARDWARE:
            #if WIFI_SUPPORT
                if (!_fastybirdPropagateThingHardwareField(FASTYBIRD_HARDWARE_MAC_ADDRESS, WiFi.macAddress().c_str())) {
                    return;
                }
            #endif

            if (!_fastybirdPropagateThingHardwareManufacturer(MANUFACTURER)) {
                return;
            }

            if (!_fastybirdPropagateThingHardwareModelName(THING)) {
                return;
            }

            if (!_fastybirdPropagateThingHardwareVersion(HARWARE_VERSION)) {
                return;
            }

            _fastybird_thing_advertisement_progress = FASTYBIRD_PUB_FIRMWARE;
            break;

        // Describe thing firmware details to cloud broker
        case FASTYBIRD_PUB_FIRMWARE:
            if (!_fastybirdPropagateThingFirmwareManufacturer(FIRMWARE_MANUFACTURER)) {
                return;
            }

            if (!_fastybirdPropagateThingFirmwareName(FIRMWARE_MANUFACTURER)) {
                return;
            }

            if (!_fastybirdPropagateThingFirmwareVersion(FIRMWARE_VERSION)) {
                return;
            }

            _fastybird_thing_advertisement_progress = FASTYBIRD_PUB_CHANNELS;
            break;

        case FASTYBIRD_PUB_CHANNELS:
            if (_fastybird_channels.size() <= 0) {
                _fastybird_thing_advertisement_progress = FASTYBIRD_PUB_CONTROL_STRUCTURE;

                return;
            }

            if (!_fastybirdPropagateThingChannels(_fastybird_channels)) {
                return;
            }

            _fastybird_thing_advertisement_progress = FASTYBIRD_PUB_CONTROL_STRUCTURE;
            break;

        case FASTYBIRD_PUB_CONTROL_STRUCTURE:
            for (uint8_t i = 0; i < _fastybird_on_control_callbacks.size(); i++) {
                thing_controls.push_back(_fastybird_on_control_callbacks[i].controlName);
            }

            if (!_fastybirdPropagateThingControlConfiguration(thing_controls)) {
                return;
            }

            _fastybird_thing_advertisement_progress = FASTYBIRD_PUB_CONFIGURATION_SCHEMA;
            break;

        case FASTYBIRD_PUB_CONFIGURATION_SCHEMA:
            for (uint8_t i = 0; i < _fastybird_report_configuration_schema_callbacks.size(); i++) {
                (_fastybird_report_configuration_schema_callbacks[i])(configurationSchema);
            }

            if (configurationSchema.size() > 0) {
                if (!_fastybirdPropagateThingConfigurationSchema(configurationSchema)) {
                    return;
                }
            }

            _fastybird_thing_advertisement_progress = FASTYBIRD_PUB_INITIALIZE_CHANNELS;
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

            _fastybird_thing_advertisement_progress = FASTYBIRD_PUB_READY;
            break;

        case FASTYBIRD_PUB_READY:
            if (!_fastybirdPropagateThingProperty(FASTYBIRD_PROPERTY_STATE, FASTYBIRD_STATUS_READY)) {
                return;
            }

            _fastybird_thing_advertisement_progress = FASTYBIRD_PUB_CONFIGURATION;
            break;

        case FASTYBIRD_PUB_CONFIGURATION:
            if (!fastybirdReportConfiguration()) {
                return;
            }

            _fastybird_thing_advertisement_progress = FASTYBIRD_PUB_CHANNELS_CONFIGURATION;
            break;

        case FASTYBIRD_PUB_CHANNELS_CONFIGURATION:
            if (_fastybird_channels_report_configuration_callbacks.size() > 0) {
                for (uint8_t i = 0; i < _fastybird_channels_report_configuration_callbacks.size(); i++) {
                    if (!(_fastybird_channels_report_configuration_callbacks[i])()) {
                        return;
                    }
                }
            }

            #if SCHEDULER_SUPPORT
                _fastybird_thing_advertisement_progress = FASTYBIRD_PUB_CHANNELS_SCHEDULE;
            #else
                _fastybird_thing_advertisement_progress = FASTYBIRD_PUB_HEARTBEAT;
            #endif
            break;

        #if SCHEDULER_SUPPORT
        case FASTYBIRD_PUB_CHANNELS_SCHEDULE:
            if (_fastybird_channels_report_scheduler_callbacks.size() > 0) {
                for (uint8_t i = 0; i < _fastybird_channels_report_scheduler_callbacks.size(); i++) {
                    if (!(_fastybird_channels_report_scheduler_callbacks[i])()) {
                        return;
                    }
                }
            }

            _fastybird_thing_advertisement_progress = FASTYBIRD_PUB_HEARTBEAT;
            break;
        #endif

        case FASTYBIRD_PUB_HEARTBEAT:
            _fastybird_initialized = true;

            #if WEB_SUPPORT && WS_SUPPORT
                wsSend(_fastybirdWSOnUpdate);
            #endif
            
            systemSendHeartbeat();
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
        return _fastybirdPropagateThingConfiguration(configuration);
    }

    return true;
}

// -----------------------------------------------------------------------------

bool fastybirdReportChannelConfiguration(
    const uint8_t index,
    const uint8_t channelId,
    String configuration
) {
    if (index >= 0 && index < _fastybird_channels.size()) {
        return _fastybirdPropagateChannelConfiguration(
            _fastybird_channels[index],
            channelId,
            configuration
        );
    }
}

// -----------------------------------------------------------------------------

bool fastybirdReportChannelConfiguration(
    const uint8_t index,
    String configuration
) {
    return fastybirdReportChannelConfiguration(index, 0, configuration);
}

// -----------------------------------------------------------------------------

bool fastybirdReportChannelScheduler(
    const uint8_t index,
    const uint8_t channelId,
    String schedules
) {
    if (index >= 0 && index < _fastybird_channels.size()) {
        return _fastybirdPropagateChannelSchedulerConfiguration(
            _fastybird_channels[index],
            channelId,
            schedules
        );
    }
}

// -----------------------------------------------------------------------------

bool fastybirdReportChannelScheduler(
    const uint8_t index,
    String schedules
) {
    return fastybirdReportChannelScheduler(index, 0, schedules);
}

// -----------------------------------------------------------------------------

bool fastybirdReportChannelValue(
    const uint8_t index,
    const uint8_t property,
    const uint8_t channelId,
    const char * payload
) {
    if (
        index >= 0 && index < _fastybird_channels.size()
        && property >= 0 && property < _fastybird_channels[index].properties.size()
    ) {
        return _fastybirdPropagateChannelValue(
            _fastybird_channels[index],
            _fastybird_channels[index].properties[property],
            channelId,
            payload
        );
    }

    return false;
}

// -----------------------------------------------------------------------------

bool fastybirdReportChannelValue(
    const uint8_t index,
    const uint8_t property,
    const char * payload
) {
    return fastybirdReportChannelValue(index, property, 0, payload);
}

// -----------------------------------------------------------------------------

void fastybirdResetThingInitialization() {
    _fastybird_initialized = false;
    _fastybird_thing_advertisement_progress = FASTYBIRD_PUB_CONNECTION;

    for (uint8_t i = 0; i < _fastybird_channels.size(); i++) {
        _fastybird_channels[i].initialized = false;

        for (uint8_t j = 0; j < _fastybird_channels[i].properties.size(); j++) {
            _fastybird_channels[i].properties[j].initialized = false;
        }
    }

    _fastybird_channel_advertisement_progress = FASTYBIRD_PUB_CHANNEL_NAME;
    _fastybird_channel_property_advertisement_progress = FASTYBIRD_PUB_CHANNEL_PROPERTY_NAME;

    #if WEB_SUPPORT && WS_SUPPORT
        wsSend(_fastybirdWSOnUpdate);
    #endif
    
    _fastybirdBeforeInitialization();
}

// -----------------------------------------------------------------------------

bool fastybirdIsThingInitialzed() {
    return _fastybird_initialized;
}

// -----------------------------------------------------------------------------

uint8_t fastybirdRegisterChannel(
    fastybird_channel_t channel
) {
    _fastybird_channels.push_back(channel);

    return (_fastybird_channels.size() - 1);
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
        FASTYBIRD_THING_CONTROL_CONFIGURE
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
        FASTYBIRD_THING_CONTROL_REBOOT
    );

    // Register firmware callbacks
    firmwareRegisterLoop(fastybirdLoop);
    firmwareRegisterReload([]() {
        _fastybirdApiRestore(_fastybird_channels);
    });
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
