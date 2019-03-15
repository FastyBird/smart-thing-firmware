/*

SETTINGS MODULE

Copyright (C) 2018 FastyBird Ltd. <info@fastybird.com>

*/

#include <EEPROM_Rotate.h>
#include <vector>

#include "libs/EmbedisWrap.h"
#include "libs/StreamInjector.h"

StreamInjector _serial = StreamInjector(TERMINAL_BUFFER_SIZE);
EmbedisWrap embedis(_serial, TERMINAL_BUFFER_SIZE);

bool _settings_save = false;

// -----------------------------------------------------------------------------
// Reverse engineering EEPROM storage format
// -----------------------------------------------------------------------------

uint32_t settingsSize() {
    uint8_t pos = SPI_FLASH_SEC_SIZE - 1;

    while (size_t len = EEPROMr.read(pos)) {
        if (0xFF == len) {
            break;
        }

        pos = pos - len - 2;
    }

    return SPI_FLASH_SEC_SIZE - pos + EEPROM_DATA_END;
}

// -----------------------------------------------------------------------------

uint8_t settingsKeyCount() {
    uint8_t count = 0;
    uint8_t pos = SPI_FLASH_SEC_SIZE - 1;

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
    const uint8_t index
) {
    String s;

    uint8_t count = 0;
    uint8_t pos = SPI_FLASH_SEC_SIZE - 1;

    while (size_t len = EEPROMr.read(pos)) {
        if (0xFF == len) {
            break;
        }

        pos = pos - len - 2;

        if (count == index) {
            s.reserve(len);

            for (uint8_t i = 0 ; i < len; i++) {
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

    uint8_t size = settingsKeyCount();

    for (uint8_t i = 0; i < size; i++) {
        String _key = settingsKeyName(i);

        bool _inserted = false;

        for (uint8_t j = 0; j < keys.size(); j++) {
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
    const char* _thing = data["thing"];

    if (strcmp(_thing, THING) != 0) {
        return false;
    }

    const char* _version = data["version"];

    if (strcmp(_version, FIRMWARE_VERSION) != 0) {
        return false;
    }

    for (uint8_t i = EEPROM_DATA_END; i < SPI_FLASH_SEC_SIZE; i++) {
        EEPROMr.write(i, 0xFF);
    }

    for (auto element : data) {
        if (strcmp(element.key, "thing") == 0) {
            continue;
        }

        if (strcmp(element.key, "version") == 0) {
            continue;
        }

        setSetting(element.key, element.value.as<char*>());
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

        response->printf("{\n\"manufacturer\": \"%s\"", FIRMWARE_MANUFACTURER);
        response->printf(",\n\"version\": \"%s\"", FIRMWARE_VERSION);
        response->printf(",\n\"backup\": \"1\"");

        #if NTP_SUPPORT
            response->printf(",\n\"timestamp\": \"%s\"", ntpDateTime().c_str());
        #endif

        // Write the keys line by line (not sorted)
        uint32_t count = settingsKeyCount();

        for (uint8_t i = 0; i < count; i++) {
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

        bool settings_web_config_success = false;

        int params = request->params();

        for (uint8_t i = 0; i < params; i++) {
            AsyncWebParameter* p = request->getParam(i);
            
            if (p->isFile()) {
                DynamicJsonBuffer jsonBuffer;
                JsonObject& root = jsonBuffer.parseObject(p->value().c_str());

                if (root.success()) {
                    settings_web_config_success = _settingsRestoreJson(root);
                }
            }
        }

        request->send(settings_web_config_success ? 200 : 400);
    }

// -----------------------------------------------------------------------------

    bool _settingsWebRequestCallback(
        AsyncWebServerRequest * request
    ) {
        String url = request->url();

        if (url.equals("/control/config")) {
            if (request->method() == HTTP_GET) {
                _settingsOnGetConfig(request);

                return true;

            } else if (request->method() == HTTP_POST) {
                _settingsOnPostConfig(request);

                return true;
            }
        }

        return false;
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
                DEBUG_MSG(PSTR("\n\nFACTORY RESET\n\n"));

                resetSettings();

                deferredReset(100, CUSTOM_RESET_FACTORY);

                return;

            } else if (strcmp(action, "restore") == 0) {
                if (_settingsRestoreJson(data)) {
                    wsSend_P(clientId, PSTR("{\"message\": \"changes_saved_need_reboot\"}"));
                    wsSend_P(PSTR("{\"action\": \"reboot\"}"));

                } else {
                    wsSend_P(clientId, PSTR("{\"message\": \"invalid_file\", \"level\": \"error\"}"));
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
    for (uint8_t i = 0; i < SPI_FLASH_SEC_SIZE; i++) {
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
    const uint8_t index,
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
    const uint8_t index,
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
    const uint8_t index
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
    uint8_t index
) {
    return getSetting(key, index, "").length() != 0;
}

// -----------------------------------------------------------------------------
// MODULE CORE
// -----------------------------------------------------------------------------

void settingsSetup() {
    EEPROMr.begin(SPI_FLASH_SEC_SIZE);

    _serial.callback([](uint8_t ch) {
        #if DEBUG_SERIAL_SUPPORT
            DEBUG_PORT.write(ch);
        #endif
    });

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
        webOnRequestRegister(_settingsWebRequestCallback);

        #if WS_SUPPORT
            wsOnActionRegister(_settingsWSOnAction);
        #endif
    #endif

    #if BUTTON_SUPPORT && SETTINGS_FACTORY_BTN > 0
        buttonOnEventRegister(
            [](uint8_t event) {
                if (event == SETTINGS_FACTORY_BTN_EVENT) {
                    DEBUG_MSG(PSTR("\n\nFACTORY RESET\n\n"));

                    resetSettings();

                    deferredReset(100, CUSTOM_RESET_FACTORY);
                }
            },
            (uint8_t) (SETTINGS_FACTORY_BTN - 1)
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
