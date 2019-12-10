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

#if BUTTON1_PIN == GPIO_NONE && BUTTON2_PIN == GPIO_NONE && BUTTON3_PIN == GPIO_NONE && BUTTON4_PIN == GPIO_NONE && BUTTON5_PIN == GPIO_NONE && BUTTON6_PIN == GPIO_NONE && BUTTON7_PIN == GPIO_NONE && BUTTON8_PIN == GPIO_NONE
    #undef BUTTON_SUPPORT
    #define BUTTON_SUPPORT              0           // Dissable button when no button is defined
#endif

#if FASTYBIRD_SUPPORT
    #undef MQTT_SUPPORT
    #define MQTT_SUPPORT                1           // FastyBird needs MQTT
#endif

#if NODES_GATEWAY_SUPPORT && NODES_GATEWAY_TX_PIN == GPIO_NONE && NODES_GATEWAY_RX_PIN == GPIO_NONE
    #undef NODES_GATEWAY_SUPPORT
    #define NODES_GATEWAY_SUPPORT       0
#endif

#if NODES_GATEWAY_SUPPORT
    #undef SPIFFS_SUPPORT
    #define SPIFFS_SUPPORT              1           // Enabling SPIFFS for storing configuration
#endif

#if STATUS_LED_PIN == GPIO_NONE
    #undef LED_SUPPORT
    #define LED_SUPPORT                 0           // Disable led module because no status led pin is defined
#endif