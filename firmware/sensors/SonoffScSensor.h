/*

SONOFF SC SENSOR - Environment monitor sensor

Copyright (C) 2018 FastyBird s.r.o. <code@fastybird.com>

*/

#if SENSOR_SUPPORT && ITEAD_SONOFF_SC_SUPPORT

#pragma once

#include "base/BaseSensor.h"

class SonoffScSensor : public BaseSensor {

    public:

        // -----------------------------------------------------------------------------
        // SENSOR PUBLIC
        // -----------------------------------------------------------------------------

        SonoffScSensor(): BaseSensor() {
            _count = 5;
            _sensor_id = SENSOR_SONOFF_SC_ID;

            _appendMagnitude(MAGNITUDE_TEMPERATURE, 1, MAGNITUDE_UNIT_CELSIUS);
            _appendMagnitude(MAGNITUDE_HUMIDITY, 0, MAGNITUDE_UNIT_PERCENTAGE);
            _appendMagnitude(MAGNITUDE_AIR_QUALITY_LEVEL, 0, MAGNITUDE_UNIT_NONE);
            _appendMagnitude(MAGNITUDE_NOISE_LEVEL, 0, MAGNITUDE_UNIT_NONE);
            _appendMagnitude(MAGNITUDE_LIGHT_LEVEL, 0, MAGNITUDE_UNIT_NONE);
        }

        ~SonoffScSensor() {
        }

        // ---------------------------------------------------------------------
        // Sensor API
        // ---------------------------------------------------------------------

        // Initialization method, must be idempotent
        void begin() {
            _temperature = 0;
            _humidity = 0;
            _light_level = 0;
            _dust_level = 0;
            _noise_level = 0;
           
           // Sensor is ready to communicate
           _ready = true;

            // Init communication
            _send("AT+START");
        }

        // Descriptive name of the sensor
        String description() {
            return String("SONOFF SC @ SENSOR(ATMEGA)");
        }

        // Name of the sensor
        uint8_t type() {
            return SENSOR_TYPE_ENVIRONMENT;
        }

        // Current value for slot # index
        double _magnitudeCurrentValue(uint8_t index) {
            _error = SENSOR_ERROR_OK;

            if (index == 0) return _temperature;
            if (index == 1) return _humidity;
            if (index == 2) return _dust_level;
            if (index == 3) return _noise_level;
            if (index == 4) return _light_level;

            _error = SENSOR_ERROR_OUT_OF_RANGE;

            return 0;
        }

        // Loop-like method, call it in your main loop
        void tick() {
            _read();
        }

    protected:

        // ---------------------------------------------------------------------
        // COMMUNICATIONS
        // ---------------------------------------------------------------------

        void _send(const char *data) {
            SERIAL_PORT.write(data);
            SERIAL_PORT.write(0x1B);
            SERIAL_PORT.write("\n");
        }

        bool _read() {
            static String rec_string = "";
            int16_t index1;

            while (SERIAL_PORT.available() > 0)
            {
                char a = (char) SERIAL_PORT.read();

                rec_string += a;

                // Search for end char
                if (a == 0x1B) {
                    break;
                }
            }

            if (rec_string.indexOf(0x1B) !=-1) {
                if (rec_string.indexOf("AT+UPDATE=") != -1) {
                    // Strip message end char
                    rec_string = rec_string.substring(0, rec_string.length() - 1);
                    // Convert to JSON
                    rec_string = "{" + rec_string.substring(10) + "}";

                    char rec_chars[rec_string.length() + 1];
                    rec_string.toCharArray(rec_chars, rec_string.length() + 1);

                    DynamicJsonBuffer json_buffer;

                    JsonObject& root = json_buffer.parseObject((char *) rec_chars);

                    if (root.success()) {
                        if (root.containsKey("temperature")) {
                            _temperature = atof(root["temperature"].as<char*>());
                        }

                        if (root.containsKey("humidity")) {
                            _humidity = atof(root["humidity"].as<char*>());
                        }

                        if (root.containsKey("light")) {
                            _light_level = atof(root["light"].as<char*>());
                        }

                        if (root.containsKey("noise")) {
                            _noise_level = atof(root["noise"].as<char*>());
                        }

                        if (root.containsKey("dusty")) {
                            _dust_level = atof(root["dusty"].as<char*>());
                        }

                        _send("AT+SEND=ok");

                    } else {
                        _send("AT+SEND=fail");
                    }

                } else if (rec_string.indexOf("AT+STATUS?") != -1) {
                    _send("AT+STATUS=4");

                } else {
                    _send("AT+SEND=unknown");
                }

                rec_string = "";

            } else if (rec_string.indexOf("AT") != -1) {
                SERIAL_PORT.flush();

            } else {
                // Do nothing
            }
        }

        // ---------------------------------------------------------------------

        float _temperature;
        int _humidity;
        int _light_level;
        float _dust_level;
        int _noise_level;
};

#endif // SENSOR_SUPPORT && ITEAD_SONOFF_SC_SUPPORT
