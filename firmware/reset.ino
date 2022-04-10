/*

RESET MODULE

Copyright (C) 2018 FastyBird s.r.o. <code@fastybird.com>

*/

Ticker _defer_reset;

uint8_t _reset_reason = 0;

union reset_rtcmem_t {
    struct {
        uint8_t reset_reason;
        uint16_t _reserved_;
    } parts;
    uint32_t value;
};

// -----------------------------------------------------------------------------
// MODULE PRIVATE
// -----------------------------------------------------------------------------

uint8_t _resetReason()
{
    reset_rtcmem_t data;

    data.value = Rtcmem->sys;

    return data.parts.reset_reason;
}

// -----------------------------------------------------------------------------

void _resetReason(
    uint8_t reason
) {
    reset_rtcmem_t data;

    data.value = Rtcmem->sys;
    data.parts.reset_reason = reason;

    Rtcmem->sys = data.value;
}

// -----------------------------------------------------------------------------
// MODULE API
// -----------------------------------------------------------------------------

// system_get_rst_info() result is cached by the Core init for internal use
uint32_t resetReasonSystem()
{
    return resetInfo.reason;
}

// -----------------------------------------------------------------------------

uint8_t resetReason()
{
    static uint8_t status = 255;

    if (status == 255) {
        if (rtcmemStatus()) {
            status = _resetReason();
        }

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
    uint8_t reason
) {
    _reset_reason = reason;

    _resetReason(reason);
}

// -----------------------------------------------------------------------------

void reset()
{
    ESP.restart();
}

// -----------------------------------------------------------------------------

void deferredReset(
    uint32_t delay,
    uint8_t reason
) {
    _defer_reset.once_ms(delay, resetReason, reason);
}

// -----------------------------------------------------------------------------

bool checkNeedsReset()
{
    return _reset_reason > 0;
}
