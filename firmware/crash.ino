/*

CRASH REPORT MODULE

Copyright (C) 2018 FastyBird s.r.o. <info@fastybird.com>

Taken from krzychb EspSaveCrash [https://github.com/krzychb/EspSaveCrash]

*/

#include <stdio.h>
#include <stdarg.h>
#include <EEPROM_Rotate.h>

extern "C" {
    #include "user_interface.h"
}

#define SAVE_CRASH_EEPROM_OFFSET    0x0100  // initial address for crash data

/**
 * Structure of the single crash data set
 *
 *  1. Crash time
 *  2. Restart reason
 *  3. Exception cause
 *  4. epc1
 *  5. epc2
 *  6. epc3
 *  7. excvaddr
 *  8. depc
 *  9. adress of stack start
 * 10. adress of stack end
 * 11. stack trace bytes
 *     ...
 */
#define SAVE_CRASH_CRASH_TIME       0x00  // 4 bytes
#define SAVE_CRASH_RESTART_REASON   0x04  // 1 byte
#define SAVE_CRASH_EXCEPTION_CAUSE  0x05  // 1 byte
#define SAVE_CRASH_EPC1             0x06  // 4 bytes
#define SAVE_CRASH_EPC2             0x0A  // 4 bytes
#define SAVE_CRASH_EPC3             0x0E  // 4 bytes
#define SAVE_CRASH_EXCVADDR         0x12  // 4 bytes
#define SAVE_CRASH_DEPC             0x16  // 4 bytes
#define SAVE_CRASH_STACK_START      0x1A  // 4 bytes
#define SAVE_CRASH_STACK_END        0x1E  // 4 bytes
#define SAVE_CRASH_STACK_TRACE      0x22  // variable

/**
 * Save crash information in EEPROM
 * This function is called automatically if ESP8266 suffers an exception
 * It should be kept quick / consise to be able to execute before hardware wdt may kick in
 */
extern "C" void custom_crash_callback(
    struct rst_info * rst_info,
    uint32_t stack_start,
    uint32_t stack_end
) {
    // Do not record crash data when resetting the board
    if (checkNeedsReset()) {
        return;
    }

    // This method assumes EEPROM has already been initialized
    // which is the first thing firmware does

    // write crash time to EEPROM
    uint32_t crash_time = millis();

    EEPROMr.put(SAVE_CRASH_EEPROM_OFFSET + SAVE_CRASH_CRASH_TIME, crash_time);

    // write reset info to EEPROM
    EEPROMr.write(SAVE_CRASH_EEPROM_OFFSET + SAVE_CRASH_RESTART_REASON, rst_info->reason);
    EEPROMr.write(SAVE_CRASH_EEPROM_OFFSET + SAVE_CRASH_EXCEPTION_CAUSE, rst_info->exccause);

    // write epc1, epc2, epc3, excvaddr and depc to EEPROM
    EEPROMr.put(SAVE_CRASH_EEPROM_OFFSET + SAVE_CRASH_EPC1, rst_info->epc1);
    EEPROMr.put(SAVE_CRASH_EEPROM_OFFSET + SAVE_CRASH_EPC2, rst_info->epc2);
    EEPROMr.put(SAVE_CRASH_EEPROM_OFFSET + SAVE_CRASH_EPC3, rst_info->epc3);
    EEPROMr.put(SAVE_CRASH_EEPROM_OFFSET + SAVE_CRASH_EXCVADDR, rst_info->excvaddr);
    EEPROMr.put(SAVE_CRASH_EEPROM_OFFSET + SAVE_CRASH_DEPC, rst_info->depc);

    // write stack start and end address to EEPROM
    EEPROMr.put(SAVE_CRASH_EEPROM_OFFSET + SAVE_CRASH_STACK_START, stack_start);
    EEPROMr.put(SAVE_CRASH_EEPROM_OFFSET + SAVE_CRASH_STACK_END, stack_end);

    // starting address of Embedis data plus reserve
    const uint16_t settings_start = SPI_FLASH_SEC_SIZE - settingsSize() - 0x10;

    // write stack trace to EEPROM and avoid overwriting settings
    int16_t current_address = SAVE_CRASH_EEPROM_OFFSET + SAVE_CRASH_STACK_TRACE;

    for (uint32_t i = stack_start; i < stack_end; i++) {
        if (current_address >= settings_start) {
            break;
        }

        byte* byteValue = (byte*) i;

        EEPROMr.write(current_address++, *byteValue);
    }

    EEPROMr.commit();
}

// -----------------------------------------------------------------------------
// MODULE PRIVATE
// -----------------------------------------------------------------------------

