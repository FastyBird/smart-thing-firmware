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

        if (file && file.size()) {
            String configuration;

            while (file.available())
            {
                configuration += char(file.read());
            }

            file.close();
        }
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
        DEBUG_MSG(PSTR("[STORAGE] Open file for storing configuration failed\n"));

        return false;
    }

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