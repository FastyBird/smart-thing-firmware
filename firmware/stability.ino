/*

STABILTY MODULE

Copyright (C) 2018 FastyBird Ltd. <info@fastybird.com>

*/

#if STABILTY_CHECK_ENABLED

#include <EEPROM_Rotate.h>

bool _stability_system_stable = true;

// -----------------------------------------------------------------------------
// MODULE API
// -----------------------------------------------------------------------------

// Call this method on boot with start=true to increase the crash counter
// Call it again once the system is stable to decrease the counter
// If the counter reaches STABILTY_CHECK_MAX then the system is flagged as unstable
// _stability_system_stable = false;
//
// An unstable system will only have serial access, WiFi in AP mode and OTA

void stabiltyCheck(
    bool stable
) {
    unsigned int value = EEPROMr.read(EEPROM_CRASH_COUNTER);

    if (stable) {
        value = 0;

        DEBUG_MSG(PSTR("[STABILTY] System OK\n"));

    } else {
        if (++value > STABILTY_CHECK_MAX) {
            _stability_system_stable = false;
            value = 0;

            DEBUG_MSG(PSTR("[STABILTY] System UNSTABLE\n"));
        }
    }

    EEPROMr.write(EEPROM_CRASH_COUNTER, value);

    eepromCommit();
}

// -----------------------------------------------------------------------------

bool stabiltyCheck() {
    return _stability_system_stable;
}

// -----------------------------------------------------------------------------
// MODULE CORE
// -----------------------------------------------------------------------------

void stabilitySetup() {
    // Register loop
    firmwareRegisterLoop(stabiltyLoop);
}

// -----------------------------------------------------------------------------

void stabiltyLoop() {
    static bool checked = false;

    if (!checked && millis() > STABILTY_CHECK_TIME) {
        // Check system as stable
        stabiltyCheck(true);

        checked = true;
    }
}

#endif // STABILTY_CHECK_ENABLED
