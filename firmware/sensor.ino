/*

SENSOR MODULE

Copyright (C) 2018 FastyBird s.r.o. <code@fastybird.com>

*/

#if SENSOR_SUPPORT

#include <float.h>

std::vector<BaseSensor *> _sensors;

uint32_t _sensor_read_interval = 1000 * SENSOR_READ_INTERVAL;
uint8_t _sensor_report_every = SENSOR_REPORT_EVERY;
uint8_t _sensor_save_every = SENSOR_SAVE_EVERY;

uint8_t _sensor_power_units = SENSOR_POWER_UNITS;
uint8_t _sensor_energy_units = SENSOR_ENERGY_UNITS;
uint8_t _sensor_temperature_units = SENSOR_TEMPERATURE_UNITS;
double _sensor_temperature_correction = SENSOR_TEMPERATURE_CORRECTION;
double _sensor_humidity_correction = SENSOR_HUMIDITY_CORRECTION;
double _sensor_lux_correction = SENSOR_LUX_CORRECTION;

String _sensor_energy_reset_ts = String();

#define FILLARRAY(a, n) a[0] = n, memcpy(((char *) a) + sizeof(a[0]), a, sizeof(a) - sizeof(a[0]));

// -----------------------------------------------------------------------------
// MODULE PRIVATE
// -----------------------------------------------------------------------------

bool _sensorHasMagnitude(
    uint8_t type
) {
    for (uint8_t i = 0; i < _sensors.size(); i++) {
        for (uint8_t k = 0; k < _sensors[i]->magnitudesCount(); k++) {
            if (_sensors[i]->magnitudeType(k) == type) {
                return true;
            }
        }
    }

    return false;
}

// -----------------------------------------------------------------------------

void _sensorResetPowerCalibration()
{
    for (uint8_t i = 0; i < _sensors.size(); i++) {
        #if HLW8012_SUPPORT
            if (_sensors[i]->getID() == SENSOR_HLW8012_ID) {
                HLW8012Sensor * sensor = (HLW8012Sensor *) _sensors[i];

                sensor->resetRatios();
            }
        #endif // HLW8012_SUPPORT

        #if CSE7766_SUPPORT
            if (_sensors[i]->getID() == SENSOR_CSE7766_ID) {
                CSE7766Sensor * sensor = (CSE7766Sensor *) _sensors[i];

                sensor->resetRatios();
            }
        #endif // CSE7766_SUPPORT
    }

    delSetting("sensor_pwr_ratio_c");
    delSetting("sensor_pwr_ratio_v");
    delSetting("sensor_pwr_ratio_p");

    // Reload & cache settings
    firmwareReload();
}

// -----------------------------------------------------------------------------

void _sensorResetEnergyCalibration()
{
    for (uint8_t i = 0; i < _sensors.size(); i++) {
        #if HLW8012_SUPPORT
            if (_sensors[i]->getID() == SENSOR_HLW8012_ID) {
                HLW8012Sensor * sensor = (HLW8012Sensor *) _sensors[i];

                sensor->resetEnergy();
            }
        #endif // HLW8012_SUPPORT

        #if CSE7766_SUPPORT
            if (_sensors[i]->getID() == SENSOR_CSE7766_ID) {
                CSE7766Sensor * sensor = (CSE7766Sensor *) _sensors[i];

                sensor->resetEnergy();
            }
        #endif // CSE7766_SUPPORT
    }

    delSetting("sensor_ene_total");

    // Reload & cache settings
    firmwareReload();
}

// -----------------------------------------------------------------------------

