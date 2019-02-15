/*

FASTYBIRD MODULE

Copyright (C) 2018 FastyBird Ltd. <info@fastybird.com>

*/

#if FASTYBIRD_SUPPORT

#if MQTT_SUPPORT
    #include "./fastybird/mqtt_api.h"
#endif

#if FASTYBIRD_GATEWAY_SUPPORT
    #include "./fastybird/nodes.h"
#endif

bool _fastybird_initialized = false;
unsigned int _fastybird_thing_advertisement_progress = FASTYBIRD_PUB_CONNECTION;

unsigned int _fastybird_channel_advertisement_progress = FASTYBIRD_PUB_CHANNEL_NAME;

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
#endif

// -----------------------------------------------------------------------------

/**
 * Call configuration update for each module
 */
void _fastybirdConfigureModules(JsonObject& configuration) {
    DEBUG_MSG(PSTR("[FASTYBIRD] Sending configuration to modules\n"));

    for (unsigned int i = 0; i < _fastybird_on_configure_callbacks.size(); i++) {
        (_fastybird_on_configure_callbacks[i])(configuration);
    }

    DEBUG_MSG(PSTR("[FASTYBIRD] Changes were saved\n"));

    #if WEB_SUPPORT && WS_SUPPORT
        wsReportConfiguration();
    #endif

    // Report back updated configuration
    fastybirdReportConfiguration();
}

// -----------------------------------------------------------------------------

/**
 * Remote thing reset
 */
void _fastybirdResetThing() {
    DEBUG_MSG(PSTR("[FASTYBIRD] Requested reset action\n"));

    deferredReset(100, CUSTOM_RESET_MQTT);
}

// -----------------------------------------------------------------------------

/**
 * Remote thing reconnect
 */
void _fastybirdReconnectThing() {
    DEBUG_MSG(PSTR("[FASTYBIRD] Requested reconnect action\n"));

    #if WIFI_SUPPORT
        wifiDisconnect();
    #endif

    // Reload & cache settings
    firmwareReload();
}

// -----------------------------------------------------------------------------

/**
 * Remote thing factory reset
 */
void _fastybirdFactoryResetThing() {
    DEBUG_MSG(PSTR("[FASTYBIRD] Requested factory reset action\n"));
    DEBUG_MSG(PSTR("\n\nFACTORY RESET\n\n"));

    resetSettings();

    deferredReset(100, CUSTOM_RESET_FACTORY);
}

// -----------------------------------------------------------------------------

/**
 * Initialize all thing channels
 */
