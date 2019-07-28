// -----------------------------------------------------------------------------
// MICS-2710 (and MICS-4514) NO2 Analog Sensor
// Copyright (C) 2019 by Xose Pérez <xose dot perez at gmail dot com>
// -----------------------------------------------------------------------------

#if SENSOR_SUPPORT && MICS2710_SUPPORT

#pragma once

// Set ADC to TOUT pin
#undef ADC_MODE_VALUE
#define ADC_MODE_VALUE ADC_TOUT

#include "Arduino.h"
#include "BaseSensor.h"

extern "C" {
    #include "../libs/fs_math.h"
}

class MICS2710Sensor : public BaseSensor {

    public:

        // ---------------------------------------------------------------------
        // Public
        // ---------------------------------------------------------------------

        MICS2710Sensor(): BaseSensor() {
            _count = 2;
            _sensor_id = SENSOR_MICS2710_ID;
        }

        void calibrate() {
            setR0(_getResistance());
        }

        // ---------------------------------------------------------------------

        void setAnalogGPIO(uint8_t gpio) {
            _noxGPIO = gpio;
        }

        uint8_t getAnalogGPIO() {
            return _noxGPIO;
        }

        void setPreHeatGPIO(uint8_t gpio) {
            _preGPIO = gpio;
        }

        uint8_t getPreHeatGPIO() {
            return _preGPIO;
        }

        void setRL(uint32_t Rl) {
            if (Rl > 0) _Rl = Rl;
        }

        uint32_t getRL() {
            return _Rl;
        }

        void setR0(uint32_t R0) {
            if (R0 > 0) _R0 = R0;
        }

        uint32_t getR0() {
            return _R0;
        }

        // ---------------------------------------------------------------------
        // Sensor API
        // ---------------------------------------------------------------------

        // Initialization method, must be idempotent
        void begin() {

            // Set NOX as analog input
            pinMode(_noxGPIO, INPUT);

            // Start pre-heating
            pinMode(_preGPIO, OUTPUT);
            digitalWrite(_preGPIO, HIGH);
            _heating = true;
            _start = millis();

            _ready = true;

        }

        // Pre-read hook (usually to populate registers with up-to-date data)
        void pre() {

            // Check pre-heat time
            if (_heating && (millis() - _start > MICS2710_PREHEAT_TIME)) {
                digitalWrite(_preGPIO, LOW);
                _heating = false;
            }

            if (_ready) {
                _Rs = _getResistance();
            }

        }

        // Descriptive name of the sensor
        String description() {
            return String("MICS-2710 @ TOUT");
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
            if (0 == index) return MAGNITUDE_RESISTANCE;
            if (1 == index) return MAGNITUDE_NO2;
            return MAGNITUDE_NONE;
        }

        // Current value for slot # index
        double value(uint8_t index) {
            if (0 == index) return _Rs;
            if (1 == index) return _getPPM();
            return 0;
        }

    private:

        uint32_t _getReading() {
            return analogRead(_noxGPIO);
        }

        double _getResistance() {

            // get voltage (1 == reference) from analog pin
            double voltage = (float) _getReading() / 1024.0;

            // schematic: 3v3 - Rs - P - Rl - GND
            // V(P) = 3v3 * Rl / (Rs + Rl)
            // Rs = 3v3 * Rl / V(P) - Rl = Rl * ( 3v3 / V(P) - 1)
            // 3V3 voltage is cancelled
            double resistance = (voltage > 0) ? _Rl * ( 1 / voltage - 1 ) : 0;

            return resistance;

        }

        double _getPPM() {

            // According to the datasheet (https://www.cdiweb.com/datasheets/e2v/mics-2710.pdf)
            // there is an almost linear relation between log(Rs/R0) and log(ppm).
            // Regression parameters have been calculated based on the graph
            // in the datasheet with these readings:
            //
            // Rs/R0    NO2(ppm)
            // 23       0.20
            // 42       0.30
            // 90       0.40
            // 120      0.50
            // 200      0.60
            // 410      0.90
            // 500      1.00
            // 1000     1.30
            // 10000	5.00

            return fs_pow(10, 0.5170 * fs_log10(_Rs / _R0) - 1.3954);

        }

        bool _heating = false;
        uint32_t _start = 0;                   // monitors the pre-heating time
        uint32_t _R0 = MICS2710_R0;            // R0, calikbration value at 25º
        uint32_t _Rl = MICS2710_RL;            // RL, load resistance
        uint32_t _Rs = 0;                      // cached resistance
        uint8_t _noxGPIO = MICS2710_PRE_PIN;
        uint8_t _preGPIO = MICS2710_NOX_PIN;

};

#endif // SENSOR_SUPPORT && MICS2710_SUPPORT
