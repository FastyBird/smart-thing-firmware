/*

SENSOR MODULE

Copyright (C) 2018 FastyBird s.r.o. <info@fastybird.com>

*/

#if SENSOR_SUPPORT

#include <vector>
#include "filters/LastFilter.h"
#include "filters/MaxFilter.h"
#include "filters/MedianFilter.h"
#include "filters/MovingAverageFilter.h"
#include "sensors/BaseSensor.h"

#include <float.h>

typedef struct {
    BaseSensor * sensor;        // Sensor object
    BaseFilter * filter;        // Filter object
    uint8_t local;              // Local index in its provider
    uint8_t type;               // Type of measurement
    uint8_t decimals;           // Number of decimals in textual representation
    uint8_t global;             // Global index in its type
    double last;                // Last raw value from sensor (unfiltered)
    double reported;            // Last reported value
    double min_change;          // Minimum value change to report
    double max_change;          // Maximum value change to report
} sensor_magnitude_t;

std::vector<BaseSensor *> _sensors;
std::vector<sensor_magnitude_t> _magnitudes;
bool _sensors_ready = false;

uint8_t _magnitudes_count[MAGNITUDE_MAX];
uint32_t _sensor_read_interval = 1000 * SENSOR_READ_INTERVAL;
uint8_t _sensor_report_every = SENSOR_REPORT_EVERY;
uint8_t _sensor_save_every = SENSOR_SAVE_EVERY;

uint8_t _sensor_power_units = SENSOR_POWER_UNITS;
uint8_t _sensor_energy_units = SENSOR_ENERGY_UNITS;
uint8_t _sensor_temperature_units = SENSOR_TEMPERATURE_UNITS;
double _sensor_temperature_correction = SENSOR_TEMPERATURE_CORRECTION;
double _sensor_humidity_correction = SENSOR_HUMIDITY_CORRECTION;
double _sensor_lux_correction = SENSOR_LUX_CORRECTION;

#if PZEM004T_SUPPORT
    PZEM004TSensor *pzem004t_sensor;
#endif

String _sensor_energy_reset_ts = String();

#if FASTYBIRD_SUPPORT
    uint8_t _sensor_fastybird_channel_index = 0xFF;
#endif

// -----------------------------------------------------------------------------
// MODULE PRIVATE
// -----------------------------------------------------------------------------

uint8_t _magnitudeDecimals(
    uint8_t type
) {
    // Hardcoded decimals (these should be linked to the unit, instead of the magnitude)

    if (type == MAGNITUDE_ANALOG) {
        return ANALOG_DECIMALS;
    }

    if (
        type == MAGNITUDE_ENERGY
        || type == MAGNITUDE_ENERGY_DELTA
    ) {
        _sensor_energy_units = getSetting("eneUnits", SENSOR_ENERGY_UNITS).toInt();

        if (_sensor_energy_units == ENERGY_KWH) {
            return 3;
        }
    }

    if (
        type == MAGNITUDE_POWER_ACTIVE
        || type == MAGNITUDE_POWER_APPARENT
        || type == MAGNITUDE_POWER_REACTIVE
    ) {
        if (_sensor_power_units == POWER_KILOWATTS) {
            return 3;
        }
    }

    if (type < MAGNITUDE_MAX) {
        return pgm_read_byte(magnitude_decimals + type);
    }

    return 0;
}

// -----------------------------------------------------------------------------

double _magnitudeProcess(
    uint8_t type,
    uint8_t decimals,
    double value
) {
    // Hardcoded conversions (these should be linked to the unit, instead of the magnitude)

    if (type == MAGNITUDE_TEMPERATURE) {
        if (_sensor_temperature_units == TMP_FAHRENHEIT) {
            value = value * 1.8 + 32;
        }

        value = value + _sensor_temperature_correction;
    }

    if (type == MAGNITUDE_HUMIDITY) {
        value = constrain(value + _sensor_humidity_correction, 0, 100);
    }

    if (type == MAGNITUDE_LUX) {
        value = value + _sensor_lux_correction;
    }

    if (
        type == MAGNITUDE_ENERGY
        || type == MAGNITUDE_ENERGY_DELTA
    ) {
        if (_sensor_energy_units == ENERGY_KWH) {
            value = value  / 3600000;
        }
    }

    if (
        type == MAGNITUDE_POWER_ACTIVE
        || type == MAGNITUDE_POWER_APPARENT
        || type == MAGNITUDE_POWER_REACTIVE
    ) {
        if (_sensor_power_units == POWER_KILOWATTS) {
            value = value  / 1000;
        }
    }

    return roundTo(value, decimals);
}

// -----------------------------------------------------------------------------

bool _sensorHasMagnitude(
    uint8_t type
) {
    for (uint8_t i = 0; i < magnitudeCount(); i++) {
        sensor_magnitude_t magnitude = _magnitudes[i];

        if (magnitude.type == type) {
            return true;
        }
    }

    return false;
}

// -----------------------------------------------------------------------------

#if FASTYBIRD_SUPPORT || (WEB_SUPPORT && WS_SUPPORT)
    void _sensorResetMICSCalibration() {
        for (uint8_t i = 0; i < _sensors.size(); i++) {
            #if MICS2710_SUPPORT
                if (_sensors[i]->getID() == SENSOR_MICS2710_ID) {
                    MICS2710Sensor * sensor = (MICS2710Sensor *) _sensors[i];

                    sensor->calibrate();

                    setSetting("snsR0", sensor->getR0());
                }
            #endif // MICS2710_SUPPORT

            #if MICS5525_SUPPORT
                if (_sensors[i]->getID() == SENSOR_MICS5525_ID) {
                    MICS5525Sensor * sensor = (MICS5525Sensor *) _sensors[i];

                    sensor->calibrate();

                    setSetting("snsR0", sensor->getR0());
                }
            #endif // MICS5525_SUPPORT
        }

        // Reload & cache settings
        firmwareReload();
    }

// -----------------------------------------------------------------------------

    void _sensorResetPowerCalibration() {
        for (uint8_t i = 0; i < _sensors.size(); i++) {
            #if EMON_ANALOG_SUPPORT
                if (_sensors[i]->getID() == SENSOR_EMON_ANALOG_ID) {
                    EmonAnalogSensor * sensor = (EmonAnalogSensor *) _sensors[i];

                    sensor->setCurrentRatio(0, EMON_CURRENT_RATIO);

                    delSetting("pwrRatioC");
                }
            #endif // EMON_ANALOG_SUPPORT

            #if HLW8012_SUPPORT
                if (_sensors[i]->getID() == SENSOR_HLW8012_ID) {
                    HLW8012Sensor * sensor = (HLW8012Sensor *) _sensors[i];

                    sensor->resetRatios();

                    delSetting("pwrRatioC");
                    delSetting("pwrRatioV");
                    delSetting("pwrRatioP");
                }
            #endif // HLW8012_SUPPORT

            #if CSE7766_SUPPORT
                if (_sensors[i]->getID() == SENSOR_CSE7766_ID) {
                    sensor->resetRatios();

                    delSetting("pwrRatioC");
                    delSetting("pwrRatioV");
                    delSetting("pwrRatioP");
                }
            #endif // CSE7766_SUPPORT
        }

        // Reload & cache settings
        firmwareReload();
    }

// -----------------------------------------------------------------------------

    void _sensorResetEnergyCalibration() {
        for (uint8_t i = 0; i < _sensors.size(); i++) {
            #if EMON_ANALOG_SUPPORT
                if (_sensors[i]->getID() == SENSOR_EMON_ANALOG_ID) {
                    EmonAnalogSensor * sensor = (EmonAnalogSensor *) _sensors[i];

                    sensor->resetEnergy();

                    delSetting("eneTotal");

                    _sensorResetTS();
                }
            #endif // EMON_ANALOG_SUPPORT

            #if EMON_ADC121_SUPPORT
                if (_sensors[i]->getID() == SENSOR_EMON_ADC121_ID) {
                    EmonADC121Sensor * sensor = (EmonADC121Sensor *) _sensors[i];

                    sensor->resetEnergy();

                    delSetting("eneTotal");

                    _sensorResetTS();
                }
            #endif // EMON_ADC121_SUPPORT

            #if EMON_ADS1X15_SUPPORT
                if (_sensors[i]->getID() == SENSOR_EMON_ADS1X15_ID) {
                    EmonADS1X15Sensor * sensor = (EmonADS1X15Sensor *) _sensors[i];

                    sensor->resetEnergy();

                    delSetting("eneTotal");

                    _sensorResetTS();
                }
            #endif // EMON_ADS1X15_SUPPORT

            #if HLW8012_SUPPORT
                if (_sensors[i]->getID() == SENSOR_HLW8012_ID) {
                    HLW8012Sensor * sensor = (HLW8012Sensor *) _sensors[i];

                    sensor->resetEnergy();

                    delSetting("eneTotal");

                    _sensorResetTS();
                }
            #endif // HLW8012_SUPPORT

            #if CSE7766_SUPPORT
                if (_sensors[i]->getID() == SENSOR_CSE7766_ID) {
                    CSE7766Sensor * sensor = (CSE7766Sensor *) _sensors[i];

                    sensor->resetEnergy();

                    delSetting("eneTotal");

                    _sensorResetTS();
                }
            #endif // CSE7766_SUPPORT

            #if PULSEMETER_SUPPORT
                if (_sensors[i]->getID() == SENSOR_PULSEMETER_ID) {
                    PulseMeterSensor * sensor = (PulseMeterSensor *) _sensors[i];

                    sensor->resetEnergy();

                    delSetting("eneTotal");

                    _sensorResetTS();
                }
            #endif // PULSEMETER_SUPPORT

            #if PZEM004T_SUPPORT
                if (_sensors[i]->getID() == SENSOR_PZEM004T_ID) {
                    PZEM004TSensor * sensor = (PZEM004TSensor *) _sensors[i];

                    uint8_t dev_count = sensor->getAddressesCount();

                    for (uint8_t dev = 0; dev < dev_count; dev++) {
                        sensor->resetEnergy(dev, 0);

                        delSetting("pzemEneTotal", dev);
                    }

                    _sensorResetTS();
                }
            #endif // PZEM004T_SUPPORT
        }

        // Reload & cache settings
        firmwareReload();
    }

