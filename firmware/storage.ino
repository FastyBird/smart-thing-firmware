/*

MEMORY STORAGE MODULE

Copyright (C) 2018 FastyBird Ltd. <info@fastybird.com>

*/

#if SPIFFS_SUPPORT

#include <FS.h>

// -----------------------------------------------------------------------------
// MODULE API
// -----------------------------------------------------------------------------

String storageReadConfiguration(
    const char * filename
) {
    if (SPIFFS.exists(filename)) {
        File file = SPIFFS.open(filename, "r");

        if (!file) {
            DEBUG_MSG(PSTR("[STORAGE] Open file: %s for reading configuration failed\n"), filename);

            return String("");
        }
        
        if (!file.size()) {
            DEBUG_MSG(PSTR("[STORAGE] Filesize of file: %s is invalid\n"), filename);

            return String("");
        }
        
        String configuration;

        while (file.available())
        {
            configuration += char(file.read());
        }

        file.close();

        return configuration;

    } else {
        DEBUG_MSG(PSTR("[STORAGE] File: %s for reading configuration does not exists\n"), filename);
    }

    return String("");
}

// -----------------------------------------------------------------------------

bool storageWriteConfiguration(
    const char * filename,
    String configuration
) {
    File file = SPIFFS.open(filename, "w");

    if (!file) {
        DEBUG_MSG(PSTR("[STORAGE] Open file: %s for storing configuration failed\n"), filename);

        return false;
    }

    DEBUG_MSG(PSTR("[STORAGE] WRITING: %s\n"), configuration.c_str());

    file.println(configuration.c_str());

    file.close();

    DEBUG_MSG(PSTR("[STORAGE] Stored configuration to file: %s\n"), filename);

    return true;
}

// -----------------------------------------------------------------------------
// MODULE CORE
// -----------------------------------------------------------------------------

void storageSetup() {
    SPIFFS.begin();
}

#endif // SPIFFS_SUPPORT