/*

GATEWAY MODULE CONFIGURATION

Copyright (C) 2018 FastyBird Ltd. <info@fastybird.com>

*/

#pragma once

// -----------------------------------------------------------------------------
// GATEWAY - General data
// -----------------------------------------------------------------------------

#ifndef FB_GATEWAY_SUPPORT
    #define FB_GATEWAY_SUPPORT                      0               // Do not build with gateway support by default
#endif

#ifndef FB_GATEWAY_TX_PIN
    #define FB_GATEWAY_TX_PIN                       GPIO_NONE
#endif

#ifndef FB_GATEWAY_RX_PIN
    #define FB_GATEWAY_RX_PIN                       GPIO_NONE
#endif

#ifndef FB_GATEWAY_MASTER_ID
    #define FB_GATEWAY_MASTER_ID                    254
#endif

#ifndef FB_GATEWAY_MAX_NODES
    #define FB_GATEWAY_MAX_NODES                    10              // Define maximum slave nodes count that could connect to the master
#endif

#ifndef FB_GATEWAY_MAX_SENDING_FAILURES
    #define FB_GATEWAY_MAX_SENDING_FAILURES         5               // Maximum count of attempts before gateway delay node communication process
#endif

#ifndef FB_GATEWAY_LOST_DELAY
    #define FB_GATEWAY_LOST_DELAY                   15000           // Delay in ms after reaching maximum packet sendings attempts
#endif

#ifndef FB_GATEWAY_SEARCH_NODES_BROADCAST_TIME
    #define FB_GATEWAY_SEARCH_NODES_BROADCAST_TIME  1000            // Master reception time during GATEWAY_ACQUIRE_ADDRESS_LIST broadcast (250 milliseconds)
#endif

#ifndef FB_GATEWAY_NODES_BUFFER_FULL
    #define FB_GATEWAY_NODES_BUFFER_FULL            254
#endif

#ifndef FB_GATEWAY_FAIL
    #define FB_GATEWAY_FAIL                         65535
#endif

#ifndef FB_GATEWAY_START_DELAY
    #define FB_GATEWAY_START_DELAY                  6000            // Little delay before gateway start communication with nodes
#endif

#ifndef FB_GATEWAY_SEARCHING_TIMEOUT
    #define FB_GATEWAY_SEARCHING_TIMEOUT            4000            // After timeout is reached, gateway stop searching for new nodes
#endif

#ifndef FB_GATEWAY_SEARCHING_WAITING_TIMEOUT
    #define FB_GATEWAY_SEARCHING_WAITING_TIMEOUT    4000            // After timeout is reached, gateway discard node reserved slot
#endif

#ifndef FB_GATEWAY_WEB_API_CONFIGURATION
    #define FB_GATEWAY_WEB_API_CONFIGURATION        "/control/gateway-configuration"    //
#endif

// -----------------------------------------------------------------------------
// GATEWAY - Dependencies
// -----------------------------------------------------------------------------

#if FB_GATEWAY_SUPPORT && FB_GATEWAY_TX_PIN == GPIO_NONE && FB_GATEWAY_RX_PIN == GPIO_NONE
    #undef FB_GATEWAY_SUPPORT
    #define FB_GATEWAY_SUPPORT                      0           // IO GPIO have to be configured
#endif

#if FB_GATEWAY_SUPPORT
    #undef SPIFFS_SUPPORT
    #define SPIFFS_SUPPORT                          1           // Enabling SPIFFS for storing configuration
#endif

// -----------------------------------------------------------------------------
// GATEWAY - Communication packets
// -----------------------------------------------------------------------------

// Node searching
#define GATEWAY_PACKET_SEARCH_NODES                 0x01
#define GATEWAY_PACKET_SEARCH_NODES_CONFIRM         0x02
#define GATEWAY_PACKET_NODE_ADDRESS                 0x03
#define GATEWAY_PACKET_NODE_ADDRESS_CONFIRM         0x04
#define GATEWAY_PACKET_ADDRESS_DISCARD              0x05

