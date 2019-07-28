/*

MAX FILTER

Copyright (C) 2018 FastyBird s.r.o. <info@fastybird.com>

*/

#if SENSOR_SUPPORT

#pragma once

#include "BaseFilter.h"

class MaxFilter : public BaseFilter {

    public:

        void add(double value) {
            if (value > _max) _max = value;
        }

        uint8_t count() {
            return 1;
        }

        void reset() {
            _max = 0;
        }

        double result() {
            return _max;
        }

        void resize(uint8_t size) {}

    protected:

        double _max = 0;

};

#endif // SENSOR_SUPPORT
