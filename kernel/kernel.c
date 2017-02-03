#include "drivers/framebuffer.h"
#include "drivers/pic.h"
#include "drivers/pit.h"
#include "drivers/serial.h"
#include "integer.h"
#include "interrupt.h"
#include "print.h"

void main(void);

void main(void) {
	framebuffer_clear_screen();
	struct framebuffer_offset offset = {0, 0};
	framebuffer_set_cursor(offset);

    /*
    struct printer p;
    p.target = PRINT_FRAMEBUFFER;
    struct printer* printer = &p;
    */

    struct serial_port port = {
        SERIAL_COM_1_BASE,
        false
    };

    struct printer l;
    l.target = PRINT_SERIAL_COM_1;
    l.port = port;
    struct printer* logger = &l;

    print(logger, "starting kernel\n");
    print(logger, "remapping PIC\n");
    pic_remap();
    pic_set_all_mask();

    // configure timer
    pit_initialize();

    print(logger, "initializing interrupt handlers\n");
    interrupt_initialize(logger);
    print(logger, "enabling interrupt\n");
    interrupt_enable();

    pic_unset_mask(PIC_MASTER_OFFSET + 0x0);
    //pic_unset_mask(PIC_MASTER_OFFSET + 0x1);
    //pic_unset_mask(PIC_MASTER_OFFSET + 0x2);
    //pic_unset_mask(PIC_SLAVE_OFFSET);

    //interrupt(1);

    print(logger, "exiting kernel\n");
    while (1) {
    }
}