// -----------------------------------------------------------------------------

    /**
     * Provide module configuration schema
     */
    void _sensorReportConfigurationSchema(
        JsonArray& configuration
    ) {
        bool enabled_emon = false;
        bool enabled_pwr = false;
        bool enabled_hlw = false;
        bool enabled_cse = false;
        bool enabled_pzem = false;
        bool enabled_pm = false;

        for (uint8_t i = 0; i < _sensors.size(); i++) {
            BaseSensor * sensor = _sensors[i];

            #if EMON_ANALOG_SUPPORT
                if (sensor->getID() == SENSOR_EMON_ANALOG_ID) {
                    enabled_emon = true;
                    enabled_pwr = true;
                }
            #endif

            #if HLW8012_SUPPORT
                if (sensor->getID() == SENSOR_HLW8012_ID) {
                    enabled_hlw = true;
                    enabled_pwr = true;
                }
            #endif

            #if CSE7766_SUPPORT
                if (sensor->getID() == SENSOR_CSE7766_ID) {
                    enabled_cse = true;
                    enabled_pwr = true;
                }
            #endif

            #if V9261F_SUPPORT
                if (sensor->getID() == SENSOR_V9261F_ID) {
                    enabled_pwr = true;
                }
            #endif

            #if ECH1560_SUPPORT
                if (sensor->getID() == SENSOR_ECH1560_ID) {
                    enabled_pwr = true;
                }
            #endif

            #if PZEM004T_SUPPORT
                if (sensor->getID() == SENSOR_PZEM004T_ID) {
                    enabled_pzem = true;
                    enabled_pwr = true;
                }
            #endif

            #if PULSEMETER_SUPPORT
                if (sensor->getID() == SENSOR_PULSEMETER_ID) {
                    enabled_pm = true;
                }
            #endif
        }

        // SENSORS GLOBAL
        JsonObject& sensors_read_interval = configuration.createNestedObject();

        sensors_read_interval["name"] = "sensors_read_interval";
        sensors_read_interval["type"] = "select";
        sensors_read_interval["default"] = SENSOR_READ_INTERVAL;

        JsonArray& sensors_read_interval_values = sensors_read_interval.createNestedArray("values");

        JsonObject& sensors_read_interval_values_value1 = sensors_read_interval_values.createNestedObject();
        sensors_read_interval_values_value1["value"] = 1;
        sensors_read_interval_values_value1["name"] = "1";

        JsonObject& sensors_read_interval_values_value2 = sensors_read_interval_values.createNestedObject();
        sensors_read_interval_values_value2["value"] = 6;
        sensors_read_interval_values_value2["name"] = "6";

        JsonObject& sensors_read_interval_values_value3 = sensors_read_interval_values.createNestedObject();
        sensors_read_interval_values_value3["value"] = 10;
        sensors_read_interval_values_value3["name"] = "10";

        JsonObject& sensors_read_interval_values_value4 = sensors_read_interval_values.createNestedObject();
        sensors_read_interval_values_value4["value"] = 15;
        sensors_read_interval_values_value4["name"] = "15";

        JsonObject& sensors_read_interval_values_value5 = sensors_read_interval_values.createNestedObject();
        sensors_read_interval_values_value5["value"] = 30;
        sensors_read_interval_values_value5["name"] = "30";

        JsonObject& sensors_read_interval_values_value7 = sensors_read_interval_values.createNestedObject();
        sensors_read_interval_values_value7["value"] = 60;
        sensors_read_interval_values_value7["name"] = "60";

        JsonObject& sensors_read_interval_values_value8 = sensors_read_interval_values.createNestedObject();
        sensors_read_interval_values_value8["value"] = 300;
        sensors_read_interval_values_value8["name"] = "300";

        JsonObject& sensors_read_interval_values_value9 = sensors_read_interval_values.createNestedObject();
        sensors_read_interval_values_value9["value"] = 600;
        sensors_read_interval_values_value9["name"] = "600";

        JsonObject& sensors_read_interval_values_value10 = sensors_read_interval_values.createNestedObject();
        sensors_read_interval_values_value10["value"] = 900;
        sensors_read_interval_values_value10["name"] = "900";

        JsonObject& sensors_read_interval_values_value11 = sensors_read_interval_values.createNestedObject();
        sensors_read_interval_values_value11["value"] = 1800;
        sensors_read_interval_values_value11["name"] = "1800";

        JsonObject& sensors_read_interval_values_value12 = sensors_read_interval_values.createNestedObject();
        sensors_read_interval_values_value12["value"] = 3600;
        sensors_read_interval_values_value12["name"] = "3600";
        
        JsonObject& sensors_report_interval = configuration.createNestedObject();

        sensors_report_interval["name"] = "sensors_report_interval";
        sensors_report_interval["type"] = "number";
        sensors_report_interval["min"] = SENSOR_REPORT_MIN_EVERY;
        sensors_report_interval["max"] = SENSOR_REPORT_MAX_EVERY;
        sensors_report_interval["step"] = SENSOR_REPORT_STEP_EVERY;
        sensors_report_interval["default"] = SENSOR_REPORT_EVERY;

        if (enabled_pwr) {
            // POWER METER
            JsonObject& sensors_save_interval = configuration.createNestedObject();

            sensors_save_interval["name"] = "sensors_save_interval";
            sensors_save_interval["type"] = "number";
            sensors_save_interval["min"] = 0;
            sensors_save_interval["max"] = 200;
            sensors_save_interval["step"] = 1;
            sensors_save_interval["default"] = SENSOR_SAVE_EVERY;

            JsonObject& power_units = configuration.createNestedObject();

            power_units["name"] = "power_units";
            power_units["type"] = "select";
            power_units["default"] = SENSOR_POWER_UNITS;

            JsonArray& power_units_values = power_units.createNestedArray("values");

            JsonObject& power_units_values_value1 = power_units_values.createNestedObject();
            power_units_values_value1["value"] = POWER_WATTS;
            power_units_values_value1["name"] = "watts";

            JsonObject& power_units_values_value2 = power_units_values.createNestedObject();
            power_units_values_value2["value"] = POWER_KILOWATTS;
            power_units_values_value2["name"] = "kilowatts";

            // ENERGY METER
            JsonObject& energy_units = configuration.createNestedObject();

            energy_units["name"] = "energy_units";
            energy_units["type"] = "select";
            energy_units["default"] = SENSOR_ENERGY_UNITS;

            JsonArray& energy_units_values = energy_units.createNestedArray("values");

            JsonObject& energy_units_values_value1 = energy_units_values.createNestedObject();
            energy_units_values_value1["value"] = ENERGY_JOULES;
            energy_units_values_value1["name"] = "joules";

            JsonObject& energy_units_values_value2 = energy_units_values.createNestedObject();
            energy_units_values_value2["value"] = ENERGY_KWH;
            energy_units_values_value2["name"] = "kilowatts_hours";
        }

        // ENVIRONMENT METER
        if (_sensorHasMagnitude(MAGNITUDE_TEMPERATURE)) {
            JsonObject& temperature_units = configuration.createNestedObject();

            temperature_units["name"] = "temperature_units";
            temperature_units["type"] = "select";
            temperature_units["default"] = SENSOR_TEMPERATURE_UNITS;

            JsonArray& temperature_units_values = temperature_units.createNestedArray("values");

            JsonObject& temperature_units_values_value1 = temperature_units_values.createNestedObject();
            temperature_units_values_value1["value"] = TMP_CELSIUS;
            temperature_units_values_value1["name"] = "celsius";

            JsonObject& temperature_units_values_value2 = temperature_units_values.createNestedObject();
            temperature_units_values_value2["value"] = TMP_FAHRENHEIT;
            temperature_units_values_value2["name"] = "fahrenheit";

            JsonObject& temperature_correction = configuration.createNestedObject();

            temperature_correction["name"] = "temperature_correction";
            temperature_correction["type"] = "number";
            temperature_correction["min"] = -100;
            temperature_correction["max"] = 100;
            temperature_correction["step"] = 0.1;
            temperature_correction["default"] = SENSOR_TEMPERATURE_CORRECTION;
        }

        if (_sensorHasMagnitude(MAGNITUDE_HUMIDITY)) {
            JsonObject& humidity_correction = configuration.createNestedObject();

            humidity_correction["name"] = "humidity_correction";
            humidity_correction["type"] = "number";
            humidity_correction["min"] = -100;
            humidity_correction["max"] = 100;
            humidity_correction["step"] = 0.1;
            humidity_correction["default"] = SENSOR_HUMIDITY_CORRECTION;
        }

        if (enabled_emon) {
            JsonObject& power_voltage = configuration.createNestedObject();

            power_voltage["name"] = "power_voltage";
            power_voltage["type"] = "number";
            power_voltage["readonly"] = true;
            power_voltage["min"] = 0;
            power_voltage["max"] = 500;
            power_voltage["step"] = 1;
            power_voltage["default"] = 0;
        }

        if (enabled_hlw || enabled_cse) {
            JsonObject& expected_current = configuration.createNestedObject();

            expected_current["name"] = "expected_current";
            expected_current["type"] = "number";
            expected_current["min"] = 0;
            expected_current["max"] = 500;
            expected_current["step"] = 1;
            expected_current["default"] = (char*) NULL;

            JsonObject& expected_voltage = configuration.createNestedObject();

            expected_voltage["name"] = "expected_voltage";
            expected_voltage["type"] = "number";
            expected_voltage["min"] = 0;
            expected_voltage["max"] = 500;
            expected_voltage["step"] = 1;
            expected_voltage["default"] = (char*) NULL;
        }

        if (enabled_hlw || enabled_cse || enabled_emon) {
            JsonObject& expected_power = configuration.createNestedObject();

            expected_power["name"] = "expected_power";
            expected_power["type"] = "number";
            expected_power["min"] = 0;
            expected_power["max"] = 500;
            expected_power["step"] = 1;
            expected_power["default"] = (char*) NULL;
        }

        if (enabled_pm) {
            JsonObject& energy_ratio = configuration.createNestedObject();

            energy_ratio["name"] = "energy_ratio";
            energy_ratio["type"] = "number";
            energy_ratio["readonly"] = true;
            energy_ratio["min"] = 0;
            energy_ratio["max"] = 500;
            energy_ratio["step"] = 1;
            energy_ratio["default"] = 0;
        }
    }

