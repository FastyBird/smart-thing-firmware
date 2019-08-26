// =============================================================================
// SENSORS - General data
// =============================================================================

#define SENSOR_DEBUG                        0               // Debug sensors

#define SENSOR_READ_INTERVAL                6               // Read data from sensors every 6 seconds
#define SENSOR_READ_MIN_INTERVAL            1               // Minimum read interval
#define SENSOR_READ_MAX_INTERVAL            3600            // Maximum read interval
#define SENSOR_INIT_INTERVAL                10000           // Try to re-init non-ready sensors every 10s

#define SENSOR_REPORT_EVERY                 10              // Report every this many readings
#define SENSOR_REPORT_MIN_EVERY             1               // Minimum every value
#define SENSOR_REPORT_MAX_EVERY             60              // Maximum
#define SENSOR_REPORT_STEP_EVERY            1               // Step

#ifndef SENSOR_POWER_CHECK_STATUS
#define SENSOR_POWER_CHECK_STATUS           0               // If set to 1 the reported power/current/energy will be 0 if the relay[0] is OFF
#endif

#ifndef SENSOR_TEMPERATURE_CORRECTION
#define SENSOR_TEMPERATURE_CORRECTION       0.0             // Offset correction
#endif

#ifndef TEMPERATURE_MIN_CHANGE
#define TEMPERATURE_MIN_CHANGE              0.0             // Minimum temperature change to report
#endif

#ifndef SENSOR_HUMIDITY_CORRECTION
#define SENSOR_HUMIDITY_CORRECTION          0.0             // Offset correction
#endif

#ifndef HUMIDITY_MIN_CHANGE
#define HUMIDITY_MIN_CHANGE                 0               // Minimum humidity change to report
#endif

#ifndef SENSOR_LUX_CORRECTION
#define SENSOR_LUX_CORRECTION               0.0             // Offset correction
#endif

#ifndef ENERGY_MAX_CHANGE
#define ENERGY_MAX_CHANGE                   0               // Maximum energy change to report (if > 0 it will allways report when delta(E) is greater than this)
#endif

#ifndef SENSOR_SAVE_EVERY
#define SENSOR_SAVE_EVERY                   0               // Save accumulating values to EEPROM (atm only energy)
                                                            // A 0 means do not save and it's the default value
                                                            // A number different from 0 means it should store the value in EEPROM
                                                            // after these many reports
                                                            // Warning: this might wear out flash fast!
#endif

#ifndef SENSOR_TEMPERATURE_UNITS
#define SENSOR_TEMPERATURE_UNITS            TMP_CELSIUS     // Temperature units (TMP_CELSIUS | TMP_FAHRENHEIT)
#endif

#ifndef SENSOR_ENERGY_UNITS
#define SENSOR_ENERGY_UNITS                 ENERGY_JOULES   // Energy units (ENERGY_JOULES | ENERGY_KWH)
#endif

#ifndef SENSOR_POWER_UNITS
#define SENSOR_POWER_UNITS                  POWER_WATTS     // Power units (POWER_WATTS | POWER_KILOWATTS)
#endif

#ifndef ANALOG_DECIMALS
#define ANALOG_DECIMALS                     2               // Round to this number of decimals
#endif

// =============================================================================
// Specific data for each sensor
// =============================================================================

//------------------------------------------------------------------------------
// HLW8012 Energy monitor IC
// Enable support by passing HLW8012_SUPPORT=1 build flag
//------------------------------------------------------------------------------

#ifndef HLW8012_SUPPORT
#define HLW8012_SUPPORT                 0
#endif

#ifndef HLW8012_SEL_PIN
#define HLW8012_SEL_PIN                 5
#endif

#ifndef HLW8012_CF1_PIN
#define HLW8012_CF1_PIN                 13
#endif

#ifndef HLW8012_CF_PIN
#define HLW8012_CF_PIN                  14
#endif

