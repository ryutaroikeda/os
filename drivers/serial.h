#ifndef _serial_h_
#define _serial_h_

#include "lib/bool.h"
#include "port.h"

enum {
    SERIAL_COM_1_BASE = 0x3f8,
    SERIAL_COM_2_BASE = 0x2f8,
    SERIAL_COM_3_BASE = 0x3e8,
    SERIAL_COM_4_BASE = 0x2e8
};

struct serial_port {
    Port port;
    bool is_configured;
};

struct serial_line_configuration {
    unsigned short baud_rate_divisor;
    unsigned char character_length;
    unsigned char stop_bits_length;
    unsigned char parity;
    unsigned char interrupt_enable;
    /** unsigned char break_control; */
};

struct serial_buffer_configuration {
    unsigned char buffer_size;
    bool enable_64_byte_fifo;
    bool clear_transmit_fifo;
    bool clear_receive_fifo;
    bool enable_fifo;
};

void serial_configure_line(struct serial_port,
        struct serial_line_configuration);

void serial_configure_buffer(struct serial_port,
        struct serial_buffer_configuration);

void serial_initialize(struct serial_port, struct serial_line_configuration,
        struct serial_buffer_configuration);

void serial_write(struct serial_port, const char* s, int len);

#endif

