// -----------------------------------------------------------------------------
// TMP3X Temperature Analog Sensor
// Copyright (C) 2019 by Xose Pérez <xose dot perez at gmail dot com>
// -----------------------------------------------------------------------------

#if SENSOR_SUPPORT && TMP3X_SUPPORT

#pragma once

// Set ADC to TOUT pin
#undef ADC_MODE_VALUE
#define ADC_MODE_VALUE ADC_TOUT

#include "Arduino.h"
#include "BaseSensor.h"

#define TMP3X_TMP35                 35
#define TMP3X_TMP36                 36
#define TMP3X_TMP37                 37

class TMP3XSensor : public BaseSensor {

    public:

        // ---------------------------------------------------------------------
        // Public
        // ---------------------------------------------------------------------

        TMP3XSensor(): BaseSensor() {
            _count = 1;
            _sensor_id = SENSOR_TMP3X_ID;
        }

        void setType(uint8_t type) {
            if (35 <= type && type <= 37) {
              _type = type;
            }
        }

        uint8_t getType() {
            return _type;
        }

        // ---------------------------------------------------------------------
        // Sensor API
        // ---------------------------------------------------------------------

        // Initialization method, must be idempotent
        void begin() {
            pinMode(0, INPUT);
            _ready = true;
        }

        // Descriptive name of the sensor
        String description() {
            char buffer[14];
            snprintf(buffer, sizeof(buffer), "TMP%d @ TOUT", _type);
            return String(buffer);
        }

        // Descriptive name of the slot # index
        String slot(uint8_t index) {
            return description();
        };

        // Address of the sensor (it could be the GPIO or I2C address)
        String address(uint8_t index) {
            return String("0");
        }

        // Type for slot # index
        uint8_t type(uint8_t index) {
            if (index == 0) return MAGNITUDE_TEMPERATURE;
            return MAGNITUDE_NONE;
        }

        // Current value for slot # index
        double value(uint8_t index) {
            if (index == 0) {
                double mV = 3300.0 * analogRead(0) / 1024.0;
                if (_type == TMP3X_TMP35) return mV / 10.0;
                if (_type == TMP3X_TMP36) return mV / 10.0 - 50.0;
                if (_type == TMP3X_TMP37) return mV / 20.0;
            }
            return 0;
        }

    private:

        uint8_t _type = TMP3X_TMP35;

};

#endif // SENSOR_SUPPORT && TMP3X_SUPPORT
