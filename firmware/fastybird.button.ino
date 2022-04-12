/*

FASTYBIRD & BUTTON MODULE

Copyright (C) 2022 FastyBird s.r.o. <code@fastybird.com>

*/

#if FASTYBIRD_SUPPORT && BUTTON_SUPPORT

// -----------------------------------------------------------------------------
// MODULE PRIVATE
// -----------------------------------------------------------------------------

void _fastyBirdButtonRegisterToChannel(
    const uint8_t buttonIndex,
    const uint8_t channelIndex
) {
    char format[110];

    strcpy(format, FASTYBIRD_BTN_PAYLOAD_PRESS);
    strcat(format, ",");
    strcat(format, FASTYBIRD_BTN_PAYLOAD_CLICK);
    strcat(format, ",");
    strcat(format, FASTYBIRD_BTN_PAYLOAD_DBL_CLICK);
    strcat(format, ",");
    strcat(format, FASTYBIRD_BTN_PAYLOAD_TRIPLE_CLICK);
    strcat(format, ",");
    strcat(format, FASTYBIRD_BTN_PAYLOAD_LNG_CLICK);
    strcat(format, ",");
    strcat(format, FASTYBIRD_BTN_PAYLOAD_LNG_LNG_CLICK);

    // Create button property structure
    uint8_t propertyIndex = fastybirdRegisterProperty(
        FASTYBIRD_PROPERTY_BUTTON,
        FASTYBIRD_PROPERTY_DATA_TYPE_ENUM,
        "",
        format
    );

    // Register property to channel
    fastybirdMapPropertyToChannel(
        FASTYBIRD_MAIN_DEVICE_INDEX,
        channelIndex,
        propertyIndex
    );

    buttonOnEventRegister(
        [channelIndex, propertyIndex](uint8_t event) {
            switch (event) {
                case BUTTON_EVENT_PRESSED:
                    fastybirdReportChannelPropertyValue(
                        FASTYBIRD_MAIN_DEVICE_INDEX,
                        channelIndex,
                        propertyIndex,
                        FASTYBIRD_BTN_PAYLOAD_PRESS
                    );
                    break;

                case BUTTON_EVENT_CLICK:
                    fastybirdReportChannelPropertyValue(
                        FASTYBIRD_MAIN_DEVICE_INDEX,
                        channelIndex,
                        propertyIndex,
                        FASTYBIRD_BTN_PAYLOAD_CLICK
                    );
                    break;

                case BUTTON_EVENT_DBL_CLICK:
                    fastybirdReportChannelPropertyValue(
                        FASTYBIRD_MAIN_DEVICE_INDEX,
                        channelIndex,
                        propertyIndex,
                        FASTYBIRD_BTN_PAYLOAD_DBL_CLICK
                    );
                    break;

                case BUTTON_EVENT_TRIPLE_CLICK:
                    fastybirdReportChannelPropertyValue(
                        FASTYBIRD_MAIN_DEVICE_INDEX,
                        channelIndex,
                        propertyIndex,
                        FASTYBIRD_BTN_PAYLOAD_TRIPLE_CLICK
                    );
                    break;

                case BUTTON_EVENT_LNG_CLICK:
                    fastybirdReportChannelPropertyValue(
                        FASTYBIRD_MAIN_DEVICE_INDEX,
                        channelIndex,
                        propertyIndex,
                        FASTYBIRD_BTN_PAYLOAD_LNG_CLICK
                    );
                    break;

                case BUTTON_EVENT_LNG_LNG_CLICK:
                    fastybirdReportChannelPropertyValue(
                        FASTYBIRD_MAIN_DEVICE_INDEX,
                        channelIndex,
                        propertyIndex,
                        FASTYBIRD_BTN_PAYLOAD_LNG_LNG_CLICK
                    );
                    break;

                default:
                    break;
            }
        },
        buttonIndex
    );
}

// -----------------------------------------------------------------------------
// MODULE CORE
// -----------------------------------------------------------------------------

void fastyBirdButtonSetup()
{
    #if FASTYBIRD_BUTTON1_CHANNEL_INDEX != INDEX_NONE
        _fastyBirdButtonRegisterToChannel(0, FASTYBIRD_BUTTON1_CHANNEL_INDEX);
    #endif

    #if FASTYBIRD_BUTTON2_CHANNEL_INDEX != INDEX_NONE
        _fastyBirdButtonRegisterToChannel(1, FASTYBIRD_BUTTON2_CHANNEL_INDEX);
    #endif

    #if FASTYBIRD_BUTTON3_CHANNEL_INDEX != INDEX_NONE
        _fastyBirdButtonRegisterToChannel(2, FASTYBIRD_BUTTON3_CHANNEL_INDEX);
    #endif

    #if FASTYBIRD_BUTTON4_CHANNEL_INDEX != INDEX_NONE
        _fastyBirdButtonRegisterToChannel(3, FASTYBIRD_BUTTON4_CHANNEL_INDEX);
    #endif

    #if FASTYBIRD_BUTTON5_CHANNEL_INDEX != INDEX_NONE
        _fastyBirdButtonRegisterToChannel(4, FASTYBIRD_BUTTON5_CHANNEL_INDEX);
    #endif

    #if FASTYBIRD_BUTTON6_CHANNEL_INDEX != INDEX_NONE
        _fastyBirdButtonRegisterToChannel(5, FASTYBIRD_BUTTON6_CHANNEL_INDEX);
    #endif

    #if FASTYBIRD_BUTTON7_CHANNEL_INDEX != INDEX_NONE
        _fastyBirdButtonRegisterToChannel(6, FASTYBIRD_BUTTON7_CHANNEL_INDEX);
    #endif

    #if FASTYBIRD_BUTTON8_CHANNEL_INDEX != INDEX_NONE
        _fastyBirdButtonRegisterToChannel(7, FASTYBIRD_BUTTON8_CHANNEL_INDEX);
    #endif
}

#endif // FASTYBIRD_SUPPORT && BUTTON_SUPPORT