bool _fastybirdInitializeChannel(
    fastybird_channel_t channelStructure
) {
    if (channelStructure.initialized) {
        return true;
    }

    std::vector<String> channel_controls;

    switch (_fastybird_channel_advertisement_progress) {
        case FASTYBIRD_PUB_CHANNEL_NAME:
            if (!_fastybirdPropagateChannelName(channelStructure, channelStructure.name)) {
                return false;
            }
            
            _fastybird_channel_advertisement_progress = FASTYBIRD_PUB_CHANNEL_TYPE;
            break;

        case FASTYBIRD_PUB_CHANNEL_TYPE:
            if (!_fastybirdPropagateChannelType(channelStructure, channelStructure.type)) {
                return false;
            }

            _fastybird_channel_advertisement_progress = FASTYBIRD_PUB_CHANNEL_PROPERTIES;
            break;

        case FASTYBIRD_PUB_CHANNEL_PROPERTIES:
            if (!_fastybirdPropagateChannelProperties(channelStructure, channelStructure.properties)) {
                return false;
            }

            // Check if channel definition is for single channel or multichannel
            if (channelStructure.properties.size() > 0) {
                _fastybird_channel_advertisement_progress = FASTYBIRD_PUB_CHANNEL_ARRAY;

            } else {
                _fastybird_channel_advertisement_progress = FASTYBIRD_PUB_CHANNEL_PROPERTY_NAME;
            }
            break;

        case FASTYBIRD_PUB_CHANNEL_ARRAY:
            if (!_fastybirdPropagateChannelSize(channelStructure, channelStructure.length)) {
                return false;
            }

            if (channelStructure.properties.size() > 0) {
                _fastybird_channel_advertisement_progress = FASTYBIRD_PUB_CHANNEL_PROPERTY_NAME;

            } else {
                _fastybird_channel_advertisement_progress = FASTYBIRD_PUB_CHANNEL_CONTROL_STRUCTURE;
            }
            break;

// -----------------------------------------------------------------------------
// CHANNEL PROPERTIES
// -----------------------------------------------------------------------------

        case FASTYBIRD_PUB_CHANNEL_PROPERTY_NAME:
            if (!_fastybirdPropagateChannelPropertyName(channelStructure, channelStructure.properties[0])) {
                return false;
            }

            _fastybird_channel_advertisement_progress = FASTYBIRD_PUB_CHANNEL_PROPERTY_SETABLE;
            break;

        case FASTYBIRD_PUB_CHANNEL_PROPERTY_SETABLE:
            if (!_fastybirdPropagateChannelPropertySettable(channelStructure, channelStructure.properties[0])) {
                return false;
            }

            _fastybird_channel_advertisement_progress = FASTYBIRD_PUB_CHANNEL_PROPERTY_DATA_TYPE;
            break;

        case FASTYBIRD_PUB_CHANNEL_PROPERTY_DATA_TYPE:
            if (!_fastybirdPropagateChannelPropertyDataType(channelStructure, channelStructure.properties[0])) {
                return false;
            }

            _fastybird_channel_advertisement_progress = FASTYBIRD_PUB_CHANNEL_PROPERTY_FORMAT;
            break;

        case FASTYBIRD_PUB_CHANNEL_PROPERTY_FORMAT:
            if (!_fastybirdPropagateChannelPropertyFormat(channelStructure, channelStructure.properties[0])) {
                return false;
            }

            _fastybird_channel_advertisement_progress = FASTYBIRD_PUB_CHANNEL_PROPERTY_MAPPING;
            break;

        case FASTYBIRD_PUB_CHANNEL_PROPERTY_MAPPING:
            if (!_fastybirdPropagateChannelPropertyMappings(channelStructure, channelStructure.properties[0])) {
                return false;
            }

            _fastybird_channel_advertisement_progress = FASTYBIRD_PUB_CHANNEL_CONTROL_STRUCTURE;
            break;

// -----------------------------------------------------------------------------

        case FASTYBIRD_PUB_CHANNEL_CONTROL_STRUCTURE:
            #if FASTYBIRD_ENABLE_CONFIGURATION
                if (channelStructure.isConfigurable) {
                    channel_controls.push_back(FASTYBIRD_CHANNEL_CONTROL_VALUE_CONFIGURATION);
                }
            #endif

            #if DIRECT_CONTROL_SUPPORT
                if (channelStructure.hasDirectControl) {
                    channel_controls.push_back(FASTYBIRD_CHANNEL_CONTROL_VALUE_DIRECT_CONTROL);
                }
            #endif

            #if SCHEDULER_SUPPORT
                if (channelStructure.hasScheduler) {
                    channel_controls.push_back(FASTYBIRD_CHANNEL_CONTROL_VALUE_SCHEDULER);
                }
            #endif

            if (!_fastybirdPropagateChannelControlConfiguration(channelStructure, channel_controls)) {
                return false;
            }

            _fastybird_channel_advertisement_progress = FASTYBIRD_PUB_CHANNEL_CONFIGURATION_SCHEMA;
            break;

        case FASTYBIRD_PUB_CHANNEL_CONFIGURATION_SCHEMA:
            #if FASTYBIRD_ENABLE_CONFIGURATION
                if (channelStructure.isConfigurable) {
                    if (!_fastybirdPropagateChannelConfigurationSchema(channelStructure, channelStructure.configurationSchema)) {
                        return false;
                    }
                }
            #endif

            _fastybird_channel_advertisement_progress = FASTYBIRD_PUB_CHANNEL_DONE;
            break;

        case FASTYBIRD_PUB_CHANNEL_DONE:
            return true;
            break;
    }

    return false;
}