#ifndef HLW8012_SEL_CURRENT
#define HLW8012_SEL_CURRENT             HIGH    // SEL pin to HIGH to measure current
#endif

#ifndef HLW8012_CURRENT_R
#define HLW8012_CURRENT_R               0.001   // Current resistor
#endif

#ifndef HLW8012_VOLTAGE_R_UP
#define HLW8012_VOLTAGE_R_UP            ( 5 * 470000 )  // Upstream voltage resistor
#endif

#ifndef HLW8012_VOLTAGE_R_DOWN
#define HLW8012_VOLTAGE_R_DOWN          ( 1000 )        // Downstream voltage resistor
#endif

#ifndef HLW8012_CURRENT_RATIO
#define HLW8012_CURRENT_RATIO           0       // Set to 0 to use factory defaults
#endif

#ifndef HLW8012_VOLTAGE_RATIO
#define HLW8012_VOLTAGE_RATIO           0       // Set to 0 to use factory defaults
#endif

#ifndef HLW8012_POWER_RATIO
#define HLW8012_POWER_RATIO             0       // Set to 0 to use factory defaults
#endif

#ifndef HLW8012_USE_INTERRUPTS
#define HLW8012_USE_INTERRUPTS          1       // Use interrupts to trap HLW8012 signals
#endif

#ifndef HLW8012_WAIT_FOR_WIFI
#define HLW8012_WAIT_FOR_WIFI           0       // Weather to enable interrupts only after
                                                // wifi connection has been stablished
#endif

#ifndef HLW8012_INTERRUPT_ON
#define HLW8012_INTERRUPT_ON            CHANGE  // When to trigger the interrupt
                                                // Use CHANGE for HLW8012
                                                // Use FALLING for BL0937 / HJL0
#endif

//------------------------------------------------------------------------------
// CSE7766 based power sensor
// Enable support by passing CSE7766_SUPPORT=1 build flag
//------------------------------------------------------------------------------

#ifndef CSE7766_SUPPORT
#define CSE7766_SUPPORT                 0
#endif

#ifndef CSE7766_PIN
#define CSE7766_PIN                     1       // TX pin from the CSE7766
#endif

#ifndef CSE7766_PIN_INVERSE
#define CSE7766_PIN_INVERSE             0       // Signal is inverted
#endif

#define CSE7766_SYNC_INTERVAL           300     // Safe time between transmissions (ms)
#define CSE7766_BAUDRATE                4800    // UART baudrate

#define CSE7766_V1R                     1.0     // 1mR current resistor
#define CSE7766_V2R                     1.0     // 1M voltage resistor

//------------------------------------------------------------------------------
// SonOff SC evironment unit sensor
// Enable support by passing ITEAD_SONOFF_SC_SUPPORT=1 build flag
//------------------------------------------------------------------------------

#ifndef ITEAD_SONOFF_SC_SUPPORT
#define ITEAD_SONOFF_SC_SUPPORT         0
#endif

#ifndef ITEAD_SONOFF_SC_PRO_SUPPORT
#define ITEAD_SONOFF_SC_PRO_SUPPORT     0
#endif

// =============================================================================
// Sensor helpers configuration - can't move to dependencies.h
// =============================================================================

#ifndef SENSOR_SUPPORT
#define SENSOR_SUPPORT ( \
    HLW8012_SUPPORT || \
    CSE7766_SUPPORT || \
    ITEAD_SONOFF_SC_SUPPORT || \
    ITEAD_SONOFF_SC_PRO_SUPPORT \
)
#endif

// -----------------------------------------------------------------------------
// ADC
// -----------------------------------------------------------------------------

// Default ADC mode is to monitor internal power supply
#ifndef ADC_MODE_VALUE
#define ADC_MODE_VALUE                  ADC_VCC
#endif

//--------------------------------------------------------------------------------
// Class loading
//--------------------------------------------------------------------------------

#if SENSOR_SUPPORT

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
