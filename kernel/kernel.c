#include "drivers/framebuffer.h"
#include "drivers/serial.h"
#include "print.h"

void main(void);

void main(void) {
	framebuffer_clear_screen();
	struct framebuffer_offset offset = {0, 0};
	framebuffer_set_cursor(offset);

    struct serial_port port = {
        SERIAL_COM_1_BASE,
        false
    };
    struct printer printer = { PRINT_FRAMEBUFFER, port };
    print0(printer, "hello world!\n");
    print0(printer, "hola mundo!\n");

    printer.target = PRINT_SERIAL_COM_1;
    print0(printer, "testing serial write with printer");

    printer.target = PRINT_FRAMEBUFFER;
    print0(printer, "exiting");
}

