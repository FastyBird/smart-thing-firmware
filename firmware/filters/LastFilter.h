/*

LAST FILTER

Copyright (C) 2018 FastyBird Ltd. <info@fastybird.com>

*/

#if SENSOR_SUPPORT

#pragma once

#include "BaseFilter.h"

class LastFilter : public BaseFilter {

    public:

        void add(double value) {
            _value = value;
        }

        uint8_t count() {
            return 1;
        }

        void reset() {
            _value = 0;
        }

        double result() {
            return _value;
        }

        void resize(uint8_t size) {}

    protected:

        double _value = 0;

};

#endif // SENSOR_SUPPORT
