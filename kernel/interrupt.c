#include "drivers/pic.h"
#include "interrupt.h"
#include "gdt.h"
#include "memory.h"
#include "panic.h"
#include "print.h"

enum {
    INTERRUPT_DIVIDE_BY_ZERO = 0x0,
    INTERRUPT_INVALID_OPCODE = 0x6,
    INTERRUPT_GENERAL_PROTECTION = 0xd,
    INTERRUPT_TIMER = 0x20
};

enum gate_type {
    TRAP_GATE_32 = 0xf,
    INTERRUPT_GATE_32 = 0xe
};

enum {
    INTERRUPT_GATEWAY_SIZE = 256
};

static struct interrupt_descriptor idt[INTERRUPT_GATEWAY_SIZE];
static struct interrupt_descriptor_table table_info;

struct interrupt_segment_registers {
    uint32 gs;
    uint32 fs;
    uint32 es;
    uint32 ds;
    uint32 ss;
} __attribute__((packed));

static uint8 pack_attributes(
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
    print(p, "eip: %x\n", &stack->eip);
    print(p, "cs: %u\n", &stack->cs);
    print(p, "eflags: %u\n", &stack->eflags);
}

void interrupt_print_descriptor(struct printer* p,
        const struct interrupt_descriptor* desc) {
    uint32 offset = (uint32)((desc->offset_high << 16) | desc->offset_low);
    print(p, "offset: %x\n", &offset);
    uint32 segment_selector = desc->segment_selector;
    print(p, "segment_selector: %u\n", &segment_selector);
    uint32 reserved = desc->reserved;
    print(p, "reserved: %l\n", &reserved);
    uint32 level = (desc->attributes >> 5) & 3;
    char* privilege = "unknown";
    if (INTERRUPT_PRIVILEGE_KERNEL == level) {
        privilege = "kernel";
    }
    print(p, "privilege level: %s\n", privilege);
    uint32 handler_present = (desc->attributes >> 7) & 1;
    print(p, "handler present: %l\n", &handler_present);
    char* type = "unknown";
    if (INTERRUPT_GATE_32 == (desc->attributes & 0xf)) {
        type = "interrupt 32";
    } else if (TRAP_GATE_32 == (desc->attributes & 0xf)) {
        type = "trap 32";
    }
    print(p, "type: %s\n", type);
    uint32 storage_segment = (desc->attributes >> 4) & 1;
    print(p, "storage segment: %u\n", &storage_segment);
}

void interrupt_print_packed_descriptor(struct printer* printer,
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

static void interrupt_print_segment_registers(struct printer* p,
        const struct interrupt_segment_registers* registers) {
    print(p, "ss: %u\n", &registers->ss);
    print(p, "ds: %u\n", &registers->ds);
    print(p, "es: %u\n", &registers->es);
    print(p, "fs: %u\n", &registers->fs);
    print(p, "gs: %u\n", &registers->gs);
}

void interrupt_set_descriptor(struct interrupt_descriptor* descriptor,
        uint32 offset, uint16 segment_selector,
        enum privilege_level privilege_level) {
    descriptor->offset_low = (uint16) (offset & 0xffff);
    descriptor->offset_high = (uint16) ((offset >> 16) & 0xffff);
    descriptor->segment_selector = segment_selector;
    descriptor->reserved = 0;
    descriptor->attributes =
        pack_attributes(INTERRUPT_GATE_32, 0, privilege_level, 1);
}

void interrupt_handler(const struct interrupt_segment_registers* registers,
        const struct interrupt_stack* stack, uint32 irq) {
    struct printer p;
    p.target = PRINT_FRAMEBUFFER;

    if (INTERRUPT_DIVIDE_BY_ZERO == irq) {
        panic_with_stack("division by zero", stack, irq);
    }
    if (INTERRUPT_INVALID_OPCODE == irq) {
        panic_with_stack("invalid opcode", stack, irq);
    }
    if (INTERRUPT_GENERAL_PROTECTION == irq) {
        if (stack->error_code) {
            uint8 table = (stack->error_code >> 1) & 0x3;
            uint32 index = (stack->error_code >> 3) & 0x1fff;
            if (0 == table) {
                print(&p, "GDT index %u\n", &index);
            } else if (table & 1) {
                print(&p, "IDT index %u\n", &index);
                interrupt_print_descriptor(&p, &idt[index]);
                interrupt_print_packed_descriptor(&p, &idt[index]);
            } else if (2 == table) {
                print(&p, "LDT index %u\n", &index);
            }
        }
        interrupt_print_segment_registers(&p, registers);
        panic_with_stack("general protection fault", stack, irq);
    }
    if (INTERRUPT_TIMER == irq) {
        pic_acknowledge((uint8)irq);
        return;
    }
    if (PIC_MASTER_OFFSET + 0x1 == irq) {
        print(&p, "keyboard\n");
        pic_acknowledge((uint8) irq);
        return;
    }
    /*
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

    panic_with_stack("unknown interrupt", stack, irq);
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
    interrupt_set_descriptor(&idt[15],
            (uint32)&interrupt_handler_15, GDT_CODE_SEGMENT,
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
    interrupt_set_descriptor(&idt[255],
            (uint32)&interrupt_handler_255, GDT_CODE_SEGMENT,
            INTERRUPT_PRIVILEGE_KERNEL);

    table_info.limit = sizeof(idt) - 1;
    table_info.base = idt;
    interrupt_load_descriptor_table(&table_info);
    print(printer, "table info: .limit = %t .base = %t\n",
            &table_info.limit, &table_info.base);
    interrupt_print_descriptor(printer, &idt[0]);

}

void interrupt(uint32 irq) {
    switch (irq) {
        case 0:
            interrupt_0();
            break;
        case 1:
            interrupt_1();
            break;
        case 2:
            interrupt_2();
            break;
        case 3:
            interrupt_3();
            break;
        case 4:
            interrupt_4();
            break;
        case 5:
            interrupt_5();
            break;
        case 6:
            interrupt_6();
            break;
        case 7:
            interrupt_7();
            break;
        case 8:
            interrupt_8();
            break;
        case 9:
            interrupt_9();
            break;
        case 10:
            interrupt_10();
            break;
        case 11:
            interrupt_11();
            break;
        case 12:
            interrupt_12();
            break;
        case 13:
            interrupt_13();
            break;
        case 14:
            interrupt_14();
            break;
        case 16:
            interrupt_16();
            break;
        case 32:
            interrupt_32();
            break;
        case 33:
            interrupt_33();
            break;
        case 34:
            interrupt_34();
            break;
        case 35:
            interrupt_35();
            break;
        case 36:
            interrupt_36();
            break;
        case 37:
            interrupt_37();
            break;
        case 38:
            interrupt_38();
            break;
        case 39:
            interrupt_39();
            break;
        case 40:
            interrupt_40();
            break;
        case 41:
            interrupt_41();
            break;
        case 42:
            interrupt_42();
            break;
        case 43:
            interrupt_43();
            break;
        case 44:
            interrupt_44();
            break;
        case 45:
            interrupt_45();
            break;
        case 46:
            interrupt_46();
            break;
        case 47:
            interrupt_47();
            break;
        default:
            interrupt_255();
            break;
    }
}

