/*

SENSOR MODULE CONFIGURATION

Copyright (C) 2018 FastyBird Ltd. <info@fastybird.com>

*/

#pragma once

// -----------------------------------------------------------------------------
// SENSORS - General data
// -----------------------------------------------------------------------------

#define SENSOR_DEBUG                            0                       // Debug sensors

#define SENSOR_READ_INTERVAL                    6                       // Read data from sensors every 6 seconds
#define SENSOR_READ_MIN_INTERVAL                1                       // Minimum read interval
#define SENSOR_READ_MAX_INTERVAL                3600                    // Maximum read interval
#define SENSOR_INIT_INTERVAL                    10000                   // Try to re-init non-ready sensors every 10s

#define SENSOR_REPORT_EVERY                     10                      // Report every this many readings
#define SENSOR_REPORT_MIN_EVERY                 1                       // Minimum every value
#define SENSOR_REPORT_MAX_EVERY                 60                      // Maximum
#define SENSOR_REPORT_STEP_EVERY                1                       // Step

#ifndef SENSOR_POWER_CHECK_STATUS
    #define SENSOR_POWER_CHECK_STATUS           0                       // If set to 1 the reported power/current/energy will be 0 if the relay[0] is OFF
#endif

#ifndef SENSOR_TEMPERATURE_CORRECTION
    #define SENSOR_TEMPERATURE_CORRECTION       0.0                     // Offset correction
#endif

#ifndef SENSOR_TEMPERATURE_MIN_CHANGE
    #define SENSOR_TEMPERATURE_MIN_CHANGE       0.0                     // Minimum temperature change to report
#endif

#ifndef SENSOR_HUMIDITY_CORRECTION
    #define SENSOR_HUMIDITY_CORRECTION          0.0                     // Offset correction
#endif

#ifndef SENSOR_HUMIDITY_MIN_CHANGE
    #define SENSOR_HUMIDITY_MIN_CHANGE          0                       // Minimum humidity change to report
#endif

#ifndef SENSOR_LUX_CORRECTION
    #define SENSOR_LUX_CORRECTION               0.0                     // Offset correction
#endif

#ifndef SENSOR_ENERGY_MAX_CHANGE
    #define SENSOR_ENERGY_MAX_CHANGE            0                       // Maximum energy change to report (if > 0 it will allways report when delta(E) is greater than this)
#endif

#ifndef SENSOR_SAVE_EVERY
    #define SENSOR_SAVE_EVERY                   0                       // Save accumulating values to EEPROM (atm only energy)
                                                                        // A 0 means do not save and it's the default value
                                                                        // A number different from 0 means it should store the value in EEPROM
                                                                        // after these many reports
                                                                        // Warning: this might wear out flash fast!
#endif

#ifndef SENSOR_TEMPERATURE_UNITS
    #define SENSOR_TEMPERATURE_UNITS            SENSOR_TMP_CELSIUS      // Temperature units (SENSOR_TMP_CELSIUS | SENSOR_TMP_FAHRENHEIT)
#endif

#ifndef SENSOR_ENERGY_UNITS
    #define SENSOR_ENERGY_UNITS                 SENSOR_ENERGY_JOULES    // Energy units (SENSOR_ENERGY_JOULES | SENSOR_ENERGY_KWH)
#endif

#ifndef SENSOR_POWER_UNITS
    #define SENSOR_POWER_UNITS                  SENSOR_POWER_WATTS      // Power units (SENSOR_POWER_WATTS | SENSOR_POWER_KILOWATTS)
#endif

#ifndef SENSOR_ANALOG_DECIMALS
    #define SENSOR_ANALOG_DECIMALS              2                       // Round to this number of decimals
#endif

// -----------------------------------------------------------------------------
// SENSORS - Dependencies
// -----------------------------------------------------------------------------

#ifndef SENSOR_SUPPORT
    #define SENSOR_SUPPORT ( \
        HLW8012_SUPPORT || \
        CSE7766_SUPPORT || \
        ITEAD_SONOFF_SC_SUPPORT || \
        ITEAD_SONOFF_SC_PRO_SUPPORT \
    )
#endif

// -----------------------------------------------------------------------------
// SENSORS - Specific data for each sensor
// -----------------------------------------------------------------------------

//------------------------------------------------------------------------------
// HLW8012 Energy monitor IC
// Enable support by passing HLW8012_SUPPORT=1 build flag
//------------------------------------------------------------------------------

#ifndef HLW8012_SUPPORT
    #define HLW8012_SUPPORT                     0
#endif