#define GATEWAY_PACKET_SEARCH_MAX                   5

// Node initialization
#define GATEWAY_PACKET_HW_MODEL                     0x11
#define GATEWAY_PACKET_HW_MANUFACTURER              0x12
#define GATEWAY_PACKET_HW_VERSION                   0x13
#define GATEWAY_PACKET_FW_MODEL                     0x14
#define GATEWAY_PACKET_FW_MANUFACTURER              0x15
#define GATEWAY_PACKET_FW_VERSION                   0x16
#define GATEWAY_PACKET_REGISTERS_SIZE               0x17
#define GATEWAY_PACKET_AI_REGISTERS_STRUCTURE       0x18
#define GATEWAY_PACKET_AO_REGISTERS_STRUCTURE       0x19

#define GATEWAY_PACKET_NODE_INIT_MAX                9

// Registers reading
#define GATEWAY_PACKET_READ_SINGLE_DI               0x21   // Master requested DI one regiter reading
#define GATEWAY_PACKET_READ_MULTI_DI                0x22   // Master requested DI multiple regiters reading
#define GATEWAY_PACKET_READ_SINGLE_DO               0x23   // Master requested DO one regiter reading
#define GATEWAY_PACKET_READ_MULTI_DO                0x24   // Master requested DO multiple regiters reading
#define GATEWAY_PACKET_READ_SINGLE_AI               0x25   // Master requested AI one regiter reading
#define GATEWAY_PACKET_READ_MULTI_AI                0x26   // Master requested AI multiple regiters reading
#define GATEWAY_PACKET_READ_SINGLE_AO               0x27   // Master requested AO one regiter reading
#define GATEWAY_PACKET_READ_MULTI_AO                0x28   // Master requested AO multiple regiters reading
#define GATEWAY_PACKET_READ_SINGLE_EV               0x29   // Master requested EV one regiter reading
#define GATEWAY_PACKET_READ_MULTI_EV                0x2A   // Master requested EV multiple regiters reading

#define GATEWAY_PACKET_REGISTERS_READING_MAX        10

// Registers writing
#define GATEWAY_PACKET_WRITE_ONE_DO                 0x31
#define GATEWAY_PACKET_WRITE_ONE_AO                 0x32
#define GATEWAY_PACKET_WRITE_MULTI_DO               0x33
#define GATEWAY_PACKET_WRITE_MULTI_AO               0x34

#define GATEWAY_PACKET_REGISTERS_WRITING_MAX        4

// Misc communication
#define GATEWAY_PACKET_PING                         0x41
#define GATEWAY_PACKET_PONG                         0x42
#define GATEWAY_PACKET_HELLO                        0x43
#define GATEWAY_PACKET_NONE                         0xFF

#define GATEWAY_PACKET_MISC_MAX                     4

// -----------------------------------------------------------------------------
// GATEWAY - Registers datatypes
// -----------------------------------------------------------------------------

#define GATEWAY_DATA_TYPE_UNKNOWN                   0xFF
#define GATEWAY_DATA_TYPE_UINT8                     0x01
#define GATEWAY_DATA_TYPE_UINT16                    0x02
#define GATEWAY_DATA_TYPE_UINT32                    0x03
#define GATEWAY_DATA_TYPE_INT8                      0x04
#define GATEWAY_DATA_TYPE_INT16                     0x05
#define GATEWAY_DATA_TYPE_INT32                     0x06
#define GATEWAY_DATA_TYPE_FLOAT32                   0x07
#define GATEWAY_DATA_TYPE_BOOL                      0x08

// -----------------------------------------------------------------------------
// GATEWAY - Registers types
// -----------------------------------------------------------------------------

#define GATEWAY_REGISTER_NONE                       0xFF
#define GATEWAY_REGISTER_DI                         0
#define GATEWAY_REGISTER_DO                         1
#define GATEWAY_REGISTER_AI                         2
#define GATEWAY_REGISTER_AO                         3
#define GATEWAY_REGISTER_EV                         4

