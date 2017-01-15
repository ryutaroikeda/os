#include "kernel/bool.h"
#include "port.h"
#include "serial.h"

#define DATA_REGISTER(base) (Port) (base)
#define INTERRUPT_ENABLE_REGISTER(base) (Port) ((base) + 1)
#define BAUD_LOW_REGISTER(base) (Port) (base)
#define BAUD_HIGH_REGISTER(base) (Port) ((base) + 1)
#define FIFO_CONTROL_REGISTER(base) (Port) ((base) + 2)
#define LINE_CONTROL_REGISTER(base) (Port) ((base) + 3)
#define MODEM_CONTROL_REGISTER(base) (Port) ((base) + 4)
#define LINE_STATUS_REGISTER(base) (Port) ((base) + 5)
#define MODEM_STATUS_REGISTER(base) (Port) ((base) + 6)
#define SCRATCH_REGISTER(base) (Port) ((base) + 7)

/**
 * DLAB stands for divisor latch access bit.
 * When DLAB is set to 1, offsets 0 and 1 (from the base address of the port) 
 * are mapped to the low and high bytes the divisor registers for setting the
 * baud rate.
 * DLAB is a bit in the line control register.
 */
#define LINE_ENABLE_DLAB 0x80

/**
 * Set the divisor for the baud rate. This will mangle the serial line
 * register.
 * @param port The communication port to configure
 * @param divisor The baud rate is set to 115200 bits per second divided by
 * the divsor.
 */
static void configure_baud_rate(struct serial_port port,
        unsigned short divisor) {
    unsigned char low = (unsigned char) (divisor & 0xff);
    unsigned char high = (unsigned char) ((divisor >> 8) & 0xff);
    port_byte_out(LINE_CONTROL_REGISTER(port.port), LINE_ENABLE_DLAB);
    port_byte_out(BAUD_LOW_REGISTER(port.port), low);
    port_byte_out(BAUD_HIGH_REGISTER(port.port), high);
    port_byte_out(LINE_CONTROL_REGISTER(port.port), 0x00);
}

static unsigned char get_line_configuration_bits(
        struct serial_line_configuration config) {
    return (unsigned char) (
            (config.character_length & 0x03) |
            ((config.stop_bits_length & 0x01) << 2) |
            ((config.parity & 0x07) << 3));
}

void serial_configure_line(struct serial_port port,
        struct serial_line_configuration config) {
    configure_baud_rate(port, config.baud_rate_divisor);
    port_byte_out(LINE_CONTROL_REGISTER(port.port),
            get_line_configuration_bits(config));
}

static unsigned char get_buffer_configuration_bits(
        struct serial_buffer_configuration config) {
    return (unsigned char) (
            (config.enable_fifo & 0x01) |
            ((config.clear_receive_fifo & 0x01) << 1) |
            ((config.clear_transmit_fifo & 0x01) << 2) |
            ((config.enable_64_byte_fifo & 0x01) << 5) |
            ((config.buffer_size & 0x03) << 6));
}

void serial_configure_buffer(struct serial_port port,
        struct serial_buffer_configuration config) {
    port_byte_out(FIFO_CONTROL_REGISTER(port.port),
            get_buffer_configuration_bits(config));
}

static bool transmission_buffer_is_empty(Port port) {
    /** The transmission buffer is empty if bit 5 of the line status register
     * is set. */
    unsigned char status = port_byte_in(LINE_STATUS_REGISTER(port));
    return status & (1 << 5);
}

void serial_write(struct serial_port port, const char* s, unsigned int len) {
    while (true) {
        if (transmission_buffer_is_empty(port.port)) { break; }
    }
    for (unsigned int i = 0; i < len; i++) {
        if (!s[i]) { break; }
        port_byte_out(DATA_REGISTER(port.port), (unsigned char) s[i]);
    }
}

