/*

FASTYBIRD & RELAY MODULE

Copyright (C) 2022 FastyBird s.r.o. <code@fastybird.com>

*/

#if FASTYBIRD_SUPPORT && RELAY_PROVIDER != RELAY_PROVIDER_NONE

// -----------------------------------------------------------------------------
// MODULE PRIVATE
// -----------------------------------------------------------------------------

uint8_t _relayFastyBirdRelayIndexToChannelIndex(
    const uint8_t relayIndex
) {
    if (relayIndex == 0) {
        return FASTYBIRD_RELAY1_CHANNEL_INDEX;

    } else if (relayIndex == 1) {
        return FASTYBIRD_RELAY2_CHANNEL_INDEX;

    } else if (relayIndex == 2) {
        return FASTYBIRD_RELAY3_CHANNEL_INDEX;

    } else if (relayIndex == 3) {
        return FASTYBIRD_RELAY4_CHANNEL_INDEX;

    } else if (relayIndex == 4) {
        return FASTYBIRD_RELAY5_CHANNEL_INDEX;

    } else if (relayIndex == 5) {
        return FASTYBIRD_RELAY6_CHANNEL_INDEX;

    } else if (relayIndex == 6) {
        return FASTYBIRD_RELAY7_CHANNEL_INDEX;

    } else if (relayIndex == 7) {
        return FASTYBIRD_RELAY8_CHANNEL_INDEX;
    }

    return INDEX_NONE;
}

// -----------------------------------------------------------------------------

uint8_t _relayFastyBirdChannelIndexToRelayIndex(
    const uint8_t channelIndex
) {
    if (channelIndex == FASTYBIRD_RELAY1_CHANNEL_INDEX) {
        return 0;
    } else if (channelIndex == FASTYBIRD_RELAY2_CHANNEL_INDEX) {
        return 1;
    } else if (channelIndex == FASTYBIRD_RELAY3_CHANNEL_INDEX) {
        return 2;
    } else if (channelIndex == FASTYBIRD_RELAY4_CHANNEL_INDEX) {
        return 3;
    } else if (channelIndex == FASTYBIRD_RELAY5_CHANNEL_INDEX) {
        return 4;
    } else if (channelIndex == FASTYBIRD_RELAY6_CHANNEL_INDEX) {
        return 5;
    } else if (channelIndex == FASTYBIRD_RELAY7_CHANNEL_INDEX) {
        return 6;
    } else if (channelIndex == FASTYBIRD_RELAY8_CHANNEL_INDEX) {
        return 7;
    }

    return INDEX_NONE;
}

// -----------------------------------------------------------------------------

void _relayFastyBirdProperySet(
    const uint8_t channelIndex,
    const uint8_t propertyIndex,
    const char * payload
) {
    uint8_t relay_index = _relayFastyBirdChannelIndexToRelayIndex(channelIndex);

    if (relay_index != INDEX_NONE) {
        // Toggle relay status
        if (strcmp(payload, FASTYBIRD_SWITCH_PAYLOAD_TOGGLE) == 0) {
            relayToggle(relay_index);

        // Set relay status
        } else {
            relayStatus(relay_index, strcmp(payload, FASTYBIRD_SWITCH_PAYLOAD_ON) == 0);
        }
    }
}

// -----------------------------------------------------------------------------

void _relayFastyBirdProperyQuery(
    const uint8_t channelIndex,
    const uint8_t propertyIndex
) {
    uint8_t relay_index = _relayFastyBirdChannelIndexToRelayIndex(channelIndex);

    if (relay_index != INDEX_NONE) {
        fastybirdReportChannelPropertyValue(
            FASTYBIRD_MAIN_DEVICE_INDEX,
            channelIndex,
            fastybirdFindChannelPropertyIndex(
                FASTYBIRD_MAIN_DEVICE_INDEX,
                channelIndex,
                FASTYBIRD_PROPERTY_SWITCH
            ),
            relayStatus(relay_index) ? FASTYBIRD_SWITCH_PAYLOAD_ON : FASTYBIRD_SWITCH_PAYLOAD_OFF
        );
    }
}

// -----------------------------------------------------------------------------

