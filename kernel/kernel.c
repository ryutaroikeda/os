#include "drivers/framebuffer.h"
#include "drivers/pic.h"
#include "drivers/serial.h"
#include "gdt.h"
#include "integer.h"
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
    pic_remap(PIC_MASTER_PORT, PIC_SLAVE_PORT);
    pic_set_all_mask();
    /*
    pic_unset_mask(PIC_MASTER_OFFSET + 0x2);
    pic_unset_mask(PIC_SLAVE_OFFSET + 0x7);
    */
    /*
    pic_unset_mask(PIC_MASTER_PORT + 0x0);
    pic_unset_mask(PIC_MASTER_PORT + 0x1);
    */

    struct interrupt_descriptor table[256];
    memory_set(table, 0, sizeof(table));
    int tabsize = sizeof(table);
    int tabsize0 = sizeof(table[0]);
    print(logger, "sizeof(table) = %d, sizeof(table[0]) = %d\n",
            &tabsize, &tabsize0);

    print(logger, "loading interrupt descriptor table\n");
    uint32 idtr_size = sizeof(struct interrupt_descriptor_table);
    print(logger, "size of IDTR struct is %u\n", &idtr_size);

    interrupt_set_descriptor(&table[0],
            (uint32)&interrupt_handler_0, GDT_CODE_SEGMENT,
            INTERRUPT_PRIVILEGE_KERNEL);
    interrupt_set_descriptor(&table[6],
            (uint32)&interrupt_handler_6, GDT_CODE_SEGMENT,
            INTERRUPT_PRIVILEGE_KERNEL);
    interrupt_set_descriptor(&table[8],
            (uint32)&interrupt_handler_8, GDT_CODE_SEGMENT,
            INTERRUPT_PRIVILEGE_KERNEL);
    /*
    interrupt_set_descriptor(&table[1],
            (unsigned int)&interrupt_handler_1, GDT_CODE_SEGMENT,
            INTERRUPT_PRIVILEGE_KERNEL);
            */

    struct interrupt_descriptor_table table_info;
    table_info.limit = (sizeof(table) / sizeof(table[0])) - 1;
    table_info.base = table;
    interrupt_load_descriptor_table(&table_info);
    print(logger, "table info: .limit = %t .base = %t\n",
            &table_info.limit, &table_info.base);
    interrupt_print_descriptor(logger, &table[0]);

    uint32 iss = sizeof(struct interrupt_stack);
    print(logger, "sizeof(struct interrupt_stack) = %u\n", &iss);

    print(logger, "enabling interrupt\n");
    interrupt_enable();

    print(logger, "dividing by zero\n");
    int y = 0;
    int a = 1 / y;
    print(logger, "%d", &a);

    print(logger, "exiting kernel\n");
    while (1) {
    }
}

