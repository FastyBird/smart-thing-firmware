/*

VIRTUAL BUTTON MODULE

Copyright (C) 2018 FastyBird s.r.o. <code@fastybird.com>

*/

#if VIRTUAL_BTN_SUPPORT

// -----------------------------------------------------------------------------
// MODULE PRIVATE
// -----------------------------------------------------------------------------

void _virtualButtonLoop()
{
    uint8_t counter = getSetting("virtual_btn_counter", 0).toInt();

    if (counter == VIRTUAL_BTN_COUNT && millis() > VIRTUAL_BTN_WAIT_INTERVAL) {
        DEBUG_MSG(PSTR("[INFO][VIRTUAL_BUTTON] Requested factory reset action\n"));
        DEBUG_MSG(PSTR("\n\nFACTORY RESET\n\n"));

        resetSettings();

        #if WEB_SUPPORT && WS_SUPPORT
            // Send notification to all clients
            wsSend_P(PSTR("{\"doAction\": \"reload\", \"reason\": \"factory\"}"));
        #endif

        deferredReset(250, CUSTOM_FACTORY_BUTTON);
    }

    // Virtual button timeout
    if (millis() > VIRTUAL_BTN_CLEAR_INTERVAL && counter > 0) {
        // Clear counter
        setSetting("virtual_btn_counter", 0);
    }
}

// -----------------------------------------------------------------------------
// MODULE API
// -----------------------------------------------------------------------------

void virtualButtonSetup()
{
    uint8_t counter = getSetting("virtual_btn_counter", 0).toInt();

    setSetting("virtual_btn_counter", counter + 1);

    // Register loop
    firmwareRegisterLoop(_virtualButtonLoop);
}

#endif // VIRTUAL_BTN_SUPPORT