// -----------------------------------------------------------------------------
// GATEWAY - Registers misc
// -----------------------------------------------------------------------------

#define GATEWAY_DESCRIPTION_NOT_SET                 "none"

// -----------------------------------------------------------------------------
// GATEWAY - Prototypes
// -----------------------------------------------------------------------------

#if FB_GATEWAY_SUPPORT
    #define PJON_INCLUDE_TS

    #ifndef PJON_PACKET_MAX_LENGTH
        #define PJON_PACKET_MAX_LENGTH 80
    #endif

    #include <PJON.h>

    //#define TSA_RESPONSE_TIME_OUT 20000

    struct gateway_node_description_t {
        char    manufacturer[20]    = GATEWAY_DESCRIPTION_NOT_SET;
        char    model[20]           = GATEWAY_DESCRIPTION_NOT_SET;
        char    version[10]         = GATEWAY_DESCRIPTION_NOT_SET;
    };

    struct gateway_node_t {
        // Node is ready for data exchange flag
        bool ready = false;

        // Lost communication timestamp
        uint32_t lost = 0;

        // Node unique identifier
        char serial_number[15] = GATEWAY_DESCRIPTION_NOT_SET;

        gateway_node_description_t hardware;
        gateway_node_description_t firmware;
    };

    struct gateway_register_reading_t {
        uint8_t     register_type   = GATEWAY_REGISTER_NONE;
        uint8_t     start           = 0;
    };

    typedef struct {
        uint8_t         datatype;
        uint8_t         size;
        char            value[4];
    } gateway_register_t;

    struct gateway_registers_t {
        std::vector<gateway_register_t> digital_inputs;
        std::vector<gateway_register_t> digital_outputs;

        std::vector<gateway_register_t> analog_inputs;
        std::vector<gateway_register_t> analog_outputs;
        
        std::vector<gateway_register_t> event_inputs;
    };

    struct gateway_node_initiliazation_t {
        bool        state               = false;                // Initialization process state
        uint8_t     step                = GATEWAY_PACKET_NONE;  // Node initialization step
        uint8_t     register_position   = 0;
    };

    struct gateway_node_addressing_t {
        bool        state               = false;                // Addressing process state
        uint32_t    address             = PJON_NOT_ASSIGNED;    // Node assigned address
        uint32_t    registration        = 0;                    // Timestamp when node confirmed search request
    };

    struct gateway_node_communication_t {
        uint8_t     max_packet_size     = PJON_PACKET_MAX_LENGTH;
        uint8_t     waiting_for         = GATEWAY_PACKET_NONE;
        uint8_t     attempts            = 0;
    };

    typedef union {
        bool        number;
        uint8_t     bytes[4];
    } BOOL_UNION_t;

    typedef union {
        uint8_t     number;
        uint8_t     bytes[4];
    } UINT8_UNION_t;

    typedef union {
        uint16_t    number;
        uint8_t     bytes[4];
    } UINT16_UNION_t;

    typedef union {
        uint32_t    number;
        uint8_t     bytes[4];
    } UINT32_UNION_t;

    typedef union {
        int8_t      number;
        uint8_t     bytes[4];
    } INT8_UNION_t;

    typedef union {
        int16_t     number;
        uint8_t     bytes[4];
    } INT16_UNION_t;

    typedef union {
        int32_t     number;
        uint8_t     bytes[4];
    } INT32_UNION_t;

    typedef union {
        float       number;
        uint8_t     bytes[4];
    } FLOAT32_UNION_t;
#else
    #define gateway_node_t void *
    #define PJON_Packet_Info void *
#endif

// -----------------------------------------------------------------------------
// GATEWAY - PROGMEM structures
// -----------------------------------------------------------------------------

