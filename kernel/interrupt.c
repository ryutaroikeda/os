#include "drivers/pic.h"
#include "interrupt.h"
#include "gdt.h"
#include "memory.h"
#include "panic.h"
#include "print.h"

enum {
    INTERRUPT_DIVIDE_BY_ZERO = 0x0,
    INTERRUPT_INVALID_OPCODE = 0x6,
    INTERRUPT_GENERAL_PROTECTION = 0xd
};

enum gate_type {
    TRAP_GATE_32 = 0xf
};

enum {
    INTERRUPT_GATEWAY_SIZE = 256
};

static struct interrupt_descriptor idt[INTERRUPT_GATEWAY_SIZE];
static struct interrupt_descriptor_table table_info;

static unsigned char pack_attributes(
        enum gate_type type, int storage_segment,
        enum privilege_level level, int handler_present) {
    return (unsigned char) (
            (unsigned int) (type & 0xf) +
            (unsigned int) ((storage_segment & 0x1) << 4) +
            (unsigned int) ((level & 0x3) << 5) +
            (unsigned int) ((handler_present & 0x1) << 7));
}

void interrupt_print_stack(struct printer* p,
        const struct interrupt_stack* stack) {
    print(p, "error_code: %u\n", &stack->error_code);
    print(p, "eip: %u\n", &stack->eip);
    print(p, "cs: %u\n", &stack->cs);
    print(p, "eflags: %u\n", &stack->eflags);
}

void interrupt_print_descriptor(struct printer* printer,
        const struct interrupt_descriptor* desc) {
    const char* bytes = (const char*) desc;
    for (unsigned int i = 0; i < 4; i++) {
        print(printer, "%b", &bytes[i]);
    }
    print(printer, "\n");
    for (unsigned int i = 4; i < 8; i++) {
        print(printer, "%b", &bytes[i]);
    }
    print(printer, "\n");
}

void interrupt_set_descriptor(struct interrupt_descriptor* descriptor,
        unsigned int offset, unsigned short segment_selector,
        enum privilege_level privilege_level) {
    descriptor->offset_low = (unsigned short) (offset & 0xffff);
    descriptor->offset_high = (unsigned short) ((offset >> 16) & 0xffff);
    descriptor->segment_selector = segment_selector;
    descriptor->reserved = 0;
    descriptor->attributes =
        pack_attributes(TRAP_GATE_32, 0, privilege_level, 1);
}

void interrupt_handler(const struct interrupt_stack* stack, uint32 irq) {
    //struct printer p;
    //p.target = PRINT_FRAMEBUFFER;

    if (INTERRUPT_DIVIDE_BY_ZERO == irq) {
        panic(stack, irq, "division by zero");
    }
    if (INTERRUPT_INVALID_OPCODE == irq) {
        panic(stack, irq, "invalid opcode");
    }
    if (INTERRUPT_GENERAL_PROTECTION == irq) {
        panic(stack, irq, "general protection fault");
    }
    // causes gpf, maybe because we don't initialize the timer
    // gpf happens without this
    /*
    if (PIC_MASTER_OFFSET + 0x0 == irq) {
        print(&p, "timer chip\n");
        pic_acknowledge((uint8) irq);
        return;
    }
    */
    /*
    if (PIC_MASTER_OFFSET + 0x1 == irq) {
        print(&p, "keyboard\n");
        pic_acknowledge((uint8) irq);
        return;
    }
    if (PIC_MASTER_OFFSET + 0x7 == irq) {
        // Is this spurious? 
        uint16 isr = pic_get_in_service_register();
        if (!(isr & (1 << 7))) {
            // We have a spurious interrupt. Don't acknowledge.
            interrupt_print_stack(&p, stack);
            print(&p, "got spurious interrupt\n");
            return;
        }
    }
    if (PIC_SLAVE_OFFSET + 0x7 == irq) {
        // Is this spurious?
        uint16 isr = pic_get_in_service_register();
        if (!(isr & (1 << 15))) {
            // We have a spurious interrupt. Tell the master PIC
            pic_acknowledge((uint8) (PIC_MASTER_OFFSET + 0x2));
            interrupt_print_stack(&p, stack);
            print(&p, "got spurious interrupt 2");
            return;
        }
    }
    */

    panic(stack, irq, "unknown interrupt");
}

