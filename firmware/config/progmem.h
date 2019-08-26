//--------------------------------------------------------------------------------
// PROGMEM definitions
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
// Reset reasons
//--------------------------------------------------------------------------------

const char custom_reset_web[] PROGMEM       = "Reboot from web interface";
const char custom_reset_broker[] PROGMEM    = "Reboot from MQTT";
const char custom_reset_button[] PROGMEM    = "Reboot by button";
const char custom_factory_web[] PROGMEM     = "Reboot by factory reset from web interface";
const char custom_factory_broker[] PROGMEM  = "Reboot by factory reset from MQTT";
const char custom_factory_button[] PROGMEM  = "Reboot by factory reset by bytton";
const char custom_upgrade_web[] PROGMEM     = "Reboot after successful web update";
const char custom_upgrade_ota[] PROGMEM     = "Reboot after successful OTA update";
const char custom_upgrade_nofuss[] PROGMEM  = "Reboot after successful NoFUSS update";
const char custom_restore_web[] PROGMEM     = "Reboot by settings restore from web interface";

PROGMEM const char * const custom_reset_string[] = {
    custom_reset_web, custom_reset_broker, custom_reset_button,
    custom_factory_web, custom_factory_broker, custom_factory_button,
    custom_upgrade_web, custom_upgrade_ota, custom_upgrade_nofuss,
    custom_restore_web
};

//--------------------------------------------------------------------------------
// Capabilities
//--------------------------------------------------------------------------------

const char firmware_modules[] PROGMEM =
    #if ALEXA_SUPPORT
        "ALEXA "
    #endif
    #if RELAY_PROVIDER != RELAY_PROVIDER_NONE
        "RELAY "
    #endif
    #if LIGHT_PROVIDER != LIGHT_PROVIDER_NONE
        "LIGHT "
    #endif
    #if BUTTON_SUPPORT
        "BUTTON "
    #endif
    #if DEBUG_SERIAL_SUPPORT
        "DEBUG_SERIAL "
    #endif
    #if DEBUG_WEB_SUPPORT
        "DEBUG_WEB "
    #endif
    #if DEBUG_MQTT_SUPPORT
        "DEBUG_MQTT "
    #endif
    #if ENCODER_SUPPORT
        "ENCODER "
    #endif
    #if I2C_SUPPORT
        "I2C "
    #endif
    #if IR_SUPPORT
        "IR "
    #endif
    #if LED_SUPPORT
        "LED "
    #endif
    #if MQTT_SUPPORT
        "MQTT "
    #endif
    #if NOFUSS_SUPPORT
        "NOFUSS "
    #endif
    #if NTP_SUPPORT
        "NTP "
    #endif
    #if RFM69_SUPPORT
        "RFM69 "
    #endif
    #if RF_SUPPORT
        "RF "
    #endif
    #if SCHEDULER_SUPPORT
        "SCHEDULER "
    #endif
    #if SENSOR_SUPPORT
        "SENSOR "
    #endif
    #if SPIFFS_SUPPORT
        "SPIFFS "
    #endif
    #if WIFI_SUPPORT
        "WIFI "
    #endif
    #if WEB_SUPPORT
        "WEB "
    #endif
    #if FASTYBIRD_SUPPORT
        "FASTYBIRD "
    #endif
    #if NODES_GATEWAY_SUPPORT
        "NODES_GATEWAY "
    #endif
    "";

//--------------------------------------------------------------------------------
// Nodes gateway
//--------------------------------------------------------------------------------

#if NODES_GATEWAY_SUPPORT

const char gateway_packet_srarch_new_nodes[] PROGMEM            = "GATEWAY_PACKET_SEARCH_NEW_NODES";
const char gateway_packet_node_address_confirm[] PROGMEM        = "GATEWAY_PACKET_NODE_ADDRESS_CONFIRM";

PROGMEM const char * const gateway_packets_searching_string[] = {
    gateway_packet_srarch_new_nodes, gateway_packet_node_address_confirm
};

const char gateway_packet_search_nodes[] PROGMEM                = "GATEWAY_PACKET_SEARCH_NODES";
const char gateway_packet_address_discard[] PROGMEM             = "GATEWAY_PACKET_ADDRESS_DISCARD";

PROGMEM const char * const gateway_packets_addresing_string[] = {
    gateway_packet_search_nodes, gateway_packet_address_discard
};

