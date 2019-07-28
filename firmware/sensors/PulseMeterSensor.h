// -----------------------------------------------------------------------------
// Pulse Meter Power Monitor Sensor
// Copyright (C) 2019 by Xose Pérez <xose dot perez at gmail dot com>
// -----------------------------------------------------------------------------

#if SENSOR_SUPPORT && PULSEMETER_SUPPORT

#pragma once

#include "Arduino.h"
#include "BaseSensor.h"

class PulseMeterSensor : public BaseSensor {

    public:

        // ---------------------------------------------------------------------
        // Public
        // ---------------------------------------------------------------------

        PulseMeterSensor(): BaseSensor() {
            _count = 2;
            _sensor_id = SENSOR_PULSEMETER_ID;
        }

        ~PulseMeterSensor() {
            _enableInterrupts(false);
        }

        void resetEnergy(double value = 0) {
            _energy = value;
        }

        // ---------------------------------------------------------------------

        void setGPIO(uint8_t gpio) {
            if (_gpio == gpio) return;
            _gpio = gpio;
            _dirty = true;
        }

        void setEnergyRatio(uint32_t ratio) {
            if (ratio > 0) _ratio = ratio;
        }

        void setDebounceTime(uint32_t debounce) {
            _debounce = debounce;
        }

        // ---------------------------------------------------------------------

        uint8_t getGPIO() {
            return _gpio;
        }

        uint32_t getEnergyRatio() {
            return _ratio;
        }

        uint32_t getDebounceTime() {
            return _debounce;
        }

        // ---------------------------------------------------------------------
        // Sensors API
        // ---------------------------------------------------------------------

        // Initialization method, must be idempotent
        // Defined outside the class body
        void begin() {

            _enableInterrupts(true);
            _ready = true;

        }