void interrupt_initialize(struct printer* printer) {
    memory_set(idt, 0, sizeof(idt));
    memory_set(&table_info, 0, sizeof(struct interrupt_descriptor_table));
    int tabsize = sizeof(idt);
    int tabsize0 = sizeof(idt[0]);
    print(printer, "sizeof(idt) = %d, sizeof(idt[0]) = %d\n",
            &tabsize, &tabsize0);

    print(printer, "loading interrupt descriptor table\n");
    uint32 idtr_size = sizeof(struct interrupt_descriptor_table);
    print(printer, "size of IDTR struct is %u\n", &idtr_size);

    interrupt_set_descriptor(&idt[0],
            (uint32)&interrupt_handler_0, GDT_CODE_SEGMENT,
            INTERRUPT_PRIVILEGE_KERNEL);
    interrupt_set_descriptor(&idt[1],
            (uint32)&interrupt_handler_1, GDT_CODE_SEGMENT,
            INTERRUPT_PRIVILEGE_KERNEL);
    interrupt_set_descriptor(&idt[3],
            (uint32)&interrupt_handler_3, GDT_CODE_SEGMENT,
            INTERRUPT_PRIVILEGE_KERNEL);
    interrupt_set_descriptor(&idt[4],
            (uint32)&interrupt_handler_4, GDT_CODE_SEGMENT,
            INTERRUPT_PRIVILEGE_KERNEL);
    interrupt_set_descriptor(&idt[5],
            (uint32)&interrupt_handler_5, GDT_CODE_SEGMENT,
            INTERRUPT_PRIVILEGE_KERNEL);
    interrupt_set_descriptor(&idt[6],
            (uint32)&interrupt_handler_6, GDT_CODE_SEGMENT,
            INTERRUPT_PRIVILEGE_KERNEL);
    interrupt_set_descriptor(&idt[7],
            (uint32)&interrupt_handler_7, GDT_CODE_SEGMENT,
            INTERRUPT_PRIVILEGE_KERNEL);
    interrupt_set_descriptor(&idt[8],
            (uint32)&interrupt_handler_8, GDT_CODE_SEGMENT,
            INTERRUPT_PRIVILEGE_KERNEL);
    interrupt_set_descriptor(&idt[9],
            (uint32)&interrupt_handler_9, GDT_CODE_SEGMENT,
            INTERRUPT_PRIVILEGE_KERNEL);
    interrupt_set_descriptor(&idt[10],
            (uint32)&interrupt_handler_10, GDT_CODE_SEGMENT,
            INTERRUPT_PRIVILEGE_KERNEL);
    interrupt_set_descriptor(&idt[11],
            (uint32)&interrupt_handler_11, GDT_CODE_SEGMENT,
            INTERRUPT_PRIVILEGE_KERNEL);
    interrupt_set_descriptor(&idt[12],
            (uint32)&interrupt_handler_12, GDT_CODE_SEGMENT,
            INTERRUPT_PRIVILEGE_KERNEL);
    interrupt_set_descriptor(&idt[13],
            (uint32)&interrupt_handler_13, GDT_CODE_SEGMENT,
            INTERRUPT_PRIVILEGE_KERNEL);
    interrupt_set_descriptor(&idt[14],
            (uint32)&interrupt_handler_14, GDT_CODE_SEGMENT,
            INTERRUPT_PRIVILEGE_KERNEL);
    interrupt_set_descriptor(&idt[16],
            (uint32)&interrupt_handler_16, GDT_CODE_SEGMENT,
            INTERRUPT_PRIVILEGE_KERNEL);

    interrupt_set_descriptor(&idt[32],
            (uint32)&interrupt_handler_32, GDT_CODE_SEGMENT,
            INTERRUPT_PRIVILEGE_KERNEL);
    interrupt_set_descriptor(&idt[33],
            (uint32)&interrupt_handler_33, GDT_CODE_SEGMENT,
            INTERRUPT_PRIVILEGE_KERNEL);
    interrupt_set_descriptor(&idt[34],
            (uint32)&interrupt_handler_34, GDT_CODE_SEGMENT,
            INTERRUPT_PRIVILEGE_KERNEL);
    interrupt_set_descriptor(&idt[35],
            (uint32)&interrupt_handler_35, GDT_CODE_SEGMENT,
            INTERRUPT_PRIVILEGE_KERNEL);
    interrupt_set_descriptor(&idt[36],
            (uint32)&interrupt_handler_36, GDT_CODE_SEGMENT,
            INTERRUPT_PRIVILEGE_KERNEL);
    interrupt_set_descriptor(&idt[37],
            (uint32)&interrupt_handler_37, GDT_CODE_SEGMENT,
            INTERRUPT_PRIVILEGE_KERNEL);
    interrupt_set_descriptor(&idt[38],
            (uint32)&interrupt_handler_38, GDT_CODE_SEGMENT,
            INTERRUPT_PRIVILEGE_KERNEL);
    interrupt_set_descriptor(&idt[39],
            (uint32)&interrupt_handler_39, GDT_CODE_SEGMENT,
            INTERRUPT_PRIVILEGE_KERNEL);

    interrupt_set_descriptor(&idt[40],
            (uint32)&interrupt_handler_40, GDT_CODE_SEGMENT,
            INTERRUPT_PRIVILEGE_KERNEL);
    interrupt_set_descriptor(&idt[41],
            (uint32)&interrupt_handler_41, GDT_CODE_SEGMENT,
            INTERRUPT_PRIVILEGE_KERNEL);
    interrupt_set_descriptor(&idt[42],
            (uint32)&interrupt_handler_42, GDT_CODE_SEGMENT,
            INTERRUPT_PRIVILEGE_KERNEL);
    interrupt_set_descriptor(&idt[43],
            (uint32)&interrupt_handler_43, GDT_CODE_SEGMENT,
            INTERRUPT_PRIVILEGE_KERNEL);
    interrupt_set_descriptor(&idt[44],
            (uint32)&interrupt_handler_44, GDT_CODE_SEGMENT,
            INTERRUPT_PRIVILEGE_KERNEL);
    interrupt_set_descriptor(&idt[45],
            (uint32)&interrupt_handler_45, GDT_CODE_SEGMENT,
            INTERRUPT_PRIVILEGE_KERNEL);
    interrupt_set_descriptor(&idt[46],
            (uint32)&interrupt_handler_46, GDT_CODE_SEGMENT,
            INTERRUPT_PRIVILEGE_KERNEL);
    interrupt_set_descriptor(&idt[47],
            (uint32)&interrupt_handler_47, GDT_CODE_SEGMENT,
            INTERRUPT_PRIVILEGE_KERNEL);

    table_info.limit = (sizeof(idt) / sizeof(idt[0])) - 1;
    table_info.base = idt;
    interrupt_load_descriptor_table(&table_info);
    print(printer, "table info: .limit = %t .base = %t\n",
            &table_info.limit, &table_info.base);
    interrupt_print_descriptor(printer, &idt[0]);

}

