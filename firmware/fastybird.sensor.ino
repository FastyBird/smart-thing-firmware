/*

FASTYBIRD & SENSOR MODULE

Copyright (C) 2022 FastyBird s.r.o. <code@fastybird.com>

*/

#if FASTYBIRD_SUPPORT && SENSOR_SUPPORT

// -----------------------------------------------------------------------------
// MODULE PRIVATE
// -----------------------------------------------------------------------------

uint8_t _sensorFastyBirdSensorIndexToChannelIndex(
    const uint8_t sensorIndex
) {
    if (sensorIndex == 0) {
        return FASTYBIRD_SENSOR1_CHANNEL_INDEX;
    }

    return INDEX_NONE;
}

// -----------------------------------------------------------------------------

uint8_t _sensorFastyBirdChannelIndexToSensorIndex(
    const uint8_t channelIndex
) {
    if (channelIndex == FASTYBIRD_SENSOR1_CHANNEL_INDEX) {
        return 0;
    }

    return INDEX_NONE;
}

// -----------------------------------------------------------------------------

void _sensorFastyBirdProperyQuery(
    const uint8_t channelIndex,
    const uint8_t propertyIndex
) {
    fastybird_property_t property = fastybirdGetProperty(propertyIndex);

    uint8_t sensor_index = _sensorFastyBirdChannelIndexToSensorIndex(channelIndex);

    BaseSensor * sensor = sensorSensor(sensor_index);

    for (uint8_t k = 0; k < sensor->magnitudesCount(); k++) {
        if (strcmp(sensor->magnitudeName(k).c_str(), property.name) == 0) {
            char buffer[10];

            dtostrf(sensor->magnitudeProcess(k), 1 - sizeof(buffer), sensor->magnitudeDecimals(k), buffer);

            fastybirdReportChannelPropertyValue(
                FASTYBIRD_MAIN_DEVICE_INDEX,
                channel_index,
                fastybirdFindChannelPropertyIndex(
                    FASTYBIRD_MAIN_DEVICE_INDEX,
                    channel_index,
                    sensor->magnitudeName(k).c_char()
                ),
                buffer
            );
        }
    }
}

// -----------------------------------------------------------------------------

void _fastyBirdSensorRegisterToChannel(
    const uint8_t sensorIndex,
    const uint8_t channelIndex
) {
    BaseSensor * sensor = sensorSensor(sensorIndex);
s
    for (uint8_t i = 0; i < sensor->magnitudesCount(); i++) {
        // Create magnitude property structure
        uint8_t property_index = fastybirdRegisterProperty(
            sensor->magnitudeName(i).c_str(),
            FASTYBIRD_PROPERTY_DATA_TYPE_FLOAT,
            sensor->magnitudeUnits(i).c_str(),
            "",
            _sensorFastyBirdProperyQuery
        );

        // Register property to channel
        fastybirdMapPropertyToChannel(
            FASTYBIRD_MAIN_DEVICE_INDEX,
            channelIndex,
            property_index
        );
    }
}

// -----------------------------------------------------------------------------
// MODULE CORE
// -----------------------------------------------------------------------------

void fastyBirdSensorSetup()
{
    #if FASTYBIRD_SENSOR1_CHANNEL_INDEX != INDEX_NONE
        _fastyBirdSensorRegisterToChannel(0, FASTYBIRD_SENSOR1_CHANNEL_INDEX);
    #endif

    fastybirdOnConnectRegister([](){
        for (uint8_t i = 0; i < sensorCount(); i++) {
            uint8_t channel_index = _sensorFastyBirdSensorIndexToChannelIndex(i);

            if (channel_index != INDEX_NONE) {
                BaseSensor * sensor = sensorSensor(sensorIndex);

                for (uint8_t k = 0; k < sensor->magnitudesCount(); k++) {
                    char buffer[10];

                    dtostrf(sensor->magnitudeProcess(k), 1 - sizeof(buffer), sensor->magnitudeDecimals(k), buffer);

                    fastybirdReportChannelPropertyValue(
                        FASTYBIRD_MAIN_DEVICE_INDEX,
                        channel_index,
                        fastybirdFindChannelPropertyIndex(
                            FASTYBIRD_MAIN_DEVICE_INDEX,
                            channel_index,
                            sensor->magnitudeName(k).c_char()
                        ),
                        buffer
                    );
                }
            }
        }
    });
}

#endif // FASTYBIRD_SUPPORT && SENSOR_SUPPORT