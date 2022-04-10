/*

CONFIGURATION IMPORT CONTAINER

Copyright (C) 2018 FastyBird s.r.o. <code@fastybird.com>

*/

//#define DEBUG_SUPPORT 1
//#define DEBUG_SERIAL_SUPPORT 1

#include "01.version.h"
#include "02.types.h"
#include "03.arduino.h"
#include "04.hardware.h"
#include "05.defaults.h"
#include "06.general.h"
#include "08.prototypes.h"

// Core modules
#include "20.debug.h"
#include "21.system.h"
#include "22.settings.h"

// Basic modules
#include "30.wifi.h"
#include "31.web.h"
#include "32.ws.h"
#include "33.mqtt.h"

// Custom modules
#include "40.button.h"
#include "41.led.h"
#include "42.fastybird.h"
#include "43.relay.h"
#include "44.sensors.h"
#include "45.virtualbtn.h"

#include "50.progmem.h"