const char gateway_packet_hw_model[] PROGMEM                    = "GATEWAY_PACKET_HW_MODEL";
const char gateway_packet_hw_manufacturer[] PROGMEM             = "GATEWAY_PACKET_HW_MANUFACTURER";
const char gateway_packet_hw_version[] PROGMEM                  = "GATEWAY_PACKET_HW_VERSION";
const char gateway_packet_fw_model[] PROGMEM                    = "GATEWAY_PACKET_FW_MODEL";
const char gateway_packet_fw_manufacturer[] PROGMEM             = "GATEWAY_PACKET_FW_MANUFACTURER";
const char gateway_packet_fw_version[] PROGMEM                  = "GATEWAY_PACKET_FW_VERSION";

PROGMEM const char * const gateway_packets_node_initialization_string[] = {
    gateway_packet_hw_model, gateway_packet_hw_manufacturer, gateway_packet_hw_version,
    gateway_packet_fw_model, gateway_packet_fw_manufacturer, gateway_packet_fw_version
};

const char gateway_packet_registers_size[] PROGMEM              = "GATEWAY_PACKET_REGISTERS_SIZE";
const char gateway_packet_ai_registers_structure[] PROGMEM      = "GATEWAY_PACKET_AI_REGISTERS_STRUCTURE";
const char gateway_packet_ao_registers_structure[] PROGMEM      = "GATEWAY_PACKET_AO_REGISTERS_STRUCTURE";

PROGMEM const char * const gateway_packets_registers_initialization_string[] = {
    gateway_packet_registers_size,
    gateway_packet_ai_registers_structure, gateway_packet_ao_registers_structure
};

const char gateway_packet_read_single_di[] PROGMEM              = "GATEWAY_PACKET_READ_SINGLE_DI";
const char gateway_packet_read_multi_di[] PROGMEM               = "GATEWAY_PACKET_READ_MULTI_DI";
const char gateway_packet_read_single_do[] PROGMEM              = "GATEWAY_PACKET_READ_SINGLE_DO";
const char gateway_packet_read_multi_do[] PROGMEM               = "GATEWAY_PACKET_READ_MULTI_DO";
const char gateway_packet_read_single_ai[] PROGMEM              = "GATEWAY_PACKET_READ_SINGLE_AI";
const char gateway_packet_read_multi_ai[] PROGMEM               = "GATEWAY_PACKET_READ_MULTI_AI";
const char gateway_packet_read_single_ao[] PROGMEM              = "GATEWAY_PACKET_READ_SINGLE_AO";
const char gateway_packet_read_multi_ao[] PROGMEM               = "GATEWAY_PACKET_READ_MULTI_AO";
const char gateway_packet_read_single_ev[] PROGMEM              = "GATEWAY_PACKET_READ_SINGLE_EV";
const char gateway_packet_read_multi_ev[] PROGMEM               = "GATEWAY_PACKET_READ_MULTI_EV";

PROGMEM const char * const gateway_packets_registers_reading_string[] = {
    gateway_packet_read_single_di, gateway_packet_read_multi_di,
    gateway_packet_read_single_do, gateway_packet_read_multi_do,
    gateway_packet_read_single_ai, gateway_packet_read_multi_ai,
    gateway_packet_read_single_ao, gateway_packet_read_multi_ao,
    gateway_packet_read_single_ev, gateway_packet_read_multi_ev
};

const char gateway_packet_write_one_do[] PROGMEM                = "GATEWAY_PACKET_WRITE_ONE_DO";
const char gateway_packet_write_one_ao[] PROGMEM                = "GATEWAY_PACKET_WRITE_ONE_AO";
const char gateway_packet_write_multi_do[] PROGMEM              = "GATEWAY_PACKET_WRITE_MULTI_DO";
const char gateway_packet_write_multi_ao[] PROGMEM              = "GATEWAY_PACKET_WRITE_MULTI_AO";

PROGMEM const char * const gateway_packets_registers_writing_string[] = {
    gateway_packet_write_one_do, gateway_packet_write_one_ao,
    gateway_packet_write_multi_do, gateway_packet_write_multi_ao
};

const char gateway_packet_none[] PROGMEM                        = "GATEWAY_PACKET_NONE";
const char gateway_packet_ping[] PROGMEM                        = "GATEWAY_PACKET_GATEWAY_PING";

PROGMEM const char * const gateway_packets_misc_string[] = {
    gateway_packet_none, gateway_packet_ping
};

//--------------------------------------------------------------------------------

const int gateway_packets_searching[] PROGMEM = {
    GATEWAY_PACKET_SEARCH_NEW_NODES, GATEWAY_PACKET_NODE_ADDRESS_CONFIRM
};