        // Descriptive name of the sensor
        String description() {
            char buffer[24];
            snprintf(buffer, sizeof(buffer), "PulseMeter @ GPIO(%u)", _gpio);
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

        // Pre-read hook (usually to populate registers with up-to-date data)
        void pre() {

            uint32_t lapse = millis() - _previous_time;
            _previous_time = millis();
            uint32_t pulses = _pulses - _previous_pulses;
            _previous_pulses = _pulses;

            uint32_t _energy_delta = 1000 * 3600 * pulses / _ratio;
            _energy += _energy_delta;
            if (lapse > 0) _active = 1000 * _energy_delta / lapse;

        }

        // Type for slot # index
        uint8_t type(uint8_t index) {
            if (index == 0) return MAGNITUDE_POWER_ACTIVE;
            if (index == 1) return MAGNITUDE_ENERGY;
            return MAGNITUDE_NONE;
        }

        // Current value for slot # index
        double value(uint8_t index) {
            if (index == 0) return _active;
            if (index == 1) return _energy;
            return 0;
        }

        // Handle interrupt calls
        void ICACHE_RAM_ATTR handleInterrupt(uint8_t gpio) {
            static uint32_t last = 0;

            if (millis() - last > _debounce) {
                last = millis();
                _pulses++;
            }
        }

    protected:

        // ---------------------------------------------------------------------
        // Interrupt management
        // ---------------------------------------------------------------------

        void _attach(PulseMeterSensor * instance, uint8_t gpio, uint8_t mode);
        void _detach(uint8_t gpio);

        void _enableInterrupts(bool value) {

            if (value) {

                if (_gpio != _previous) {
                    if (_previous != GPIO_NONE) _detach(_previous);
                    _attach(this, _gpio, PULSEMETER_INTERRUPT_ON);
                    _previous = _gpio;
                }

            } else {

                _detach(_previous);
                _previous = GPIO_NONE;

            }

        }

        // ---------------------------------------------------------------------

        uint8_t _previous = GPIO_NONE;
        uint8_t _gpio = GPIO_NONE;
        uint32_t _ratio = PULSEMETER_ENERGY_RATIO;
        uint32_t _debounce = PULSEMETER_DEBOUNCE;

        double _active = 0;
        double _energy = 0;

        volatile uint32_t _pulses = 0;
        uint32_t _previous_pulses = 0;
        uint32_t _previous_time = 0;


};

// -----------------------------------------------------------------------------
// Interrupt helpers
// -----------------------------------------------------------------------------

PulseMeterSensor * _pulsemeter_sensor_instance[10] = {NULL};

void ICACHE_RAM_ATTR _pulsemeter_sensor_isr(uint8_t gpio) {
    uint8_t index = gpio > 5 ? gpio-6 : gpio;
    if (_pulsemeter_sensor_instance[index]) {
        _pulsemeter_sensor_instance[index]->handleInterrupt(gpio);
    }
}

void ICACHE_RAM_ATTR _pulsemeter_sensor_isr_0() { _pulsemeter_sensor_isr(0); }
void ICACHE_RAM_ATTR _pulsemeter_sensor_isr_1() { _pulsemeter_sensor_isr(1); }
void ICACHE_RAM_ATTR _pulsemeter_sensor_isr_2() { _pulsemeter_sensor_isr(2); }
void ICACHE_RAM_ATTR _pulsemeter_sensor_isr_3() { _pulsemeter_sensor_isr(3); }
void ICACHE_RAM_ATTR _pulsemeter_sensor_isr_4() { _pulsemeter_sensor_isr(4); }
void ICACHE_RAM_ATTR _pulsemeter_sensor_isr_5() { _pulsemeter_sensor_isr(5); }
void ICACHE_RAM_ATTR _pulsemeter_sensor_isr_12() { _pulsemeter_sensor_isr(12); }
void ICACHE_RAM_ATTR _pulsemeter_sensor_isr_13() { _pulsemeter_sensor_isr(13); }
void ICACHE_RAM_ATTR _pulsemeter_sensor_isr_14() { _pulsemeter_sensor_isr(14); }
void ICACHE_RAM_ATTR _pulsemeter_sensor_isr_15() { _pulsemeter_sensor_isr(15); }

static void (*_pulsemeter_sensor_isr_list[10])() = {
    _pulsemeter_sensor_isr_0, _pulsemeter_sensor_isr_1, _pulsemeter_sensor_isr_2,
    _pulsemeter_sensor_isr_3, _pulsemeter_sensor_isr_4, _pulsemeter_sensor_isr_5,
    _pulsemeter_sensor_isr_12, _pulsemeter_sensor_isr_13, _pulsemeter_sensor_isr_14,
    _pulsemeter_sensor_isr_15
};

void PulseMeterSensor::_attach(PulseMeterSensor * instance, uint8_t gpio, uint8_t mode) {
    if (!gpioValid(gpio)) return;
    _detach(gpio);
    uint8_t index = gpio > 5 ? gpio-6 : gpio;
    _pulsemeter_sensor_instance[index] = instance;
    attachInterrupt(gpio, _pulsemeter_sensor_isr_list[index], mode);
    #if SENSOR_DEBUG
        DEBUG_MSG(PSTR("[SENSOR] GPIO%u interrupt attached to %s\n"), gpio, instance->description().c_str());
    #endif
}

void PulseMeterSensor::_detach(uint8_t gpio) {
    if (!gpioValid(gpio)) return;
    uint8_t index = gpio > 5 ? gpio-6 : gpio;
    if (_pulsemeter_sensor_instance[index]) {
        detachInterrupt(gpio);
        #if SENSOR_DEBUG
            DEBUG_MSG(PSTR("[SENSOR] GPIO%u interrupt detached from %s\n"), gpio, _pulsemeter_sensor_instance[index]->description().c_str());
        #endif
        _pulsemeter_sensor_instance[index] = NULL;
    }
}

#endif // SENSOR_SUPPORT && PULSEMETER_SUPPORT
