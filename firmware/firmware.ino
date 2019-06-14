/*

CORE MODULE

Copyright (C) 2018 FastyBird Ltd. <info@fastybird.com>

*/

#include "config/all.h"

#include <Arduino.h>
#include <Hash.h>
#include <vector>

std::vector<void (*)()> _firmware_loop_callbacks;
std::vector<void (*)()> _firmware_reload_callbacks;

// -----------------------------------------------------------------------------
// MODULE API
// -----------------------------------------------------------------------------

void firmwareRegisterLoop(
    void (*callback)()
) {
    _firmware_loop_callbacks.push_back(callback);
}

// -----------------------------------------------------------------------------

void firmwareRegisterReload(
    void (*callback)()
) {
    _firmware_reload_callbacks.push_back(callback);
}

// -----------------------------------------------------------------------------

void firmwareReload() {
    for (uint8_t i = 0; i < _firmware_reload_callbacks.size(); i++) {
        (_firmware_reload_callbacks[i])();
    }
}

// -----------------------------------------------------------------------------
// BOOTING
// -----------------------------------------------------------------------------

void setup() {

    // -------------------------------------------------------------------------
    // Basic modules, will always run
    // -------------------------------------------------------------------------

    // Cache initial free heap value
    getInitialFreeHeap();

    // Serial debug
    #if DEBUG_SUPPORT
        debugSetup();
    #endif

    // Init EEPROM
    eepromSetup();

    #if STABILTY_CHECK_ENABLED
        // Init stabilty check
        stabilitySetup();
    #endif

    #if BUTTON_SUPPORT
        buttonSetup();
    #endif

    // Init Serial, SPIFFS
    systemSetup();

    // Init persistance and terminal features
    settingsSetup();

    #if WIFI_SUPPORT
        wifiSetup();
    #endif

    // -------------------------------------------------------------------------
    // Check if system is stable
    // -------------------------------------------------------------------------

    #if STABILTY_CHECK_ENABLED
        if (!stabiltyCheck()) {
            return;
        }
    #endif

    // -------------------------------------------------------------------------
    // Next modules will be only loaded if system is flagged as stable
    // -------------------------------------------------------------------------

    #if SPIFFS_SUPPORT
        storageSetup();
    #endif

    // Init webserver required before any module that uses API
    #if WEB_SUPPORT
        webSetup();

        #if WS_SUPPORT
            wsSetup();
        #endif
    #endif

    crashSetup();

    #if LED_SUPPORT
        ledSetup();
    #endif

    #if RELAY_PROVIDER != RELAY_PROVIDER_NONE
        relaySetup();
    #endif

    #if MQTT_SUPPORT
        mqttSetup();
    #endif

    #if NTP_SUPPORT
        ntpSetup();
    #endif
    
    #if FASTYBIRD_SUPPORT
        fastybirdSetup();
    #endif

    #if NODES_GATEWAY_SUPPORT
        gatewaySetup();
    #endif

    #if NOFUSS_SUPPORT
        nofussSetup();
    #endif

    #if SCHEDULER_SUPPORT
        schSetup();
    #endif
}

void loop() {
    // Call registered loop callbacks
    for (uint8_t i = 0; i < _firmware_loop_callbacks.size(); i++) {
        (_firmware_loop_callbacks[i])();
    }
}