const int gateway_packets_addresing[] PROGMEM = {
    GATEWAY_PACKET_SEARCH_NODES, GATEWAY_PACKET_ADDRESS_DISCARD
};

const int gateway_packets_node_initialization[] PROGMEM = {
    GATEWAY_PACKET_HW_MODEL, GATEWAY_PACKET_HW_MANUFACTURER, GATEWAY_PACKET_HW_VERSION,
    GATEWAY_PACKET_FW_MODEL, GATEWAY_PACKET_FW_MANUFACTURER, GATEWAY_PACKET_FW_VERSION
};

const int gateway_packets_registers_initialization[] PROGMEM = {
    GATEWAY_PACKET_REGISTERS_SIZE,
    GATEWAY_PACKET_AI_REGISTERS_STRUCTURE, GATEWAY_PACKET_AO_REGISTERS_STRUCTURE
};

const int gateway_packets_registers_reading[] PROGMEM = {
    GATEWAY_PACKET_READ_SINGLE_DI, GATEWAY_PACKET_READ_MULTI_DI,
    GATEWAY_PACKET_READ_SINGLE_DO, GATEWAY_PACKET_READ_MULTI_DO,
    GATEWAY_PACKET_READ_SINGLE_AI, GATEWAY_PACKET_READ_MULTI_AI,
    GATEWAY_PACKET_READ_SINGLE_AO, GATEWAY_PACKET_READ_MULTI_AO,
    GATEWAY_PACKET_READ_SINGLE_EV, GATEWAY_PACKET_READ_MULTI_EV
};

const int gateway_packets_registers_writing[] PROGMEM = {
    GATEWAY_PACKET_WRITE_ONE_DO, GATEWAY_PACKET_WRITE_ONE_AO,
    GATEWAY_PACKET_WRITE_MULTI_DO, GATEWAY_PACKET_WRITE_MULTI_AO
};

const int gateway_packets_misc[] PROGMEM = {
    GATEWAY_PACKET_NONE, GATEWAY_PACKET_GATEWAY_PING
};

#endif

//--------------------------------------------------------------------------------
// Sensors
//--------------------------------------------------------------------------------

#if SENSOR_SUPPORT

PROGMEM const char firmware_sensors[] =
    #if HLW8012_SUPPORT
        "HLW8012 "
    #endif
    #if CSE7766_SUPPORT
        "CSE7766 "
    #endif
    #if ITEAD_SONOFF_SC_SUPPORT
        "SONOFF_SC "
    #endif
    #if ITEAD_SONOFF_SC_PRO_SUPPORT
        "SONOFF_SC_PRO "
    #endif
    "";

PROGMEM const uint8_t magnitude_decimals[] = {
    0,
    1, 0, 2, // THP
    3, 0, 0, 0, 0, 0, 0, 0, // Power decimals
    0, 0, 0, // analog, digital, event
    0, 0, 0, // PM
    0, 0,
    0, 0, 3, // UVA, UVB, UVI
    3, 0,
    4, 4, // Geiger Counter decimals
    0,
    0, 0, 0, 3,    // NO2, CO, Ohms, pH
    0, 0, 0
};

PROGMEM const char magnitude_unknown_name[] = "unknown";
PROGMEM const char magnitude_temperature_name[] =  "temperature";
PROGMEM const char magnitude_humidity_name[] = "humidity";
PROGMEM const char magnitude_pressure_name[] = "pressure";
PROGMEM const char magnitude_current_name[] = "current";
PROGMEM const char magnitude_voltage_name[] = "voltage";
PROGMEM const char magnitude_active_power_name[] = "power";
PROGMEM const char magnitude_apparent_power_name[] = "apparent";
PROGMEM const char magnitude_reactive_power_name[] = "reactive";
PROGMEM const char magnitude_power_factor_name[] = "factor";
PROGMEM const char magnitude_energy_name[] = "energy";
PROGMEM const char magnitude_energy_delta_name[] = "energy_delta";
PROGMEM const char magnitude_analog_name[] = "analog";
PROGMEM const char magnitude_digital_name[] = "digital";
PROGMEM const char magnitude_event_name[] = "event";
PROGMEM const char magnitude_pm1dot0_name[] = "pm1dot0";
PROGMEM const char magnitude_pm2dot5_name[] = "pm2dot5";
PROGMEM const char magnitude_pm10_name[] = "pm10";
PROGMEM const char magnitude_co2_name[] = "co2";
PROGMEM const char magnitude_lux_name[] = "lux";
PROGMEM const char magnitude_uva_name[] = "uva";
PROGMEM const char magnitude_uvb_name[] = "uvb";
PROGMEM const char magnitude_uvi_name[] = "uvi";
PROGMEM const char magnitude_distance_name[] = "distance";
PROGMEM const char magnitude_hcho_name[] = "hcho";
PROGMEM const char magnitude_geiger_cpm_name[] = "ldr_cpm";  // local dose rate [Counts per minute]
PROGMEM const char magnitude_geiger_sv_name[] = "ldr_uSvh";  // local dose rate [µSievert per hour]
PROGMEM const char magnitude_count_name[] = "count";
PROGMEM const char magnitude_no2_name[] = "no2";
PROGMEM const char magnitude_co_name[] = "co";
PROGMEM const char magnitude_resistance_name[] = "resistance";
PROGMEM const char magnitude_ph_name[] = "ph";
PROGMEM const char magnitude_sonoff_sc_air_quality_name[] = "air_quality";
PROGMEM const char magnitude_sonoff_sc_light_level_name[] = "light_level";
PROGMEM const char magnitude_sonoff_sc_noise_level_name[] = "noise_level";

