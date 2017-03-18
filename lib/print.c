#include "drivers/framebuffer.h"
#include "drivers/serial.h"
#include "lib/integer.h"
#include "lib/queue.h"
#include "lib/null.h"
#include "print.h"

const struct serial_line_configuration
LINE_CONFIG = {
    .baud_rate_divisor = 3,
    .character_length = 0x3,
    .stop_bits_length = 0,
    .parity = 0,
    .interrupt_enable = 0
};

const struct serial_buffer_configuration
BUFFER_CONFIG = {
    .buffer_size = 3,
    .enable_64_byte_fifo = 0,
    .clear_transmit_fifo = 1,
    .clear_receive_fifo = 1,
    .enable_fifo = 1
};

enum {
    FORMAT_PERCENT = '%',
    FORMAT_INT = 'd',
    FORMAT_UNSIGNED_INT = 'u',
    FORMAT_UINT16 = 't',
    FORMAT_BYTE = 'b',
    FORMAT_FLOAT = 'f',
    FORMAT_CHAR = 'c',
    FORMAT_STRING = 's',
    FORMAT_HEX = 'x',
    FORMAT_BOOL = 'l'
};

enum {
    MAX_STRING = 1024
};

struct print_command {
    char* format;
    struct print_argument* args;
    uint16 arg_size;
};

static int print_to_device(struct printer* printer, const char* format,
        int len) {
    if (PRINT_FRAMEBUFFER == printer->target) {
        framebuffer_write(format, len);
    } else if (PRINT_SERIAL_COM_1 == printer->target) {
        if (! printer->port.is_configured) {
            serial_configure_line(printer->port, LINE_CONFIG);
            serial_configure_buffer(printer->port, BUFFER_CONFIG);
            printer->port.is_configured = true;
        }
        serial_write(printer->port, format, len);
    } else {
        /** this should not happen */
        return -1;
    }
    return len;
}

static int print_char(struct printer* printer, char value) {
    if (PRINT_BUFFER_SIZE <= printer->buffer_offset) {
        return 0;
    }
    printer->buffer[printer->buffer_offset] = value;
    printer->buffer_offset += 1;
    return 1;
}

static int print_uint16(struct printer* printer, uint16 value) {
    if (0 == value) {
        print_char(printer, '0');
        return 1;
    }
    int initial_offset = printer->buffer_offset;
    char buffer[32];
    int offset = 0;
    while (0 < value) {
        buffer[offset] = (char) ((value % 10) + '0');
        offset += 1;
        value /= 10;
    }
    for (int i = 0; i < offset; i++) {
        print_char(printer, buffer[offset - 1 - i]);
    }
    return printer->buffer_offset - initial_offset;
}

static int print_uint32(struct printer* printer, uint32 value) {
    if (0 == value) {
        print_char(printer, '0');
        return 1;
    }
    int initial_offset = printer->buffer_offset;
    char buffer[32];
    int offset = 0;
    while (0 < value) {
        buffer[offset] = (char) ((value % 10) + '0');
        offset += 1;
        value /= 10;
    }
    for (int i = 0; i < offset; i++) {
        print_char(printer, buffer[offset - 1 - i]);
    }
    return printer->buffer_offset - initial_offset;
}

static int print_byte(struct printer* printer, unsigned char value) {
    int initial_offset = printer->buffer_offset;
    char buffer[32];
    for (int i = 0; i < 2; i++) {
        char c = (char) (value & 0xf);
        if (10 <= c) {
            c = (char) (c - 10 + 'a');
        } else {
            c = (char) (c + '0');
        }
        buffer[i] = c;
        value = value >> 4;
    }
    print_char(printer, buffer[1]);
    print_char(printer, buffer[0]);
    return printer->buffer_offset - initial_offset;
}

static int print_hex(struct printer* printer, uint32 value) {
    int result = 0;
    result += print_byte(printer, (uint8) ((value >> 24) & 0xff));
    result += print_byte(printer, (uint8) ((value >> 16) & 0xff));
    result += print_byte(printer, (uint8) ((value >> 8) & 0xff));
    result += print_byte(printer, (uint8) (value & 0xff));
    return result;
}

