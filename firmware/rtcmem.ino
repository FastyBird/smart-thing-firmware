/*

RTCMEM MODULE

Copyright (C) 2018 FastyBird s.r.o. <info@fastybird.com>

*/

bool _rtcmem_status = false;

// -----------------------------------------------------------------------------
// MODULE PRIVATE
// -----------------------------------------------------------------------------

void _rtcmemErase() {
    auto ptr = reinterpret_cast<volatile uint32_t*>(RTCMEM_ADDR);
    const auto end = ptr + RTCMEM_BLOCKS;

    DEBUG_MSG(PSTR("[RTCMEM] Erasing start=%p end=%p\n"), ptr, end);

    do {
        *ptr = 0;
    } while (++ptr != end);
}

// -----------------------------------------------------------------------------

void _rtcmemInit() {
    _rtcmemErase();

    Rtcmem->magic = RTCMEM_MAGIC;
}

// -----------------------------------------------------------------------------

// Treat memory as dirty on cold boot, hardware wdt reset and rst pin
bool _rtcmemStatus() {
    bool readable;

    switch (resetReasonSystem()) {
        case REASON_EXT_SYS_RST:
        case REASON_WDT_RST:
        case REASON_DEFAULT_RST:
            readable = false;
            break;

        default:
            readable = true;
    }

    readable = readable and (RTCMEM_MAGIC == Rtcmem->magic);

    return readable;
}

// -----------------------------------------------------------------------------
// MODULE API
// -----------------------------------------------------------------------------

bool rtcmemStatus() {
    return _rtcmem_status;
}

// -----------------------------------------------------------------------------
// MODULE CORE
// -----------------------------------------------------------------------------

void rtcmemSetup() {
    _rtcmem_status = _rtcmemStatus();

    if (!_rtcmem_status) {
        _rtcmemInit();
    }
}
