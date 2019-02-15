//--------------------------------------------------------------------------------
// PROGMEM definitions
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
// Reset reasons
//--------------------------------------------------------------------------------

PROGMEM const char custom_reset_hardware[] = "Hardware button";
PROGMEM const char custom_reset_web[] = "Reboot from web interface";
PROGMEM const char custom_reset_mqtt[] = "Reboot from MQTT";
PROGMEM const char custom_reset_ota[] = "Reboot after successful OTA update";
PROGMEM const char custom_reset_nofuss[] = "Reboot after successful NoFUSS update";
PROGMEM const char custom_reset_upgrade[] = "Reboot after successful web update";
PROGMEM const char custom_reset_factory[] = "Factory reset";
PROGMEM const char* const custom_reset_string[] = {
    custom_reset_hardware, custom_reset_web,
    custom_reset_mqtt, custom_reset_ota,
    custom_reset_nofuss, custom_reset_upgrade,
    custom_reset_factory
};

//--------------------------------------------------------------------------------
// Capabilities
//--------------------------------------------------------------------------------

PROGMEM const char firmware_modules[] =
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

PROGMEM const char gateway_packet_none[] = "GATEWAY_PACKET_NONE";
PROGMEM const char gateway_packet_unknown[] = "GATEWAY_PACKET_UNKNOWN";
PROGMEM const char gateway_packet_error[] = "GATEWAY_PACKET_ERROR";
PROGMEM const char gateway_packet_accepted[] = "GATEWAY_PACKET_ACCEPTED";
PROGMEM const char gateway_packet_master_lookup[] = "GATEWAY_PACKET_MASTER_LOOKUP";
PROGMEM const char gateway_packet_heartbeat[] = "GATEWAY_PACKET_HEARTBEAT";
PROGMEM const char gateway_packet_ping[] = "GATEWAY_PACKET_PING";
PROGMEM const char gateway_packet_pong[] = "GATEWAY_PACKET_PONG";
PROGMEM const char gateway_packet_data[] = "GATEWAY_PACKET_DATA";
PROGMEM const char gateway_packet_init_start[] = "GATEWAY_PACKET_INIT_START";
PROGMEM const char gateway_packet_init_end[] = "GATEWAY_PACKET_INIT_END";
PROGMEM const char gateway_packet_init_restart[] = "GATEWAY_PACKET_INIT_RESTART";
PROGMEM const char gateway_packet_hw_info[] = "GATEWAY_PACKET_HARDWARE_INFO";
PROGMEM const char gateway_packet_fw_info[] = "GATEWAY_PACKET_FIRMWARE_INFO";
PROGMEM const char gateway_packet_settings_schema[] = "GATEWAY_PACKET_SETTINGS_SCHEMA";
PROGMEM const char gateway_packet_channels_schema[] = "GATEWAY_PACKET_CHANNELS_SCHEMA";
PROGMEM const char gateway_packet_who_are_you[] = "GATEWAY_PACKET_WHO_ARE_YOU";

PROGMEM const char* const gateway_packet_string[] = {
    gateway_packet_none, gateway_packet_unknown, gateway_packet_error, gateway_packet_accepted,
    gateway_packet_master_lookup,
    gateway_packet_heartbeat, gateway_packet_ping, gateway_packet_pong, gateway_packet_data,
    gateway_packet_init_start, gateway_packet_init_end, gateway_packet_init_restart,
    gateway_packet_hw_info, gateway_packet_fw_info,
    gateway_packet_settings_schema, gateway_packet_channels_schema,
    gateway_packet_who_are_you
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