#ifndef HLW8012_SEL_PIN
    #define HLW8012_SEL_PIN                     5
#endif

#ifndef HLW8012_CF1_PIN
    #define HLW8012_CF1_PIN                     13
#endif

#ifndef HLW8012_CF_PIN
    #define HLW8012_CF_PIN                      14
#endif

#ifndef HLW8012_SEL_CURRENT
    #define HLW8012_SEL_CURRENT                 HIGH                    // SEL pin to HIGH to measure current
#endif

#ifndef HLW8012_CURRENT_R
    #define HLW8012_CURRENT_R                   0.001                   // Current resistor
#endif

#ifndef HLW8012_VOLTAGE_R_UP
    #define HLW8012_VOLTAGE_R_UP                ( 5 * 470000 )          // Upstream voltage resistor
#endif

#ifndef HLW8012_VOLTAGE_R_DOWN
    #define HLW8012_VOLTAGE_R_DOWN              ( 1000 )                // Downstream voltage resistor
#endif

#ifndef HLW8012_CURRENT_RATIO
    #define HLW8012_CURRENT_RATIO               0                       // Set to 0 to use factory defaults
#endif

#ifndef HLW8012_VOLTAGE_RATIO
    #define HLW8012_VOLTAGE_RATIO               0                       // Set to 0 to use factory defaults
#endif

#ifndef HLW8012_POWER_RATIO
    #define HLW8012_POWER_RATIO                 0                       // Set to 0 to use factory defaults
#endif

#ifndef HLW8012_USE_INTERRUPTS
    #define HLW8012_USE_INTERRUPTS              1                       // Use interrupts to trap HLW8012 signals
#endif

#ifndef HLW8012_WAIT_FOR_WIFI
    #define HLW8012_WAIT_FOR_WIFI               0                       // Weather to enable interrupts only after
                                                                    // wifi connection has been stablished
#endif

#ifndef HLW8012_INTERRUPT_ON
    #define HLW8012_INTERRUPT_ON                CHANGE                  // When to trigger the interrupt
                                                                    // Use CHANGE for HLW8012
                                                                    // Use FALLING for BL0937 / HJL0
#endif

//------------------------------------------------------------------------------
// CSE7766 based power sensor
// Enable support by passing CSE7766_SUPPORT=1 build flag
//------------------------------------------------------------------------------

#ifndef CSE7766_SUPPORT
    #define CSE7766_SUPPORT                     0
#endif

#ifndef CSE7766_PIN
    #define CSE7766_PIN                         1                       // TX pin from the CSE7766
#endif

#ifndef CSE7766_PIN_INVERSE
    #define CSE7766_PIN_INVERSE                 0                       // Signal is inverted
#endif

#define CSE7766_SYNC_INTERVAL                   300                     // Safe time between transmissions (ms)
#define CSE7766_BAUDRATE                        4800                    // UART baudrate

#define CSE7766_V1R                             1.0                     // 1mR current resistor
#define CSE7766_V2R                             1.0                     // 1M voltage resistor

//------------------------------------------------------------------------------
// SonOff SC evironment unit sensor
// Enable support by passing ITEAD_SONOFF_SC_SUPPORT=1 build flag
//------------------------------------------------------------------------------

#ifndef ITEAD_SONOFF_SC_SUPPORT
    #define ITEAD_SONOFF_SC_SUPPORT             0
#endif

#ifndef ITEAD_SONOFF_SC_PRO_SUPPORT
    #define ITEAD_SONOFF_SC_PRO_SUPPORT         0
#endif

// -----------------------------------------------------------------------------
// SENSORS - Types
// -----------------------------------------------------------------------------

#define SENSOR_TYPE_ENERGY                      0
#define SENSOR_TYPE_ENVIRONMENT                 1
#define SENSOR_TYPE_ANALOG                      2

#define SENSOR_TYPES_MAX                        3

// -----------------------------------------------------------------------------
// SENSORS - Identifiers
// -----------------------------------------------------------------------------

#define SENSOR_HLW8012_ID                       1
#define SENSOR_CSE7766_ID                       2
#define SENSOR_SONOFF_SC_ID                     3
#define SENSOR_SONOFF_SC_PRO_ID                 4

// -----------------------------------------------------------------------------
// SENSORS - Misc data
// -----------------------------------------------------------------------------

// American Society of Heating, Refrigerating and Air-Conditioning Engineers suggests a range of 45% - 55% humidity to manage health effects and illnesses.
// Comfortable: 30% - 60%
// Recommended: 45% - 55%
// High       : 55% - 80%

