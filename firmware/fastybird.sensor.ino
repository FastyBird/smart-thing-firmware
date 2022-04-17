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

    for (uint8_t k = 0; k < sensorMagnitudesCount(sensor_index); k++) {
        if (strcmp(sensorMagnitudeName(sensor_index, k).c_str(), property.name) == 0) {
            char buffer[10];

            dtostrf(sensorMagnitudeValue(sensor_index, k), 1 - sizeof(buffer), sensorMagnitudeDecimals(sensor_index, k), buffer);

            fastybirdReportChannelPropertyValue(
                FASTYBIRD_MAIN_DEVICE_INDEX,
                channelIndex,
                fastybirdFindChannelPropertyIndex(
                    FASTYBIRD_MAIN_DEVICE_INDEX,
                    channelIndex,
                    sensorMagnitudeName(sensor_index, k).c_str()
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
    for (uint8_t i = 0; i < sensorMagnitudesCount(sensorIndex); i++) {
        // Create magnitude property structure
        uint8_t property_index = fastybirdRegisterProperty(
            sensorMagnitudeName(sensorIndex, i).c_str(),
            FASTYBIRD_PROPERTY_DATA_TYPE_FLOAT,
            sensorMagnitudeUnit(sensorIndex, i).c_str(),
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
                for (uint8_t k = 0; k < sensorMagnitudesCount(i); k++) {
                    char buffer[10];

                    dtostrf(sensorMagnitudeValue(i, k), 1 - sizeof(buffer), sensorMagnitudeDecimals(i, k), buffer);

                    fastybirdReportChannelPropertyValue(
                        FASTYBIRD_MAIN_DEVICE_INDEX,
                        channel_index,
                        fastybirdFindChannelPropertyIndex(
                            FASTYBIRD_MAIN_DEVICE_INDEX,
                            channel_index,
                            sensorMagnitudeName(i, k).c_str()
                        ),
                        buffer
                    );
                }
            }
        }
    });
}

#endif // FASTYBIRD_SUPPORT && SENSOR_SUPPORT