/*

SETTINGS MODULE

Copyright (C) 2018 FastyBird s.r.o. <info@fastybird.com>

*/

#include <EEPROM_Rotate.h>
#include <vector>

bool _settings_save = false;
bool _web_config_success = false;
std::vector<uint8_t> * _web_config_buffer;

// -----------------------------------------------------------------------------
// Reverse engineering EEPROM storage format
// -----------------------------------------------------------------------------

uint32_t settingsSize() {
    uint32_t pos = SPI_FLASH_SEC_SIZE - 1;

    while (size_t len = EEPROMr.read(pos)) {
        if (0xFF == len) {
            break;
        }

        pos = pos - len - 2;
    }

    return SPI_FLASH_SEC_SIZE - pos + EEPROM_DATA_END;
}

// -----------------------------------------------------------------------------

uint32_t settingsKeyCount() {
    uint32_t count = 0;
    uint32_t pos = SPI_FLASH_SEC_SIZE - 1;

    while (size_t len = EEPROMr.read(pos)) {
        if (0xFF == len) {
            break;
        }

        pos = pos - len - 2;
        len = EEPROMr.read(pos);
        pos = pos - len - 2;
        count++;
    }

    return count;
}

// -----------------------------------------------------------------------------

String settingsKeyName(
    const uint32_t index
) {
    String s;

    uint32_t count = 0;
    uint32_t pos = SPI_FLASH_SEC_SIZE - 1;

    while (size_t len = EEPROMr.read(pos)) {
        if (0xFF == len) {
            break;
        }

        pos = pos - len - 2;

        if (count == index) {
            s.reserve(len);

            for (uint32_t i = 0 ; i < len; i++) {
                s += (char) EEPROMr.read(pos + i + 1);
            }

            break;
        }

        count++;

        len = EEPROMr.read(pos);

        pos = pos - len - 2;
    }

    return s;
}

// -----------------------------------------------------------------------------
// MODULE PRIVATE
// -----------------------------------------------------------------------------

std::vector<String> _settingsKeys() {
    // Get sorted list of keys
    std::vector<String> keys;

    uint32_t size = settingsKeyCount();

    for (uint32_t i = 0; i < size; i++) {
        String _key = settingsKeyName(i);

        bool _inserted = false;

        for (uint32_t j = 0; j < keys.size(); j++) {
            // Check if we have to insert it before the current element
            if (keys[j].compareTo(_key) > 0) {
                keys.insert(keys.begin() + j, _key);
                _inserted = true;

                break;
            }
        }

        // If we could not insert it, just push it at the end
        if (!_inserted) {
            keys.push_back(_key);
        }
    }

    return keys;
}

// -----------------------------------------------------------------------------

bool _settingsRestoreJson(
    JsonObject& data
) {
    const char * _thing = data["thing"];

    if (strcmp(_thing, THING) != 0) {
        return false;
    }

    const char * _version = data["version"];

    if (strcmp(_version, FIRMWARE_VERSION) != 0) {
        return false;
    }

    for (unsigned int i = EEPROM_DATA_END; i < SPI_FLASH_SEC_SIZE; i++) {
        EEPROMr.write(i, 0xFF);
    }

    for (auto element : data) {
        if (
            strcmp(element.key, "thing") == 0
            || strcmp(element.key, "manufacturer") == 0
            || strcmp(element.key, "version") == 0
            || strcmp(element.key, "backup") == 0
            || strcmp(element.key, "timestamp") == 0
        ) {
            continue;
        }

        setSetting(element.key, element.value.as<char *>());
    }

    saveSettings();

    DEBUG_MSG(PSTR("[SETTINGS] Settings restored successfully\n"));

    return true;
}

// -----------------------------------------------------------------------------

#if WEB_SUPPORT
    void _settingsOnGetConfig(
        AsyncWebServerRequest * request
    ) {
        webLog(request);

        if (!webAuthenticate(request)) {
            return request->requestAuthentication(getIdentifier().c_str());
        }

        AsyncResponseStream *response = request->beginResponseStream("text/json");

        char buffer[100];

        snprintf_P(buffer, sizeof(buffer), PSTR("attachment; filename=\"%s-backup.json\""), (char *) getIdentifier().c_str());

        response->addHeader("Content-Disposition", buffer);
        response->addHeader("X-XSS-Protection", "1; mode=block");
        response->addHeader("X-Content-Type-Options", "nosniff");
        response->addHeader("X-Frame-Options", "deny");

        snprintf_P(buffer, sizeof(buffer), PSTR("%s-backup.json"), (char *) getIdentifier().c_str());

        response->addHeader("X-Suggested-Filename", buffer);

        response->printf("{\n\"thing\": \"%s\"", THING);
        response->printf(",\n\"manufacturer\": \"%s\"", FIRMWARE_MANUFACTURER);
        response->printf(",\n\"version\": \"%s\"", FIRMWARE_VERSION);
        response->printf(",\n\"backup\": \"1\"");

        #if NTP_SUPPORT
            response->printf(",\n\"timestamp\": \"%s\"", ntpDateTime().c_str());
        #endif

        // Write the keys line by line (not sorted)
        uint32_t count = settingsKeyCount();

        for (unsigned int i = 0; i < count; i++) {
            String key = settingsKeyName(i);
            String value = getSetting(key);
            
            response->printf(",\n\"%s\": \"%s\"", key.c_str(), value.c_str());
        }

        response->printf("\n}");

        request->send(response);
    }

