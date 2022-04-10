/*

BASE FILTER

Copyright (C) 2018 FastyBird s.r.o. <code@fastybird.com>

*/

#if SENSOR_SUPPORT

#pragma once

class BaseFilter {

    public:
        virtual void add(double value);
        virtual uint8_t count();
        virtual void reset();
        virtual double result();
        virtual void resize(uint8_t size);
        uint8_t size() { return _size; };

    protected:
        uint8_t _size;

};

#endif // SENSOR_SUPPORT