// -----------------------------------------------------------------------------

    /**
     * Report module configuration
     */
    void _sensorReportConfiguration(
        JsonObject& configuration
    ) {
        bool enabled_emon = false;
        bool enabled_pwr = false;
        bool enabled_hlw = false;
        bool enabled_cse = false;
        bool enabled_pzem = false;
        bool enabled_pm = false;

        for (uint8_t i = 0; i < _sensors.size(); i++) {
            BaseSensor * sensor = _sensors[i];

            #if EMON_ANALOG_SUPPORT
                if (sensor->getID() == SENSOR_EMON_ANALOG_ID) {
                    enabled_emon = true;
                    enabled_pwr = true;
                }
            #endif

            #if HLW8012_SUPPORT
                if (sensor->getID() == SENSOR_HLW8012_ID) {
                    enabled_hlw = true;
                    enabled_pwr = true;
                }
            #endif

            #if CSE7766_SUPPORT
                if (sensor->getID() == SENSOR_CSE7766_ID) {
                    enabled_cse = true;
                    enabled_pwr = true;
                }
            #endif

            #if V9261F_SUPPORT
                if (sensor->getID() == SENSOR_V9261F_ID) {
                    enabled_pwr = true;
                }
            #endif

            #if ECH1560_SUPPORT
                if (sensor->getID() == SENSOR_ECH1560_ID) {
                    enabled_pwr = true;
                }
            #endif

            #if PZEM004T_SUPPORT
                if (sensor->getID() == SENSOR_PZEM004T_ID) {
                    enabled_pzem = true;
                    enabled_pwr = true;
                }
            #endif

            #if PULSEMETER_SUPPORT
                if (sensor->getID() == SENSOR_PULSEMETER_ID) {
                    enabled_pm = true;
                }
            #endif
        }

        configuration["sensors_read_interval"] = _sensor_read_interval / 1000;
        configuration["sensors_report_interval"] = _sensor_report_every;

        if (enabled_pwr) {
            configuration["sensors_save_interval"] = _sensor_save_every;
            configuration["power_units"] = _sensor_power_units;
            configuration["energy_units"] = _sensor_energy_units;
        }

        if (_sensorHasMagnitude(MAGNITUDE_TEMPERATURE)) {
            configuration["temperature_units"] = _sensor_temperature_units;
            configuration["temperature_correction"] = _sensor_temperature_correction;
        }

        if (_sensorHasMagnitude(MAGNITUDE_HUMIDITY)) {
            configuration["humidity_correction"] = _sensor_humidity_correction;
        }

        if (enabled_emon) {
            configuration["power_voltage"] = (char*) NULL;
        }

        if (enabled_pm) {
            configuration["energy_ratio"] = (char*) NULL;
        }

        for (uint8_t i = 0; i < _sensors.size(); i++) {
            BaseSensor * sensor = _sensors[i];

            #if EMON_ANALOG_SUPPORT
                if (sensor->getID() == SENSOR_EMON_ANALOG_ID) {
                    configuration["power_voltage"] = ((EmonAnalogSensor *) sensor)->getVoltage();
                }
            #endif

            #if PULSEMETER_SUPPORT
                if (sensor->getID() == SENSOR_PULSEMETER_ID) {
                    configuration["energy_ratio"] = ((PulseMeterSensor *) sensor)->getEnergyRatio();
                }
            #endif
        }

        if (enabled_hlw || enabled_cse) {
            configuration["expected_current"] = getSetting("pwrExpectedC", 0).toFloat();
        }

        if (enabled_hlw || enabled_cse) {
            configuration["expected_voltage"] = getSetting("pwrExpectedV", 0).toInt();
        }

        if (enabled_hlw || enabled_cse || enabled_emon) {
            configuration["expected_power"] = getSetting("pwrExpectedP", 0).toInt();
        }
    }

// -----------------------------------------------------------------------------

    /**
     * Update module configuration via WS or MQTT etc.
     */
    bool _sensorUpdateConfiguration(
        JsonObject& configuration
    ) {
        DEBUG_MSG(PSTR("[SENSOR] Updating module\n"));

        bool is_updated = false;

        if (
            configuration.containsKey("sensors_read_interval")
            && configuration["sensors_read_interval"].as<uint16_t>() >= SENSOR_READ_MIN_INTERVAL
            && configuration["sensors_read_interval"].as<uint16_t>() <= SENSOR_READ_MAX_INTERVAL
            && configuration["sensors_read_interval"].as<uint16_t>() != getSetting("snsRead").toInt()
        )  {
            DEBUG_MSG(PSTR("[SENSOR] Setting: \"sensors_read_interval\" to: %d\n"), configuration["sensors_read_interval"].as<uint16_t>());

            setSetting("snsRead", configuration["sensors_read_interval"].as<uint16_t>());

            is_updated = true;
        }

        if (
            configuration.containsKey("sensors_report_interval")
            && configuration["sensors_report_interval"].as<uint8_t>() >= SENSOR_REPORT_MIN_EVERY
            && configuration["sensors_report_interval"].as<uint8_t>() <= SENSOR_REPORT_MAX_EVERY
            && configuration["sensors_report_interval"].as<uint8_t>() != getSetting("snsReport").toInt()
        )  {
            DEBUG_MSG(PSTR("[SENSOR] Setting: \"sensors_report_interval\" to: %d\n"), configuration["sensors_report_interval"].as<uint8_t>());

            setSetting("snsReport", configuration["sensors_report_interval"].as<uint8_t>());

            is_updated = true;
        }

        if (
            configuration.containsKey("sensors_save_interval")
            && configuration["sensors_save_interval"].as<uint8_t>() >= 0
            && configuration["sensors_save_interval"].as<uint8_t>() <= 200
            && configuration["sensors_save_interval"].as<uint8_t>() != getSetting("snsSave").toInt()
        )  {
            DEBUG_MSG(PSTR("[SENSOR] Setting: \"sensors_save_interval\" to: %d\n"), configuration["sensors_save_interval"].as<uint8_t>());

            setSetting("snsSave", configuration["sensors_save_interval"].as<uint8_t>());

            is_updated = true;
        }

        if (
            configuration.containsKey("power_units")
            && (
                configuration["power_units"].as<uint8_t>() == POWER_WATTS
                || configuration["power_units"].as<uint8_t>() == POWER_KILOWATTS
            )
            && configuration["power_units"].as<uint8_t>() != getSetting("pwrUnits").toInt()
        )  {
            DEBUG_MSG(PSTR("[SENSOR] Setting: \"power_units\" to: %d\n"), configuration["power_units"].as<uint8_t>());

            setSetting("pwrUnits", configuration["power_units"].as<uint8_t>());

            is_updated = true;
        }

        if (
            configuration.containsKey("energy_units")
            && (
                configuration["energy_units"].as<uint8_t>() == ENERGY_JOULES
                || configuration["energy_units"].as<uint8_t>() == ENERGY_KWH
            )
            && configuration["energy_units"].as<uint8_t>() != getSetting("eneUnits").toInt()
        )  {
            DEBUG_MSG(PSTR("[SENSOR] Setting: \"energy_units\" to: %d\n"), configuration["energy_units"].as<uint8_t>());

            setSetting("eneUnits", configuration["energy_units"].as<uint8_t>());

            is_updated = true;
        }

        if (
            configuration.containsKey("temperature_units")
            && (
                configuration["temperature_units"].as<uint8_t>() == TMP_CELSIUS
                || configuration["temperature_units"].as<uint8_t>() == TMP_FAHRENHEIT
            )
            && configuration["temperature_units"].as<uint8_t>() != getSetting("tmpUnits").toInt()
        )  {
            DEBUG_MSG(PSTR("[SENSOR] Setting: \"temperature_units\" to: %d\n"), configuration["temperature_units"].as<uint8_t>());

            setSetting("tmpUnits", configuration["temperature_units"].as<uint8_t>());

            is_updated = true;
        }

        if (
            configuration.containsKey("temperature_correction")
            && configuration["temperature_correction"].as<float>() >= -100
            && configuration["temperature_correction"].as<float>() <= 100
            && configuration["temperature_correction"].as<float>() != getSetting("tmpCorrection").toFloat()
        )  {
            DEBUG_MSG(PSTR("[SENSOR] Setting: \"temperature_correction\" to: %d\n"), configuration["temperature_correction"].as<float>());

            setSetting("tmpCorrection", configuration["temperature_correction"].as<float>());

            is_updated = true;
        }

        if (
            configuration.containsKey("humidity_correction")
            && configuration["humidity_correction"].as<float>() >= -100
            && configuration["humidity_correction"].as<float>() <= 100
            && configuration["humidity_correction"].as<float>() != getSetting("humCorrection").toFloat()
        )  {
            DEBUG_MSG(PSTR("[SENSOR] Setting: \"humidity_correction\" to: %d\n"), configuration["humidity_correction"].as<float>());

            setSetting("humCorrection", configuration["humidity_correction"].as<float>());

            is_updated = true;
        }

        return is_updated;
    }
