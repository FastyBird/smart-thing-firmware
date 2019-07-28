/*

SYSTEM MODULE

Copyright (C) 2018 FastyBird s.r.o. <info@fastybird.com>

*/

#include <EEPROM_Rotate.h>

uint32_t _system_loop_delay = 0;

// Calculated load average 0 to 100;
uint16_t _system_load_average = 100;

extern "C" uint32_t _SPIFFS_start;
extern "C" uint32_t _SPIFFS_end;

bool _system_send_heartbeat = false;

std::vector<system_on_heartbeat_callback_f> _system_on_heartbeat_callbacks;

// -----------------------------------------------------------------------------
// MODULE PRIVATE
// -----------------------------------------------------------------------------

uint8_t _systemBytes2Sectors(
    size_t size
) {
    return (uint8_t) (size + SPI_FLASH_SEC_SIZE - 1) / SPI_FLASH_SEC_SIZE;
}

// -----------------------------------------------------------------------------

uint32_t _systemOTASpace() {
    return (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
}

// -----------------------------------------------------------------------------

uint32_t _systemFilesystemSpace() {
    return ((uint32_t)&_SPIFFS_end - (uint32_t)&_SPIFFS_start);
}

// -----------------------------------------------------------------------------

uint32_t _systemEepromSpace() {
    return EEPROMr.reserved() * SPI_FLASH_SEC_SIZE;
}

// -----------------------------------------------------------------------------

void _systemPrintMemoryLayoutLine(
    const char * name,
    uint32_t bytes,
    bool reset
) {
    static uint32_t index = 0;

    if (reset) {
        index = 0;
    }

    if (0 == bytes) {
        return;
    }

    uint8_t _sectors = _systemBytes2Sectors(bytes);

    DEBUG_MSG(PSTR("[SYSTEM] %-20s: %8lu bytes / %4d sectors (%4d to %4d)\n"), name, bytes, _sectors, index, index + _sectors - 1);

    index += _sectors;
}

// -----------------------------------------------------------------------------

void _systemPrintMemoryLayoutLine(
    const char * name,
    uint32_t bytes
) {
    _systemPrintMemoryLayoutLine(name, bytes, false);
}

// -----------------------------------------------------------------------------

void _systemHeartbeat() {
    // Callbacks
    for (uint8_t i = 0; i < _system_on_heartbeat_callbacks.size(); i++) {
        (_system_on_heartbeat_callbacks[i])();
    }
}

// -----------------------------------------------------------------------------

void _systemInfoOnHeartbeat() {
    uint32_t uptime_seconds = getUptime();
    uint8_t free_heap = getFreeHeap();

    DEBUG_MSG(PSTR("[SYSTEM] Uptime: %lu seconds\n"), uptime_seconds);
    DEBUG_MSG(PSTR("[SYSTEM] Free heap: %lu bytes\n"), free_heap);

    #if ADC_MODE_VALUE == ADC_VCC
        DEBUG_MSG(PSTR("[SYSTEM] Power: %lu mV\n"), ESP.getVcc());
    #endif
}

// -----------------------------------------------------------------------------

#if WEB_SUPPORT && WS_SUPPORT
    void _systemStatus(
        JsonObject& root
    ) {
        root["free_heap"] = getFreeHeap();
        root["uptime"] = getUptime();
        root["rssi"] = WiFi.RSSI();
        root["load_average"] = systemLoadAverage();

        #if ADC_MODE_VALUE == ADC_VCC
            root["vcc"] = ESP.getVcc();
        #endif

        uint8_t reason = resetReason();

        if (reason > 0) {
            char buffer[32];
            strcpy_P(buffer, custom_reset_string[reason - 1]);

            root["last_reset_reason"] = buffer;
            root["last_reset_info"] = "";

        } else {
            root["last_reset_reason"] = (char *) ESP.getResetReason().c_str();
            root["last_reset_info"] = (char *) ESP.getResetInfo().c_str();
        }
    }

// -----------------------------------------------------------------------------

    // WS client is connected
    void _systemWSOnConnect(
        JsonObject& root
    ) {
        JsonArray& modules = root.containsKey("modules") ? root["modules"] : root.createNestedArray("modules");
        JsonObject& module = modules.createNestedObject();

        module["module"] = "system";
        module["visible"] = true;

        char chipid[7];

        snprintf_P(chipid, sizeof(chipid), PSTR("%06X"), ESP.getChipId());

        uint8_t * bssid = WiFi.BSSID();

        char bssid_str[20];

        snprintf_P(bssid_str, sizeof(bssid_str),
            PSTR("%02X:%02X:%02X:%02X:%02X:%02X"),
            bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5]
        );

        // Data container
        JsonObject& data = module.createNestedObject("data");

        // Firmware
        JsonObject& firmware = data.createNestedObject("firmware");

        firmware["name"] = FIRMWARE_MANUFACTURER;
        firmware["version"] = FIRMWARE_VERSION;
        firmware["build"] = getBuildTime();
        firmware["sketch_size"] = ESP.getSketchSize();
        firmware["free_space"] = ESP.getFreeSketchSpace();
        firmware["max_ota_size"] = _systemOTASpace();

        // Thing
        JsonObject& thing = data.createNestedObject("thing");

        thing["thing"] = THING;
        thing["manufacturer"] = MANUFACTURER;
        thing["chipid"] = String(chipid);
        thing["sdk"] = ESP.getSdkVersion();
        thing["core"] = getCoreVersion();
        thing["mac"] = WiFi.macAddress();

        // Network
        JsonObject& network = data.createNestedObject("network");

        network["bssid"] = String(bssid_str);
        network["channel"] = WiFi.channel();
        network["hostname"] = getIdentifier();
        network["network"] = getNetwork();
        network["ip"] = getIP();

        // Status
        JsonObject& status = data.createNestedObject("status");

        _systemStatus(status);
    }

