#include "drivers/framebuffer.h"
#include "drivers/pic.h"
#include "drivers/serial.h"
#include "interrupt.h"
#include "memory.h"
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
    pic_remap(0x20, 0xa0);
    pic_set_all_mask();
    pic_unset_mask(PIC_MASTER_OFFSET + 0x0);
    //pic_unset_mask(PIC_MASTER_OFFSET + 0x2);

    struct interrupt_descriptor table[256];
    memory_set(table, 0, sizeof(table) / sizeof(table[0]));
    int tabsize = sizeof(table);
    int tabsize0 = sizeof(table[0]);
    print(logger, "sizeof(table) = %d, sizeof(table[0]) = %d\n",
            &tabsize, &tabsize0);

    print(logger, "loading interrupt descriptor table\n");
    unsigned int idtr_size = sizeof(struct interrupt_descriptor_table);
    print(logger, "size of IDTR struct is %u\n", &idtr_size);

    /* Hard code the code segment offset in the global descriptor table. */
    interrupt_set_descriptor(&table[0],
            (unsigned int)&interrupt_handler_0, 0x8, 0);
    interrupt_set_descriptor(&table[1],
            (unsigned int)&interrupt_handler_1, 0x8, 0);

    struct interrupt_descriptor_table table_info;
    table_info.limit = (sizeof(table) / sizeof(table[0])) - 1;
    table_info.base = table;
    interrupt_load_descriptor_table(&table_info);

    print(logger, "table info: .limit = %t .base = %t\n",
            &table_info.limit, &table_info.base);
    unsigned int sizeoftable = sizeof(table);
    unsigned int sizeoftable0 = sizeof(table[0]);
    print(logger, "sizeof(table) = %t, sizeof(table[0]) = %t\n",
            &sizeoftable, &sizeoftable0);


    interrupt_print_descriptor(logger, &table[0]);

    print(logger, "enabling interrupt\n");
    interrupt_enable();
    print(logger, "testing interrupt\n");
    interrupt_test();
    print(logger, "test again\n");
    interrupt_test();

    /*
    print(logger, "dividing by zero\n");
    int y = 0;
    int x = 1 / y;
    print(logger, "x = %d", &x);
    */

    print(logger, "exiting kernel\n");
    while (1) {
    }
}

