/*

DEBUG MODULE

Copyright (C) 2018 FastyBird s.r.o. <code@fastybird.com>

*/

#if DEBUG_SUPPORT

// -----------------------------------------------------------------------------
// MODULE PRIVATE
// -----------------------------------------------------------------------------

void _debugSend(
    char * message
) {
    bool pause = false;

    #if DEBUG_ADD_TIMESTAMP
        char timestamp[10] = {0};

        snprintf_P(timestamp, sizeof(timestamp), PSTR("[%06lu] "), millis() % 1000000);
    #endif

    #if DEBUG_SERIAL_SUPPORT
        #if DEBUG_ADD_TIMESTAMP
            DEBUG_PORT.printf(timestamp);
        #endif

        DEBUG_PORT.printf(message);
    #endif

    #if DEBUG_WEB_SUPPORT && WEB_SUPPORT && WS_SUPPORT
        if (wsConnected() && (getFreeHeap() > 10000)) {
            String m = String(message);

            m.replace("\"", "&quot;");
            m.replace("{", "&#123");
            m.replace("}", "&#125");

            char buffer[m.length() + 24];

            #if DEBUG_ADD_TIMESTAMP
                snprintf_P(buffer, sizeof(buffer), PSTR("%s:%s"), timestamp, m.c_str());
            #else
                snprintf_P(buffer, sizeof(buffer), PSTR("%s"), m.c_str());
            #endif

            DynamicJsonBuffer jsonBuffer;
            JsonObject& message = jsonBuffer.createObject();

            message["module"] = "dbg";
            message["data"] = buffer;

            wsSend(message);

            pause = true;
        }
    #endif

    #if DEBUG_MQTT_SUPPORT
        if (mqttConnected() && (getFreeHeap() > 10000)) {
            // TODO: implement MQTT debug report to broker
        }
    #endif

    if (pause) {
        optimistic_yield(100);
    }
}

// -----------------------------------------------------------------------------
// MODULE API
// -----------------------------------------------------------------------------

void debugSend(
    PGM_P format_P,
    ...
) {
    char format[strlen_P(format_P)+1];

    memcpy_P(format, format_P, sizeof(format));

    va_list args;
    va_start(args, format_P);
        char test[1];
        int len = ets_vsnprintf(test, 1, format, args) + 1;
        char * buffer = new char[len];
        ets_vsnprintf(buffer, len, format, args);
    va_end(args);

    _debugSend(buffer);

    delete[] buffer;
}

// -----------------------------------------------------------------------------
// MODULE CORE
// -----------------------------------------------------------------------------

void debugSetup()
{
    #if DEBUG_SERIAL_SUPPORT
        DEBUG_PORT.begin(SERIAL_BAUDRATE);

        #if DEBUG_ESP_WIFI
            DEBUG_PORT.setDebugOutput(true);
        #endif
    #endif

    #if DEBUG_WEB_SUPPORT && WEB_SUPPORT && WS_SUPPORT
        wsOnConnectRegister([](JsonObject& root) {
            JsonArray& modules = root.containsKey("modules") ? root["modules"] : root.createNestedArray("modules");
            JsonObject& module = modules.createNestedObject();

            module["module"] = "dbg";
            module["visible"] = true;
        });
    #endif
}

#endif // DEBUG_SUPPORT