// -----------------------------------------------------------------------------

    void _systemWSOnUpdate(
        JsonObject& root
    ) {
        JsonArray& modules = root.containsKey("modules") ? root["modules"] : root.createNestedArray("modules");
        JsonObject& module = modules.createNestedObject();

        module["module"] = "system";

        JsonObject& data = module.createNestedObject("data");

        // Status
        JsonObject& status = data.createNestedObject("status");

        _systemStatus(status);
    }
#endif // WEB_SUPPORT && WS_SUPPORT

// -----------------------------------------------------------------------------

void _systemSetupSpecificHardware() {
    // These devices use the hardware UART
    // to communicate to secondary microcontrollers
    #if defined(ITEAD_SONOFF_RFBRIDGE) || defined(ITEAD_SONOFF_DUAL) || (RELAY_PROVIDER == RELAY_PROVIDER_STM)
        Serial.begin(SERIAL_BAUDRATE);
    #endif
}

// -----------------------------------------------------------------------------

void _systemInfo() {
    DEBUG_MSG(PSTR("\n\n---8<-------\n\n"));

    // -------------------------------------------------------------------------

    DEBUG_MSG(PSTR("[SYSTEM] " FIRMWARE_MANUFACTURER " " FIRMWARE_VERSION "\n"));
    DEBUG_MSG(PSTR("[SYSTEM] " MANUFACTURER_WEBSITE "\n"));
    DEBUG_MSG(PSTR("[SYSTEM] " MANUFACTURER_CONTACT "\n\n"));
    DEBUG_MSG(PSTR("[SYSTEM] CPU chip ID: 0x%06X\n"), ESP.getChipId());
    DEBUG_MSG(PSTR("[SYSTEM] CPU frequency: %u MHz\n"), ESP.getCpuFreqMHz());
    DEBUG_MSG(PSTR("[SYSTEM] SDK version: %s\n"), ESP.getSdkVersion());
    DEBUG_MSG(PSTR("[SYSTEM] Core version: %s\n"), getCoreVersion().c_str());
    DEBUG_MSG(PSTR("[SYSTEM] Core revision: %s\n"), getCoreRevision().c_str());
    DEBUG_MSG(PSTR("\n"));

    // -------------------------------------------------------------------------

    FlashMode_t mode = ESP.getFlashChipMode();
    DEBUG_MSG(PSTR("[SYSTEM] Flash chip ID: 0x%06X\n"), ESP.getFlashChipId());
    DEBUG_MSG(PSTR("[SYSTEM] Flash speed: %u Hz\n"), ESP.getFlashChipSpeed());
    DEBUG_MSG(PSTR("[SYSTEM] Flash mode: %s\n"), mode == FM_QIO ? "QIO" : mode == FM_QOUT ? "QOUT" : mode == FM_DIO ? "DIO" : mode == FM_DOUT ? "DOUT" : "UNKNOWN");
    DEBUG_MSG(PSTR("\n"));

    // -------------------------------------------------------------------------

    _systemPrintMemoryLayoutLine("Flash size (CHIP)", ESP.getFlashChipRealSize(), true);
    _systemPrintMemoryLayoutLine("Flash size (SDK)", ESP.getFlashChipSize(), true);
    _systemPrintMemoryLayoutLine("Reserved", 1 * SPI_FLASH_SEC_SIZE, true);
    _systemPrintMemoryLayoutLine("Firmware size", ESP.getSketchSize());
    _systemPrintMemoryLayoutLine("Max OTA size", _systemOTASpace());
    _systemPrintMemoryLayoutLine("SPIFFS size", _systemFilesystemSpace());
    _systemPrintMemoryLayoutLine("EEPROM size", _systemEepromSpace());
    _systemPrintMemoryLayoutLine("Reserved", 4 * SPI_FLASH_SEC_SIZE);

    DEBUG_MSG(PSTR("\n"));

    // -------------------------------------------------------------------------

    #if SPIFFS_SUPPORT
        FSInfo fs_info;
        bool fs = SPIFFS.info(fs_info);
        if (fs) {
            DEBUG_MSG(PSTR("[SYSTEM] SPIFFS total size   : %8u bytes / %4d sectors\n"), fs_info.totalBytes, _systemBytes2Sectors(fs_info.totalBytes));
            DEBUG_MSG(PSTR("[SYSTEM]        used size    : %8u bytes\n"), fs_info.usedBytes);
            DEBUG_MSG(PSTR("[SYSTEM]        block size   : %8u bytes\n"), fs_info.blockSize);
            DEBUG_MSG(PSTR("[SYSTEM]        page size    : %8u bytes\n"), fs_info.pageSize);
            DEBUG_MSG(PSTR("[SYSTEM]        max files    : %8u\n"), fs_info.maxOpenFiles);
            DEBUG_MSG(PSTR("[SYSTEM]        max length   : %8u\n"), fs_info.maxPathLength);
        } else {
            DEBUG_MSG(PSTR("[SYSTEM] No SPIFFS partition\n"));
        }
        DEBUG_MSG(PSTR("\n"));
    #endif

    // -------------------------------------------------------------------------

    eepromSectorsDebug();

    DEBUG_MSG(PSTR("\n"));

    // -------------------------------------------------------------------------

    systemMemory("EEPROM", SPI_FLASH_SEC_SIZE, SPI_FLASH_SEC_SIZE - settingsSize());
    systemMemory("Heap", getInitialFreeHeap(), getFreeHeap());
    systemMemory("Stack", 4096, getFreeStack());

    DEBUG_MSG(PSTR("\n"));

    // -------------------------------------------------------------------------

    DEBUG_MSG(PSTR("[SYSTEM] Boot version: %d\n"), ESP.getBootVersion());
    DEBUG_MSG(PSTR("[SYSTEM] Boot mode: %d\n"), ESP.getBootMode());

    uint8_t reason = resetReason();

    if (reason > 0) {
        char buffer[32];
        strcpy_P(buffer, custom_reset_string[reason - 1]);

        DEBUG_MSG(PSTR("[SYSTEM] Last reset reason: %s\n"), buffer);

    } else {
        DEBUG_MSG(PSTR("[SYSTEM] Last reset reason: %s\n"), (char *) ESP.getResetReason().c_str());
        DEBUG_MSG(PSTR("[SYSTEM] Last reset info: %s\n"), (char *) ESP.getResetInfo().c_str());
    }

    DEBUG_MSG(PSTR("\n"));

    // -------------------------------------------------------------------------

    DEBUG_MSG(PSTR("[SYSTEM] Board: %s\n"), THING);
    DEBUG_MSG(PSTR("[SYSTEM] Support: %s\n"), getFirmwareModules().c_str());

    #if SENSOR_SUPPORT
        DEBUG_MSG(PSTR("[SYSTEM] Sensors: %s\n"), getFirmwareSensors().c_str());
    #endif

    DEBUG_MSG(PSTR("\n"));

    // -------------------------------------------------------------------------

    DEBUG_MSG(PSTR("[SYSTEM] Firmware MD5: %s\n"), (char *) ESP.getSketchMD5().c_str());

    #if ADC_MODE_VALUE == ADC_VCC
        DEBUG_MSG(PSTR("[SYSTEM] Power: %u mV\n"), ESP.getVcc());
    #endif

    DEBUG_MSG(PSTR("[SYSTEM] Power saving delay value: %lu ms\n"), systemLoopDelay());

    // -------------------------------------------------------------------------

    #if STABILTY_CHECK_ENABLED
        if (!stabiltyCheck()) {
            DEBUG_MSG(PSTR("\n"));
            DEBUG_MSG(PSTR("[SYSTEM] Thing is in SAFE MODE\n"));
        }
    #endif

    // -------------------------------------------------------------------------

    DEBUG_MSG(PSTR("\n\n---8<-------\n\n"));
}