#endif // FASTYBIRD_SUPPORT || (WEB_SUPPORT && WS_SUPPORT)

// -----------------------------------------------------------------------------

#if WEB_SUPPORT && WS_SUPPORT
    void _sensorWSChannelsData(
        JsonArray& channels
    ) {
        char buffer[10];

        for (uint8_t i = 0; i < magnitudeCount(); i++) {
            sensor_magnitude_t magnitude = _magnitudes[i];

            if (magnitude.type == MAGNITUDE_EVENT) {
                continue;
            }

            JsonObject& channel = channels.createNestedObject();

            double value_show = _magnitudeProcess(magnitude.type, magnitude.decimals, magnitude.last);

            dtostrf(value_show, 1 - sizeof(buffer), magnitude.decimals, buffer);

            channel["index"] = magnitude.global;
            channel["type"] = magnitude.type;
            channel["value"] = buffer;
            channel["raw"] = value_show;
            channel["decimals"] = magnitude.decimals;
            channel["units"] = magnitudeUnits(magnitude.type);
            channel["error"] = magnitude.sensor->error();
            channel["name"] = magnitudeName(magnitude.type);

            if (magnitude.type == MAGNITUDE_ENERGY) {
                if (_sensor_energy_reset_ts.length() == 0) {
                    _sensorResetTS();
                }

                channel["description"] = magnitude.sensor->slot(magnitude.local) + String(" (since ") + _sensor_energy_reset_ts + String(")");

            } else {
                channel["description"] = magnitude.sensor->slot(magnitude.local);
            }
        }
    }

// -----------------------------------------------------------------------------

    void _sensorWebSocketSendData(
        JsonObject& root
    ) {
        JsonArray& modules = root.containsKey("modules") ? root["modules"] : root.createNestedArray("modules");
        JsonObject& module = modules.createNestedObject();

        module["module"] = "sensor";

        // Data container
        JsonObject& data = module.createNestedObject("data");

        // Channels statuses
        JsonArray& channels_data = data.createNestedArray("channels");

        _sensorWSChannelsData(channels_data);
    }

// -----------------------------------------------------------------------------

    void _sensorWSOnConnect(
        JsonObject& root
    ) {
        if (_sensors.size() == 0) {
            return;
        }

        DynamicJsonBuffer jsonBuffer;

        JsonArray& modules = root.containsKey("modules") ? root["modules"] : root.createNestedArray("modules");
        JsonObject& module = modules.createNestedObject();

        module["module"] = "sensor";
        module["visible"] = true;

        // Data container
        JsonObject& data = module.createNestedObject("data");

        // Channels statuses
        JsonArray& channels_data = data.createNestedArray("channels");

        _sensorWSChannelsData(channels_data);

        // Configuration container
        JsonObject& configuration = module.createNestedObject("config");

        // MODULE CONFIGURATION

        // Configuration schema container
        JsonArray& configuration_schema = configuration.createNestedArray("schema");

        _sensorReportConfigurationSchema(configuration_schema);

        // Configuration values container
        JsonObject& configuration_values = configuration.createNestedObject("values");

        _sensorReportConfiguration(configuration_values);

        // Enabled sensors container
        JsonObject& configuration_sensors = configuration.createNestedObject("sensors");

        configuration_sensors["pwr"] = false;
        configuration_sensors["emon"] = false;
        configuration_sensors["hlw"] = false;
        configuration_sensors["cse"] = false;
        configuration_sensors["pzem"] = false;
        configuration_sensors["pm"] = false;
        configuration_sensors["temperature"] = false;
        configuration_sensors["humidity"] = false;
        configuration_sensors["mics"] = false;

        for (uint8_t i = 0; i < _sensors.size(); i++) {
            BaseSensor * sensor = _sensors[i];

            #if EMON_ANALOG_SUPPORT
                if (sensor->getID() == SENSOR_EMON_ANALOG_ID) {
                    configuration_sensors["emon"] = true;
                    configuration_sensors["pwr"] = true;
                }
            #endif

            #if HLW8012_SUPPORT
                if (sensor->getID() == SENSOR_HLW8012_ID) {
                    configuration_sensors["hlw"] = 1;
                    configuration_sensors["pwr"] = true;
                }
            #endif

            #if CSE7766_SUPPORT
                if (sensor->getID() == SENSOR_CSE7766_ID) {
                    configuration_sensors["cse"] = true;
                    configuration_sensors["pwr"] = true;
                }
            #endif

            #if V9261F_SUPPORT
                if (sensor->getID() == SENSOR_V9261F_ID) {
                    configuration_sensors["pwr"] = true;
                }
            #endif

            #if ECH1560_SUPPORT
                if (sensor->getID() == SENSOR_ECH1560_ID) {
                    configuration_sensors["pwr"] = true;
                }
            #endif

            #if PZEM004T_SUPPORT
                if (sensor->getID() == SENSOR_PZEM004T_ID) {
                    configuration_sensors["pzem"] = true;
                    configuration_sensors["pwr"] = true;
                }
            #endif

            #if PULSEMETER_SUPPORT
                if (sensor->getID() == SENSOR_PULSEMETER_ID) {
                    configuration_sensors["pm"] = true;
                }
            #endif
        }

        for (uint8_t i = 0; i < magnitudeCount(); i++) {
            sensor_magnitude_t magnitude = _magnitudes[i];

            if (magnitude.type == MAGNITUDE_TEMPERATURE) {
                configuration_sensors["temperature"] = true;
            }

            if (magnitude.type == MAGNITUDE_HUMIDITY) {
                configuration_sensors["humidity"] = true;
            }

            #if MICS2710_SUPPORT || MICS5525_SUPPORT
                if (
                    magnitude.type == MAGNITUDE_CO
                    || magnitude.type == MAGNITUDE_NO2
                ) {
                    configuration_sensors["mics"] = true;
                }
            #endif
        }
    }

// -----------------------------------------------------------------------------

    // WS client requested configuration update
    void _sensorWSOnConfigure(
        const uint32_t clientId, 
        JsonObject& module
    ) {
        if (module.containsKey("module") && module["module"] == "sensor") {
            if (module.containsKey("config")) {
                // Extract configuration container
                JsonObject& configuration = module["config"].as<JsonObject&>();

                if (
                    configuration.containsKey("values")
                    && _sensorUpdateConfiguration(configuration["values"])
                ) {
                    wsSend_P(clientId, PSTR("{\"message\": \"sensors_updated\"}"));

                    // Reload & cache settings
                    firmwareReload();
                }
            }
        }
    }

// -----------------------------------------------------------------------------

    // WS client called action
    void _sensorWSOnAction(
        const uint32_t client_id,
        const char * action,
        JsonObject& data
    ) {
        if (strcmp(action, "reset_mics_calibration") == 0) {
            _sensorResetMICSCalibration();

        } else if(strcmp(action, "reset_power_calibration") == 0) {
            _sensorResetPowerCalibration();

        } else if(strcmp(action, "reset_energy_calibration") == 0) {
            _sensorResetEnergyCalibration();
        }
    }
#endif // WEB_SUPPORT && WS_SUPPORT

// -----------------------------------------------------------------------------

#if FASTYBIRD_SUPPORT
    fastybird_channel_property_t _sensorFastybirdGetChannelStatePropertyStructure() {
        fastybird_channel_property_t property = {
            FASTYBIRD_PROPERTY_STATE,
            FASTYBIRD_PROPERTY_STATE,
            false,
            false,
            FASTYBIRD_PROPERTY_DATA_TYPE_ENUM,
        };

        return property;
    }

// -----------------------------------------------------------------------------

    fastybird_channel_t _sensorFastybirdGetChannelStructure(
        uint8_t index
    ) {
        BaseSensor * sensor = _sensors[index];

        fastybird_channel_t channel = {
            FASTYBIRD_CHANNEL_TYPE_SENSOR,
            FASTYBIRD_CHANNEL_TYPE_SENSOR,
            sensorCount(),
            false,
            false,
            false
        };

        channel.properties.push_back(_sensorFastybirdGetChannelStatePropertyStructure());

        return channel;
    }
#endif

// -----------------------------------------------------------------------------