#if WEB_SUPPORT && WS_SUPPORT
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
        }

        // SENSORS GLOBAL
        JsonObject& sensor_read_interval = configuration.createNestedObject();

        sensor_read_interval["name"] = "read_interval";
        sensor_read_interval["type"] = "select";
        sensor_read_interval["default"] = SENSOR_READ_INTERVAL;

        JsonArray& sensor_read_interval_values = sensor_read_interval.createNestedArray("values");

        JsonObject& sensor_read_interval_values_value_1 = sensor_read_interval_values.createNestedObject();
        sensor_read_interval_values_value_1["value"] = 1;
        sensor_read_interval_values_value_1["name"] = "1";

        JsonObject& sensor_read_interval_values_value_2 = sensor_read_interval_values.createNestedObject();
        sensor_read_interval_values_value_2["value"] = 6;
        sensor_read_interval_values_value_2["name"] = "6";

        JsonObject& sensor_read_interval_values_value_3 = sensor_read_interval_values.createNestedObject();
        sensor_read_interval_values_value_3["value"] = 10;
        sensor_read_interval_values_value_3["name"] = "10";

        JsonObject& sensor_read_interval_values_value_4 = sensor_read_interval_values.createNestedObject();
        sensor_read_interval_values_value_4["value"] = 15;
        sensor_read_interval_values_value_4["name"] = "15";

        JsonObject& sensor_read_interval_values_value_5 = sensor_read_interval_values.createNestedObject();
        sensor_read_interval_values_value_5["value"] = 30;
        sensor_read_interval_values_value_5["name"] = "30";

        JsonObject& sensor_read_interval_values_value_7 = sensor_read_interval_values.createNestedObject();
        sensor_read_interval_values_value_7["value"] = 60;
        sensor_read_interval_values_value_7["name"] = "60";

        JsonObject& sensor_read_interval_values_value_8 = sensor_read_interval_values.createNestedObject();
        sensor_read_interval_values_value_8["value"] = 300;
        sensor_read_interval_values_value_8["name"] = "300";

        JsonObject& sensor_read_interval_values_value_9 = sensor_read_interval_values.createNestedObject();
        sensor_read_interval_values_value_9["value"] = 600;
        sensor_read_interval_values_value_9["name"] = "600";

        JsonObject& sensor_read_interval_values_value_10 = sensor_read_interval_values.createNestedObject();
        sensor_read_interval_values_value_10["value"] = 900;
        sensor_read_interval_values_value_10["name"] = "900";

        JsonObject& sensor_read_interval_values_value_11 = sensor_read_interval_values.createNestedObject();
        sensor_read_interval_values_value_11["value"] = 1800;
        sensor_read_interval_values_value_11["name"] = "1800";

        JsonObject& sensor_read_interval_values_value_12 = sensor_read_interval_values.createNestedObject();
        sensor_read_interval_values_value_12["value"] = 3600;
        sensor_read_interval_values_value_12["name"] = "3600";
        
        JsonObject& sensor_report_interval = configuration.createNestedObject();

        sensor_report_interval["name"] = "report_interval";
        sensor_report_interval["type"] = "number";
        sensor_report_interval["min"] = SENSOR_REPORT_MIN_EVERY;
        sensor_report_interval["max"] = SENSOR_REPORT_MAX_EVERY;
        sensor_report_interval["step"] = SENSOR_REPORT_STEP_EVERY;
        sensor_report_interval["default"] = SENSOR_REPORT_EVERY;

        if (enabled_pwr) {
            // POWER METER
            JsonObject& sensor_save_interval = configuration.createNestedObject();

            sensor_save_interval["name"] = "save_interval";
            sensor_save_interval["type"] = "number";
            sensor_save_interval["min"] = 0;
            sensor_save_interval["max"] = 200;
            sensor_save_interval["step"] = 1;
            sensor_save_interval["default"] = SENSOR_SAVE_EVERY;

            JsonObject& power_units = configuration.createNestedObject();

            power_units["name"] = "power_units";
            power_units["type"] = "select";
            power_units["default"] = SENSOR_POWER_UNITS;

            JsonArray& power_units_values = power_units.createNestedArray("values");

            JsonObject& power_units_values_value_1 = power_units_values.createNestedObject();
            power_units_values_value_1["value"] = SENSOR_POWER_WATTS;
            power_units_values_value_1["name"] = "watts";

            JsonObject& power_units_values_value_2 = power_units_values.createNestedObject();
            power_units_values_value_2["value"] = SENSOR_POWER_KILOWATTS;
            power_units_values_value_2["name"] = "kilowatts";

            // ENERGY METER
            JsonObject& energy_units = configuration.createNestedObject();

            energy_units["name"] = "energy_units";
            energy_units["type"] = "select";
            energy_units["default"] = SENSOR_ENERGY_UNITS;

            JsonArray& energy_units_values = energy_units.createNestedArray("values");

            JsonObject& energy_units_values_value_1 = energy_units_values.createNestedObject();
            energy_units_values_value_1["value"] = SENSOR_ENERGY_JOULES;
            energy_units_values_value_1["name"] = "joules";

            JsonObject& energy_units_values_value_2 = energy_units_values.createNestedObject();
            energy_units_values_value_2["value"] = SENSOR_ENERGY_KWH;
            energy_units_values_value_2["name"] = "kilowatts_hours";
        }

        // ENVIRONMENT METER
        if (_sensorHasMagnitude(MAGNITUDE_TEMPERATURE)) {
            JsonObject& temperature_units = configuration.createNestedObject();

            temperature_units["name"] = "temperature_units";
            temperature_units["type"] = "select";
            temperature_units["default"] = SENSOR_TEMPERATURE_UNITS;

            JsonArray& temperature_units_values = temperature_units.createNestedArray("values");

            JsonObject& temperature_units_values_value_1 = temperature_units_values.createNestedObject();
            temperature_units_values_value_1["value"] = SENSOR_TMP_CELSIUS;
            temperature_units_values_value_1["name"] = "celsius";

            JsonObject& temperature_units_values_value_2 = temperature_units_values.createNestedObject();
            temperature_units_values_value_2["value"] = SENSOR_TMP_FAHRENHEIT;
            temperature_units_values_value_2["name"] = "fahrenheit";

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
        }

        configuration["read_interval"] = _sensor_read_interval / 1000;
        configuration["report_interval"] = _sensor_report_every;

        if (enabled_pwr) {
            configuration["save_interval"] = _sensor_save_every;
            configuration["power_units"] = getSetting("sensor_pwr_units", SENSOR_POWER_UNITS).toInt();
            configuration["energy_units"] = getSetting("sensor_ene_units", SENSOR_ENERGY_UNITS).toInt();
        }

        if (_sensorHasMagnitude(MAGNITUDE_TEMPERATURE)) {
            configuration["temperature_units"] = getSetting("sensor_tmp_units", SENSOR_TEMPERATURE_UNITS).toInt();
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

        if (enabled_hlw || enabled_cse) {
            configuration["expected_current"] = getSetting("sensor_pwr_expected_c", 0).toFloat();
        }

        if (enabled_hlw || enabled_cse) {
            configuration["expected_voltage"] = getSetting("sensor_pwr_expected_v", 0).toInt();
        }

        if (enabled_hlw || enabled_cse || enabled_emon) {
            configuration["expected_power"] = getSetting("sensor_pwr_expected_p", 0).toInt();
        }
    }

