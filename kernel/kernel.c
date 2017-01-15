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
    struct printer p;
    p.target = PRINT_FRAMEBUFFER;
    p.port = port;
    struct printer* printer = &p;

    print_0(printer, "hello world!\n", 100);
    print_0(printer, "hola mundo!\n", 100);

    printer->target = PRINT_SERIAL_COM_1;
    print_0(printer, "testing serial write\nwith printer", 100);

    printer->target = PRINT_FRAMEBUFFER;
    int a = 100;
    struct print_argument args[] = {
        { &a }
    };
    print_1(printer, "printing number: %d\n", args, 1);
    print_0(printer, "exiting", 100);
}