static int print_int32(struct printer* printer, int32 value) {
    if (value < 0) {
        print_char(printer, '-');
        value *= -1;
    }
    return print_uint32(printer, (uint32) value);
}

static int print_string(struct printer* printer, const char* value) {
    int initial_offset = printer->buffer_offset;
    int offset = 0;
    while (value[offset]) {
        print_char(printer, value[offset]);
        offset += 1;
    }
    return printer->buffer_offset - initial_offset;
}

static int print_bool(struct printer* printer, uint32 value) {
    char* result = "false";
    if (value) {
        result = "true";
    }
    return print_string(printer, result);
}

/**
 * @todo
 */
static int print_float(struct printer* printer, float value) {
    int initial_offset = printer->buffer_offset;
    if (value < 0) {
        print_char(printer, '-');
        value *= -1.0f;
    }
    int d = (int32) value;
    print_int32(printer, d);
    return printer->buffer_offset - initial_offset;
}

int print_n(struct printer* printer, const char* format,
        const struct print_argument* args, int arg_len) {
    printer->buffer_offset = 0;
    int arg_num = 0;
    int in = 0;
    for (int i = 0; i < MAX_STRING; i++) {
        if (!format[in]) {
            break;
        }
        if ('%' != format[in]) {
            print_char(printer, format[in]);
            in += 1;
            continue;
        }
        in += 1;
        if (FORMAT_PERCENT == format[in]) {
            print_char(printer, '%');
            continue;
        }
        if (arg_len <= arg_num) {
            return -1;
        }
        if (FORMAT_INT == format[in]) {
            print_int32(printer, *(const int32*)args[arg_num].value);
        } else if (FORMAT_UNSIGNED_INT == format[in]) {
            print_uint32(printer,
                    *(const uint32*)args[arg_num].value);
        } else if (FORMAT_UINT16 == format[in]) {
            print_uint16(printer, *(const uint16*)args[arg_num].value);
        } else if (FORMAT_BYTE == format[in]) {
            print_byte(printer,
                    *(const unsigned char*)args[arg_num].value);
        } else if (FORMAT_STRING == format[in]) {
            print_string(printer, (const char*)args[arg_num].value);
        } else if (FORMAT_CHAR == format[in]) {
            print_char(printer, *(const char*)args[arg_num].value);
        } else if (FORMAT_FLOAT == format[in]) {
            print_float(printer, *(const float*)args[arg_num].value);
        } else if (FORMAT_BOOL == format[in]) {
            print_bool(printer, *(const uint32*)args[arg_num].value);
        } else if (FORMAT_HEX == format[in]) {
            print_hex(printer, *(const uint32*)args[arg_num].value);
        } else {
            /** this should not happen */
            return -1;
        }
        in += 1;
        arg_num += 1;
    }
    return print_to_device(printer, printer->buffer, printer->buffer_offset);
}

int print_0(struct printer* printer, const char* format) {
    return print_n(printer, format, NULL, 0);
}

int print_1(struct printer* printer, const char* format, const void* first) {
    const struct print_argument args[] = {{first}};
    return print_n(printer, format, args, 1);
}

int print_2(struct printer* printer, const char* format, const void* first,
        const void* second) {
    const struct print_argument args[] = {{first}, {second}};
    return print_n(printer, format, args, 2);
}

int print_3(struct printer* printer, const char* format, const void* first,
        const void* second, const void* third) {
    const struct print_argument args[] = {{first}, {second}, {third}};
    return print_n(printer, format, args, 3);
}

int print_4(struct printer* printer, const char* format, const void* first,
        const void* second, const void* third, const void* fourth) {
    const struct print_argument args[] = {{first}, {second}, {third},
        {fourth}};
    return print_n(printer, format, args, 4);
}

int print_5(struct printer* printer, const char* format, const void* first,
        const void* second, const void* third, const void* fourth,
        const void* fifth) {
    const struct print_argument args[] = {{first}, {second}, {third},
        {fourth}, {fifth}};
    return print_n(printer, format, args, 5);
}

