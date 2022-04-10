/*

LED MODULE CONFIGURATION

Copyright (C) 2018 FastyBird s.r.o. <code@fastybird.com>

*/

#pragma once

// =============================================================================
// MODULE DEFAULTS
// =============================================================================

#ifndef LED_SUPPORT
    #define LED_SUPPORT                         0
#endif

// =============================================================================
// MODULE TYPES
// =============================================================================

#define LED_MODE_WIFI                           1       // LED will blink according to the WIFI status
#define LED_MODE_ON                             2       // LED always ON
#define LED_MODE_OFF                            3       // LED always OFF

// =============================================================================
// MODULE DEPENDENCIES CHECK & CONFIGURATION
// =============================================================================

#if STATUS_LED_PIN == GPIO_NONE
    #undef LED_SUPPORT
    #define LED_SUPPORT                         0       // Disable led module because no status led pin is defined
#endif

// =============================================================================
// MODULE DEFAULTS
// =============================================================================

#ifndef STATUS_LED_PIN
    #define STATUS_LED_PIN                      GPIO_NONE
#endif

#ifndef STATUS_LED_PIN_INVERSE
    #define STATUS_LED_PIN_INVERSE              0
#endif

#ifndef STATUS_LED_MODE
    #define STATUS_LED_MODE                     LED_MODE_WIFI
#endif