void _fastyBirdRelayRegisterToChannel(
    const uint8_t relayIndex,
    const uint8_t channelIndex
) {
    char format[36];

    strcpy(format, FASTYBIRD_SWITCH_PAYLOAD_ON);
    strcat(format, ",");
    strcat(format, FASTYBIRD_SWITCH_PAYLOAD_OFF);
    strcat(format, ",");
    strcat(format, FASTYBIRD_SWITCH_PAYLOAD_TOGGLE);

    // Create relay property structure
    uint8_t property_index = fastybirdRegisterProperty(
        FASTYBIRD_PROPERTY_SWITCH,
        FASTYBIRD_PROPERTY_DATA_TYPE_ENUM,
        "",
        format,
        _relayFastyBirdProperySet,
        _relayFastyBirdProperyQuery
    );

    // Register property to channel
    fastybirdMapPropertyToChannel(
        FASTYBIRD_MAIN_DEVICE_INDEX,
        channelIndex,
        property_index
    );

    relayOnEventRegister(
        [channelIndex, property_index](uint8_t event) {
        },
        relayIndex
    );

    relayOnEventRegister(
        [channelIndex, property_index](bool state) {
            fastybirdReportChannelPropertyValue(
                FASTYBIRD_MAIN_DEVICE_INDEX,
                channelIndex,
                fastybirdFindChannelPropertyIndex(
                    FASTYBIRD_MAIN_DEVICE_INDEX,
                    channelIndex,
                    FASTYBIRD_PROPERTY_SWITCH
                ),
                state ? FASTYBIRD_SWITCH_PAYLOAD_ON : FASTYBIRD_SWITCH_PAYLOAD_OFF
            );
        },
        relayIndex
    );
}

// -----------------------------------------------------------------------------
// MODULE CORE
// -----------------------------------------------------------------------------

void fastyBirdRelaySetup()
{
    #if FASTYBIRD_RELAY1_CHANNEL_INDEX != INDEX_NONE
        _fastyBirdRelayRegisterToChannel(0, FASTYBIRD_RELAY1_CHANNEL_INDEX);
    #endif

    #if FASTYBIRD_RELAY2_CHANNEL_INDEX != INDEX_NONE
        _fastyBirdRelayRegisterToChannel(1, FASTYBIRD_RELAY2_CHANNEL_INDEX);
    #endif

    #if FASTYBIRD_RELAY2_CHANNEL_INDEX != INDEX_NONE
        _fastyBirdRelayRegisterToChannel(2, FASTYBIRD_RELAY3_CHANNEL_INDEX);
    #endif

    #if FASTYBIRD_RELAY4_CHANNEL_INDEX != INDEX_NONE
        _fastyBirdRelayRegisterToChannel(3, FASTYBIRD_RELAY4_CHANNEL_INDEX);
    #endif

    #if FASTYBIRD_RELAY5_CHANNEL_INDEX != INDEX_NONE
        _fastyBirdRelayRegisterToChannel(4, FASTYBIRD_RELAY5_CHANNEL_INDEX);
    #endif

    #if FASTYBIRD_RELAY6_CHANNEL_INDEX != INDEX_NONE
        _fastyBirdRelayRegisterToChannel(5, FASTYBIRD_RELAY6_CHANNEL_INDEX);
    #endif

    #if FASTYBIRD_RELAY7_CHANNEL_INDEX != INDEX_NONE
        _fastyBirdRelayRegisterToChannel(6, FASTYBIRD_RELAY7_CHANNEL_INDEX);
    #endif

    #if FASTYBIRD_RELAY8_CHANNEL_INDEX != INDEX_NONE
        _fastyBirdRelayRegisterToChannel(7, FASTYBIRD_RELAY8_CHANNEL_INDEX);
    #endif

    fastybirdOnConnectRegister([](){
        for (uint8_t i = 0; i < relayCount(); i++) {
            uint8_t channel_index = _relayFastyBirdRelayIndexToChannelIndex(i);

            if (channel_index != INDEX_NONE) {
                fastybirdReportChannelPropertyValue(
                    FASTYBIRD_MAIN_DEVICE_INDEX,
                    channel_index,
                    fastybirdFindChannelPropertyIndex(
                        FASTYBIRD_MAIN_DEVICE_INDEX,
                        channel_index,
                        FASTYBIRD_PROPERTY_SWITCH
                    ),
                    relayStatus(i) ? FASTYBIRD_SWITCH_PAYLOAD_ON : FASTYBIRD_SWITCH_PAYLOAD_OFF
                );
            }
        }
    });
}

#endif // FASTYBIRD_SUPPORT && RELAY_PROVIDER != RELAY_PROVIDER_NONE
