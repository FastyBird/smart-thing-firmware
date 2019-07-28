// -----------------------------------------------------------------------------
// Digital Sensor (maps to a digitalRead)
// Copyright (C) 2017-2019 by Xose Pérez <xose dot perez at gmail dot com>
// -----------------------------------------------------------------------------

#if SENSOR_SUPPORT && DIGITAL_SUPPORT

#pragma once

#include "Arduino.h"
#include "BaseSensor.h"

class DigitalSensor : public BaseSensor {

    public:

        // ---------------------------------------------------------------------
        // Public
        // ---------------------------------------------------------------------

        DigitalSensor(): BaseSensor() {
            _count = 1;
            _sensor_id = SENSOR_DIGITAL_ID;
        }

        // ---------------------------------------------------------------------

        void setGPIO(uint8_t gpio) {
            _gpio = gpio;
        }

        void setMode(uint8_t mode) {
            _mode = mode;
        }

        void setDefault(bool value) {
            _default = value;
        }

        // ---------------------------------------------------------------------

        uint8_t getGPIO() {
            return _gpio;
        }

        uint8_t getMode() {
            return _mode;
        }

        bool getDefault() {
            return _default;
        }

        // ---------------------------------------------------------------------
        // Sensor API
        // ---------------------------------------------------------------------

        // Initialization method, must be idempotent
        void begin() {
            pinMode(_gpio, _mode);
            _ready = true;
        }

        // Descriptive name of the sensor
        String description() {
            char buffer[20];
            snprintf(buffer, sizeof(buffer), "DIGITAL @ GPIO%d", _gpio);
            return String(buffer);
        }

        // Descriptive name of the slot # index
        String slot(uint8_t index) {
            return description();
        };

        // Address of the sensor (it could be the GPIO or I2C address)
        String address(uint8_t index) {
            return String(_gpio);
        }

        // Type for slot # index
        uint8_t type(uint8_t index) {
            if (index == 0) return MAGNITUDE_DIGITAL;
            return MAGNITUDE_NONE;
        }

        // Current value for slot # index
        double value(uint8_t index) {
            if (index == 0) return (digitalRead(_gpio) == _default) ? 0 : 1;
            return 0;
        }


    protected:

        // ---------------------------------------------------------------------
        // Protected
        // ---------------------------------------------------------------------

        uint8_t _gpio;
        uint8_t _mode;
        bool _default = false;

};

#endif // SENSOR_SUPPORT && DIGITAL_SUPPORT
