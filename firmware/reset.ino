/*

RESET MODULE

Copyright (C) 2018 FastyBird Ltd. <info@fastybird.com>

*/

#include <Ticker.h>
#include <EEPROM_Rotate.h>

Ticker _defer_reset;

uint8_t _reset_reason = 0;

// -----------------------------------------------------------------------------
// MODULE API
// -----------------------------------------------------------------------------

unsigned int resetReason() {
    static unsigned int status = 255;

    if (status == 255) {
        status = EEPROMr.read(EEPROM_CUSTOM_RESET);

        if (status > 0) {
            resetReason(0);
        }

        if (status > CUSTOM_RESET_MAX) {
            status = 0;
        }
    }

    return status;
}

// -----------------------------------------------------------------------------

void resetReason(
    unsigned int reason
) {
    _reset_reason = reason;

    EEPROMr.write(EEPROM_CUSTOM_RESET, reason);

    eepromCommit();
}

// -----------------------------------------------------------------------------

void reset() {
    ESP.restart();
}

// -----------------------------------------------------------------------------

void deferredReset(
    unsigned long delay,
    unsigned int reason
) {
    _defer_reset.once_ms(delay, resetReason, reason);
}

// -----------------------------------------------------------------------------

bool checkNeedsReset() {
    return _reset_reason > 0;
}