#define SENSOR_HUMIDITY_NORMAL                  0       // > %30
#define SENSOR_HUMIDITY_COMFORTABLE             1       // > %45
#define SENSOR_HUMIDITY_DRY                     2       // < %30
#define SENSOR_HUMIDITY_WET                     3       // > %70

// United States Environmental Protection Agency - UV Index Scale
// One UV Index unit is equivalent to 25 milliWatts per square meter.
#define SENSOR_UV_INDEX_LOW                     0       // 0 to 2 means low danger from the sun's UV rays for the average person.
#define SENSOR_UV_INDEX_MODERATE                1       // 3 to 5 means moderate risk of harm from unprotected sun exposure.
#define SENSOR_UV_INDEX_HIGH                    2       // 6 to 7 means high risk of harm from unprotected sun exposure. Protection against skin and eye damage is needed.
#define SENSOR_UV_INDEX_VERY_HIGH               3       // 8 to 10 means very high risk of harm from unprotected sun exposure.
                                                        // Take extra precautions because unprotected skin and eyes will be damaged and can burn quickly.
#define SENSOR_UV_INDEX_EXTREME                 4       // 11 or more means extreme risk of harm from unprotected sun exposure.
                                                        // Take all precautions because unprotected skin and eyes can burn in minutes.

// -----------------------------------------------------------------------------
// SENSORS - Units
// -----------------------------------------------------------------------------

#define SENSOR_POWER_WATTS                      0
#define SENSOR_POWER_KILOWATTS                  1

#define SENSOR_ENERGY_JOULES                    0
#define SENSOR_ENERGY_KWH                       1

#define SENSOR_TMP_CELSIUS                      0
#define SENSOR_TMP_FAHRENHEIT                   1
#define SENSOR_TMP_KELVIN                       2

// -----------------------------------------------------------------------------
// SENSORS - Magnitudes
// -----------------------------------------------------------------------------

#define MAGNITUDE_NONE                          0
#define MAGNITUDE_TEMPERATURE                   1
#define MAGNITUDE_HUMIDITY                      2
#define MAGNITUDE_PRESSURE                      3
#define MAGNITUDE_CURRENT                       4
#define MAGNITUDE_VOLTAGE                       5
#define MAGNITUDE_POWER_ACTIVE                  6
#define MAGNITUDE_POWER_APPARENT                7
#define MAGNITUDE_POWER_REACTIVE                8
#define MAGNITUDE_POWER_FACTOR                  9
#define MAGNITUDE_ENERGY                        10
#define MAGNITUDE_ENERGY_DELTA                  11
#define MAGNITUDE_ANALOG                        12
#define MAGNITUDE_DIGITAL                       13
#define MAGNITUDE_EVENT                         14
#define MAGNITUDE_PM1dot0                       15
#define MAGNITUDE_PM2dot5                       16
#define MAGNITUDE_PM10                          17
#define MAGNITUDE_CO2                           18
#define MAGNITUDE_LUX                           19
#define MAGNITUDE_UVA                           20
#define MAGNITUDE_UVB                           21
#define MAGNITUDE_UVI                           22
#define MAGNITUDE_DISTANCE                      23
#define MAGNITUDE_HCHO                          24
#define MAGNITUDE_GEIGER_CPM                    25
#define MAGNITUDE_GEIGER_SIEVERT                26
#define MAGNITUDE_COUNT                         27
#define MAGNITUDE_NO2                           28
#define MAGNITUDE_CO                            29
#define MAGNITUDE_RESISTANCE                    30
#define MAGNITUDE_PH                            31
#define MAGNITUDE_AIR_QUALITY_LEVEL             32
#define MAGNITUDE_LIGHT_LEVEL                   33
#define MAGNITUDE_NOISE_LEVEL                   34

#define MAGNITUDE_MAX                           35

// -----------------------------------------------------------------------------
// SENSORS - Class loading
// -----------------------------------------------------------------------------

#if SENSOR_SUPPORT
    #include "../sensors/base/BaseSensor.h"

    #if HLW8012_SUPPORT
        #include "../sensors/HLW8012Sensor.h"
    #endif

    #if CSE7766_SUPPORT
        #include "../sensors/CSE7766Sensor.h"
    #endif

    #if ITEAD_SONOFF_SC_SUPPORT
        #include "../sensors/SonoffScSensor.h"
    #endif

    #if ITEAD_SONOFF_SC_PRO_SUPPORT
        #include "../sensors/SonoffScProSensor.h"
    #endif

