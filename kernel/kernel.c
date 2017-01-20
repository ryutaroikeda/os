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

    print(printer, "hello world!\n");
    print(printer, "hola mundo!\n");

    printer->target = PRINT_SERIAL_COM_1;
    print(printer, "testing serial write\nwith printer");

    printer->target = PRINT_FRAMEBUFFER;
    print(printer, "printing string: %s\n", "arg string");
    int test = 10;
    print(printer, "printing number: %d\n", &test);
    print(printer, "printing number %d and string %s\n", &test, "testing");
    print(printer, "exiting");
}