// -----------------------------------------------------------------------------
// MODULE API
// -----------------------------------------------------------------------------

void systemOnHeartbeatRegister(
    system_on_heartbeat_callback_f callback
) {
    _system_on_heartbeat_callbacks.push_back(callback);
}

// -----------------------------------------------------------------------------

void systemSendHeartbeat() {
    _system_send_heartbeat = true;
}

// -----------------------------------------------------------------------------

void systemMemory(
    const char * name,
    unsigned int total_memory,
    unsigned int free_memory
) {
    DEBUG_MSG(
        PSTR("[SYSTEM] %-6s: %5u bytes initially | %5u bytes used (%2u%%) | %5u bytes free (%2u%%)\n"),
        name,
        total_memory,
        total_memory - free_memory,
        100 * (total_memory - free_memory) / total_memory,
        free_memory,
        100 * free_memory / total_memory
    );
}

// -----------------------------------------------------------------------------

uint32_t systemLoopDelay() {
    return _system_loop_delay;
}

// -----------------------------------------------------------------------------

uint32_t systemLoadAverage() {
    return _system_load_average;
}

// -----------------------------------------------------------------------------
// MODULE CORE
// -----------------------------------------------------------------------------

void systemSetup() {
    #if SPIFFS_SUPPORT
        SPIFFS.begin();
    #endif

    // Question system stability
    #if STABILTY_CHECK_ENABLED
        stabiltyCheck(false);
    #endif

    // Init device-specific hardware
    _systemSetupSpecificHardware();

    // Cache loop delay value to speed things (recommended max 250ms)
    _system_loop_delay = atol(getSetting("loopDelay", LOOP_DELAY_TIME).c_str());
    _system_loop_delay = constrain(_system_loop_delay, 0, 300);

    #if WEB_SUPPORT && WS_SUPPORT
        wsOnConnectRegister(_systemWSOnConnect);
        wsOnUpdateRegister(_systemWSOnUpdate);
    #endif

    systemOnHeartbeatRegister(_systemInfoOnHeartbeat);

    _systemInfo();

    // Register loop
    firmwareRegisterLoop(systemLoop);
}