#endif // SENSOR_SUPPORT

// -----------------------------------------------------------------------------
// SENSORS - PROGMEM structures
// -----------------------------------------------------------------------------

#if SENSOR_SUPPORT

    PROGMEM const char firmware_sensors[] =
        #if HLW8012_SUPPORT
            "HLW8012 "
        #endif
        #if CSE7766_SUPPORT
            "CSE7766 "
        #endif
        #if ITEAD_SONOFF_SC_SUPPORT
            "SONOFF_SC "
        #endif
        #if ITEAD_SONOFF_SC_PRO_SUPPORT
            "SONOFF_SC_PRO "
        #endif
        "";

    PROGMEM const uint8_t magnitude_decimals[] = {
        0,
        1, 0, 2, // THP
        3, 0, 0, 0, 0, 0, 0, 0, // Power decimals
        0, 0, 0, // analog, digital, event
        0, 0, 0, // PM
        0, 0,
        0, 0, 3, // UVA, UVB, UVI
        3, 0,
        4, 4, // Geiger Counter decimals
        0,
        0, 0, 0, 3,    // NO2, CO, Ohms, pH
        0, 0, 0
    };

    PROGMEM const char magnitude_unknown_name[]                 = "unknown";
    PROGMEM const char magnitude_temperature_name[]             = "temperature";
    PROGMEM const char magnitude_humidity_name[]                = "humidity";
    PROGMEM const char magnitude_pressure_name[]                = "pressure";
    PROGMEM const char magnitude_current_name[]                 = "current";
    PROGMEM const char magnitude_voltage_name[]                 = "voltage";
    PROGMEM const char magnitude_active_power_name[]            = "power";
    PROGMEM const char magnitude_apparent_power_name[]          = "apparent";
    PROGMEM const char magnitude_reactive_power_name[]          = "reactive";
    PROGMEM const char magnitude_power_factor_name[]            = "factor";
    PROGMEM const char magnitude_energy_name[]                  = "energy";
    PROGMEM const char magnitude_energy_delta_name[]            = "energy-delta";
    PROGMEM const char magnitude_analog_name[]                  = "analog";
    PROGMEM const char magnitude_digital_name[]                 = "digital";
    PROGMEM const char magnitude_event_name[]                   = "event";
    PROGMEM const char magnitude_pm1dot0_name[]                 = "pm1dot0";
    PROGMEM const char magnitude_pm2dot5_name[]                 = "pm2dot5";
    PROGMEM const char magnitude_pm10_name[]                    = "pm10";
    PROGMEM const char magnitude_co2_name[]                     = "co2";
    PROGMEM const char magnitude_lux_name[]                     = "lux";
    PROGMEM const char magnitude_uva_name[]                     = "uva";
    PROGMEM const char magnitude_uvb_name[]                     = "uvb";
    PROGMEM const char magnitude_uvi_name[]                     = "uvi";
    PROGMEM const char magnitude_distance_name[]                = "distance";
    PROGMEM const char magnitude_hcho_name[]                    = "hcho";
    PROGMEM const char magnitude_geiger_cpm_name[]              = "ldr-cpm";        // local dose rate [Counts per minute]
    PROGMEM const char magnitude_geiger_sv_name[]               = "ldr-uSvh";       // local dose rate [µSievert per hour]
    PROGMEM const char magnitude_count_name[]                   = "count";
    PROGMEM const char magnitude_no2_name[]                     = "no2";
    PROGMEM const char magnitude_co_name[]                      = "co";
    PROGMEM const char magnitude_resistance_name[]              = "resistance";
    PROGMEM const char magnitude_ph_name[]                      = "ph";
    PROGMEM const char magnitude_sonoff_sc_air_quality_name[]   = "air-quality";
    PROGMEM const char magnitude_sonoff_sc_light_level_name[]   = "light-level";
    PROGMEM const char magnitude_sonoff_sc_noise_level_name[]   = "noise-level";

    PROGMEM const char * const magnitude_names[] = {
        magnitude_unknown_name, magnitude_temperature_name, magnitude_humidity_name,
        magnitude_pressure_name, magnitude_current_name, magnitude_voltage_name,
        magnitude_active_power_name, magnitude_apparent_power_name, magnitude_reactive_power_name,
        magnitude_power_factor_name, magnitude_energy_name, magnitude_energy_delta_name,
        magnitude_analog_name, magnitude_digital_name, magnitude_event_name,
        magnitude_pm1dot0_name, magnitude_pm2dot5_name, magnitude_pm10_name,
        magnitude_co2_name, magnitude_lux_name,
        magnitude_uva_name, magnitude_uvb_name, magnitude_uvi_name,
        magnitude_distance_name, magnitude_hcho_name,
        magnitude_geiger_cpm_name, magnitude_geiger_sv_name,
        magnitude_count_name,
        magnitude_no2_name, magnitude_co_name, magnitude_resistance_name, magnitude_ph_name,
        magnitude_sonoff_sc_air_quality_name, magnitude_sonoff_sc_light_level_name, magnitude_sonoff_sc_noise_level_name
    };

    PROGMEM const char magnitude_empty[]        = "";
    PROGMEM const char magnitude_celsius[]      = "°C";
    PROGMEM const char magnitude_fahrenheit[]   = "°F";
    PROGMEM const char magnitude_percentage[]   = "%";
    PROGMEM const char magnitude_hectopascals[] = "hPa";
    PROGMEM const char magnitude_amperes[]      = "A";
    PROGMEM const char magnitude_volts[]        = "V";
    PROGMEM const char magnitude_watts[]        = "W";
    PROGMEM const char magnitude_kw[]           = "kW";
    PROGMEM const char magnitude_joules[]       = "J";
    PROGMEM const char magnitude_kwh[]          = "kWh";
    PROGMEM const char magnitude_ugm3[]         = "µg/m³";
    PROGMEM const char magnitude_ppm[]          = "ppm";
    PROGMEM const char magnitude_lux[]          = "lux";
    PROGMEM const char magnitude_distance[]     = "m";
    PROGMEM const char magnitude_mgm3[]         = "mg/m³";
    PROGMEM const char magnitude_geiger_cpm[]   = "cpm";    // Counts per Minute: Unit of local dose rate (Geiger counting)
    PROGMEM const char magnitude_geiger_sv[]    = "µSv/h";  // µSievert per hour: 2nd unit of local dose rate (Geiger counting)
    PROGMEM const char magnitude_resistance[]   = "ohm";

    PROGMEM const char * const magnitude_units[] = {
        magnitude_empty, magnitude_celsius, magnitude_percentage,
        magnitude_hectopascals, magnitude_amperes, magnitude_volts,
        magnitude_watts, magnitude_watts, magnitude_watts,
        magnitude_percentage, magnitude_joules, magnitude_joules,
        magnitude_empty, magnitude_empty, magnitude_empty,
        magnitude_ugm3, magnitude_ugm3, magnitude_ugm3,
        magnitude_ppm, magnitude_lux,
        magnitude_empty, magnitude_empty, magnitude_empty,
        magnitude_distance, magnitude_mgm3,
        magnitude_geiger_cpm, magnitude_geiger_sv,                  // Geiger counter units
        magnitude_empty,                                            //
        magnitude_ppm, magnitude_ppm,                               // NO2 & CO2
        magnitude_resistance,
        magnitude_empty,                                             // pH
        magnitude_empty, magnitude_empty, magnitude_empty
    };

    PROGMEM const char magnitude_unknown_type[]         = "unknown";
    PROGMEM const char magnitude_environment_type[]     = "environment";
    PROGMEM const char magnitude_energy_type[]          = "energy";
    PROGMEM const char magnitude_analog_sensor_type[]   = "analog-sensor";
    PROGMEM const char magnitude_binary_sensor_type[]   = "binary-sensor";
    PROGMEM const char magnitude_event_type[]           = "event";

    PROGMEM const char * const magnitude_types[] = {
        magnitude_unknown_type, magnitude_environment_type, magnitude_environment_type,
        magnitude_analog_sensor_type, magnitude_energy_type, magnitude_energy_type,
        magnitude_energy_type, magnitude_energy_type, magnitude_energy_type,
        magnitude_energy_type, magnitude_energy_type, magnitude_energy_type,
        magnitude_analog_sensor_type, magnitude_binary_sensor_type, magnitude_event_type,
        magnitude_environment_type, magnitude_environment_type, magnitude_environment_type,
        magnitude_environment_type, magnitude_environment_type,
        magnitude_environment_type, magnitude_environment_type, magnitude_environment_type,
        magnitude_analog_sensor_type, magnitude_environment_type,
        magnitude_environment_type, magnitude_environment_type,
        magnitude_binary_sensor_type,
        magnitude_environment_type, magnitude_environment_type, magnitude_analog_sensor_type, magnitude_environment_type,
        magnitude_environment_type, magnitude_environment_type, magnitude_environment_type
    };

#endif // SENSOR_SUPPORT