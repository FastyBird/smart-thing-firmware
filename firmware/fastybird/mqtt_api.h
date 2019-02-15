/*

FASTYBIRD MQTT API MODULE

Copyright (C) 2018 FastyBird Ltd. <info@fastybird.com>

*/

#if FASTYBIRD_SUPPORT && MQTT_SUPPORT

#include "./callbacks.h"

// Define thing cloud identifier
const char * _fastybird_mqtt_thing_id = MQTT_USER;

// -----------------------------------------------------------------------------

String fastybirdMqttApiBuildTopicPrefix(const char * thingId) {
    String topic_prefix;

    if (_fastybird_mqtt_thing_id != thingId) {
        // Replace identifier
        topic_prefix = FASTYBIRD_MQTT_NODE_BASE_TOPIC;
        topic_prefix.replace("{cloudThingId}", _fastybird_mqtt_thing_id);
        topic_prefix.replace("{nodeId}", thingId);

    } else {
        // Replace identifier
        topic_prefix = FASTYBIRD_MQTT_BASE_TOPIC;
        topic_prefix.replace("{cloudThingId}", thingId);
    }

    // Check if topic prefix is ending with "/"
    if (!topic_prefix.endsWith("/")) {
        // ...if not, add it
        topic_prefix = topic_prefix + "/";
    }

    return topic_prefix;
}

// -----------------------------------------------------------------------------

bool fastybirdMqttApiIsSameTopic(const char * compareTopic, const char * topic) {
    return (String(topic)).endsWith(String(compareTopic));
}

// -----------------------------------------------------------------------------

void _fastybirdMQTTOnConnect() {
    // Unsubscribe from all thing topics
    mqttUnsubscribeRaw((fastybirdMqttApiBuildTopicPrefix(_fastybird_mqtt_thing_id) + "#").c_str());

    fastybirdResetThingInitialization();
}

// -----------------------------------------------------------------------------

void _fastybirdMQTTOnDisconnect() {
    fastybirdResetThingInitialization();
}

// -----------------------------------------------------------------------------

bool _fastybirdIsApiReady() {
    return mqttConnected();
}

// -----------------------------------------------------------------------------

void _fastybirdBeforeInitialization() {
    if (mqttConnected()) {
        // Unsubscribe from all thing topics
        mqttUnsubscribeRaw((fastybirdMqttApiBuildTopicPrefix(_fastybird_mqtt_thing_id) + "#").c_str());
    }
}

// -----------------------------------------------------------------------------
// MODULE: INCLUDE REST OF METHODS
// -----------------------------------------------------------------------------

#include "./mqtt_api/thing_api.h"
#include "./mqtt_api/channel_api.h"

// -----------------------------------------------------------------------------

void _fastybirdApiRestore(std::vector<fastybird_channel_t> channels) {
    #if DIRECT_CONTROL_SUPPORT
        for(unsigned int i; i < channels.size(); i++) {
            _fastybirdMqttApiChannelUnsubscribeDirectControls(channels[i]);
            _fastybirdMqttApiChannelSubscribeDirectControls(channels[i]);
        }

        if (_fastybird_channels_report_direct_controls_callbacks.size() > 0) {
            for (unsigned int i = 0; i < _fastybird_channels_report_direct_controls_callbacks.size(); i++) {
                if (!(_fastybird_channels_report_direct_controls_callbacks[i])()) {
                    return;
                }
            }
        }
    #endif

    #if SCHEDULER_SUPPORT
        if (_fastybird_channels_report_scheduler_callbacks.size() > 0) {
            for (unsigned int i = 0; i < _fastybird_channels_report_scheduler_callbacks.size(); i++) {
                if (!(_fastybird_channels_report_scheduler_callbacks[i])()) {
                    return;
                }
            }
        }
    #endif
}

// -----------------------------------------------------------------------------

void _fastybirdApiSetup() {
    mqttOnConnectRegister(_fastybirdMQTTOnConnect);
    mqttOnConnectRegister(_fastybirdMQTTOnDisconnect);

    mqttSetWill(
        _fastybirdMqttApiCreatePropertyTopicString(_fastybird_mqtt_thing_id, FASTYBIRD_PROPERTY_STATE),
        FASTYBIRD_STATUS_LOST
    );
}

// -----------------------------------------------------------------------------

void _fastybirdOnHeartbeat() {
    mqttSendRaw(
        _fastybirdMqttApiCreateStatTopicString(_fastybird_mqtt_thing_id, FASTYBIRD_STAT_FREE_HEAP).c_str(),
        String(getFreeHeap()).c_str()
    );
    mqttSendRaw(
        _fastybirdMqttApiCreateStatTopicString(_fastybird_mqtt_thing_id, FASTYBIRD_STAT_UPTIME).c_str(),
        String(getUptime()).c_str()
    );

    #if WIFI_SUPPORT
        mqttSendRaw(
            _fastybirdMqttApiCreateStatTopicString(_fastybird_mqtt_thing_id, FASTYBIRD_STAT_RSSI).c_str(),
            String(WiFi.RSSI()).c_str()
        );
        mqttSendRaw(
            _fastybirdMqttApiCreateStatTopicString(_fastybird_mqtt_thing_id, FASTYBIRD_STAT_SSID).c_str(),
            getNetwork().c_str()
        );
    #endif

    mqttSendRaw(
        _fastybirdMqttApiCreateStatTopicString(_fastybird_mqtt_thing_id, FASTYBIRD_STAT_CPU_LOAD).c_str(),
        String(systemLoadAverage()).c_str()
    );

    #if ADC_MODE_VALUE == ADC_VCC
        mqttSendRaw(
            _fastybirdMqttApiCreateStatTopicString(_fastybird_mqtt_thing_id, FASTYBIRD_STAT_VCC).c_str(),
            String(ESP.getVcc()).c_str()
        );
    #endif
}

#endif
