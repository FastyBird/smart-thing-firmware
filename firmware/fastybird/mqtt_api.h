/*

FASTYBIRD MQTT API MODULE

Copyright (C) 2018 FastyBird s.r.o. <info@fastybird.com>

*/

#if FASTYBIRD_SUPPORT && MQTT_SUPPORT

#include "./callbacks.h"

String fastybirdDeviceSN() {
    char buffer[20];

    snprintf_P(buffer, sizeof(buffer), PSTR("%08X"), ESP.getChipId());

    return String(buffer);
}

// -----------------------------------------------------------------------------

String fastybirdMqttApiBuildTopicPrefix(
    const char * deviceId
) {
    String topic_prefix;

    if ((fastybirdDeviceSN()).equals(String(deviceId)) == false) {
        // Replace identifier
        topic_prefix = FASTYBIRD_MQTT_NODE_BASE_TOPIC;
        topic_prefix.replace("{deviceId}", (fastybirdDeviceSN()).c_str());
        topic_prefix.replace("{nodeId}", deviceId);

    } else {
        // Replace identifier
        topic_prefix = FASTYBIRD_MQTT_DEVICE_BASE_TOPIC;
        topic_prefix.replace("{deviceId}", deviceId);
    }

    // Check if topic prefix is ending with "/"
    if (!topic_prefix.endsWith("/")) {
        // ...if not, add it
        topic_prefix = topic_prefix + "/";
    }

    return topic_prefix;
}

// -----------------------------------------------------------------------------

String _fastybirdMqttApiCreateTopicString(
    String topic
) {
    // Replace identifier
    String topic_prefix = FASTYBIRD_MQTT_BASE_TOPIC;

    // Check if topic prefix is ending with "/"
    if (!topic_prefix.endsWith("/")) {
        // ...if not, add it
        topic_prefix = topic_prefix + "/";
    }

    return topic_prefix + topic;
}

// -----------------------------------------------------------------------------

String _fastybirdMqttApiCreateTopicString(
    String topic,
    String replace,
    String replaceWith
) {
    topic.replace("{" + replace + "}", replaceWith);

    return _fastybirdMqttApiCreateTopicString(topic);
}

// -----------------------------------------------------------------------------

bool fastybirdMqttApiIsSameTopic(
    const char * compareTopic,
    const char * topic
) {
    return (String(topic)).endsWith(String(compareTopic));
}

// -----------------------------------------------------------------------------

void _fastybirdMQTTOnConnect() {
    // Unsubscribe from all device topics
    mqttUnsubscribe((fastybirdMqttApiBuildTopicPrefix((fastybirdDeviceSN()).c_str()) + "#").c_str());

    fastybirdResetDeviceInitialization();

    char init_topic[100];
    
    strcpy(init_topic, _fastybirdMqttApiCreateTopicString(FASTYBIRD_TOPIC_BROADCAST_INIT).c_str());

    // Subscribe to init request
    mqttSubscribe(
        init_topic,
        [](const char * topic, const char * payload) {
            fastybirdResetDeviceInitialization();
        }
    );
}

// -----------------------------------------------------------------------------

void _fastybirdMQTTOnDisconnect() {
    fastybirdResetDeviceInitialization();
}

// -----------------------------------------------------------------------------

bool _fastybirdIsApiReady() {
    return mqttConnected();
}

// -----------------------------------------------------------------------------

void _fastybirdBeforeInitialization() {
    if (mqttConnected()) {
        // Unsubscribe from all device topics
        mqttUnsubscribe((fastybirdMqttApiBuildTopicPrefix((fastybirdDeviceSN()).c_str()) + "#").c_str());
    }
}

// -----------------------------------------------------------------------------
// MODULE: INCLUDE REST OF METHODS
// -----------------------------------------------------------------------------

#include "./mqtt_api/device_api.h"
#include "./mqtt_api/channel_api.h"

// -----------------------------------------------------------------------------

void _fastybirdApiSetup() {
    mqttOnConnectRegister(_fastybirdMQTTOnConnect);
    mqttOnConnectRegister(_fastybirdMQTTOnDisconnect);

    char will_topic[100];
    
    strcpy(will_topic, _fastybirdMqttApiCreatePropertyTopicString((fastybirdDeviceSN()).c_str(), FASTYBIRD_PROPERTY_STATE).c_str());

    mqttSetWill(
        will_topic,
        FASTYBIRD_STATUS_LOST
    );
}

// -----------------------------------------------------------------------------

void _fastybirdOnHeartbeat() {
    mqttSend(
        _fastybirdMqttApiCreatePropertyTopicString((fastybirdDeviceSN()).c_str(), FASTYBIRD_PROPERTY_FREE_HEAP).c_str(),
        String(getFreeHeap()).c_str()
    );

    mqttSend(
        _fastybirdMqttApiCreatePropertyTopicString((fastybirdDeviceSN()).c_str(), FASTYBIRD_PROPERTY_UPTIME).c_str(),
        String(getUptime()).c_str()
    );

    #if WIFI_SUPPORT
        mqttSend(
            _fastybirdMqttApiCreatePropertyTopicString((fastybirdDeviceSN()).c_str(), FASTYBIRD_PROPERTY_RSSI).c_str(),
            String(WiFi.RSSI()).c_str()
        );
        mqttSend(
            _fastybirdMqttApiCreatePropertyTopicString((fastybirdDeviceSN()).c_str(), FASTYBIRD_PROPERTY_SSID).c_str(),
            getNetwork().c_str()
        );
    #endif

    mqttSend(
        _fastybirdMqttApiCreatePropertyTopicString((fastybirdDeviceSN()).c_str(), FASTYBIRD_PROPERTY_CPU_LOAD).c_str(),
        String(systemLoadAverage()).c_str()
    );

    #if ADC_MODE_VALUE == ADC_VCC
        mqttSend(
            _fastybirdMqttApiCreatePropertyTopicString((fastybirdDeviceSN()).c_str(), FASTYBIRD_PROPERTY_VCC).c_str(),
            String(ESP.getVcc()).c_str()
        );
    #endif
}

#endif