void _sensorTick() {
    for (uint8_t i = 0; i < _sensors.size(); i++) {
        _sensors[i]->tick();
    }
}

// -----------------------------------------------------------------------------

void _sensorPreRead() {
    for (uint8_t i = 0; i < _sensors.size(); i++) {
        _sensors[i]->pre();

        if (!_sensors[i]->status()) {
            DEBUG_MSG(PSTR("[SENSOR] Error reading data from %s (error: %d)\n"),
                _sensors[i]->description().c_str(),
                _sensors[i]->error()
            );
        }
    }
}

// -----------------------------------------------------------------------------

void _sensorPost() {
    for (uint8_t i = 0; i < _sensors.size(); i++) {
        _sensors[i]->post();
    }
}

// -----------------------------------------------------------------------------

void _sensorResetTS() {
    #if NTP_SUPPORT
        if (ntpSynced()) {
            if (_sensor_energy_reset_ts.length() == 0) {
                _sensor_energy_reset_ts = ntpDateTime(now() - millis() / 1000);

            } else {
                _sensor_energy_reset_ts = ntpDateTime(now());
            }

        } else {
            _sensor_energy_reset_ts = String();
        }

        setSetting("snsResetTS", _sensor_energy_reset_ts);
    #endif
}

// -----------------------------------------------------------------------------

double _sensorEnergyTotal() {
    double value = 0;

    if (rtcmemStatus()) {
        value = Rtcmem->energy;

    } else {
        value = (_sensor_save_every > 0) ? getSetting("eneTotal", 0).toInt() : 0;
    }

    return value;
}

// -----------------------------------------------------------------------------

void _sensorEnergyTotal(
    double value
) {
    static uint32_t save_count = 0;

    // Save to EEPROM every '_sensor_save_every' readings
    if (_sensor_save_every > 0) {
        save_count = (save_count + 1) % _sensor_save_every;

        if (save_count == 0) {
            setSetting("eneTotal", value);
            saveSettings();
        }
    }

    // Always save to RTCMEM
    Rtcmem->energy = value;
}

// -----------------------------------------------------------------------------
// Sensor initialization
// -----------------------------------------------------------------------------

