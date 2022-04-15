/*

SENSORS BASE

Copyright (C) 2018 FastyBird s.r.o. <code@fastybird.com>

*/

#pragma once

#if SENSOR_SUPPORT

#include "../../filters/LastFilter.h"
#include "../../filters/MaxFilter.h"
#include "../../filters/MedianFilter.h"
#include "../../filters/MovingAverageFilter.h"

#define SENSOR_ERROR_OK             0       // No error
#define SENSOR_ERROR_OUT_OF_RANGE   1       // Result out of sensor range
#define SENSOR_ERROR_WARM_UP        2       // Sensor is warming-up
#define SENSOR_ERROR_TIMEOUT        3       // Response from sensor timed out
#define SENSOR_ERROR_UNKNOWN_ID     4       // Sensor did not report a known ID
#define SENSOR_ERROR_CRC            5       // Sensor data corrupted
#define SENSOR_ERROR_I2C            6       // Wrong or locked I2C address
#define SENSOR_ERROR_GPIO_USED      7       // The GPIO is already in use
#define SENSOR_ERROR_CALIBRATION    8       // Calibration error or Not calibrated
#define SENSOR_ERROR_OTHER          99      // Any other error

typedef struct {
    BaseFilter * filter;        // Filter object
    uint8_t type;               // Type of measurement
    signed char decimals;       // Number of decimals in textual representation
} sensor_magnitude_t;

typedef std::function<void(uint8_t, double)> TSensorCallback;

class BaseSensor {

    public:

        // Constructor
        BaseSensor() {}

        // Destructor
        ~BaseSensor() {}

        // Initialization method, must be idempotent
        virtual void begin() {}

        // Loop-like method, call it in your main loop
        virtual void tick() {}

        // Pre-read hook (usually to populate registers with up-to-date data)
        virtual void pre() {}

        // Post-read hook (usually to reset things)
        virtual void post() {}

        // Type for sensor
        virtual uint8_t type() = 0;

        // Descriptive name of the sensor
        virtual String description() = 0;

        // Retrieve current instance configuration
        virtual void getConfig(JsonObject& root) {};

        // Save current instance configuration
        virtual void setConfig(JsonObject& root) {};

        // Load the configuration manifest
        static void manifest(JsonArray& root) {};

        // Sensor ID
        uint8_t getID() { return _sensor_id; };

        // Return status (true if no errors)
        bool status() { return 0 == _error; }

        // Return ready status (true for ready)
        bool ready() { return _ready; }

        // Return sensor last internal error
        int error() { return _error; }

        // Hook for event callback
        void onEvent(TSensorCallback fn) { _callback = fn; };

        void process() {
            for (uint8_t k = 0; k < _magnitudes.size(); k++) {
                _magnitudes[k].filter->add(magnitudeCurrentValue(k));
            }
        }

        // Sensor magnitudes

        // Number of available magnitudes
        uint8_t magnitudesCount() {
            return _magnitudes.size();
        }

        // Current value for magnitude # index
        virtual double magnitudeCurrentValue(uint8_t index) = 0;

        // Type for magnitude # index
        uint8_t magnitudeType(uint8_t index) {
            return _magnitudes[index].type;
        };

	    // Number of decimals for a magnitude (or -1 for default)
	    uint8_t magnitudeDecimals(uint8_t index) {
            if (index >= _magnitudes.size()) {
                return 0;
            }

            if (_magnitudes[index].decimals >= 0) {
                return _magnitudes[index].decimals;
            }

            // Hardcoded decimals (these should be linked to the unit, instead of the magnitude)
            if (_magnitudes[index].type == MAGNITUDE_ANALOG) {
                return SENSOR_ANALOG_DECIMALS;
            }

            if (
                _magnitudes[index].type == MAGNITUDE_ENERGY
                || _magnitudes[index].type == MAGNITUDE_ENERGY_DELTA
            ) {
                if (getSetting("sensor_ene_units", SENSOR_ENERGY_UNITS).toInt() == SENSOR_ENERGY_KWH) {
                    return 3;
                }
            }

            if (
                _magnitudes[index].type == MAGNITUDE_POWER_ACTIVE
                || _magnitudes[index].type == MAGNITUDE_POWER_APPARENT
                || _magnitudes[index].type == MAGNITUDE_POWER_REACTIVE
            ) {
                if (getSetting("sensor_pwr_units", SENSOR_POWER_UNITS).toInt() == SENSOR_POWER_KILOWATTS) {
                    return 3;
                }
            }

            if (_magnitudes[index].type < MAGNITUDE_MAX) {
                return pgm_read_byte(magnitude_decimals + _magnitudes[index].type);
            }

            return 0;
        }

