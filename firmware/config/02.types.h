/*

CONSTANTS DEFINITION

Copyright (C) 2018 FastyBird s.r.o. <code@fastybird.com>

*/

#pragma once

// =============================================================================
// RESET
// =============================================================================

#define CUSTOM_RESET_WEB                        1       // Reset from web interface
#define CUSTOM_RESET_BROKER                     2       // Reset via broker
#define CUSTOM_RESET_BUTTON                     3       // Reset via hardware button

#define CUSTOM_FACTORY_WEB                      4       // Reset after factory reset from web interface
#define CUSTOM_FACTORY_BROKER                   5       // Reset after factory reset via broker
#define CUSTOM_FACTORY_BUTTON                   6       // Reset after factory reset via hardware button

#define CUSTOM_UPGRADE_WEB                      7       // Reset after successful update from web interface

#define CUSTOM_RESTORE_WEB                      8      // Reset after successful setting restore from web interface

#define CUSTOM_RESET_MAX                        8

// =============================================================================
// HEARTBEAT
// =============================================================================

#define HEARTBEAT_NONE                          0       // Never send heartbeat
#define HEARTBEAT_ONCE                          1       // Send it only once upon MQTT connection
#define HEARTBEAT_REPEAT                        2       // Send it upon MQTT connection and every HEARTBEAT_INTERVAL
