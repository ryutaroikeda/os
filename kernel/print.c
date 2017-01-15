#include "drivers/framebuffer.h"
#include "drivers/serial.h"
#include "print.h"

const struct serial_line_configuration
LINE_CONFIG = { 1, 0x3, 0, 0, 0 };

const struct serial_buffer_configuration
BUFFER_CONFIG = { 3, 0, 1, 1, 1 };

enum {
    FORMAT_PERCENT = '%',
    FORMAT_INT = 'd',
    FORMAT_FLOAT = 'f',
    FORMAT_CHAR = 'c',
    FORMAT_STRING = 's'
};

enum {
    MAX_STRING = 1024
};

int print_0(struct printer* printer, const char* format, int len) {
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

static int print_int(struct printer* printer, int value) {
    if (0 == value) {
        print_char(printer, '0');
        return 1;
    }
    int initial_offset = printer->buffer_offset;
    if (value < 0) {
        print_char(printer, '-');
        value *= -1;
    }
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

static int print_string(struct printer* printer, char* value) {
    int initial_offset = printer->buffer_offset;
    int offset = 0;
    while (value[offset]) {
        print_char(printer, value[offset]);
        offset += 1;
    }
    return printer->buffer_offset - initial_offset;
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
    int d = (int) value;
    print_int(printer, d);
    return printer->buffer_offset - initial_offset;
}

int print_1(struct printer* printer, const char* format,
        /*struct print_argument* args,*/
        struct print_argument* args,
        int arg_len) {
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
            print_int(printer, args[arg_num].value.d);
        } else if (FORMAT_STRING == format[in]) {
            print_string(printer, args[arg_num].value.s);
        } else if (FORMAT_CHAR == format[in]) {
            print_char(printer, args[arg_num].value.c);
        } else if (FORMAT_FLOAT == format[in]) {
            print_float(printer, args[arg_num].value.f);
        } else {
            /** this should not happen */
            return -1;
        }
        in += 1;
        arg_num += 1;
    }
    return print_0(printer, printer->buffer, printer->buffer_offset);
}