// -----------------------------------------------------------------------------

    /**
     * Update module configuration via WS or MQTT etc.
     */
    bool _sensorUpdateConfiguration(
        JsonObject& configuration
    ) {
        DEBUG_MSG(PSTR("[INFO][SENSOR] Updating module\n"));

        bool is_updated = false;

        if (
            configuration.containsKey("read_interval")
            && configuration["read_interval"].as<uint16_t>() >= SENSOR_READ_MIN_INTERVAL
            && configuration["read_interval"].as<uint16_t>() <= SENSOR_READ_MAX_INTERVAL
            && configuration["read_interval"].as<uint16_t>() != getSetting("sensor_read_interval").toInt()
        )  {
            DEBUG_MSG(PSTR("[INFO][SENSOR] Setting: \"sensor_read_interval\" to: %d\n"), configuration["read_interval"].as<uint16_t>());

            setSetting("sensor_read_interval", configuration["read_interval"].as<uint16_t>());

            is_updated = true;
        }

        if (
            configuration.containsKey("report_interval")
            && configuration["report_interval"].as<uint8_t>() >= SENSOR_REPORT_MIN_EVERY
            && configuration["report_interval"].as<uint8_t>() <= SENSOR_REPORT_MAX_EVERY
            && configuration["report_interval"].as<uint8_t>() != getSetting("sensor_report_interval").toInt()
        )  {
            DEBUG_MSG(PSTR("[INFO][SENSOR] Setting: \"sensor_report_interval\" to: %d\n"), configuration["report_interval"].as<uint8_t>());

            setSetting("sensor_report_interval", configuration["report_interval"].as<uint8_t>());

            is_updated = true;
        }

        if (
            configuration.containsKey("save_interval")
            && configuration["save_interval"].as<uint8_t>() >= 0
            && configuration["save_interval"].as<uint8_t>() <= 200
            && configuration["save_interval"].as<uint8_t>() != getSetting("sensor_save_interval").toInt()
        )  {
            DEBUG_MSG(PSTR("[INFO][SENSOR] Setting: \"sensor_save_interval\" to: %d\n"), configuration["save_interval"].as<uint8_t>());

            setSetting("sensor_save_interval", configuration["save_interval"].as<uint8_t>());

            is_updated = true;
        }

        if (
            configuration.containsKey("power_units")
            && (
                configuration["power_units"].as<uint8_t>() == SENSOR_POWER_WATTS
                || configuration["power_units"].as<uint8_t>() == SENSOR_POWER_KILOWATTS
            )
            && configuration["power_units"].as<uint8_t>() != getSetting("sensor_pwr_units").toInt()
        )  {
            DEBUG_MSG(PSTR("[INFO][SENSOR] Setting: \"sensor_pwr_units\" to: %d\n"), configuration["power_units"].as<uint8_t>());

            setSetting("sensor_pwr_units", configuration["power_units"].as<uint8_t>());

            is_updated = true;
        }

        if (
            configuration.containsKey("energy_units")
            && (
                configuration["energy_units"].as<uint8_t>() == SENSOR_ENERGY_JOULES
                || configuration["energy_units"].as<uint8_t>() == SENSOR_ENERGY_KWH
            )
            && configuration["energy_units"].as<uint8_t>() != getSetting("sensor_ene_units").toInt()
        )  {
            DEBUG_MSG(PSTR("[INFO][SENSOR] Setting: \"sensor_ene_units\" to: %d\n"), configuration["energy_units"].as<uint8_t>());

            setSetting("sensor_ene_units", configuration["energy_units"].as<uint8_t>());

            is_updated = true;
        }

        if (
            configuration.containsKey("temperature_units")
            && (
                configuration["temperature_units"].as<uint8_t>() == SENSOR_TMP_CELSIUS
                || configuration["temperature_units"].as<uint8_t>() == SENSOR_TMP_FAHRENHEIT
            )
            && configuration["temperature_units"].as<uint8_t>() != getSetting("sensor_tmp_units").toInt()
        )  {
            DEBUG_MSG(PSTR("[INFO][SENSOR] Setting: \"sensor_tmp_units\" to: %d\n"), configuration["temperature_units"].as<uint8_t>());

            setSetting("sensor_tmp_units", configuration["temperature_units"].as<uint8_t>());

            is_updated = true;
        }

        if (
            configuration.containsKey("temperature_correction")
            && configuration["temperature_correction"].as<float>() >= -100
            && configuration["temperature_correction"].as<float>() <= 100
            && configuration["temperature_correction"].as<float>() != getSetting("sensor_tmp_correction").toFloat()
        )  {
            DEBUG_MSG(PSTR("[INFO][SENSOR] Setting: \"sensor_tmp_correction\" to: %d\n"), configuration["temperature_correction"].as<float>());

            setSetting("sensor_tmp_correction", configuration["temperature_correction"].as<float>());

            is_updated = true;
        }

        if (
            configuration.containsKey("humidity_correction")
            && configuration["humidity_correction"].as<float>() >= -100
            && configuration["humidity_correction"].as<float>() <= 100
            && configuration["humidity_correction"].as<float>() != getSetting("sensor_hum_correction").toFloat()
        )  {
            DEBUG_MSG(PSTR("[INFO][SENSOR] Setting: \"sensor_hum_correction\" to: %d\n"), configuration["humidity_correction"].as<float>());

            setSetting("sensor_hum_correction", configuration["humidity_correction"].as<float>());

            is_updated = true;
        }

        return is_updated;
    }

