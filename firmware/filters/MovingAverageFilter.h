/*

MOVING AVERAGE FILTER

Copyright (C) 2018 FastyBird s.r.o. <info@fastybird.com>

*/

#if SENSOR_SUPPORT

#pragma once

#include <vector>
#include "BaseFilter.h"

class MovingAverageFilter : public BaseFilter {

    public:

        void add(double value) {
            _sum = _sum + value - _data[_pointer];
            _data[_pointer] = value;
            _pointer = (_pointer + 1) % _size;
        }

        uint8_t count() {
            return _pointer;
        }

        void reset() {}

        double result() {
            return _sum;
        }

        void resize(uint8_t size) {
            if (_size == size) {
                return;
            }

            _size = size;

            if (_data) {
                delete _data;
            }

            _data = new double[_size];

            for (uint8_t i = 0; i < _size; i++) {
                _data[i] = 0;
            }

            _pointer = 0;
            _sum = 0;
        }

    protected:

        uint8_t _pointer = 0;
        double _sum = 0;
        double * _data = NULL;

};

#endif // SENSOR_SUPPORT
