/*

BUTTON MODULE CONFIGURATION

Copyright (C) 2018 FastyBird s.r.o. <code@fastybird.com>

*/

#pragma once

#include <DebounceEvent.h>

// =============================================================================
// MODULE DEFAULTS
// =============================================================================

#ifndef BUTTON_SUPPORT
    #define BUTTON_SUPPORT                      0
#endif

#ifndef BUTTON_DEBOUNCE_DELAY
    #define BUTTON_DEBOUNCE_DELAY               50                                  // Debounce delay (ms)
#endif

#ifndef BUTTON_DBL_CLICK_DELAY
    #define BUTTON_DBL_CLICK_DELAY              500                                 // Time in ms to wait for a second (or third...) click
#endif

#ifndef BUTTON_DEBOUNCE_DBL_CLICK_MIN
    #define BUTTON_DEBOUNCE_DBL_CLICK_MIN       0
#endif

#ifndef BUTTON_DEBOUNCE_DBL_CLICK_MAX
    #define BUTTON_DEBOUNCE_DBL_CLICK_MAX       1000
#endif

#ifndef BUTTON_DEBOUNCE_DBL_CLICK_STEP
    #define BUTTON_DEBOUNCE_DBL_CLICK_STEP      100
#endif

#ifndef BUTTON_LNG_CLICK_DELAY
    #define BUTTON_LNG_CLICK_DELAY              1000                                // Time in ms holding the button down to get a long click
#endif

#ifndef BUTTON_LNG_LNG_CLICK_DELAY
    #define BUTTON_LNG_LNG_CLICK_DELAY          10000                               // Time in ms holding the button down to get a long-long click
#endif

// =============================================================================
// MODULE TYPES
// =============================================================================

#define BUTTON_EVENT_NONE                       0
#define BUTTON_EVENT_PRESSED                    1
#define BUTTON_EVENT_RELEASED                   2
#define BUTTON_EVENT_CLICK                      2
#define BUTTON_EVENT_DBL_CLICK                  3
#define BUTTON_EVENT_TRIPLE_CLICK               4
#define BUTTON_EVENT_LNG_CLICK                  5
#define BUTTON_EVENT_LNG_LNG_CLICK              6

// -----------------------------------------------------------------------------
// Needed for ESP8285 boards under Windows using PlatformIO (?)
// -----------------------------------------------------------------------------

#ifndef BUTTON_PUSHBUTTON
    #define BUTTON_PUSHBUTTON                   0
    #define BUTTON_SWITCH                       1
    #define BUTTON_DEFAULT_HIGH                 2
    #define BUTTON_SET_PULLUP                   4
#endif

// =============================================================================
// MODULE PROTOTYPES
// =============================================================================

typedef std::function<void(unsigned int)> button_on_event_callback_t;

typedef struct {
    DebounceEvent * button;
    uint8_t channel_index;
    std::vector<button_on_event_callback_t> callbacks;
} button_t;

void buttonOnEventRegister(button_on_event_callback_t callback, unsigned int button);

// =============================================================================
// MODULE DEFAULTS
// =============================================================================

#ifndef BUTTON1_PIN
    #define BUTTON1_PIN                         GPIO_NONE
#endif

#ifndef BUTTON2_PIN
    #define BUTTON2_PIN                         GPIO_NONE
#endif

#ifndef BUTTON3_PIN
    #define BUTTON3_PIN                         GPIO_NONE
#endif

#ifndef BUTTON4_PIN
    #define BUTTON4_PIN                         GPIO_NONE
#endif

#ifndef BUTTON5_PIN
    #define BUTTON5_PIN                         GPIO_NONE
#endif

#ifndef BUTTON6_PIN
    #define BUTTON6_PIN                         GPIO_NONE
#endif

#ifndef BUTTON7_PIN
    #define BUTTON7_PIN                         GPIO_NONE
#endif

#ifndef BUTTON8_PIN
    #define BUTTON8_PIN                         GPIO_NONE
#endif
