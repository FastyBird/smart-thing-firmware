/*

MQTT MODULE CONFIGURATION

Copyright (C) 2018 FastyBird s.r.o. <code@fastybird.com>

*/

#pragma once

#include <AsyncMqttClient.h>

// =============================================================================
// MODULE DEFAULTS
// =============================================================================

#ifndef MQTT_SUPPORT
    #define MQTT_SUPPORT                    0
#endif

#ifndef MQTT_DEFAULT_PORT
    #define MQTT_DEFAULT_PORT               1883
#endif

// MQTT OVER SSL
// Using MQTT over SSL works pretty well but generates problems with the web interface.
// It could be a good idea to use it in conjuntion with WEB_SUPPORT=0.
// Requires NETWORK_SSL_ENABLED to 1 and ESP8266 Arduino Core 2.4.0.
//
// You will need the fingerprint for your MQTT server, example for CloudMQTT:
// $ echo -n | openssl s_client -connect m11.cloudmqtt.com:24055 > cloudmqtt.pem
// $ openssl x509 -noout -in cloudmqtt.pem -fingerprint -sha1

#ifndef MQTT_SSL_ENABLED
    #define MQTT_SSL_ENABLED                0                                   // By default MQTT over SSL will not be enabled
#endif

#ifndef MQTT_SSL_FINGERPRINT
    #define MQTT_SSL_FINGERPRINT            ""                                  // SSL fingerprint of the server
#endif

#ifndef MQTT_AUTOCONNECT
    #define MQTT_AUTOCONNECT                1                                   // If enabled will perform an autodiscover
#endif

#ifndef MQTT_QOS
    #define MQTT_QOS                        1                                   // MQTT QoS value for all messages
#endif

#ifndef MQTT_KEEPALIVE
    #define MQTT_KEEPALIVE                  300                                 // MQTT keepalive value
#endif

#ifndef MQTT_RECONNECT_DELAY_MIN
    #define MQTT_RECONNECT_DELAY_MIN        5000                                // Try to reconnect in 5 seconds upon disconnection
#endif

#ifndef MQTT_RECONNECT_DELAY_STEP
    #define MQTT_RECONNECT_DELAY_STEP       5000                                // Increase the reconnect delay in 5 seconds after each failed attempt
#endif

#ifndef MQTT_RECONNECT_DELAY_MAX
    #define MQTT_RECONNECT_DELAY_MAX        120000                              // Set reconnect time to 2 minutes at most
#endif

#ifndef MQTT_SKIP_RETAINED
    #define MQTT_SKIP_RETAINED              1                                   // Skip retained messages on connection
#endif

#ifndef MQTT_SKIP_TIME
    #define MQTT_SKIP_TIME                  1000                                // Skip messages for 1 second after connection
#endif

// =============================================================================
// MODULE PROTOTYPES
// =============================================================================

typedef std::function<void(const char *, const char *)> mqtt_on_message_callback_t;
void mqttOnMessageRegister(mqtt_on_message_callback_t callback);

typedef std::function<void()> mqtt_on_connect_callback_t;
void mqttOnConnectRegister(mqtt_on_connect_callback_t callback);

typedef std::function<void()> mqtt_on_disconnect_callback_t;
void mqttOnDisconnectRegister(mqtt_on_disconnect_callback_t callback);

// =============================================================================
// MODULE DEPENDENCIES CHECK & CONFIGURATION
// =============================================================================

#if not MQTT_SUPPORT
    #undef DEBUG_MQTT_SUPPORT
    #define DEBUG_MQTT_SUPPORT              0
#endif