        String magnitudeUnits(uint8_t index) {
            char buffer[8] = {0};

            if (index >= _magnitudes.size() || _magnitudes[index].type >= MAGNITUDE_MAX) {
                return String(buffer);
            }

            if (
                _magnitudes[index].type == MAGNITUDE_TEMPERATURE
                && getSetting("sensor_tmp_units", SENSOR_TEMPERATURE_UNITS).toInt() == SENSOR_TMP_FAHRENHEIT
            ) {
                strncpy_P(buffer, magnitude_fahrenheit, sizeof(buffer));

            } else if (
                (
                    _magnitudes[index].type == MAGNITUDE_ENERGY
                    || _magnitudes[index].type == MAGNITUDE_ENERGY_DELTA
                )
                && getSetting("sensor_ene_units", SENSOR_ENERGY_UNITS).toInt() == SENSOR_ENERGY_KWH
            ) {
                strncpy_P(buffer, magnitude_kwh, sizeof(buffer));

            } else if (
                (
                    _magnitudes[index].type == MAGNITUDE_POWER_ACTIVE
                    || _magnitudes[index].type == MAGNITUDE_POWER_APPARENT
                    || _magnitudes[index].type == MAGNITUDE_POWER_REACTIVE
                )
                && getSetting("sensor_pwr_units", SENSOR_POWER_UNITS).toInt() == SENSOR_POWER_KILOWATTS
            ) {
                strncpy_P(buffer, magnitude_kw, sizeof(buffer));

            } else {
                strncpy_P(buffer, magnitude_units[_magnitudes[index].type], sizeof(buffer));
            }

            return String(buffer);
        }

        String magnitudeName(uint8_t index) {
            char buffer[16] = {0};

            if (index >= _magnitudes.size() || _magnitudes[index].type >= MAGNITUDE_MAX) {
                return String(buffer);
            }

            strncpy_P(buffer, magnitude_names[_magnitudes[index].type], sizeof(buffer));

            return String(buffer);
        }

        BaseFilter * magnitudeFilter(uint8_t index) {
            return _magnitudes[index].filter;
        }

        double magnitudeValue(uint8_t index) {
            if (index >= _magnitudes.size()) {
                return 0.0;
            }

            // Hardcoded conversions (these should be linked to the unit, instead of the magnitude)

            double value = _magnitudes[index].filter->result();

            if (_magnitudes[index].type == MAGNITUDE_TEMPERATURE) {
                if (getSetting("sensor_tmp_units", SENSOR_TEMPERATURE_UNITS).toInt() == SENSOR_TMP_FAHRENHEIT) {
                    value = value * 1.8 + 32;
                }

                value = value + getSetting("sensor_tmp_correction", SENSOR_TEMPERATURE_CORRECTION).toFloat();
            }

            if (_magnitudes[index].type == MAGNITUDE_HUMIDITY) {
                value = constrain(value + getSetting("sensor_hum_correction", SENSOR_HUMIDITY_CORRECTION).toFloat(), 0, 100);
            }

            if (_magnitudes[index].type == MAGNITUDE_LUX) {
                value = value + getSetting("sensor_lux_correction", SENSOR_LUX_CORRECTION).toFloat();
            }

            if (
                _magnitudes[index].type == MAGNITUDE_ENERGY
                || _magnitudes[index].type == MAGNITUDE_ENERGY_DELTA
            ) {
                if (getSetting("sensor_ene_units", SENSOR_ENERGY_UNITS).toInt() == SENSOR_ENERGY_KWH) {
                    value = value  / 3600000;
                }
            }

            if (
                _magnitudes[index].type == MAGNITUDE_POWER_ACTIVE
                || _magnitudes[index].type == MAGNITUDE_POWER_APPARENT
                || _magnitudes[index].type == MAGNITUDE_POWER_REACTIVE
            ) {
                if (getSetting("sensor_pwr_units", SENSOR_POWER_UNITS).toInt() == SENSOR_POWER_KILOWATTS) {
                    value = value  / 1000;
                }
            }

            double multiplier = 1;

            uint8_t positions = magnitudeDecimals(index);

            while (positions-- > 0) {
                multiplier *= 10;
            }

            return round(value * multiplier) / multiplier;
        }

        // Register sensor new magnitude
        void _appendMagnitude(uint8_t type, signed char decimals) {
            BaseFilter * filter;

            if (type == MAGNITUDE_ENERGY) {
                filter = new LastFilter();

            } else if (type == MAGNITUDE_DIGITAL) {
                filter = new MaxFilter();

            } else if (
                type == MAGNITUDE_COUNT
                || type == MAGNITUDE_GEIGER_CPM
                || type == MAGNITUDE_GEIGER_SIEVERT
            ) { 
                // For geiger counting moving average filter is the most appropriate if needed at all
                filter = new MovingAverageFilter();

            } else {
                filter = new MedianFilter();
            }

            _magnitudes.push_back({
                filter,
                type,
                decimals
            });
        }

    protected:

        TSensorCallback _callback = NULL;

        uint8_t _sensor_id = 0x00;
        int _error = 0;
        bool _dirty = true;
        uint8_t _count = 0;
        bool _ready = false;

        std::vector<sensor_magnitude_t> _magnitudes;

};

#endif