#if WEB_SUPPORT
    void _crashOnGetReport(
        AsyncWebServerRequest * request
    ) {
        webLog(request);

        if (!webAuthenticate(request)) {
            return request->requestAuthentication(getIdentifier().c_str());
        }

        AsyncResponseStream *response = request->beginResponseStream("text/plain");

        char buffer[100];

        snprintf_P(buffer, sizeof(buffer), PSTR("attachment; filename=\"%s-crash_report.txt\""), (char *) getIdentifier().c_str());

        response->addHeader("Content-Disposition", buffer);
        response->addHeader("X-XSS-Protection", "1; mode=block");
        response->addHeader("X-Content-Type-Options", "nosniff");
        response->addHeader("X-Frame-Options", "deny");

        snprintf_P(buffer, sizeof(buffer), PSTR("%s-crash_report.txt"), (char *) getIdentifier().c_str());

        response->addHeader("X-Suggested-Filename", buffer);

        uint32_t crash_time;

        EEPROMr.get(SAVE_CRASH_EEPROM_OFFSET + SAVE_CRASH_CRASH_TIME, crash_time);

        if ((crash_time == 0) || (crash_time == 0xFFFFFFFF)) {
            response->printf("No crash info\n");
            return;
        }

        response->printf("Latest crash was at %lu ms after boot\n", crash_time);
        response->printf("Reason of restart: %u\n", EEPROMr.read(SAVE_CRASH_EEPROM_OFFSET + SAVE_CRASH_RESTART_REASON));
        response->printf("Exception cause: %u\n", EEPROMr.read(SAVE_CRASH_EEPROM_OFFSET + SAVE_CRASH_EXCEPTION_CAUSE));

        uint32_t epc1, epc2, epc3, excvaddr, depc;

        EEPROMr.get(SAVE_CRASH_EEPROM_OFFSET + SAVE_CRASH_EPC1, epc1);
        EEPROMr.get(SAVE_CRASH_EEPROM_OFFSET + SAVE_CRASH_EPC2, epc2);
        EEPROMr.get(SAVE_CRASH_EEPROM_OFFSET + SAVE_CRASH_EPC3, epc3);
        EEPROMr.get(SAVE_CRASH_EEPROM_OFFSET + SAVE_CRASH_EXCVADDR, excvaddr);
        EEPROMr.get(SAVE_CRASH_EEPROM_OFFSET + SAVE_CRASH_DEPC, depc);

        response->printf("epc1=0x%08x epc2=0x%08x epc3=0x%08x\n", epc1, epc2, epc3);
        response->printf("excvaddr=0x%08x depc=0x%08x\n", excvaddr, depc);

        uint32_t stack_start, stack_end;

        EEPROMr.get(SAVE_CRASH_EEPROM_OFFSET + SAVE_CRASH_STACK_START, stack_start);
        EEPROMr.get(SAVE_CRASH_EEPROM_OFFSET + SAVE_CRASH_STACK_END, stack_end);

        response->printf("sp=0x%08x end=0x%08x\n", stack_start, stack_end);

        int16_t current_address = SAVE_CRASH_EEPROM_OFFSET + SAVE_CRASH_STACK_TRACE;
        int16_t stack_len = stack_end - stack_start;

        uint32_t stack_trace;

        response->printf(">>>stack>>>\n");

        for (int16_t i = 0; i < stack_len; i += 0x10) {
            response->printf("%08x: ", stack_start + i);

            for (byte j = 0; j < 4; j++) {
                EEPROMr.get(current_address, stack_trace);
                response->printf("%08x ", stack_trace);
                current_address += 4;
            }

            response->printf("\n");
        }

        response->printf("<<<stack<<<\n");

        request->send(response);
    }

// -----------------------------------------------------------------------------

    void _crashOnDeleteReport(
        AsyncWebServerRequest * request
    ) {
        webLog(request);

        if (!webAuthenticate(request)) {
            return request->requestAuthentication(getIdentifier().c_str());
        }

        uint32_t crash_time = 0xFFFFFFFF;

        EEPROMr.put(SAVE_CRASH_EEPROM_OFFSET + SAVE_CRASH_CRASH_TIME, crash_time);
        EEPROMr.commit();

        request->send(201);
    }
#endif

// -----------------------------------------------------------------------------
// MODULE CORE
// -----------------------------------------------------------------------------

void crashSetup() {
    #if WEB_SUPPORT
        webServer()->on(WEB_API_REPORT_CRASH, HTTP_GET, _crashOnGetReport);
        webServer()->on(WEB_API_REPORT_CRASH, HTTP_DELETE, _crashOnDeleteReport);
    #endif
}