void _sensorLoad() {
    /*

    This is temporal, in the future sensors will be initialized based on
    soft configuration (data stored in EEPROM config) so you will be able
    to define and configure new sensors on the fly

    At the time being, only enabled sensors (those with *_SUPPORT to 1) are being
    loaded and initialized here. If you want to add new sensors of the same type
    just duplicate the block and change the arguments for the set* methods.

     */

    #if AM2320_SUPPORT
    {
        AM2320Sensor * sensor = new AM2320Sensor();

        sensor->setAddress(AM2320_ADDRESS);

        _sensors.push_back(sensor);
    }
    #endif

    #if ANALOG_SUPPORT
    {
        AnalogSensor * sensor = new AnalogSensor();

        sensor->setSamples(ANALOG_SAMPLES);
        sensor->setDelay(ANALOG_DELAY);
        //CICM For analog scaling
        sensor->setFactor(ANALOG_FACTOR);
        sensor->setOffset(ANALOG_OFFSET);

        _sensors.push_back(sensor);
    }
    #endif

    #if BH1750_SUPPORT
    {
        BH1750Sensor * sensor = new BH1750Sensor();

        sensor->setAddress(BH1750_ADDRESS);
        sensor->setMode(BH1750_MODE);

        _sensors.push_back(sensor);
    }
    #endif

    #if BMP180_SUPPORT
    {
        BMP180Sensor * sensor = new BMP180Sensor();

        sensor->setAddress(BMP180_ADDRESS);

        _sensors.push_back(sensor);
    }
    #endif

    #if BMX280_SUPPORT
    {
        // Support up to two sensors with full auto-discovery.
        const uint8_t number = constrain(getSetting("bmx280Number", BMX280_NUMBER).toInt(), 1, 2);

        // For second sensor, if BMX280_ADDRESS is 0x00 then auto-discover
        // otherwise choose the other unnamed sensor address
        const uint8_t first = getSetting("bmx280Address", BMX280_ADDRESS).toInt();
        const uint8_t second = (first == 0x00) ? 0x00 : (0x76 + 0x77 - first);

        const uint8_t address_map[2] = { first, second };

        for (uint8_t n = 0; n < number; ++n) {
            BMX280Sensor * sensor = new BMX280Sensor();

            sensor->setAddress(address_map[n]);

            _sensors.push_back(sensor);
        }
    }
    #endif

    #if CSE7766_SUPPORT
    {
        CSE7766Sensor * sensor = new CSE7766Sensor();

        sensor->setRX(CSE7766_PIN);

        _sensors.push_back(sensor);
    }
    #endif

    #if DALLAS_SUPPORT
    {
        DallasSensor * sensor = new DallasSensor();

        sensor->setGPIO(DALLAS_PIN);

        _sensors.push_back(sensor);
    }
    #endif

    #if DHT_SUPPORT
    {
        DHTSensor * sensor = new DHTSensor();

        sensor->setGPIO(DHT_PIN);
        sensor->setType(DHT_TYPE);

        _sensors.push_back(sensor);
    }
    #endif

    #if DIGITAL_SUPPORT
    {
        DigitalSensor * sensor = new DigitalSensor();

        sensor->setGPIO(DIGITAL_PIN);
        sensor->setMode(DIGITAL_PIN_MODE);
        sensor->setDefault(DIGITAL_DEFAULT_STATE);

        _sensors.push_back(sensor);
    }
    #endif

    #if ECH1560_SUPPORT
    {
        ECH1560Sensor * sensor = new ECH1560Sensor();

        sensor->setCLK(ECH1560_CLK_PIN);
        sensor->setMISO(ECH1560_MISO_PIN);
        sensor->setInverted(ECH1560_INVERTED);

        _sensors.push_back(sensor);
    }
    #endif

    #if EMON_ADC121_SUPPORT
    {
        EmonADC121Sensor * sensor = new EmonADC121Sensor();

        sensor->setAddress(EMON_ADC121_I2C_ADDRESS);
        sensor->setVoltage(EMON_MAINS_VOLTAGE);
        sensor->setReference(EMON_REFERENCE_VOLTAGE);
        sensor->setCurrentRatio(0, EMON_CURRENT_RATIO);

        _sensors.push_back(sensor);
    }
    #endif

    #if EMON_ADS1X15_SUPPORT
    {
        EmonADS1X15Sensor * sensor = new EmonADS1X15Sensor();

        sensor->setAddress(EMON_ADS1X15_I2C_ADDRESS);
        sensor->setType(EMON_ADS1X15_TYPE);
        sensor->setMask(EMON_ADS1X15_MASK);
        sensor->setGain(EMON_ADS1X15_GAIN);
        sensor->setVoltage(EMON_MAINS_VOLTAGE);
        sensor->setCurrentRatio(0, EMON_CURRENT_RATIO);
        sensor->setCurrentRatio(1, EMON_CURRENT_RATIO);
        sensor->setCurrentRatio(2, EMON_CURRENT_RATIO);
        sensor->setCurrentRatio(3, EMON_CURRENT_RATIO);

        _sensors.push_back(sensor);
    }
    #endif

    #if EMON_ANALOG_SUPPORT
    {
        EmonAnalogSensor * sensor = new EmonAnalogSensor();

        sensor->setVoltage(EMON_MAINS_VOLTAGE);
        sensor->setReference(EMON_REFERENCE_VOLTAGE);
        sensor->setCurrentRatio(0, EMON_CURRENT_RATIO);

        _sensors.push_back(sensor);
    }
    #endif

    #if EVENTS_SUPPORT
    {
        EventSensor * sensor = new EventSensor();

        sensor->setGPIO(EVENTS_PIN);
        sensor->setTrigger(EVENTS_TRIGGER);
        sensor->setPinMode(EVENTS_PIN_MODE);
        sensor->setDebounceTime(EVENTS_DEBOUNCE);
        sensor->setInterruptMode(EVENTS_INTERRUPT_MODE);

        _sensors.push_back(sensor);
    }
    #endif

    #if GEIGER_SUPPORT
    {
        GeigerSensor * sensor = new GeigerSensor();        // Create instance of thr Geiger module.

        sensor->setGPIO(GEIGER_PIN);                       // Interrupt pin of the attached geiger counter board.
        sensor->setMode(GEIGER_PIN_MODE);                  // This pin is an input.
        sensor->setDebounceTime(GEIGER_DEBOUNCE);          // Debounce time 25ms, because https://github.com/Trickx/espurna/wiki/Geiger-counter
        sensor->setInterruptMode(GEIGER_INTERRUPT_MODE);   // Interrupt triggering: edge detection rising.
        sensor->setCPM2SievertFactor(GEIGER_CPM2SIEVERT);  // Conversion factor from counts per minute to µSv/h

        _sensors.push_back(sensor);
    }
    #endif

    #if GUVAS12SD_SUPPORT
    {
        GUVAS12SDSensor * sensor = new GUVAS12SDSensor();

        sensor->setGPIO(GUVAS12SD_PIN);

        _sensors.push_back(sensor);
    }
    #endif

    #if SONAR_SUPPORT
    {
        SonarSensor * sensor = new SonarSensor();

        sensor->setEcho(SONAR_ECHO);
        sensor->setIterations(SONAR_ITERATIONS);
        sensor->setMaxDistance(SONAR_MAX_DISTANCE);
        sensor->setTrigger(SONAR_TRIGGER);

        _sensors.push_back(sensor);
    }
    #endif

    #if HLW8012_SUPPORT
    {
        HLW8012Sensor * sensor = new HLW8012Sensor();

        sensor->setSEL(HLW8012_SEL_PIN);
        sensor->setCF(HLW8012_CF_PIN);
        sensor->setCF1(HLW8012_CF1_PIN);
        sensor->setSELCurrent(HLW8012_SEL_CURRENT);

        _sensors.push_back(sensor);
    }
    #endif

    #if LDR_SUPPORT
    {
        LDRSensor * sensor = new LDRSensor();

        sensor->setSamples(LDR_SAMPLES);
        sensor->setDelay(LDR_DELAY);
        sensor->setType(LDR_TYPE);
        sensor->setPhotocellPositionOnGround(LDR_ON_GROUND);
        sensor->setResistor(LDR_RESISTOR);
        sensor->setPhotocellParameters(LDR_MULTIPLICATION, LDR_POWER);

        _sensors.push_back(sensor);
    }
    #endif

    #if MHZ19_SUPPORT
    {
        MHZ19Sensor * sensor = new MHZ19Sensor();

        sensor->setRX(MHZ19_RX_PIN);
        sensor->setTX(MHZ19_TX_PIN);

        if (getSetting("mhz19CalibrateAuto", 0).toInt() == 1) {
            sensor->setCalibrateAuto(true);
        }

        _sensors.push_back(sensor);
    }
    #endif

    #if MICS2710_SUPPORT
    {
        MICS2710Sensor * sensor = new MICS2710Sensor();

        sensor->setAnalogGPIO(MICS2710_NOX_PIN);
        sensor->setPreHeatGPIO(MICS2710_PRE_PIN);
        sensor->setRL(MICS2710_RL);

        _sensors.push_back(sensor);
    }
    #endif

    #if MICS5525_SUPPORT
    {
        MICS5525Sensor * sensor = new MICS5525Sensor();

        sensor->setAnalogGPIO(MICS5525_RED_PIN);
        sensor->setRL(MICS5525_RL);

        _sensors.push_back(sensor);
    }
    #endif

    #if NTC_SUPPORT
    {
        NTCSensor * sensor = new NTCSensor();

        sensor->setSamples(NTC_SAMPLES);
        sensor->setDelay(NTC_DELAY);
        sensor->setUpstreamResistor(NTC_R_UP);
        sensor->setDownstreamResistor(NTC_R_DOWN);
        sensor->setBeta(NTC_BETA);
        sensor->setR0(NTC_R0);
        sensor->setT0(NTC_T0);

        _sensors.push_back(sensor);
    }
    #endif

    #if PMSX003_SUPPORT
    {
        PMSX003Sensor * sensor = new PMSX003Sensor();

        #if PMS_USE_SOFT
            sensor->setRX(PMS_RX_PIN);
            sensor->setTX(PMS_TX_PIN);
        #else
            sensor->setSerial(& PMS_HW_PORT);
        #endif

        sensor->setType(PMS_TYPE);

        _sensors.push_back(sensor);
    }
    #endif

    #if PULSEMETER_SUPPORT
    {
        PulseMeterSensor * sensor = new PulseMeterSensor();

        sensor->setGPIO(PULSEMETER_PIN);
        sensor->setEnergyRatio(PULSEMETER_ENERGY_RATIO);
        sensor->setDebounceTime(PULSEMETER_DEBOUNCE);

        _sensors.push_back(sensor);
    }
    #endif

    #if PZEM004T_SUPPORT
    {
        String addresses = getSetting("pzemAddr", PZEM004T_ADDRESSES);

        if (!addresses.length()) {
            DEBUG_MSG(PSTR("[SENSOR] PZEM004T Error: no addresses are configured\n"));
            return;
        }

        PZEM004TSensor * sensor = pzem004t_sensor = new PZEM004TSensor();

        sensor->setAddresses(addresses.c_str());

        if (getSetting("pzemSoft", PZEM004T_USE_SOFT).toInt() == 1) {
            sensor->setRX(getSetting("pzemRX", PZEM004T_RX_PIN).toInt());
            sensor->setTX(getSetting("pzemTX", PZEM004T_TX_PIN).toInt());

        } else {
            sensor->setSerial(& PZEM004T_HW_PORT);
        }

        // Read saved energy offset
        uint8_t dev_count = sensor->getAddressesCount();

        for (uint8_t dev = 0; dev < dev_count; dev++) {
            float value = getSetting("pzemEneTotal", dev, 0).toFloat();

            if (value > 0) {
                sensor->resetEnergy(dev, value);
            }
        }

        _sensors.push_back(sensor);
    }
    #endif

    #if SENSEAIR_SUPPORT
    {
        SenseAirSensor * sensor = new SenseAirSensor();

        sensor->setRX(SENSEAIR_RX_PIN);
        sensor->setTX(SENSEAIR_TX_PIN);

        _sensors.push_back(sensor);
    }
    #endif

    #if SDS011_SUPPORT
    {
        SDS011Sensor * sensor = new SDS011Sensor();

        sensor->setRX(SDS011_RX_PIN);
        sensor->setTX(SDS011_TX_PIN);

        _sensors.push_back(sensor);
    }
    #endif

    #if SHT3X_I2C_SUPPORT
    {
        SHT3XI2CSensor * sensor = new SHT3XI2CSensor();

        sensor->setAddress(SHT3X_I2C_ADDRESS);

        _sensors.push_back(sensor);
    }
    #endif

    #if SI7021_SUPPORT
    {
        SI7021Sensor * sensor = new SI7021Sensor();

        sensor->setAddress(SI7021_ADDRESS);

        _sensors.push_back(sensor);
    }
    #endif

    #if TMP3X_SUPPORT
    {
        TMP3XSensor * sensor = new TMP3XSensor();

        sensor->setType(TMP3X_TYPE);

        _sensors.push_back(sensor);
    }
    #endif

    #if V9261F_SUPPORT
    {
        V9261FSensor * sensor = new V9261FSensor();

        sensor->setRX(V9261F_PIN);
        sensor->setInverted(V9261F_PIN_INVERSE);

        _sensors.push_back(sensor);
    }
    #endif

    #if MAX6675_SUPPORT
    {
        MAX6675Sensor * sensor = new MAX6675Sensor();

        sensor->setCS(MAX6675_CS_PIN);
        sensor->setSO(MAX6675_SO_PIN);
        sensor->setSCK(MAX6675_SCK_PIN);

        _sensors.push_back(sensor);
    }
    #endif

    #if VEML6075_SUPPORT
    {
        VEML6075Sensor * sensor = new VEML6075Sensor();

        sensor->setIntegrationTime(VEML6075_INTEGRATION_TIME);
        sensor->setDynamicMode(VEML6075_DYNAMIC_MODE);

        _sensors.push_back(sensor);
    }
    #endif

    #if VL53L1X_SUPPORT
    {
        VL53L1XSensor * sensor = new VL53L1XSensor();

        sensor->setInterMeasurementPeriod(VL53L1X_INTER_MEASUREMENT_PERIOD);
        sensor->setDistanceMode(VL53L1X_DISTANCE_MODE);
        sensor->setMeasurementTimingBudget(VL53L1X_MEASUREMENT_TIMING_BUDGET);

        _sensors.push_back(sensor);
    }
    #endif

    #if EZOPH_SUPPORT
    {
        EZOPHSensor * sensor = new EZOPHSensor();

        sensor->setRX(EZOPH_RX_PIN);
        sensor->setTX(EZOPH_TX_PIN);

        _sensors.push_back(sensor);
    }
    #endif
}

// -----------------------------------------------------------------------------

void _sensorCallback(
    uint8_t i,
    uint8_t type,
    double value
) {
    DEBUG_MSG(PSTR("[SENSOR] Sensor #%u callback, type %u, payload: '%s'\n"), i, type, String(value).c_str());

    for (uint8_t k = 0; k < _magnitudes.size(); k++) {
        if ((_sensors[i] == _magnitudes[k].sensor) && (type == _magnitudes[k].type)) {
            _sensorReport(k, value);
            return;
        }
    }
}

// -----------------------------------------------------------------------------

