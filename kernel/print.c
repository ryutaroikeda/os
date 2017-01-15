#include "drivers/framebuffer.h"
#include "drivers/serial.h"
#include "print.h"

static const struct serial_line_configuration
LINE_CONFIG = { 1, 0x3, 0, 0, 0 };

static const struct serial_buffer_configuration
BUFFER_CONFIG = { 3, 0, 1, 1, 1 };

int print0(struct printer printer, const char* format) {
    if (PRINT_FRAMEBUFFER == printer.target) {
        framebuffer_write(format, 9999);
    } else if (PRINT_SERIAL_COM_1 == printer.target) {
        if (! printer.port.is_configured) {
            serial_configure_line(printer.port, LINE_CONFIG);
            serial_configure_buffer(printer.port, BUFFER_CONFIG);
            printer.port.is_configured = true;
        }
        serial_write(printer.port, format, 9999);
    } else {
        /** this should not happen */
        return -1;
    }
    return 0;
}

