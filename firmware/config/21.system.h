/*

SYSTEM MODULE CONFIGURATION

Copyright (C) 2018 FastyBird s.r.o. <code@fastybird.com>

*/

#pragma once

// =============================================================================
// MODULE DEFAULTS
// =============================================================================

#ifndef SYSTEM_RESET_BTN_INDEX
    #define SYSTEM_RESET_BTN_INDEX              INDEX_NONE
#endif

#ifndef SYSTEM_RESET_BTN_EVENT
    #define SYSTEM_RESET_BTN_EVENT              BUTTON_EVENT_LNG_CLICK
#endif

// =============================================================================
// MODULE PROTOTYPES
// =============================================================================

typedef std::function<void()> system_on_heartbeat_callback_t;
void systemOnHeartbeatRegister(system_on_heartbeat_callback_t callback);
