/*

SETTINGS MODULE CONFIGURATION

Copyright (C) 2018 FastyBird s.r.o. <code@fastybird.com>

*/

#pragma once

#include <Embedis.h>

// =============================================================================
// MODULE DEFAULTS
// =============================================================================

#ifndef SETTINGS_FACTORY_BTN_INDEX
    #define SETTINGS_FACTORY_BTN_INDEX          INDEX_NONE
#endif

#ifndef SETTINGS_FACTORY_BTN_EVENT
    #define SETTINGS_FACTORY_BTN_EVENT          BUTTON_EVENT_LNG_LNG_CLICK
#endif

#ifndef SETTINGS_AUTOSAVE
    #define SETTINGS_AUTOSAVE                   1                                   // Autosave settings or force manual commit
#endif

#define SETTINGS_MAX_LIST_COUNT                 10                                  // Maximum index for settings lists

// =============================================================================
// MODULE PROTOTYPES
// =============================================================================

template<typename T> bool setSetting(const String& key, T value);
template<typename T> bool setSetting(const String& key, unsigned int index, T value);
template<typename T> String getSetting(const String& key, T defaultValue);
template<typename T> String getSetting(const String& key, unsigned int index, T defaultValue);