PROGMEM const char* const magnitude_names[] = {
    magnitude_unknown_name, magnitude_temperature_name, magnitude_humidity_name,
    magnitude_pressure_name, magnitude_current_name, magnitude_voltage_name,
    magnitude_active_power_name, magnitude_apparent_power_name, magnitude_reactive_power_name,
    magnitude_power_factor_name, magnitude_energy_name, magnitude_energy_delta_name,
    magnitude_analog_name, magnitude_digital_name, magnitude_event_name,
    magnitude_pm1dot0_name, magnitude_pm2dot5_name, magnitude_pm10_name,
    magnitude_co2_name, magnitude_lux_name,
    magnitude_uva_name, magnitude_uvb_name, magnitude_uvi_name,
    magnitude_distance_name, magnitude_hcho_name,
    magnitude_geiger_cpm_name, magnitude_geiger_sv_name,
    magnitude_count_name,
    magnitude_no2_name, magnitude_co_name, magnitude_resistance_name, magnitude_ph_name,
    magnitude_sonoff_sc_air_quality_name, magnitude_sonoff_sc_light_level_name, magnitude_sonoff_sc_noise_level_name
};

PROGMEM const char magnitude_empty[] = "";
PROGMEM const char magnitude_celsius[] =  "°C";
PROGMEM const char magnitude_fahrenheit[] =  "°F";
PROGMEM const char magnitude_percentage[] = "%";
PROGMEM const char magnitude_hectopascals[] = "hPa";
PROGMEM const char magnitude_amperes[] = "A";
PROGMEM const char magnitude_volts[] = "V";
PROGMEM const char magnitude_watts[] = "W";
PROGMEM const char magnitude_kw[] = "kW";
PROGMEM const char magnitude_joules[] = "J";
PROGMEM const char magnitude_kwh[] = "kWh";
PROGMEM const char magnitude_ugm3[] = "µg/m³";
PROGMEM const char magnitude_ppm[] = "ppm";
PROGMEM const char magnitude_lux[] = "lux";
PROGMEM const char magnitude_distance[] = "m";
PROGMEM const char magnitude_mgm3[] = "mg/m³";
PROGMEM const char magnitude_geiger_cpm[] = "cpm";    // Counts per Minute: Unit of local dose rate (Geiger counting)
PROGMEM const char magnitude_geiger_sv[] = "µSv/h";   // µSievert per hour: 2nd unit of local dose rate (Geiger counting)
PROGMEM const char magnitude_resistance[] = "ohm";

PROGMEM const char* const magnitude_units[] = {
    magnitude_empty, magnitude_celsius, magnitude_percentage,
    magnitude_hectopascals, magnitude_amperes, magnitude_volts,
    magnitude_watts, magnitude_watts, magnitude_watts,
    magnitude_percentage, magnitude_joules, magnitude_joules,
    magnitude_empty, magnitude_empty, magnitude_empty,
    magnitude_ugm3, magnitude_ugm3, magnitude_ugm3,
    magnitude_ppm, magnitude_lux,
    magnitude_empty, magnitude_empty, magnitude_empty,
    magnitude_distance, magnitude_mgm3,
    magnitude_geiger_cpm, magnitude_geiger_sv,                  // Geiger counter units
    magnitude_empty,                                            //
    magnitude_ppm, magnitude_ppm,                               // NO2 & CO2
    magnitude_resistance,
    magnitude_empty,                                             // pH
    magnitude_empty, magnitude_empty, magnitude_empty
};

#endif // SENSOR_SUPPORT