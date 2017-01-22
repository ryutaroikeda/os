#include "drivers/pic.h"
#include "interrupt.h"
#include "print.h"

enum {
    INTERRUPT_DIVIDE_BY_ZERO = 0
};

enum gate_type {
    TRAP_GATE_32 = 0xf
};

static unsigned char pack_attributes(
        enum gate_type type, int storage_segment,
        enum privilege_level level, int handler_present) {
    return (unsigned char) (
            (unsigned int) (type & 0xf) +
            (unsigned int) ((storage_segment & 0x1) << 4) +
            (unsigned int) ((level & 0x3) << 5) +
            (unsigned int) ((handler_present & 0x1) << 7));
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

void interrupt_handler(struct cpu_state cpu, struct interrupt_stack stack,
        unsigned int irq) {
    (void) cpu;
    struct printer p;
    p.target = PRINT_FRAMEBUFFER;
    print(&p, "error_code: %u\n", &stack.error_code);
    print(&p, "eip: %u\n", &stack.eip);

    if (INTERRUPT_DIVIDE_BY_ZERO == irq) {
    }
    // only for interrupts from pic
    //pic_acknowledge((unsigned char)irq);
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

