#pragma once

//------------------------------------------------------------------------------
// Do not change this file unless you know what you are doing
// Configuration settings are in the general.h file
//------------------------------------------------------------------------------

#if not WEB_SUPPORT
    #undef DEBUG_WEB_SUPPORT
    #define DEBUG_WEB_SUPPORT           0
#endif

#if not WEB_SUPPORT
    #undef WS_SUPPORT
    #define WS_SUPPORT                  0           // WS support requires web support
#endif

#if FASTYBIRD_SUPPORT
    #undef MQTT_SUPPORT
    #define MQTT_SUPPORT                1           // FastyBird needs MQTT
#endif

#if NODES_GATEWAY_SUPPORT && NODES_GATEWAY_TX_PIN == GPIO_NONE && NODES_GATEWAY_RX_PIN == GPIO_NONE
    #undef NODES_GATEWAY_SUPPORT
    #define NODES_GATEWAY_SUPPORT       0
#endif

#if DIRECT_CONTROL_SUPPORT && not MQTT_SUPPORT
    #undef DIRECT_CONTROL_SUPPORT
    #define DIRECT_CONTROL_SUPPORT      0           // Direct controls needs MQTT
#endif

#if BUTTON1_PIN == GPIO_NONE && BUTTON2_PIN == GPIO_NONE && BUTTON3_PIN == GPIO_NONE && BUTTON4_PIN == GPIO_NONE && BUTTON5_PIN == GPIO_NONE && BUTTON6_PIN == GPIO_NONE && BUTTON7_PIN == GPIO_NONE && BUTTON8_PIN == GPIO_NONE
    #undef BUTTON_SUPPORT
    #define BUTTON_SUPPORT              0           // Dissable button when no button is defined
#endif

#if RELAY_PROVIDER == RELAY_PROVIDER_NONE && LIGHT_PROVIDER == LIGHT_PROVIDER_NONE
    #undef SCHEDULER_SUPPORT
    #define SCHEDULER_SUPPORT           0           // Scheduler is supported only for relay module or light module
#endif

#if SCHEDULER_SUPPORT
    #undef NTP_SUPPORT
    #define NTP_SUPPORT                 1           // Scheduler needs NTP
#endif

#if FASTYBIRD_SUPPORT || SCHEDULER_SUPPORT || DIRECT_CONTROL_SUPPORT
    #undef SPIFFS_SUPPORT
    #define SPIFFS_SUPPORT              1           // Enabling SPIFFS for storing configuration
#endif