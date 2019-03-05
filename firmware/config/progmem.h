//--------------------------------------------------------------------------------
// PROGMEM definitions
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
// Reset reasons
//--------------------------------------------------------------------------------

const char custom_reset_hardware[] PROGMEM  = "Hardware button";
const char custom_reset_web[] PROGMEM       = "Reboot from web interface";
const char custom_reset_mqtt[] PROGMEM      = "Reboot from MQTT";
const char custom_reset_ota[] PROGMEM       = "Reboot after successful OTA update";
const char custom_reset_nofuss[] PROGMEM    = "Reboot after successful NoFUSS update";
const char custom_reset_upgrade[] PROGMEM   = "Reboot after successful web update";
const char custom_reset_factory[] PROGMEM   = "Factory reset";

PROGMEM const char * const custom_reset_string[] = {
    custom_reset_hardware, custom_reset_web,
    custom_reset_mqtt, custom_reset_ota,
    custom_reset_nofuss, custom_reset_upgrade,
    custom_reset_factory
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
    #if DIRECT_CONTROL_SUPPORT
        "DIRECT_CONTROL "
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

const char gateway_packet_search_nodes[] PROGMEM                = "GATEWAY_PACKET_SEARCH_NODES";
const char gateway_packet_srarch_new_nodes[] PROGMEM            = "GATEWAY_PACKET_SEARCH_NEW_NODES";
const char gateway_packet_node_address_confirm[] PROGMEM        = "GATEWAY_PACKET_NODE_ADDRESS_CONFIRM";
const char gateway_packet_address_discard[] PROGMEM             = "GATEWAY_PACKET_ADDRESS_DISCARD";

PROGMEM const char * const gateway_packets_addresing_string[] = {
    gateway_packet_search_nodes, gateway_packet_srarch_new_nodes,
    gateway_packet_node_address_confirm, gateway_packet_address_discard
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
const char gateway_packet_di_registers_structure[] PROGMEM      = "GATEWAY_PACKET_DI_REGISTERS_STRUCTURE";
const char gateway_packet_do_registers_structure[] PROGMEM      = "GATEWAY_PACKET_DO_REGISTERS_STRUCTURE";
const char gateway_packet_ai_registers_structure[] PROGMEM      = "GATEWAY_PACKET_AI_REGISTERS_STRUCTURE";
const char gateway_packet_ao_registers_structure[] PROGMEM      = "GATEWAY_PACKET_AO_REGISTERS_STRUCTURE";

PROGMEM const char * const gateway_packets_registers_initialization_string[] = {
    gateway_packet_registers_size,
    gateway_packet_di_registers_structure, gateway_packet_do_registers_structure,
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

PROGMEM const char * const gateway_packets_registers_reading_string[] = {
    gateway_packet_read_single_di, gateway_packet_read_multi_di,
    gateway_packet_read_single_do, gateway_packet_read_multi_do,
    gateway_packet_read_single_ai, gateway_packet_read_multi_ai,
    gateway_packet_read_single_ao, gateway_packet_read_multi_ao
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

const int gateway_packets_addresing[] PROGMEM = {
    GATEWAY_PACKET_SEARCH_NODES, GATEWAY_PACKET_SEARCH_NEW_NODES,
    GATEWAY_PACKET_NODE_ADDRESS_CONFIRM, GATEWAY_PACKET_ADDRESS_DISCARD
};

const int gateway_packets_node_initialization[] PROGMEM = {
    GATEWAY_PACKET_HW_MODEL, GATEWAY_PACKET_HW_MANUFACTURER, GATEWAY_PACKET_HW_VERSION,
    GATEWAY_PACKET_FW_MODEL, GATEWAY_PACKET_FW_MANUFACTURER, GATEWAY_PACKET_FW_VERSION
};

const int gateway_packets_registers_initialization[] PROGMEM = {
    GATEWAY_PACKET_REGISTERS_SIZE,
    GATEWAY_PACKET_DI_REGISTERS_STRUCTURE, GATEWAY_PACKET_DO_REGISTERS_STRUCTURE,
    GATEWAY_PACKET_AI_REGISTERS_STRUCTURE, GATEWAY_PACKET_AO_REGISTERS_STRUCTURE
};

const int gateway_packets_registers_reading[] PROGMEM = {
    GATEWAY_PACKET_READ_SINGLE_DI, GATEWAY_PACKET_READ_MULTI_DI,
    GATEWAY_PACKET_READ_SINGLE_DO, GATEWAY_PACKET_READ_MULTI_DO,
    GATEWAY_PACKET_READ_SINGLE_AI, GATEWAY_PACKET_READ_MULTI_AI,
    GATEWAY_PACKET_READ_SINGLE_AO, GATEWAY_PACKET_READ_MULTI_AO
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

PROGMEM const char espurna_sensors[] =
    #if AM2320_SUPPORT
        "AM2320_I2C "
    #endif
    #if ANALOG_SUPPORT
        "ANALOG "
    #endif
    #if BH1750_SUPPORT
        "BH1750 "
    #endif
    #if BMP180_SUPPORT
        "BMP180 "
    #endif
    #if BMX280_SUPPORT
        "BMX280 "
    #endif
    #if CSE7766_SUPPORT
        "CSE7766 "
    #endif
    #if DALLAS_SUPPORT
        "DALLAS "
    #endif
    #if DHT_SUPPORT
        "DHTXX "
    #endif
    #if DIGITAL_SUPPORT
        "DIGITAL "
    #endif
    #if ECH1560_SUPPORT
        "ECH1560 "
    #endif
    #if EMON_ADC121_SUPPORT
        "EMON_ADC121 "
    #endif
    #if EMON_ADS1X15_SUPPORT
        "EMON_ADX1X15 "
    #endif
    #if EMON_ANALOG_SUPPORT
        "EMON_ANALOG "
    #endif
    #if EVENTS_SUPPORT
        "EVENTS "
    #endif
    #if GEIGER_SUPPORT
        "GEIGER "
    #endif
    #if GUVAS12SD_SUPPORT
        "GUVAS12SD "
    #endif
    #if HLW8012_SUPPORT
        "HLW8012 "
    #endif
    #if MHZ19_SUPPORT
        "MHZ19 "
    #endif
    #if MICS2710_SUPPORT
        "MICS2710 "
    #endif
    #if MICS5525_SUPPORT
        "MICS5525 "
    #endif
    #if NTC_SUPPORT
        "NTC "
    #endif
    #if PMSX003_SUPPORT
        "PMSX003 "
    #endif
    #if PULSEMETER_SUPPORT
        "PULSEMETER "
    #endif
    #if PZEM004T_SUPPORT
        "PZEM004T "
    #endif
    #if SDS011_SUPPORT
        "SDS011 "
    #endif
    #if SENSEAIR_SUPPORT
        "SENSEAIR "
    #endif
    #if SHT3X_I2C_SUPPORT
        "SHT3X_I2C "
    #endif
    #if SI7021_SUPPORT
        "SI7021 "
    #endif
    #if SONAR_SUPPORT
        "SONAR "
    #endif
    #if TMP3X_SUPPORT
        "TMP3X "
    #endif
    #if V9261F_SUPPORT
        "V9261F "
    #endif
    #if VEML6075_SUPPORT
        "VEML6075 "
    #endif
    #if VL53L1X_SUPPORT
        "VL53L1X "
    #endif
    #if EZOPH_SUPPORT
        "EZOPH "
    #endif
    "";

#endif // SENSOR_SUPPORT