/*

FIRMWARE GENERAL SETTINGS

Copyright (C) 2018 FastyBird s.r.o. <code@fastybird.com>

*/

#pragma once

//------------------------------------------------------------------------------
// GENERAL
//------------------------------------------------------------------------------

#ifndef LOOP_DELAY_TIME
    #define LOOP_DELAY_TIME                 10                                  // Delay for this millis in the main loop [0-250]
#endif

#ifndef ADMIN_PASSWORD
    #define ADMIN_PASSWORD                  "fibonacci"
#endif

//------------------------------------------------------------------------------
// SERIAL PORT
//------------------------------------------------------------------------------

#ifndef SERIAL_PORT
    #define SERIAL_PORT                     Serial
#endif

#ifndef SERIAL_BAUDRATE
    #define SERIAL_BAUDRATE                 115200                              // Default baudrate
#endif

// Second serial port (used for RX)

#ifndef SERIAL_RX_ENABLED
    #define SERIAL_RX_ENABLED               0                                   // Secondary serial port for RX
#endif

#ifndef SERIAL_RX_PORT
    #define SERIAL_RX_PORT                  Serial                              // This setting is usually defined
                                                                                // in the hardware.h file for those
                                                                                // boards that require it
#endif

#ifndef SERIAL_RX_BAUDRATE
    #define SERIAL_RX_BAUDRATE              115200                              // Default baudrate
#endif

//------------------------------------------------------------------------------
// NETWORK
//------------------------------------------------------------------------------

#ifndef NETWORK_SSL_ENABLED
    #define NETWORK_SSL_ENABLED             0                                   // Enable SSL
#endif

//------------------------------------------------------------------------------
// STABILTY CHECK
//------------------------------------------------------------------------------

#ifndef STABILTY_CHECK_ENABLED
    #define STABILTY_CHECK_ENABLED          1                                   // Enable crash check by default
#endif

#ifndef STABILTY_CHECK_TIME
    #define STABILTY_CHECK_TIME             60000                               // The system is considered stable after these many millis
#endif

#ifndef STABILTY_CHECK_MAX
    #define STABILTY_CHECK_MAX              5                                   // After this many crashes on boot
                                                                                // the system is flagged as unstable
#endif

//------------------------------------------------------------------------------
// EEPROM
//------------------------------------------------------------------------------

#define EEPROM_SIZE                         SPI_FLASH_SEC_SIZE                  // EEPROM size in bytes (1 sector = 4096 bytes)

//#define EEPROM_RORATE_SECTORS             2                                   // Number of sectors to use for EEPROM rotation
                                                                                // If not defined the firmware will use a number based
                                                                                // on the number of available sectors

#define EEPROM_CUSTOM_RESET                 0                                   // Address for the reset reason (1 byte)
#define EEPROM_CRASH_COUNTER                1                                   // Address for the crash counter (1 byte)
#define EEPROM_MESSAGE_ID                   2                                   // Address for the MQTT message id (4 bytes)
#define EEPROM_ROTATE_DATA                  6                                   // Reserved for the EEPROM_ROTATE library (3 bytes)
#define EEPROM_RELAY_STATUS                 9                                   // Address for the relay status (1 byte)
#define EEPROM_ENERGY_COUNT                 10                                  // Address for the energy counter (4 bytes)
#define EEPROM_DATA_END                     14                                  // End of custom EEPROM data block

//------------------------------------------------------------------------------
// HEARTBEAT
//------------------------------------------------------------------------------

#ifndef HEARTBEAT_MODE
    #define HEARTBEAT_MODE                  HEARTBEAT_REPEAT
#endif

#ifndef HEARTBEAT_INTERVAL
    #define HEARTBEAT_INTERVAL              300000                              // Interval between heartbeat messages (in ms)
#endif

#define UPTIME_OVERFLOW                     4294967295                          // Uptime overflow value

//------------------------------------------------------------------------------
// LOAD AVERAGE
//------------------------------------------------------------------------------

#ifndef LOADAVG_INTERVAL
    #define LOADAVG_INTERVAL                30000                               // Interval between calculating load average (in ms)
#endif

// -----------------------------------------------------------------------------
// SPIFFS
// -----------------------------------------------------------------------------

#ifndef SPIFFS_SUPPORT
    #define SPIFFS_SUPPORT                  0                                   // Do not add support for SPIFFS by default
#endif

// -----------------------------------------------------------------------------
// ADC
// -----------------------------------------------------------------------------

// Default ADC mode is to monitor internal power supply
#ifndef ADC_MODE_VALUE
    #define ADC_MODE_VALUE                  ADC_VCC
#endif