void _sensorInit() {
    _sensors_ready = true;
    _sensor_save_every = getSetting("snsSave", 0).toInt();

    for (uint8_t i = 0; i < _sensors.size(); i++) {
        // Do not process an already initialized sensor
        if (_sensors[i]->ready()) {
            continue;
        }

        DEBUG_MSG(PSTR("[SENSOR] Initializing %s\n"), _sensors[i]->description().c_str());

        // Force sensor to reload config
        _sensors[i]->begin();

        if (!_sensors[i]->ready()) {
            if (_sensors[i]->error() != 0) {
                DEBUG_MSG(PSTR("[SENSOR]  -> ERROR %d\n"), _sensors[i]->error());
            }

            _sensors_ready = false;
            continue;
        }

        // Initialize magnitudes
        for (uint8_t k = 0; k < _sensors[i]->count(); k++) {
            // Detect magnitude type
            uint8_t type = _sensors[i]->type(k);

	        signed char decimals = _sensors[i]->decimals(type);

	        if (decimals < 0) {
                decimals = _magnitudeDecimals(type);
            }

            sensor_magnitude_t new_magnitude;

            new_magnitude.sensor = _sensors[i];
            new_magnitude.local = k;
            new_magnitude.type = type;
	        new_magnitude.decimals = (uint8_t) decimals;
            new_magnitude.global = _magnitudes_count[type];
            new_magnitude.last = 0;
            new_magnitude.reported = 0;
            new_magnitude.min_change = 0;
            new_magnitude.max_change = 0;

            // TODO: find a proper way to extend this to min/max of any magnitude
            if (type == MAGNITUDE_ENERGY) {
                new_magnitude.max_change = getSetting("eneMaxDelta", ENERGY_MAX_CHANGE).toFloat();

            } else if (type == MAGNITUDE_TEMPERATURE) {
                new_magnitude.min_change = getSetting("tmpMinDelta", TEMPERATURE_MIN_CHANGE).toFloat();

            } else if (type == MAGNITUDE_HUMIDITY) {
                new_magnitude.min_change = getSetting("humMinDelta", HUMIDITY_MIN_CHANGE).toFloat();
            }

            if (type == MAGNITUDE_ENERGY) {
                new_magnitude.filter = new LastFilter();

            } else if (type == MAGNITUDE_DIGITAL) {
                new_magnitude.filter = new MaxFilter();

            } else if (
                type == MAGNITUDE_COUNT
                || type == MAGNITUDE_GEIGER_CPM
                || type == MAGNITUDE_GEIGER_SIEVERT
            ) {  // For geiger counting moving average filter is the most appropriate if needed at all.
                new_magnitude.filter = new MovingAverageFilter();
            } else {
                new_magnitude.filter = new MedianFilter();
            }

            new_magnitude.filter->resize(_sensor_report_every);

            _magnitudes.push_back(new_magnitude);

            DEBUG_MSG(PSTR("[SENSOR]  -> %s:%d\n"), magnitudeName(new_magnitude.type).c_str(), _magnitudes_count[type]);

            // Magnitudes counter
            _magnitudes_count[type] = _magnitudes_count[type] + 1;
        } // for (uint8_t k = 0; k < _sensors[i]->count(); k++)

        // Hook callback
        _sensors[i]->onEvent([i](uint8_t type, double value) {
            _sensorCallback(i, type, value);
        });

        // Custom initializations

        #if MICS2710_SUPPORT
            if (_sensors[i]->getID() == SENSOR_MICS2710_ID) {
                MICS2710Sensor * sensor = (MICS2710Sensor *) _sensors[i];

                sensor->setR0(getSetting("snsR0", MICS2710_R0).toInt());
            }
        #endif // MICS2710_SUPPORT

        #if MICS5525_SUPPORT
            if (_sensors[i]->getID() == SENSOR_MICS5525_ID) {
                MICS5525Sensor * sensor = (MICS5525Sensor *) _sensors[i];

                sensor->setR0(getSetting("snsR0", MICS5525_R0).toInt());
            }
        #endif // MICS5525_SUPPORT

        #if EMON_ANALOG_SUPPORT
            if (_sensors[i]->getID() == SENSOR_EMON_ANALOG_ID) {
                EmonAnalogSensor * sensor = (EmonAnalogSensor *) _sensors[i];

                sensor->setCurrentRatio(0, getSetting("pwrRatioC", EMON_CURRENT_RATIO).toFloat());
                sensor->setVoltage(getSetting("pwrVoltage", EMON_MAINS_VOLTAGE).toInt());

                double value = _sensorEnergyTotal();

                if (value > 0) {
                    sensor->resetEnergy(0, value);
                }
            }
        #endif // EMON_ANALOG_SUPPORT

        #if HLW8012_SUPPORT
            if (_sensors[i]->getID() == SENSOR_HLW8012_ID) {
                HLW8012Sensor * sensor = (HLW8012Sensor *) _sensors[i];

                double value;

                value = getSetting("pwrRatioC", HLW8012_CURRENT_RATIO).toFloat();

                if (value > 0) {
                    sensor->setCurrentRatio(value);
                }

                value = getSetting("pwrRatioV", HLW8012_VOLTAGE_RATIO).toFloat();

                if (value > 0) {
                    sensor->setVoltageRatio(value);
                }

                value = getSetting("pwrRatioP", HLW8012_POWER_RATIO).toFloat();

                if (value > 0) {
                    sensor->setPowerRatio(value);
                }

                value = _sensorEnergyTotal();

                if (value > 0) {
                    sensor->resetEnergy(value);
                }
            }
        #endif // HLW8012_SUPPORT

        #if CSE7766_SUPPORT
            if (_sensors[i]->getID() == SENSOR_CSE7766_ID) {
                CSE7766Sensor * sensor = (CSE7766Sensor *) _sensors[i];

                double value;

                value = getSetting("pwrRatioC", 0).toFloat();

                if (value > 0) {
                    sensor->setCurrentRatio(value);
                }

                value = getSetting("pwrRatioV", 0).toFloat();

                if (value > 0) {
                    sensor->setVoltageRatio(value);
                }

                value = getSetting("pwrRatioP", 0).toFloat();

                if (value > 0) {
                    sensor->setPowerRatio(value);
                }

                value = _sensorEnergyTotal();

                if (value > 0) {
                    sensor->resetEnergy(value);
                }
            }
        #endif // CSE7766_SUPPORT

        #if PULSEMETER_SUPPORT
            if (_sensors[i]->getID() == SENSOR_PULSEMETER_ID) {
                PulseMeterSensor * sensor = (PulseMeterSensor *) _sensors[i];

                sensor->setEnergyRatio(getSetting("pwrRatioE", PULSEMETER_ENERGY_RATIO).toInt());
            }
        #endif // PULSEMETER_SUPPORT
    }
}

// -----------------------------------------------------------------------------

void _sensorConfigure() {
    // General sensor settings
    _sensor_read_interval = 1000 * constrain(getSetting("snsRead", SENSOR_READ_INTERVAL).toInt(), SENSOR_READ_MIN_INTERVAL, SENSOR_READ_MAX_INTERVAL);
    _sensor_report_every = constrain(getSetting("snsReport", SENSOR_REPORT_EVERY).toInt(), SENSOR_REPORT_MIN_EVERY, SENSOR_REPORT_MAX_EVERY);
    _sensor_save_every = getSetting("snsSave", SENSOR_SAVE_EVERY).toInt();
    _sensor_energy_reset_ts = getSetting("snsResetTS", "");

    _sensor_power_units = getSetting("pwrUnits", SENSOR_POWER_UNITS).toInt();
    _sensor_energy_units = getSetting("eneUnits", SENSOR_ENERGY_UNITS).toInt();
    _sensor_temperature_units = getSetting("tmpUnits", SENSOR_TEMPERATURE_UNITS).toInt();
    _sensor_temperature_correction = getSetting("tmpCorrection", SENSOR_TEMPERATURE_CORRECTION).toFloat();
    _sensor_humidity_correction = getSetting("humCorrection", SENSOR_HUMIDITY_CORRECTION).toFloat();
    _sensor_lux_correction = getSetting("luxCorrection", SENSOR_LUX_CORRECTION).toFloat();

    // Specific sensor settings
    for (uint8_t i = 0; i < _sensors.size(); i++) {
        #if EMON_ANALOG_SUPPORT
            if (_sensors[i]->getID() == SENSOR_EMON_ANALOG_ID) {
                double value;

                EmonAnalogSensor * sensor = (EmonAnalogSensor *) _sensors[i];

                if ((value = getSetting("pwrExpectedP", 0).toInt())) {
                    sensor->expectedPower(0, value);

                    setSetting("pwrRatioC", sensor->getCurrentRatio(0));
                }

                sensor->setVoltage(getSetting("pwrVoltage", EMON_MAINS_VOLTAGE).toInt());
            }
        #endif // EMON_ANALOG_SUPPORT

        #if HLW8012_SUPPORT
            if (_sensors[i]->getID() == SENSOR_HLW8012_ID) {
                double value;

                HLW8012Sensor * sensor = (HLW8012Sensor *) _sensors[i];

                if (value = getSetting("pwrExpectedC", 0).toFloat()) {
                    sensor->expectedCurrent(value);

                    setSetting("pwrRatioC", sensor->getCurrentRatio());
                }

                if (value = getSetting("pwrExpectedV", 0).toInt()) {
                    sensor->expectedVoltage(value);

                    setSetting("pwrRatioV", sensor->getVoltageRatio());
                }

                if (value = getSetting("pwrExpectedP", 0).toInt()) {
                    sensor->expectedPower(value);

                    setSetting("pwrRatioP", sensor->getPowerRatio());
                }
            }
        #endif // HLW8012_SUPPORT

        #if CSE7766_SUPPORT
            if (_sensors[i]->getID() == SENSOR_CSE7766_ID) {
                double value;

                CSE7766Sensor * sensor = (CSE7766Sensor *) _sensors[i];

                if ((value = getSetting("pwrExpectedC", 0).toFloat())) {
                    sensor->expectedCurrent(value);

                    setSetting("pwrRatioC", sensor->getCurrentRatio());
                }

                if ((value = getSetting("pwrExpectedV", 0).toInt())) {
                    sensor->expectedVoltage(value);

                    setSetting("pwrRatioV", sensor->getVoltageRatio());
                }

                if ((value = getSetting("pwrExpectedP", 0).toInt())) {
                    sensor->expectedPower(value);

                    setSetting("pwrRatioP", sensor->getPowerRatio());
                }
            }
        #endif // CSE7766_SUPPORT

        #if PULSEMETER_SUPPORT
            if (_sensors[i]->getID() == SENSOR_PULSEMETER_ID) {
                PulseMeterSensor * sensor = (PulseMeterSensor *) _sensors[i];

                sensor->setEnergyRatio(getSetting("pwrRatioE", PULSEMETER_ENERGY_RATIO).toInt());
            }
        #endif // PULSEMETER_SUPPORT
    }

    // Update filter sizes
    for (uint8_t i = 0; i < _magnitudes.size(); i++) {
        _magnitudes[i].filter->resize(_sensor_report_every);
    }

    // General processing
    if (_sensor_save_every == 0) {
        delSetting("eneTotal");
    }

    // Save settings
    delSetting("pwrExpectedP");
    delSetting("pwrExpectedC");
    delSetting("pwrExpectedV");

    saveSettings();
}

