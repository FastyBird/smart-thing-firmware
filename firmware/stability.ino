/*

STABILTY MODULE

Copyright (C) 2018 FastyBird s.r.o. <code@fastybird.com>

*/

#if STABILTY_CHECK_ENABLED

bool _stability_system_stable = true;

union stabilty_rtcmem_t {
    struct {
        uint8_t stability_counter;
        uint16_t _reserved_;
    } parts;
    uint32_t value;
};

uint8_t _stabilityCounter()
{
    stabilty_rtcmem_t data;

    data.value = Rtcmem->sys;

    return data.parts.stability_counter;
}

void _stabilityCounter(
    uint8_t counter
) {
    stabilty_rtcmem_t data;

    data.value = Rtcmem->sys;
    data.parts.stability_counter = counter;

    Rtcmem->sys = data.value;
}

// -----------------------------------------------------------------------------
// MODULE API
// -----------------------------------------------------------------------------

// Call this method on boot with start=true to increase the crash counter
// Call it again once the system is stable to decrease the counter
// If the counter reaches STABILTY_CHECK_MAX then the system is flagged as unstable
// _stability_system_stable = false;
//
// An unstable system will only have serial access, WiFi in AP mode

void stabiltyCheck(
    const bool stable
) {
    uint8_t value = EEPROMr.read(EEPROM_CRASH_COUNTER);

    if (stable) {
        value = 0;

        DEBUG_MSG(PSTR("[INFO][STABILTY] System OK\n"));

    } else {
        if (!rtcmemStatus()) {
            _stabilityCounter(1);

            return;
        }

        value = _stabilityCounter();

        if (++value > STABILTY_CHECK_MAX) {
            _stability_system_stable = false;
            value = 0;

            DEBUG_MSG(PSTR("[INFO][STABILTY] System UNSTABLE\n"));
        }
    }

    _stabilityCounter(value);
}

// -----------------------------------------------------------------------------

bool stabiltyCheck()
{
    return _stability_system_stable;
}

// -----------------------------------------------------------------------------
// MODULE CORE
// -----------------------------------------------------------------------------

void stabilitySetup()
{
    // Register loop
    firmwareRegisterLoop(stabiltyLoop);
}

// -----------------------------------------------------------------------------

void stabiltyLoop()
{
    static bool checked = false;

    if (!checked && millis() > STABILTY_CHECK_TIME) {
        // Check system as stable
        stabiltyCheck(true);

        checked = true;
    }
}

#endif // STABILTY_CHECK_ENABLED