// -----------------------------------------------------------------------------

    void _sensorWSChannelsData(
        JsonArray& channels
    ) {
        char buffer[10];

        for (uint8_t i = 0; i < _sensors.size(); i++) {
            for (uint8_t k = 0; k < _sensors[i]->magnitudesCount(); k++) {
                if (_sensors[i]->magnitudeType(k) == MAGNITUDE_EVENT) {
                    continue;
                }

                JsonObject& channel = channels.createNestedObject();

                channel["index"] = i;
                channel["type"] = _sensors[i]->magnitudeType(k);
                channel["name"] = _sensors[i]->magnitudeName(k);
                channel["units"] = _sensors[i]->magnitudeUnits(k);
                channel["value"] = _sensors[i]->magnitudeValue(k);
                channel["decimals"] = _sensors[i]->magnitudeDecimals(k);
                channel["error"] = _sensors[i]->error();

                if (_sensors[i]->magnitudeType(k) == MAGNITUDE_ENERGY) {
                    channel["sensor"] = _sensors[i]->description() + String(" (since ") + _sensor_energy_reset_ts + String(")");

                } else {
                    channel["sensor"] = _sensors[i]->description();
                }
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
        configuration_sensors["temperature"] = _sensorHasMagnitude(MAGNITUDE_TEMPERATURE);
        configuration_sensors["humidity"] = _sensorHasMagnitude(MAGNITUDE_HUMIDITY);
        configuration_sensors["mics"] = false;

        for (uint8_t i = 0; i < _sensors.size(); i++) {
            BaseSensor * sensor = _sensors[i];

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
                    wsSend_P(clientId, PSTR("{\"message\": \"sensor_updated\"}"));

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
        if(strcmp(action, "reset_power_calibration") == 0) {
            _sensorResetPowerCalibration();

        } else if(strcmp(action, "reset_energy_calibration") == 0) {
            _sensorResetEnergyCalibration();
        }
    }
#endif // WEB_SUPPORT && WS_SUPPORT

// -----------------------------------------------------------------------------

double _sensorEnergyTotal()
{
    double value = 0;

    if (rtcmemStatus()) {
        value = Rtcmem->energy;

    } else {
        value = (_sensor_save_every > 0) ? getSetting("sensor_ene_total", 0).toInt() : 0;
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
            setSetting("sensor_ene_total", value);
            saveSettings();
        }
    }

    // Always save to RTCMEM
    Rtcmem->energy = value;
}

// -----------------------------------------------------------------------------
// Sensor initialization
// -----------------------------------------------------------------------------

void _sensorLoad()
{
    /*

    This is temporal, in the future sensors will be initialized based on
    soft configuration (data stored in EEPROM config) so you will be able
    to define and configure new sensors on the fly

    At the time being, only enabled sensors (those with *_SUPPORT to 1) are being
    loaded and initialized here. If you want to add new sensors of the same type
    just duplicate the block and change the arguments for the set* methods.

     */

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

    #if CSE7766_SUPPORT
    {
        CSE7766Sensor * sensor = new CSE7766Sensor();

        sensor->setRX(CSE7766_PIN);

        _sensors.push_back(sensor);
    }
    #endif

    #if ITEAD_SONOFF_SC_SUPPORT
    {
        SonoffScSensor * sensor = new SonoffScSensor();

        _sensors.push_back(sensor);
    }
    #endif
}

// -----------------------------------------------------------------------------

void _sensorInit()
{
    _sensor_save_every = getSetting("sensor_save_interval", 0).toInt();

    for (uint8_t i = 0; i < _sensors.size(); i++) {
        // Do not process an already initialized sensor
        if (_sensors[i]->ready()) {
            continue;
        }

        DEBUG_MSG(PSTR("[INFO][SENSOR] Initializing %s\n"), _sensors[i]->description().c_str());

        // Force sensor to reload config
        _sensors[i]->begin();

        if (!_sensors[i]->ready()) {
            if (_sensors[i]->error() != 0) {
                DEBUG_MSG(PSTR("[INFO][SENSOR]  -> ERROR %d\n"), _sensors[i]->error());
            }

            continue;
        }

        // Initialize magnitudes
        for (uint8_t k = 0; k < _sensors[i]->magnitudesCount(); k++) {
            // Detect magnitude type
            uint8_t type = _sensors[i]->magnitudeType(k);

            _sensors[i]->magnitudeFilter(k)->resize(_sensor_report_every);

            DEBUG_MSG(PSTR("[INFO][SENSOR]  -> %s\n"), _sensors[i]->magnitudeName(k).c_str());
        }

        // Hook callback
        _sensors[i]->onEvent([i](uint8_t type, double value) {
            DEBUG_MSG(PSTR("[INFO][SENSOR] Sensor #%u callback, type %u, payload: '%s'\n"), sensorIndex, type, String(value).c_str());

            _sensorReport(i, type, value);
        });

        // Custom initializations

        #if HLW8012_SUPPORT
            if (_sensors[i]->getID() == SENSOR_HLW8012_ID) {
                HLW8012Sensor * sensor = (HLW8012Sensor *) _sensors[i];

                double value;

                value = getSetting("sensor_pwr_ratio_c", HLW8012_CURRENT_RATIO).toFloat();

                if (value > 0) {
                    sensor->setCurrentRatio(value);
                }

                value = getSetting("sensor_pwr_ratio_v", HLW8012_VOLTAGE_RATIO).toFloat();

                if (value > 0) {
                    sensor->setVoltageRatio(value);
                }

                value = getSetting("sensor_pwr_ratio_p", HLW8012_POWER_RATIO).toFloat();

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

                value = getSetting("sensor_pwr_ratio_c", 0).toFloat();

                if (value > 0) {
                    sensor->setCurrentRatio(value);
                }

                value = getSetting("sensor_pwr_ratio_v", 0).toFloat();

                if (value > 0) {
                    sensor->setVoltageRatio(value);
                }

                value = getSetting("sensor_pwr_ratio_p", 0).toFloat();

                if (value > 0) {
                    sensor->setPowerRatio(value);
                }

                value = _sensorEnergyTotal();

                if (value > 0) {
                    sensor->resetEnergy(value);
                }
            }
        #endif // CSE7766_SUPPORT
    }
}

// -----------------------------------------------------------------------------

bool _sensorAllReady()
{
    for (uint8_t i = 0; i < _sensors.size(); i++) {
        if (!_sensors[i]->ready()) {
            return false;
        }
    }

    return true;
}

// -----------------------------------------------------------------------------

void _sensorConfigure()
{
    // General sensor settings
    _sensor_read_interval = 1000 * constrain(getSetting("sensor_read_interval", SENSOR_READ_INTERVAL).toInt(), SENSOR_READ_MIN_INTERVAL, SENSOR_READ_MAX_INTERVAL);
    _sensor_report_every = constrain(getSetting("sensor_report_interval", SENSOR_REPORT_EVERY).toInt(), SENSOR_REPORT_MIN_EVERY, SENSOR_REPORT_MAX_EVERY);
    _sensor_save_every = getSetting("sensor_save_interval", SENSOR_SAVE_EVERY).toInt();
    _sensor_energy_reset_ts = getSetting("sensor_reset_ts", "");

    _sensor_power_units = getSetting("sensor_pwr_units", SENSOR_POWER_UNITS).toInt();
    _sensor_energy_units = getSetting("sensor_ene_units", SENSOR_ENERGY_UNITS).toInt();
    _sensor_temperature_units = getSetting("sensor_tmp_units", SENSOR_TEMPERATURE_UNITS).toInt();
    _sensor_temperature_correction = getSetting("sensor_tmp_correction", SENSOR_TEMPERATURE_CORRECTION).toFloat();
    _sensor_humidity_correction = getSetting("sensor_hum_correction", SENSOR_HUMIDITY_CORRECTION).toFloat();
    _sensor_lux_correction = getSetting("sensor_lux_correction", SENSOR_LUX_CORRECTION).toFloat();

    // Specific sensor settings
    for (uint8_t i = 0; i < _sensors.size(); i++) {
        #if HLW8012_SUPPORT
            if (_sensors[i]->getID() == SENSOR_HLW8012_ID) {
                double value;

                HLW8012Sensor * sensor = (HLW8012Sensor *) _sensors[i];

                if (value = getSetting("sensor_pwr_expected_c", 0).toFloat()) {
                    sensor->expectedCurrent(value);

                    setSetting("sensor_pwr_ratio_c", sensor->getCurrentRatio());
                }

                if (value = getSetting("sensor_pwr_expected_v", 0).toInt()) {
                    sensor->expectedVoltage(value);

                    setSetting("sensor_pwr_ratio_v", sensor->getVoltageRatio());
                }

                if (value = getSetting("sensor_pwr_expected_p", 0).toInt()) {
                    sensor->expectedPower(value);

                    setSetting("sensor_pwr_ratio_p", sensor->getPowerRatio());
                }
            }
        #endif // HLW8012_SUPPORT

        #if CSE7766_SUPPORT
            if (_sensors[i]->getID() == SENSOR_CSE7766_ID) {
                double value;

                CSE7766Sensor * sensor = (CSE7766Sensor *) _sensors[i];

                if ((value = getSetting("sensor_pwr_expected_c", 0).toFloat())) {
                    sensor->expectedCurrent(value);

                    setSetting("sensor_pwr_ratio_c", sensor->getCurrentRatio());
                }

                if ((value = getSetting("sensor_pwr_expected_v", 0).toInt())) {
                    sensor->expectedVoltage(value);

                    setSetting("sensor_pwr_ratio_v", sensor->getVoltageRatio());
                }

                if ((value = getSetting("sensor_pwr_expected_p", 0).toInt())) {
                    sensor->expectedPower(value);

                    setSetting("sensor_pwr_ratio_p", sensor->getPowerRatio());
                }
            }
        #endif // CSE7766_SUPPORT
    }

    // Update filter sizes
    for (uint8_t i = 0; i < _sensors.size(); i++) {
        for (uint8_t k = 0; k < _sensors[i]->magnitudesCount(); k++) {
            _sensors[i]->magnitudeFilter(k)->resize(_sensor_report_every);
        }
    }

    // General processing
    if (_sensor_save_every == 0) {
        delSetting("sensor_ene_total");
    }

    // Save settings
    delSetting("sensor_pwr_expected_p");
    delSetting("sensor_pwr_expected_c");
    delSetting("sensor_pwr_expected_v");

    saveSettings();
}

// -----------------------------------------------------------------------------

void _sensorReport(
    uint8_t index,
    uint8_t type,
    double value
) {
    for (uint8_t k = 0; k < _sensors[index]->magnitudesCount(); k++) {
        if (_sensors[index]->magnitudeType(k) == type) {
            char buffer[10];

            dtostrf(value, 1 - sizeof(buffer), _sensors[index]->magnitudeDecimals(k), buffer);

            #if FASTYBIRD_SUPPORT
                fastybirdReportChannelPropertyValue(
                    FASTYBIRD_MAIN_DEVICE_INDEX,
                    FASTYBIRD_SENSOR1_CHANNEL_INDEX,
                    fastybirdFindChannelPropertyIndex(
                        FASTYBIRD_MAIN_DEVICE_INDEX,
                        FASTYBIRD_SENSOR1_CHANNEL_INDEX,
                        _sensors[index]->magnitudeName(k).c_char()
                    ),
                    buffer
                );
            #endif
        }
    }
}

// -----------------------------------------------------------------------------
// MODULE API
// -----------------------------------------------------------------------------

uint8_t sensorCount()
{
    return _sensors.size();
}

// -----------------------------------------------------------------------------

BaseSensor * sensorSensor(uint8_t id) {
    return _sensors[id];
}

// -----------------------------------------------------------------------------
// MODULE CORE
// -----------------------------------------------------------------------------

void sensorSetup()
{
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

    DEBUG_MSG(PSTR("[INFO][SENSOR] Number of sensors: %d and magnitudes: %d\n"), sensorCount(), sensorMagnitudesCount());
}

// -----------------------------------------------------------------------------

void sensorLoop()
{
    // Check if we still have uninitialized sensors
    static uint32_t last_init = 0;

    if (!_sensorAllReady) {
        if (millis() - last_init > SENSOR_INIT_INTERVAL) {
            last_init = millis();

            _sensorInit();
        }
    }

    for (uint8_t i = 0; i < _sensors.size(); i++) {
        // Tick hook
        _sensors[i]->tick();
    }

    // Check if we should read new data
    static uint32_t last_update = 0;
    static uint32_t report_count = 0;

    if (millis() - last_update > _sensor_read_interval) {
        last_update = millis();
        report_count = (report_count + 1) % _sensor_report_every;

        double magnitude_value;

        // Get readings
        for (uint8_t i = 0; i < _sensors.size(); i++) {
            // Pre-read hook
            _sensors[i]->pre();

            if (!_sensors[i]->status()) {
                DEBUG_MSG(PSTR("[INFO][SENSOR] Error reading data from %s (error: %d)\n"),
                    _sensors[i]->description().c_str(),
                    _sensors[i]->error()
                );

                continue;
            }

            // Process hook
            _sensors[i]->process();

            for (uint8_t k = 0; k < _sensors[i]->magnitudesCount(); k++) {
                // -------------------------------------------------------------
                // Procesing (units and decimals)
                // -------------------------------------------------------------

                magnitude_value = _sensors[i]->magnitudeValue(k);

                // -------------------------------------------------------------
                // Debug
                // -------------------------------------------------------------

                #if SENSOR_DEBUG
                {
                    char buffer[64];

                    dtostrf(magnitude_value, 1 - sizeof(buffer), decimals, buffer);

                    DEBUG_MSG(PSTR("[INFO][SENSOR] %s - %s: %s%s\n"),
                        _sensors[i]->description().c_str(),
                        _sensors[i]->magnitudeName(k).c_str(),
                        buffer,
                        _sensors[i]->magnitudeUnits(k).c_str()
                    );
                }
                #endif // SENSOR_DEBUG

                // -------------------------------------------------------------
                // Report
                // (we do it every _sensor_report_every readings)
                // -------------------------------------------------------------

                bool report = (report_count == 0);

                if (report) {
                    _sensors[i]->magnitudeFilter(k)->reset();

                    _sensorReport(i, _sensors[i]->magnitudeType(k), magnitude_value);

                    // Persist total energy value
                    if (_sensors[i]->magnitudeType(k) == MAGNITUDE_ENERGY) {
                        _sensorEnergyTotal(magnitude_value);
                    }
                }
            }

            // Post-read hook
            _sensors[i]->post();
        }

        #if WEB_SUPPORT && WS_SUPPORT
            wsSend(_sensorWebSocketSendData);
        #endif
    }
}

#endif // SENSOR_SUPPORT
