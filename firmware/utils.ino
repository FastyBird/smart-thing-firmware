/*

UTILS MODULE

Copyright (C) 2018 FastyBird s.r.o. <code@fastybird.com>

*/

String getIdentifier()
{
    char buffer[20];

    snprintf_P(buffer, sizeof(buffer), PSTR("FB_%08X"), ESP.getChipId());

    return String(buffer);
}

// -----------------------------------------------------------------------------

String getCoreVersion()
{
    String version = ESP.getCoreVersion();

    #ifdef ARDUINO_ESP8266_RELEASE
        if (version.equals("00000000")) {
            version = String(ARDUINO_ESP8266_RELEASE);
        }
    #endif

    version.replace("_", ".");

    return version;
}

// -----------------------------------------------------------------------------

String getCoreRevision()
{
    #ifdef ARDUINO_ESP8266_GIT_VER
        return String(ARDUINO_ESP8266_GIT_VER);
    #else
        return String("");
    #endif
}

// -----------------------------------------------------------------------------

// WTF
// Calling ESP.getFreeHeap() is making the system crash on a specific
// AiLight bulb, but anywhere else...
uint16_t getFreeHeap()
{
    if (getSetting("wtfHeap", 0).toInt() == 1) {
        return (uint16_t) 9999;
    }

    return (uint16_t) ESP.getFreeHeap();
}

// -----------------------------------------------------------------------------

uint16_t getInitialFreeHeap()
{
    static uint16_t heap = 0;

    if (heap == 0) {
        heap = getFreeHeap();
    }

    return heap;
}

// -----------------------------------------------------------------------------

uint16_t getUsedHeap()
{
    return getInitialFreeHeap() - getFreeHeap();
}

// -----------------------------------------------------------------------------

String getFirmwareModules()
{
    return FPSTR(firmware_modules);
}

// -----------------------------------------------------------------------------

String getFirmwareSensors()
{
    #if SENSOR_SUPPORT
        return FPSTR(firmware_sensors);
    #else
        return String();
    #endif
}

// -----------------------------------------------------------------------------

String getBuildTime()
{
    const char time_now[] = __TIME__;   // hh:mm:ss
    uint16_t hour = atoi(&time_now[0]);
    uint16_t minute = atoi(&time_now[3]);
    uint16_t second = atoi(&time_now[6]);

    const char date_now[] = __DATE__;   // Mmm dd yyyy
    const char * monts[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

    uint16_t month = 0;

    for (int i = 0; i < 12; i++) {
        if (strncmp(date_now, monts[i], 3) == 0 ) {
            month = i + 1;
            break;
        }
    }

    uint16_t day = atoi(&date_now[3]);
    uint16_t year = atoi(&date_now[7]);

    char buffer[20];

    snprintf_P(
        buffer, sizeof(buffer), PSTR("%04d-%02d-%02d %02d:%02d:%02d"),
        year, month, day, hour, minute, second
    );

    return String(buffer);
}

// -----------------------------------------------------------------------------

uint32_t getUptime()
{
    static uint32_t last_uptime = 0;
    static uint32_t uptime_overflows = 0;

    if (millis() < last_uptime) {
        ++uptime_overflows;
    }

    last_uptime = millis();

    uint32_t uptime_seconds = uptime_overflows * (UPTIME_OVERFLOW / 1000) + (last_uptime / 1000);

    return uptime_seconds;
}

// -----------------------------------------------------------------------------
// SSL
// -----------------------------------------------------------------------------

#if NETWORK_SSL_ENABLED
    bool sslCheckFingerPrint(const char * fingerprint) {
        return (strlen(fingerprint) == 59);
    }

// -----------------------------------------------------------------------------

    bool sslFingerPrintArray(const char * fingerprint, unsigned int * bytearray) {
        // check length (20 2-character digits ':' or ' ' separated => 20*2+19 = 59)
        if (!sslCheckFingerPrint(fingerprint)) {
            return false;
        }

        // walk the fingerprint
        for (uint8_t i = 0; i < 20; i++) {
            bytearray[i] = strtol(fingerprint + 3*i, NULL, 16);
        }

        return true;
    }

// -----------------------------------------------------------------------------

    bool sslFingerPrintChar(const char * fingerprint, char * destination) {
        // check length (20 2-character digits ':' or ' ' separated => 20*2+19 = 59)
        if (!sslCheckFingerPrint(fingerprint)) {
            return false;
        }

        // copy it
        strncpy(destination, fingerprint, 59);

        // walk the fingerprint replacing ':' for ' '
        for (uint8_t i = 0; i < 59; i++) {
            if (destination[i] == ':') {
                destination[i] = ' ';
            }
        }

        return true;
    }
#endif // NETWORK_SSL_ENABLED

// -----------------------------------------------------------------------------

char * ltrim(char * s) {
    char *p = s;

    while ((unsigned int) *p == ' ') {
        ++p;
    }

    return p;
}

// -----------------------------------------------------------------------------

double roundTo(
    double num,
    unsigned int positions
) {
    double multiplier = 1;

    while (positions-- > 0) {
        multiplier *= 10;
    }

    return round(num * multiplier) / multiplier;
}

// -----------------------------------------------------------------------------

void niceDelay(uint32_t ms) {
    uint32_t start = millis();
    while (millis() - start < ms) delay(1);
}

// -----------------------------------------------------------------------------

bool isNumber(const char * s) {
    unsigned int len = strlen(s);

    if (0 == len) {
        return false;
    }

    bool decimal = false;

    for (unsigned int i = 0; i<len; i++) {
        if (s[i] == '-') {
            if (i>0) {
                return false;
            }

        } else if (s[i] == '.') {
            if (decimal) {
                return false;
            }

            decimal = true;

        } else if (!isdigit(s[i])) {
            return false;
        }
    }

    return true;
}

// -----------------------------------------------------------------------------

// This method is called by the SDK to know where to connect the ADC
int __get_adc_mode()
{
    return (int) (ADC_MODE_VALUE);
}
