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

    struct printer logger;
    logger.target = PRINT_SERIAL_COM_1;
    logger.port = port;
    struct printer* l = &logger;

    print(printer, "hello world!\n");
    print(printer, "hola mundo!\n");

    print(l, "testing serial write\nwith printer\n");

    print(printer, "printing string: %s\n", "arg string");
    int test = 10;
    print(printer, "printing number: %d\n", &test);
    print(printer, "printing number %d and string %s\n", &test, "testing");
    const int things = 5;
    print(printer, "printing %d things. %s, %s, %s, and %s!\n",
            &things, "this", "that", "those", "these");
    for (int i = 0; i < 20; i++) {
        int bytes = print(printer, "row %d\n", &i);
        print(l, "printed %d bytes\n", &bytes);
    }
    for (int i = 0; i < 80; i++) {
        print(printer, "x");
    }
    print(printer, "exiting");
}