// -----------------------------------------------------------------------------

    void _settingsOnPostConfig(
        AsyncWebServerRequest * request
    ) {
        webLog(request);

        if (!webAuthenticate(request)) {
            return request->requestAuthentication(getIdentifier().c_str());
        }

        request->send(_web_config_success ? 200 : 400);

        if (_web_config_success) {
            #if WEB_SUPPORT && WS_SUPPORT
                // Send notification to all clients
                wsSend_P(PSTR("{\"doAction\": \"reload\", \"reason\": \"restore\"}"));
            #endif

            deferredReset(250, CUSTOM_RESTORE_WEB);
        }
    }

// -----------------------------------------------------------------------------

    void _settingsOnPostConfigData(
        AsyncWebServerRequest *request,
        String filename,
        size_t index,
        uint8_t * data,
        size_t len,
        bool final
    ) {
        if (!webAuthenticate(request)) {
            return request->requestAuthentication(getIdentifier().c_str());
        }

        // No buffer
        if (final && (index == 0)) {
            DynamicJsonBuffer jsonBuffer;
            JsonObject& root = jsonBuffer.parseObject((char *) data);

            if (root.success()) {
                _web_config_success = _settingsRestoreJson(root);
            }

            return;
        }

        // Buffer start => reset
        if (index == 0) {
            if (_web_config_buffer) {
                delete _web_config_buffer;
            }
        }

        // init buffer if it doesn't exist
        if (!_web_config_buffer) {
            _web_config_buffer = new std::vector<uint8_t>();
            _web_config_success = false;
        }

        // Copy
        if (len > 0) {
            _web_config_buffer->reserve(_web_config_buffer->size() + len);
            _web_config_buffer->insert(_web_config_buffer->end(), data, data + len);
        }

        // Ending
        if (final) {
            _web_config_buffer->push_back(0);

            // Parse JSON
            DynamicJsonBuffer jsonBuffer;
            JsonObject& root = jsonBuffer.parseObject((char *) _web_config_buffer->data());

            if (root.success()) {
                _web_config_success = _settingsRestoreJson(root);
            }

            delete _web_config_buffer;
        }
    }

// -----------------------------------------------------------------------------

    #if WS_SUPPORT
        // WS client called action
        void _settingsWSOnAction(
            const uint32_t clientId,
            const char * action,
            JsonObject& data
        ) {
            if (strcmp(action, "factory_reset") == 0) {
                DEBUG_MSG(PSTR("[SETTINGS] Requested factory reset action\n"));
                DEBUG_MSG(PSTR("\n\nFACTORY RESET\n\n"));

                resetSettings();
                
                // Send notification to all clients
                wsSend_P(PSTR("{\"doAction\": \"reload\", \"reason\": \"factory\"}"));

                deferredReset(250, CUSTOM_FACTORY_WEB);
                return;

            } else if (strcmp(action, "restore") == 0) {
                if (_settingsRestoreJson(data)) {
                    wsSend_P(clientId, PSTR("{\"message\": \"restore_finished\"}"));

                    // Send notification to all clients
                    wsSend_P(PSTR("{\"doAction\": \"reload\", \"reason\": \"restore\"}"));

                    deferredReset(250, CUSTOM_RESTORE_WEB);

                } else {
                    wsSend_P(clientId, PSTR("{\"message\": \"invalid_restore_file\", \"level\": \"error\"}"));
                }
            }
        }
    #endif
#endif

// -----------------------------------------------------------------------------
// MODULE API
// -----------------------------------------------------------------------------

bool isSavingRequired() {
    return _settings_save;
}

// -----------------------------------------------------------------------------

void saveSettings() {
    #if not SETTINGS_AUTOSAVE
        _settings_save = true;
    #endif
}

// -----------------------------------------------------------------------------

void resetSettings() {
    for (unsigned int i = 0; i < SPI_FLASH_SEC_SIZE; i++) {
        EEPROMr.write(i, 0xFF);
    }

    EEPROMr.commit();
}

// -----------------------------------------------------------------------------

size_t settingsMaxSize() {
    size_t size = EEPROM_SIZE;

    if (size > SPI_FLASH_SEC_SIZE) {
        size = SPI_FLASH_SEC_SIZE;
    }

    size = (size + 3) & (~3);

    return size;
}

// -----------------------------------------------------------------------------
// Key-value API
// -----------------------------------------------------------------------------

