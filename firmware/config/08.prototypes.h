#include <Arduino.h>
#include <ArduinoJson.h>
#include <functional>
#include <vector>
#include <pgmspace.h>
#include <core_version.h>
#include <ESPAsyncTCP.h>
#include <Ticker.h>

extern "C" {
    #include "user_interface.h"
    extern struct rst_info resetInfo;
}

// -----------------------------------------------------------------------------
// DEBUG MODULE
// -----------------------------------------------------------------------------
void debugSend(PGM_P format, ...);

extern "C" {
     void custom_crash_callback(struct rst_info*, uint32_t, uint32_t);
}

// Core version 2.4.2 and higher changed the cont_t structure to a pointer:
// https://github.com/esp8266/Arduino/commit/5d5ea92a4d004ab009d5f642629946a0cb8893dd#diff-3fa12668b289ccb95b7ab334833a4ba8L35
// Core version 2.5.0 introduced EspClass helper method:
// https://github.com/esp8266/Arduino/commit/0e0e34c614fe8a47544c9998201b1d9b3c24eb18
extern "C" {
    #include <cont.h>

    #if defined(ARDUINO_ESP8266_RELEASE_2_3_0) \
        || defined(ARDUINO_ESP8266_RELEASE_2_4_0) \
        || defined(ARDUINO_ESP8266_RELEASE_2_4_1)
        extern cont_t g_cont;
        #define getFreeStack() cont_get_free_stack(&g_cont)
    #elif defined(ARDUINO_ESP8266_RELEASE_2_4_2)
        extern cont_t* g_pcont;
        #define getFreeStack() cont_get_free_stack(g_pcont)
    #else
        #define getFreeStack() ESP.getFreeContStack()
    #endif
}

// -----------------------------------------------------------------------------
// EEPROM_ROTATE
// -----------------------------------------------------------------------------
#include <EEPROM_Rotate.h>

EEPROM_Rotate EEPROMr;

// -----------------------------------------------------------------------------
// GPIO
// -----------------------------------------------------------------------------
bool gpioValid(uint8_t gpio);
bool gpioGetLock(uint8_t gpio);
bool gpioReleaseLock(uint8_t gpio);

// -----------------------------------------------------------------------------
// I2C
// -----------------------------------------------------------------------------
void i2cScan();
void i2cClearBus();
bool i2cGetLock(uint8_t address);
bool i2cReleaseLock(uint8_t address);
uint8_t i2cFindAndLock(size_t size, uint8_t * addresses);

void i2c_wakeup(uint8_t address);
uint8_t i2c_write_buffer(uint8_t address, uint8_t * buffer, size_t len);
uint8_t i2c_write_uint8(uint8_t address, uint8_t value);
uint8_t i2c_write_uint8(uint8_t address, uint8_t reg, uint8_t value);
uint8_t i2c_write_uint8(uint8_t address, uint8_t reg, uint8_t value1, uint8_t value2);
uint8_t i2c_write_uint16(uint8_t address, uint16_t value);
uint8_t i2c_write_uint16(uint8_t address, uint8_t reg, uint16_t value);
uint8_t i2c_read_uint8(uint8_t address);
uint8_t i2c_read_uint8(uint8_t address, uint8_t reg);
uint16_t i2c_read_uint16(uint8_t address);
uint16_t i2c_read_uint16(uint8_t address, uint8_t reg);
uint16_t i2c_read_uint16_le(uint8_t address, uint8_t reg);
int16_t i2c_read_int16(uint8_t address, uint8_t reg);
int16_t i2c_read_int16_le(uint8_t address, uint8_t reg);
void i2c_read_buffer(uint8_t address, uint8_t * buffer, size_t len);

// -----------------------------------------------------------------------------
// FIRMWARE UTILS
// -----------------------------------------------------------------------------
char * ltrim(char * s);
void niceDelay(uint32_t ms);

#define ARRAYINIT(type, name, ...) type name[] = {__VA_ARGS__};

// -----------------------------------------------------------------------------
// RTC MEMORY
// -----------------------------------------------------------------------------
#include "./../libs/rtcmem.h"