// -----------------------------------------------------------------------------

void _sensorReport(
    uint8_t index,
    double value
) {
    sensor_magnitude_t magnitude = _magnitudes[index];

    uint8_t decimals = magnitude.decimals;

    char buffer[10];

    dtostrf(value, 1-sizeof(buffer), decimals, buffer);

    // TODO: implement FB sending
}

// -----------------------------------------------------------------------------
// MODULE API
// -----------------------------------------------------------------------------

uint8_t sensorCount() {
    return _sensors.size();
}

// -----------------------------------------------------------------------------

uint8_t magnitudeCount() {
    return _magnitudes.size();
}

// -----------------------------------------------------------------------------

String magnitudeName(
    uint8_t type
) {
    char buffer[16] = {0};

    if (type < MAGNITUDE_MAX) {
        strncpy_P(buffer, magnitude_names[type], sizeof(buffer));
    }

    return String(buffer);
}

// -----------------------------------------------------------------------------

uint8_t magnitudeType(
    uint8_t index
) {
    if (index < _magnitudes.size()) {
        return int(_magnitudes[index].type);
    }

    return MAGNITUDE_NONE;
}

// -----------------------------------------------------------------------------

double magnitudeValue(
    uint8_t index
) {
    if (index < _magnitudes.size()) {
        return _magnitudes[index].reported;
    }

    return DBL_MIN;
}

// -----------------------------------------------------------------------------

uint8_t magnitudeIndex(
    uint8_t index
) {
    if (index < _magnitudes.size()) {
        return int(_magnitudes[index].global);
    }

    return 0;
}

// -----------------------------------------------------------------------------

String magnitudeUnits(
    uint8_t type
) {
    char buffer[8] = {0};

    if (type < MAGNITUDE_MAX) {
        if (
            type == MAGNITUDE_TEMPERATURE
            && _sensor_temperature_units == TMP_FAHRENHEIT
        ) {
            strncpy_P(buffer, magnitude_fahrenheit, sizeof(buffer));

        } else if (
            (
                type == MAGNITUDE_ENERGY
                || type == MAGNITUDE_ENERGY_DELTA
            )
            && _sensor_energy_units == ENERGY_KWH
        ) {
            strncpy_P(buffer, magnitude_kwh, sizeof(buffer));

        } else if (
            (
                type == MAGNITUDE_POWER_ACTIVE
                || type == MAGNITUDE_POWER_APPARENT
                || type == MAGNITUDE_POWER_REACTIVE
            )
            && _sensor_power_units == POWER_KILOWATTS
        ) {
            strncpy_P(buffer, magnitude_kw, sizeof(buffer));

        } else {
            strncpy_P(buffer, magnitude_units[type], sizeof(buffer));
        }
    }

    return String(buffer);
}

// -----------------------------------------------------------------------------
// MODULE CORE
// -----------------------------------------------------------------------------

void sensorSetup() {
    // Load sensors
    _sensorLoad();
    _sensorInit();

    // Configure stored values
    _sensorConfigure();

    // Main callbacks
    firmwareRegisterLoop(sensorLoop);
    firmwareRegisterReload(_sensorConfigure);

    // Websockets
    #if WEB_SUPPORT && WS_SUPPORT
        wsOnConnectRegister(_sensorWSOnConnect);
        wsOnConfigureRegister(_sensorWSOnConfigure);
        wsOnActionRegister(_sensorWSOnAction);
    #endif

    #if FASTYBIRD_SUPPORT
        // Module schema report
        fastybirdReportConfigurationSchemaRegister(_sensorReportConfigurationSchema);
        fastybirdReportConfigurationRegister(_sensorReportConfiguration);
        fastybirdOnConfigureRegister(_sensorUpdateConfiguration);

        // Channels registration
        if (sensorCount() > 0) {
            for (uint8_t i = 0; i < sensorCount(); i++) {
                _sensor_fastybird_channel_index = fastybirdRegisterChannel(_sensorFastybirdGetChannelStructure(i));
            }
        }
    #endif

    DEBUG_MSG(PSTR("[SENSOR] Number of sensors: %d and magnitudes: %d\n"), sensorCount(), magnitudeCount());
}

// -----------------------------------------------------------------------------

void sensorLoop() {
    // Check if we still have uninitialized sensors
    static uint32_t last_init = 0;

    if (!_sensors_ready) {
        if (millis() - last_init > SENSOR_INIT_INTERVAL) {
            last_init = millis();

            _sensorInit();
        }
    }

    if (_magnitudes.size() == 0) {
        return;
    }

    // Tick hook
    _sensorTick();

    // Check if we should read new data
    static uint32_t last_update = 0;
    static uint32_t report_count = 0;

    if (millis() - last_update > _sensor_read_interval) {
        last_update = millis();
        report_count = (report_count + 1) % _sensor_report_every;

        double value_raw;       // holds the raw value as the sensor returns it
        double value_show;      // holds the processed value applying units and decimals
        double value_filtered;  // holds the processed value applying filters, and the units and decimals

        // Pre-read hook
        _sensorPreRead();

        // Get the first relay state
        #if SENSOR_POWER_CHECK_STATUS
            bool relay_off = (relayCount() == 1) && (relayStatus(0) == 0);
        #endif

        // Get readings
        for (uint8_t i = 0; i < _magnitudes.size(); i++) {
            sensor_magnitude_t magnitude = _magnitudes[i];

            if (magnitude.sensor->status()) {
                // -------------------------------------------------------------
                // Instant value
                // -------------------------------------------------------------

                value_raw = magnitude.sensor->value(magnitude.local);

                // Completely remove spurious values if relay is OFF
                #if SENSOR_POWER_CHECK_STATUS
                    if (relay_off) {
                        if (
                            magnitude.type == MAGNITUDE_POWER_ACTIVE
                            || magnitude.type == MAGNITUDE_POWER_REACTIVE
                            || magnitude.type == MAGNITUDE_POWER_APPARENT
                            || magnitude.type == MAGNITUDE_CURRENT
                            || magnitude.type == MAGNITUDE_ENERGY_DELTA
                        ) {
                            value_raw = 0;
                        }
                    }
                #endif

                _magnitudes[i].last = value_raw;

                // -------------------------------------------------------------
                // Processing (filters)
                // -------------------------------------------------------------

                magnitude.filter->add(value_raw);

                // Special case for MovingAverageFilter
                if (
                    MAGNITUDE_COUNT == magnitude.type
                    || MAGNITUDE_GEIGER_CPM == magnitude. type
                    || MAGNITUDE_GEIGER_SIEVERT == magnitude.type
                ) {
                    value_raw = magnitude.filter->result();
                }

                // -------------------------------------------------------------
                // Procesing (units and decimals)
                // -------------------------------------------------------------

                value_show = _magnitudeProcess(magnitude.type, magnitude.decimals, value_raw);

                // -------------------------------------------------------------
                // Debug
                // -------------------------------------------------------------

                #if SENSOR_DEBUG
                {
                    char buffer[64];

                    dtostrf(value_show, 1 - sizeof(buffer), magnitude.decimals, buffer);

                    DEBUG_MSG(PSTR("[SENSOR] %s - %s: %s%s\n"),
                        magnitude.sensor->slot(magnitude.local).c_str(),
                        magnitudeName(magnitude.type).c_str(),
                        buffer,
                        magnitudeName(magnitude.type).c_str()
                    );
                }
                #endif // SENSOR_DEBUG

                // -------------------------------------------------------------
                // Report
                // (we do it every _sensor_report_every readings)
                // -------------------------------------------------------------

                bool report = (0 == report_count);

                if (
                    magnitude.type == MAGNITUDE_ENERGY
                    && magnitude.max_change > 0
                ) {
                    // for MAGNITUDE_ENERGY, filtered value is last value
                    report = (fabs(value_show - magnitude.reported) >= magnitude.max_change);
                }

                if (report) {
                    value_filtered = magnitude.filter->result();
                    value_filtered = _magnitudeProcess(magnitude.type, magnitude.decimals, value_filtered);

                    magnitude.filter->reset();

                    // Check if there is a minimum change threshold to report
                    if (fabs(value_filtered - magnitude.reported) >= magnitude.min_change) {
                        _magnitudes[i].reported = value_filtered;

                        _sensorReport(i, value_filtered);
                    }

                    // Persist total energy value
                    if (magnitude.type == MAGNITUDE_ENERGY) {
                        _sensorEnergyTotal(value_raw);
                    }
                }
            } // if (magnitude.sensor->status())
        } // for (uint8_t i = 0; i < _magnitudes.size(); i++)

        // Post-read hook
        _sensorPost();

        #if WEB_SUPPORT && WS_SUPPORT
            wsSend(_sensorWebSocketSendData);
        #endif
    }
}

#endif // SENSOR_SUPPORT
