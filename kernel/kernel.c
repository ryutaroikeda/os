#include "drivers/framebuffer.h"
#include "drivers/keyboard.h"
#include "drivers/pic.h"
#include "drivers/pit.h"
#include "drivers/serial.h"
#include "idle.h"
#include "interrupt.h"
#include "lib/integer.h"
#include "lib/print.h"

void main(void);

void main(void) {
	framebuffer_clear_screen();
	struct framebuffer_offset offset = {0, 0};
	framebuffer_set_cursor(offset);

    struct printer p;
    p.target = PRINT_FRAMEBUFFER;
    struct printer* printer = &p;

    print(printer, "hello");

    while (1) {
        idle();
    }
    return;

    /*
    struct serial_port port = {
        .port = SERIAL_COM_1_BASE,
        .is_configured = false
    };

    struct serial_line_configuration lcnf = {
        .baud_rate_divisor = 1,
        .character_length = 3,
        .stop_bits_length = 0,
        .parity = 0,
        .interrupt_enable = 0
    };

    const struct serial_buffer_configuration
    bcnf = {
        .buffer_size = 3,
        .enable_64_byte_fifo = 0,
        .clear_transmit_fifo = 1,
        .clear_receive_fifo = 1,
        .enable_fifo = 1
    };
    */

    //serial_initialize(port, lcnf, bcnf);

    //serial_write(port, "h", 1);
    //serial_write(port, "hello world\n", 12);

    /*
    struct printer l;
    l.target = PRINT_SERIAL_COM_1;
    l.port = port;
    struct printer* logger = &l;
    */

    //print(logger, "starting kernel\n");
 //   print(logger, "remapping PIC\n");
//    serial_write(port, "starting kernel\n", 26);
    //serial_write("remapping PIC\n");
    //pic_remap();
    //pic_set_all_mask();

    //panic_with_printer(logger, "panic");

    // configure timer
    //pit_initialize();

    //keyboard_initialize(&keyboard_global, logger);

    //print(logger, "initializing interrupt handlers\n");
    //interrupt_initialize(logger);

    /*
    print(logger, "panic");
    return;
    panic("panic");
    */

    //print(logger, "enabling interrupt\n");
    //interrupt_enable();

    //pic_unset_mask(PIC_MASTER_OFFSET);
    //pic_unset_mask(PIC_MASTER_OFFSET + 0x1);
    //pic_unset_mask(PIC_MASTER_OFFSET + 0x2);
    //pic_unset_mask(PIC_SLAVE_OFFSET);

    //print(logger, "exiting kernel\n");
    while (1) {
        idle();
    }
}

