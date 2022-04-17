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
    uint8_t unit;
} sensor_magnitude_t;

typedef std::function<void(uint8_t, double)> TSensorCallback;

class BaseSensor {

    public:

        // Constructor
        BaseSensor() {}

        // ---------------------------------------------------------------------

        // Destructor
        ~BaseSensor() {}

        // ---------------------------------------------------------------------

        // Initialization method, must be idempotent
        virtual void begin() {}

        // ---------------------------------------------------------------------

        // Type for sensor
        virtual uint8_t type() = 0;

        // ---------------------------------------------------------------------

        // Descriptive name of the sensor
        virtual String description() = 0;

        // ---------------------------------------------------------------------

        // Sensor ID
        uint8_t getID() { return _sensor_id; };

        // ---------------------------------------------------------------------

        // Return status (true if no errors)
        bool status() { return _error == 0; }

        // ---------------------------------------------------------------------

        // Return ready status (true for ready)
        bool ready() { return _ready; }

        // ---------------------------------------------------------------------

        // Return sensor last internal error
        int error() { return _error; }

        // ---------------------------------------------------------------------

        // Hook for event callback
        void onEvent(TSensorCallback fn) { _callbacks.push_back(fn); };

        // ---------------------------------------------------------------------

        void reportEvery(uint8_t reportEvery) {
            _sensor_report_every = reportEvery;

            for (uint8_t k = 0; k < _magnitudes.size(); k++) {
                _magnitudes[k].filter->resize(reportEvery);
            }
        }

        // ---------------------------------------------------------------------

        // Loop-like method
        virtual void tick() {}

        // ---------------------------------------------------------------------

        // Pre-read hook (usually to populate registers with up-to-date data)
        virtual void pre() {}

        // ---------------------------------------------------------------------

        // Post-read hook (usually to reset things)
        virtual void post() {}

        // ---------------------------------------------------------------------

        // Process readings hook
        void process() {
            static uint32_t report_count = 0;

            report_count = (report_count + 1) % _sensor_report_every;

            for (uint8_t k = 0; k < _magnitudes.size(); k++) {
                _magnitudes[k].filter->add(_magnitudeCurrentValue(k));

                double value = _magnitudes[k].filter->result();

                #if SENSOR_DEBUG
                {
                    char buffer[64];

                    dtostrf(value, 1 - sizeof(buffer), decimals, buffer);

                    DEBUG_MSG(PSTR("[INFO][SENSOR] %s - %s: %s%s\n"),
                        description().c_str(),
                        magnitudeName(k).c_str(),
                        buffer,
                        magnitudeUnit(k).c_str()
                    );
                }
                #endif

                if (report_count == 0) {
                    for (uint8_t i = 0; i < _callbacks.size(); i++) {
                        _callbacks[i](_magnitudes[k].type, value);
                    }

                    _magnitudes[k].filter->reset();
                }
            }
        }

        // ---------------------------------------------------------------------
        // Sensor magnitudes
        // ---------------------------------------------------------------------

        // Number of sensor available magnitudes
        uint8_t magnitudesCount() {
            return _magnitudes.size();
        }

        // ---------------------------------------------------------------------

        // Sensor magnitude type
        uint8_t magnitudeType(uint8_t index) {
            return _magnitudes[index].type;
        };

        // ---------------------------------------------------------------------

	    // Number of decimals for sensor magnitude
	    uint8_t magnitudeDecimals(uint8_t index) {
            if (index >= _magnitudes.size() || _magnitudes[index].decimals < 0) {
                return 0;
            }

            return _magnitudes[index].decimals;
        }

        // ---------------------------------------------------------------------

        // Sensor magnitude unit
        String magnitudeUnit(uint8_t index) {
            char buffer[8] = {0};

            if (index >= _magnitudes.size() || _magnitudes[index].unit >= MAGNITUDE_UNIT_MAX) {
                return String(buffer);
            }

            strncpy_P(buffer, magnitude_units[_magnitudes[index].unit], sizeof(buffer));

            return String(buffer);
        }

        // ---------------------------------------------------------------------

        // Sensor magnitude name
        String magnitudeName(uint8_t index) {
            char buffer[16] = {0};

            if (index >= _magnitudes.size() || _magnitudes[index].type >= MAGNITUDE_MAX) {
                return String(buffer);
            }

            strncpy_P(buffer, magnitude_names[_magnitudes[index].type], sizeof(buffer));

            return String(buffer);
        }

        // ---------------------------------------------------------------------

        double magnitudeValue(uint8_t index) {
            if (index >= _magnitudes.size()) {
                return 0.0;
            }

            return _magnitudes[index].filter->result();
        }

        // ---------------------------------------------------------------------
        // Sensor internal API
        // ---------------------------------------------------------------------

        // Current value for magnitude
        virtual double _magnitudeCurrentValue(uint8_t index) = 0;

        // ---------------------------------------------------------------------

        // Register sensor new magnitude
        void _appendMagnitude(uint8_t type, signed char decimals, uint8_t unit) {
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
                decimals,
                unit
            });
        }

    protected:

        std::vector<TSensorCallback> _callbacks;

        uint8_t _sensor_report_every = SENSOR_REPORT_EVERY;
        uint8_t _sensor_id = 0x00;
        int _error = 0;
        bool _dirty = true;
        uint8_t _count = 0;
        bool _ready = false;

        std::vector<sensor_magnitude_t> _magnitudes;

};

#endif
