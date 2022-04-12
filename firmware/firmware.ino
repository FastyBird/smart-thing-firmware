/*

CORE MODULE

Copyright (C) 2018 FastyBird s.r.o. <code@fastybird.com>

*/

#include "config/all.h"

#include <Hash.h>

std::vector<void (*)()> _firmware_loop_callbacks;
std::vector<void (*)()> _firmware_reload_callbacks;

Ticker _stability_ticker;

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

void firmwareReload()
{
    for (uint8_t i = 0; i < _firmware_reload_callbacks.size(); i++) {
        (_firmware_reload_callbacks[i])();
    }
}

// -----------------------------------------------------------------------------
// BOOTING
// -----------------------------------------------------------------------------

void setup()
{
    // -------------------------------------------------------------------------
    // Basic modules, will always run
    // -------------------------------------------------------------------------

    // Cache initial free heap value
    getInitialFreeHeap();

    // Serial debug
    #if DEBUG_SUPPORT
        debugSetup();
    #endif

    // Init RTCMEM
    rtcmemSetup();

    // Init EEPROM
    eepromSetup();

    #if STABILTY_CHECK_ENABLED
        // Init stabilty check
        stabilitySetup();
    #endif

    #if FASTYBIRD_SUPPORT
        fastybirdSetup();
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
            _stability_ticker.once_ms(500, reset);

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
    #endif

    crashSetup();

    #if WS_SUPPORT
        wsSetup();
    #endif

    #if MQTT_SUPPORT
        mqttSetup();
    #endif
    
    #if LED_SUPPORT
        ledSetup();
    #endif

    #if BUTTON_SUPPORT
        buttonSetup();
    #endif

    #if RELAY_PROVIDER != RELAY_PROVIDER_NONE
        relaySetup();
    #endif

    #if VIRTUAL_BTN_SUPPORT
        virtualButtonSetup();
    #endif

    #if SENSOR_SUPPORT
        sensorSetup();
    #endif

    // -------------------------------------------------------------------------
    // Modules relations setup
    // -------------------------------------------------------------------------

    #if BUTTON_SUPPORT && FASTYBIRD_SUPPORT
        fastyBirdButtonSetup();
    #endif

    #if RELAY_PROVIDER != RELAY_PROVIDER_NONE && FASTYBIRD_SUPPORT
        fastyBirdRelaySetup();
    #endif
}

void loop()
{
    // Call registered loop callbacks
    for (uint8_t i = 0; i < _firmware_loop_callbacks.size(); i++) {
        (_firmware_loop_callbacks[i])();
    }
}