template<typename T> String getSetting(
    const String& key,
    T defaultValue
) {
    String value;

    if (!Embedis::get(key, value)) {
        value = String(defaultValue);
    }

    return value;
}

// -----------------------------------------------------------------------------

template<typename T> String getSetting(
    const String& key,
    const uint32_t index,
    T defaultValue
) {
    return getSetting(key + String(index), defaultValue);
}

// -----------------------------------------------------------------------------

String getSetting(
    const String& key
) {
    return getSetting(key, "");
}

// -----------------------------------------------------------------------------

template<typename T> bool setSetting(
    const String& key,
    T value
) {
    return Embedis::set(key, String(value));
}

// -----------------------------------------------------------------------------

template<typename T> bool setSetting(
    const String& key,
    const uint32_t index,
    T value
) {
    return setSetting(key + String(index), value);
}

// -----------------------------------------------------------------------------

bool delSetting(
    const String& key
) {
    return Embedis::del(key);
}

// -----------------------------------------------------------------------------

bool delSetting(
    const String& key,
    const uint32_t index
) {
    return delSetting(key + String(index));
}

// -----------------------------------------------------------------------------

bool hasSetting(
    const String& key
) {
    return getSetting(key).length() != 0;
}

// -----------------------------------------------------------------------------

bool hasSetting(
    const String& key,
    uint32_t index
) {
    return getSetting(key, index, "").length() != 0;
}

// -----------------------------------------------------------------------------
// MODULE CORE
// -----------------------------------------------------------------------------

void settingsSetup() {
    EEPROMr.begin(SPI_FLASH_SEC_SIZE);

    Embedis::dictionary(F("EEPROM"),
        SPI_FLASH_SEC_SIZE,
        [](size_t pos) -> char { return EEPROMr.read(pos); },
        [](size_t pos, char value) { EEPROMr.write(pos, value); },
        #if SETTINGS_AUTOSAVE
            []() { _settings_save = true; }
        #else
            []() {}
        #endif
    );

    #if WEB_SUPPORT
        webEventsRegister([](AsyncWebServer * server) {
            webServer()->on(WEB_API_FIRMWARE_CONFIGURATION, HTTP_GET, _settingsOnGetConfig);
            webServer()->on(WEB_API_FIRMWARE_CONFIGURATION, HTTP_POST, _settingsOnPostConfig, _settingsOnPostConfigData);
        });

        #if WS_SUPPORT
            wsOnActionRegister(_settingsWSOnAction);
        #endif
    #endif

    #if BUTTON_SUPPORT && SETTINGS_FACTORY_BTN > 0
        buttonOnEventRegister(
            [](uint8_t event) {
                if (event == SETTINGS_FACTORY_BTN_EVENT) {
                    DEBUG_MSG(PSTR("[SETTINGS] Requested factory reset action\n"));
                    DEBUG_MSG(PSTR("\n\nFACTORY RESET\n\n"));

                    resetSettings();

                    #if WEB_SUPPORT && WS_SUPPORT
                        // Send notification to all clients
                        wsSend_P(PSTR("{\"doAction\": \"reload\", \"reason\": \"factory\"}"));
                    #endif

                    deferredReset(250, CUSTOM_FACTORY_BUTTON);
                }
            },
            (uint8_t) (SETTINGS_FACTORY_BTN - 1)
        );
    #endif

    #if BUTTON_SUPPORT && SYSTEM_RESET_BTN > 0
        buttonOnEventRegister(
            [](uint8_t event) {
                if (event == SYSTEM_RESET_BTN_EVENT) {
                    DEBUG_MSG(PSTR("[SETTINGS] Requested reset action\n"));

                    #if WEB_SUPPORT && WS_SUPPORT
                        // Send notification to all clients
                        wsSend_P(PSTR("{\"doAction\": \"reload\", \"reason\": \"reset\"}"));
                    #endif

                    deferredReset(250, CUSTOM_RESET_BUTTON);
                }
            },
            (uint8_t) (SYSTEM_RESET_BTN - 1)
        );
    #endif

    #if FASTYBIRD_SUPPORT
        fastybirdOnControlRegister(
            [](const char * payload) {
                DEBUG_MSG(PSTR("[SETTINGS] Requested factory reset action\n"));
                DEBUG_MSG(PSTR("\n\nFACTORY RESET\n\n"));

                resetSettings();

                #if WEB_SUPPORT && WS_SUPPORT
                    // Send notification to all clients
                    wsSend_P(PSTR("{\"doAction\": \"reload\", \"reason\": \"factory\"}"));
                #endif

                deferredReset(250, CUSTOM_FACTORY_BROKER);
            },
            FASTYBIRD_THING_CONTROL_FACTORY_RESET
        );
    #endif

    // Register loop
    firmwareRegisterLoop(settingsLoop);
}

// -----------------------------------------------------------------------------

void settingsLoop() {
    if (_settings_save) {
        EEPROMr.commit();
        _settings_save = false;
    }
}