// -----------------------------------------------------------------------------

void systemLoop() {
    // -------------------------------------------------------------------------
    // User requested reset
    // -------------------------------------------------------------------------

    if (checkNeedsReset()) {
        reset();
    }

    // -------------------------------------------------------------------------
    // Heartbeat
    // -------------------------------------------------------------------------

    #if HEARTBEAT_MODE == HEARTBEAT_ONCE
        if (_system_send_heartbeat) {
            _system_send_heartbeat = false;
            _systemHeartbeat();
        }
    #elif HEARTBEAT_MODE == HEARTBEAT_REPEAT
        static uint32_t last_hbeat = 0;
        if (_system_send_heartbeat || (last_hbeat == 0) || (millis() - last_hbeat > HEARTBEAT_INTERVAL)) {
            _system_send_heartbeat = false;
            last_hbeat = millis();
            _systemHeartbeat();
        }
    #endif // HEARTBEAT_MODE == HEARTBEAT_REPEAT

    // -------------------------------------------------------------------------
    // Load Average calculation
    // -------------------------------------------------------------------------

    static uint32_t last_loadcheck = 0;
    static uint32_t load_counter_temp = 0;
    load_counter_temp++;

    if (millis() - last_loadcheck > LOADAVG_INTERVAL) {
        static uint32_t load_counter = 0;
        static uint32_t load_counter_max = 1;

        load_counter = load_counter_temp;
        load_counter_temp = 0;

        if (load_counter > load_counter_max) {
            load_counter_max = load_counter;
        }

        _system_load_average = 100 - (100 * load_counter / load_counter_max);

        last_loadcheck = millis();
    }

    // -------------------------------------------------------------------------
    // Power saving delay
    // -------------------------------------------------------------------------

    delay(_system_loop_delay);
}
