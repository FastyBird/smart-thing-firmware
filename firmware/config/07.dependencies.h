/*

DEPENDENCIES CHECK

Copyright (C) 2018 FastyBird Ltd. <info@fastybird.com>

*/

#pragma once

//------------------------------------------------------------------------------
// Do not change this file unless you know what you are doing
// Configuration settings are in the general.h file
//------------------------------------------------------------------------------

#if STATUS_LED_PIN == GPIO_NONE
    #undef LED_SUPPORT
    #define LED_SUPPORT                 0           // Disable led module because no status led pin is defined
#endif

#if not WEB_SUPPORT
    #undef DEBUG_WEB_SUPPORT
    #define DEBUG_WEB_SUPPORT           0

    #undef WS_SUPPORT
    #define WS_SUPPORT                  0           // WS support requires web support
#endif

#if BUTTON1_PIN == GPIO_NONE && BUTTON2_PIN == GPIO_NONE && BUTTON3_PIN == GPIO_NONE && BUTTON4_PIN == GPIO_NONE && BUTTON5_PIN == GPIO_NONE && BUTTON6_PIN == GPIO_NONE && BUTTON7_PIN == GPIO_NONE && BUTTON8_PIN == GPIO_NONE
    #undef BUTTON_SUPPORT
    #define BUTTON_SUPPORT              0           // Dissable button when no button is defined
#endif