#if FB_GATEWAY_SUPPORT

    PROGMEM const char gateway_packet_search_nodes[]                = "GATEWAY_PACKET_SEARCH_NODES";
    PROGMEM const char gateway_packet_search_nodes_confirm[]        = "GATEWAY_PACKET_SEARCH_NODES_CONFIRM";
    PROGMEM const char gateway_packet_node_address[]                = "GATEWAY_PACKET_NODE_ADDRESS";
    PROGMEM const char gateway_packet_node_address_confirm[]        = "GATEWAY_PACKET_NODE_ADDRESS_CONFIRM";
    PROGMEM const char gateway_packet_address_discard[]             = "GATEWAY_PACKET_ADDRESS_DISCARD";

    PROGMEM const char * const gateway_packets_searching_string[] = {
        gateway_packet_search_nodes, gateway_packet_search_nodes_confirm,
        gateway_packet_node_address, gateway_packet_node_address_confirm, gateway_packet_address_discard
    };

    PROGMEM const char gateway_packet_hw_model[]                    = "GATEWAY_PACKET_HW_MODEL";
    PROGMEM const char gateway_packet_hw_manufacturer[]             = "GATEWAY_PACKET_HW_MANUFACTURER";
    PROGMEM const char gateway_packet_hw_version[]                  = "GATEWAY_PACKET_HW_VERSION";
    PROGMEM const char gateway_packet_fw_model[]                    = "GATEWAY_PACKET_FW_MODEL";
    PROGMEM const char gateway_packet_fw_manufacturer[]             = "GATEWAY_PACKET_FW_MANUFACTURER";
    PROGMEM const char gateway_packet_fw_version[]                  = "GATEWAY_PACKET_FW_VERSION";
    PROGMEM const char gateway_packet_registers_size[]              = "GATEWAY_PACKET_REGISTERS_SIZE";
    PROGMEM const char gateway_packet_ai_registers_structure[]      = "GATEWAY_PACKET_AI_REGISTERS_STRUCTURE";
    PROGMEM const char gateway_packet_ao_registers_structure[]      = "GATEWAY_PACKET_AO_REGISTERS_STRUCTURE";

    PROGMEM const char * const gateway_packets_node_initialization_string[] = {
        gateway_packet_hw_model, gateway_packet_hw_manufacturer, gateway_packet_hw_version,
        gateway_packet_fw_model, gateway_packet_fw_manufacturer, gateway_packet_fw_version,
        gateway_packet_registers_size,
        gateway_packet_ai_registers_structure, gateway_packet_ao_registers_structure
    };

    PROGMEM const char gateway_packet_read_single_di[]              = "GATEWAY_PACKET_READ_SINGLE_DI";
    PROGMEM const char gateway_packet_read_multi_di[]               = "GATEWAY_PACKET_READ_MULTI_DI";
    PROGMEM const char gateway_packet_read_single_do[]              = "GATEWAY_PACKET_READ_SINGLE_DO";
    PROGMEM const char gateway_packet_read_multi_do[]               = "GATEWAY_PACKET_READ_MULTI_DO";
    PROGMEM const char gateway_packet_read_single_ai[]              = "GATEWAY_PACKET_READ_SINGLE_AI";
    PROGMEM const char gateway_packet_read_multi_ai[]               = "GATEWAY_PACKET_READ_MULTI_AI";
    PROGMEM const char gateway_packet_read_single_ao[]              = "GATEWAY_PACKET_READ_SINGLE_AO";
    PROGMEM const char gateway_packet_read_multi_ao[]               = "GATEWAY_PACKET_READ_MULTI_AO";
    PROGMEM const char gateway_packet_read_single_ev[]              = "GATEWAY_PACKET_READ_SINGLE_EV";
    PROGMEM const char gateway_packet_read_multi_ev[]               = "GATEWAY_PACKET_READ_MULTI_EV";

    PROGMEM const char * const gateway_packets_registers_reading_string[] = {
        gateway_packet_read_single_di, gateway_packet_read_multi_di,
        gateway_packet_read_single_do, gateway_packet_read_multi_do,
        gateway_packet_read_single_ai, gateway_packet_read_multi_ai,
        gateway_packet_read_single_ao, gateway_packet_read_multi_ao,
        gateway_packet_read_single_ev, gateway_packet_read_multi_ev
    };

    PROGMEM const char gateway_packet_write_one_do[]                = "GATEWAY_PACKET_WRITE_ONE_DO";
    PROGMEM const char gateway_packet_write_one_ao[]                = "GATEWAY_PACKET_WRITE_ONE_AO";
    PROGMEM const char gateway_packet_write_multi_do[]              = "GATEWAY_PACKET_WRITE_MULTI_DO";
    PROGMEM const char gateway_packet_write_multi_ao[]              = "GATEWAY_PACKET_WRITE_MULTI_AO";

    PROGMEM const char * const gateway_packets_registers_writing_string[] = {
        gateway_packet_write_one_do, gateway_packet_write_one_ao,
        gateway_packet_write_multi_do, gateway_packet_write_multi_ao
    };

    PROGMEM const char gateway_packet_ping[]                        = "GATEWAY_PACKET_PING";
    PROGMEM const char gateway_packet_pong[]                        = "GATEWAY_PACKET_PONG";
    PROGMEM const char gateway_packet_hello[]                       = "GATEWAY_PACKET_HELLO";
    PROGMEM const char gateway_packet_none[]                        = "GATEWAY_PACKET_NONE";

    PROGMEM const char * const gateway_packets_misc_string[] = {
        gateway_packet_ping, gateway_packet_pong, gateway_packet_hello, gateway_packet_none
    };

    //--------------------------------------------------------------------------------

    PROGMEM const int gateway_packets_searching[] = {
        GATEWAY_PACKET_SEARCH_NODES, GATEWAY_PACKET_SEARCH_NODES_CONFIRM,
        GATEWAY_PACKET_NODE_ADDRESS, GATEWAY_PACKET_NODE_ADDRESS_CONFIRM, GATEWAY_PACKET_ADDRESS_DISCARD
    };

    PROGMEM const int gateway_packets_node_initialization[] = {
        GATEWAY_PACKET_HW_MODEL, GATEWAY_PACKET_HW_MANUFACTURER, GATEWAY_PACKET_HW_VERSION,
        GATEWAY_PACKET_FW_MODEL, GATEWAY_PACKET_FW_MANUFACTURER, GATEWAY_PACKET_FW_VERSION,
        GATEWAY_PACKET_REGISTERS_SIZE,
        GATEWAY_PACKET_AI_REGISTERS_STRUCTURE, GATEWAY_PACKET_AO_REGISTERS_STRUCTURE
    };

    PROGMEM const int gateway_packets_registers_reading[] = {
        GATEWAY_PACKET_READ_SINGLE_DI, GATEWAY_PACKET_READ_MULTI_DI,
        GATEWAY_PACKET_READ_SINGLE_DO, GATEWAY_PACKET_READ_MULTI_DO,
        GATEWAY_PACKET_READ_SINGLE_AI, GATEWAY_PACKET_READ_MULTI_AI,
        GATEWAY_PACKET_READ_SINGLE_AO, GATEWAY_PACKET_READ_MULTI_AO,
        GATEWAY_PACKET_READ_SINGLE_EV, GATEWAY_PACKET_READ_MULTI_EV
    };

    PROGMEM const int gateway_packets_registers_writing[] = {
        GATEWAY_PACKET_WRITE_ONE_DO, GATEWAY_PACKET_WRITE_ONE_AO,
        GATEWAY_PACKET_WRITE_MULTI_DO, GATEWAY_PACKET_WRITE_MULTI_AO
    };

    PROGMEM const int gateway_packets_misc[] = {
        GATEWAY_PACKET_PING, GATEWAY_PACKET_PONG, GATEWAY_PACKET_HELLO, GATEWAY_PACKET_NONE
    };

#endif // FB_GATEWAY_SUPPORT