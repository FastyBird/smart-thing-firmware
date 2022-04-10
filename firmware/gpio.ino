/*

GPIO MODULE

Copyright (C) 2018 FastyBird s.r.o. <code@fastybird.com>

*/

unsigned int _gpio_locked = 0;

// -----------------------------------------------------------------------------
// MODULE API
// -----------------------------------------------------------------------------

bool gpioValid(
    uint8_t gpio
) {
    if (gpio <= 5) {
        return true;
    }

    if (12 <= gpio && gpio <= 15) {
        return true;
    }

    return false;
}

// -----------------------------------------------------------------------------

bool gpioGetLock(
    uint8_t gpio
) {
    if (gpioValid(gpio)) {
        unsigned int mask = 1 << gpio;

        if ((_gpio_locked & mask) == 0) {
            _gpio_locked |= mask;

            DEBUG_MSG(PSTR("[INFO][GPIO] GPIO%u locked\n"), gpio);

            return true;
        }
    }

    DEBUG_MSG(PSTR("[INFO][GPIO] Failed getting lock for GPIO%u\n"), gpio);

    return false;
}

// -----------------------------------------------------------------------------

bool gpioReleaseLock(
    uint8_t gpio
) {
    if (gpioValid(gpio)) {
        unsigned int mask = 1 << gpio;

        _gpio_locked &= ~mask;

        DEBUG_MSG(PSTR("[INFO][GPIO] GPIO%u lock released\n"), gpio);

        return true;
    }

    DEBUG_MSG(PSTR("[INFO][GPIO] Failed releasing lock for GPIO%u\n"), gpio);

    return false;
}
