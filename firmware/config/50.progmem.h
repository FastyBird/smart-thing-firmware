/*

PROGMEM DEFINITIONS

Copyright (C) 2018 FastyBird s.r.o. <code@fastybird.com>

*/

#pragma once

// =============================================================================
// Reset reasons
// =============================================================================

const char custom_reset_web[] PROGMEM       = "Reboot from web interface";
const char custom_reset_broker[] PROGMEM    = "Reboot from MQTT";
const char custom_reset_button[] PROGMEM    = "Reboot by button";
const char custom_factory_web[] PROGMEM     = "Reboot by factory reset from web interface";
const char custom_factory_broker[] PROGMEM  = "Reboot by factory reset from MQTT";
const char custom_factory_button[] PROGMEM  = "Reboot by factory reset by bytton";
const char custom_upgrade_web[] PROGMEM     = "Reboot after successful web update";
const char custom_restore_web[] PROGMEM     = "Reboot by settings restore from web interface";

PROGMEM const char * const custom_reset_string[] = {
    custom_reset_web, custom_reset_broker, custom_reset_button,
    custom_factory_web, custom_factory_broker, custom_factory_button,
    custom_upgrade_web, custom_restore_web
};

// =============================================================================
// Capabilities
// =============================================================================

const char firmware_modules[] PROGMEM =
    #if RELAY_PROVIDER != RELAY_PROVIDER_NONE
        "RELAY "
    #endif
    #if BUTTON_SUPPORT
        "BUTTON "
    #endif
    #if DEBUG_SERIAL_SUPPORT
        "DEBUG_SERIAL "
    #endif
    #if DEBUG_WEB_SUPPORT
        "DEBUG_WEB "
    #endif
    #if DEBUG_MQTT_SUPPORT
        "DEBUG_MQTT "
    #endif
    #if I2C_SUPPORT
        "I2C "
    #endif
    #if LED_SUPPORT
        "LED "
    #endif
    #if MQTT_SUPPORT
        "MQTT "
    #endif
    #if SENSOR_SUPPORT
        "SENSOR "
    #endif
    #if SPIFFS_SUPPORT
        "SPIFFS "
    #endif
    #if WIFI_SUPPORT
        "WIFI "
    #endif
    #if WEB_SUPPORT
        "WEB "
    #endif
    #if WS_SUPPORT
        "WS "
    #endif
    #if FASTYBIRD_SUPPORT
        "FASTYBIRD "
    #endif
    "";