// -----------------------------------------------------------------------------

/**
 * Initilialize thing in cloud
 */
void _fastybirdInitializeSystem() {
    DynamicJsonBuffer jsonBuffer;

    JsonArray& configurationSchema = jsonBuffer.createArray();

    std::vector<String> thing_properties;
    std::vector<String> thing_stats;
    std::vector<String> thing_controls;

    switch (_fastybird_thing_advertisement_progress) {
        case FASTYBIRD_PUB_CONNECTION:
            // Notify cloud broker that thing is sending initialization sequences
            if (!_fastybirdPropagateThingProperty(FASTYBIRD_PROPERTY_STATE, FASTYBIRD_STATUS_INIT)) {
                return;
            }

            // Collect all thing properties...
            thing_properties.push_back(FASTYBIRD_PROPERTY_STATE);

            #if WIFI_SUPPORT
                thing_properties.push_back(FASTYBIRD_PROPERTY_IP_ADDRESS);
            #endif

            #if LED_SUPPORT
                thing_properties.push_back(FASTYBIRD_PROPERTY_STATUS_LED);
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

            if (!_fastybirdPropagateThingHardwareName(THING)) {
                return;
            }

            if (!_fastybirdPropagateThingHardwareModelName(THING)) {
                return;
            }

            if (!_fastybirdPropagateThingHardwareManufacturer(MANUFACTURER)) {
                return;
            }

            _fastybird_thing_advertisement_progress = FASTYBIRD_PUB_FIRMWARE;
            break;

        // Describe thing firmware details to cloud broker
        case FASTYBIRD_PUB_FIRMWARE:
            if (!_fastybirdPropagateThingFirmwareName(FIRMWARE_MANUFACTURER)) {
                return;
            }

            if (!_fastybirdPropagateThingFirmwareVersion(FIRMWARE_VERSION)) {
                return;
            }

            if (!_fastybirdPropagateThingFirmwareManufacturer(FIRMWARE_MANUFACTURER)) {
                return;
            }

            _fastybird_thing_advertisement_progress = FASTYBIRD_PUB_CHANNELS;
            break;

        case FASTYBIRD_PUB_CHANNELS:
            if (_fastybird_channels.size() <= 0) {
                _fastybird_thing_advertisement_progress = FASTYBIRD_PUB_STATS;

                return;
            }

            if (!_fastybirdPropagateThingChannels(_fastybird_channels)) {
                return;
            }

            _fastybird_thing_advertisement_progress = FASTYBIRD_PUB_STATS;
            break;

        case FASTYBIRD_PUB_STATS:
            thing_stats.push_back(FASTYBIRD_STAT_INTERVAL);
            thing_stats.push_back(FASTYBIRD_STAT_UPTIME);
            thing_stats.push_back(FASTYBIRD_STAT_FREE_HEAP);
            thing_stats.push_back(FASTYBIRD_STAT_CPU_LOAD);

            #if ADC_MODE_VALUE == ADC_VCC
                thing_stats.push_back(FASTYBIRD_STAT_VCC);
            #endif

            #if WIFI_SUPPORT
                thing_stats.push_back(FASTYBIRD_STAT_SSID);
                thing_stats.push_back(FASTYBIRD_STAT_RSSI);
            #endif

            if (!_fastybirdPropagateThingStatsStructure(thing_stats)) {
                return;
            }

            // Heartbeat interval
            if (!_fastybirdPropagateThingStat(FASTYBIRD_STAT_INTERVAL, String(HEARTBEAT_INTERVAL / 1000).c_str())) {
                return;
            }

            _fastybird_thing_advertisement_progress = FASTYBIRD_PUB_CONTROL_STRUCTURE;
            break;

        case FASTYBIRD_PUB_CONTROL_STRUCTURE:
            #if FASTYBIRD_ENABLE_CONFIGURATION
                thing_controls.push_back(FASTYBIRD_THING_CONTROL_CONFIGURATION);
            #endif

            #if FASTYBIRD_ENABLE_RESET
                thing_controls.push_back(FASTYBIRD_THING_CONTROL_RESET);
            #endif

            #if FASTYBIRD_ENABLE_RECONNECT
                thing_controls.push_back(FASTYBIRD_THING_CONTROL_RECONNECT);
            #endif

            #if FASTYBIRD_ENABLE_FACTORY_RESET
                thing_controls.push_back(FASTYBIRD_THING_CONTROL_FACTORY_RESET);
            #endif

            if (
                !_fastybirdPropagateThingControlConfiguration(
                    thing_controls,
                    _fastybirdConfigureModules,
                    _fastybirdResetThing,
                    _fastybirdReconnectThing,
                    _fastybirdFactoryResetThing
                )
            ) {
                return;
            }

            _fastybird_thing_advertisement_progress = FASTYBIRD_PUB_CONFIGURATION_SCHEMA;
            break;

        case FASTYBIRD_PUB_CONFIGURATION_SCHEMA:
            #if FASTYBIRD_ENABLE_CONFIGURATION
                for (unsigned int i = 0; i < _fastybird_report_configuration_schema_callbacks.size(); i++) {
                    (_fastybird_report_configuration_schema_callbacks[i])(configurationSchema);
                }

                if (configurationSchema.size() > 0) {
                    if (!_fastybirdPropagateThingConfigurationSchema(configurationSchema)) {
                        return;
                    }
                }
            #endif

            _fastybird_thing_advertisement_progress = FASTYBIRD_PUB_INITIALIZE_CHANNELS;
            break;

        case FASTYBIRD_PUB_INITIALIZE_CHANNELS:
            if (_fastybird_channels.size() > 0) {
                for (unsigned int i = 0; i < _fastybird_channels.size(); i++) {
                    if (_fastybird_channels[i].initialized) {
                        continue;
                    }

                    if (!_fastybirdInitializeChannel(_fastybird_channels[i])) {
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
                for (unsigned int i = 0; i < _fastybird_channels_report_configuration_callbacks.size(); i++) {
                    if (!(_fastybird_channels_report_configuration_callbacks[i])()) {
                        return;
                    }
                }
            }

            #if DIRECT_CONTROL_SUPPORT
                _fastybird_thing_advertisement_progress = FASTYBIRD_PUB_CHANNELS_DIRECT_CONTROL;
            #else
                #if SCHEDULER_SUPPORT
                    _fastybird_thing_advertisement_progress = FASTYBIRD_PUB_CHANNELS_SCHEDULE;
                #else
                    _fastybird_thing_advertisement_progress = FASTYBIRD_PUB_HEARTBEAT;
                #endif
            #endif
            break;

        #if DIRECT_CONTROL_SUPPORT
        case FASTYBIRD_PUB_CHANNELS_DIRECT_CONTROL:
            if (_fastybird_channels_report_direct_controls_callbacks.size() > 0) {
                for (unsigned int i = 0; i < _fastybird_channels_report_direct_controls_callbacks.size(); i++) {
                    if (!(_fastybird_channels_report_direct_controls_callbacks[i])()) {
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
        #endif

        #if SCHEDULER_SUPPORT
        case FASTYBIRD_PUB_CHANNELS_SCHEDULE:
            if (_fastybird_channels_report_scheduler_callbacks.size() > 0) {
                for (unsigned int i = 0; i < _fastybird_channels_report_scheduler_callbacks.size(); i++) {
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
            systemSendHeartbeat();
            break;
    }
}

// -----------------------------------------------------------------------------

bool fastybirdReportConfiguration() {
    DynamicJsonBuffer jsonBuffer;

    JsonObject& configuration = jsonBuffer.createObject();

    for (unsigned int i = 0; i < _fastybird_report_configuration_callbacks.size(); i++) {
        (_fastybird_report_configuration_callbacks[i])(configuration);
    }

    if (configuration.size() > 0) {
        return _fastybirdPropagateThingConfiguration(configuration);
    }

    return true;
}

// -----------------------------------------------------------------------------

bool fastybirdReportChannelConfiguration(
    fastybird_channel_t channelStructure,
    String configuration,
    unsigned int channelId
) {
    return _fastybirdPropagateChannelConfiguration(channelStructure, channelId, configuration);
}

// -----------------------------------------------------------------------------

bool fastybirdReportChannelConfiguration(
    fastybird_channel_t channelStructure,
    String configuration
) {
    return fastybirdReportChannelConfiguration(channelStructure, configuration, 0);
}

// -----------------------------------------------------------------------------

bool fastybirdReportChannelDirectControl(
    fastybird_channel_t channelStructure,
    String directControls,
    unsigned int channelId
) {
    return _fastybirdPropagateChannelDirectControlConfiguration(channelStructure, channelId, directControls);
}

// -----------------------------------------------------------------------------

bool fastybirdReportChannelDirectControl(
    fastybird_channel_t channelStructure,
    String directControls
) {
    return fastybirdReportChannelDirectControl(channelStructure, directControls, 0);
}

// -----------------------------------------------------------------------------

bool fastybirdReportChannelScheduler(
    fastybird_channel_t channelStructure,
    String schedules,
    unsigned int channelId
) {
    return _fastybirdPropagateChannelSchedulerConfiguration(channelStructure, channelId, schedules);
}

// -----------------------------------------------------------------------------

bool fastybirdReportChannelScheduler(
    fastybird_channel_t channelStructure,
    String schedules
) {
    return fastybirdReportChannelScheduler(channelStructure, schedules, 0);
}

// -----------------------------------------------------------------------------

bool fastybirdReportChannelValue(
    fastybird_channel_t channelStructure,
    fastybird_channel_property_t channelProperty,
    unsigned int channelId,
    const char * payload
) {
    return _fastybirdPropagateChannelValue(channelStructure, channelProperty, channelId, payload);
}

// -----------------------------------------------------------------------------

bool fastybirdReportChannelValue(
    fastybird_channel_t channelStructure,
    fastybird_channel_property_t channelProperty,
    const char * payload
) {
    return fastybirdReportChannelValue(channelStructure, channelProperty, 0, payload);
}

// -----------------------------------------------------------------------------

void fastybirdResetThingInitialization() {
    _fastybird_initialized = false;
    _fastybird_thing_advertisement_progress = FASTYBIRD_PUB_CONNECTION;

    for (unsigned int i = 0; i < _fastybird_channels.size(); i++) {
        _fastybird_channels[i].initialized = false;
    }

    _fastybird_channel_advertisement_progress = FASTYBIRD_PUB_CHANNEL_NAME;

    _fastybirdBeforeInitialization();
}

// -----------------------------------------------------------------------------

bool fastybirdIsThingInitialzed() {
    return _fastybird_initialized;
}

// -----------------------------------------------------------------------------

void fastybirdRegisterChannel(fastybird_channel_t channel) {
    _fastybird_channels.push_back(channel);
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
            static unsigned long last_init_call = 0;

            if (last_init_call == 0 || (millis() - last_init_call > 500)) {
                last_init_call = millis();

                //_fastybirdInitializeSystem();
            }
        }
    }
}

#endif // FASTYBIRD_SUPPORT
