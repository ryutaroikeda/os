#include "drivers/pic.h"
#include "interrupt.h"
#include "panic.h"
#include "print.h"

enum {
    INTERRUPT_DIVIDE_BY_ZERO = 0x0,
    INTERRUPT_INVALID_OPCODE = 0x6,
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

/*
void interrupt_print_registers(struct printer* p,
        const struct cpu_state* state)
{
    print(p, "eax %u\n", &state->eax);
    print(p, "ebx %u\n", &state->ebx);
    print(p, "ecx %u\n", &state->ecx);
    print(p, "edx %u\n", &state->edx);
    print(p, "ebp %u\n", &state->ebp);
    print(p, "esi %u\n", &state->esi);
    print(p, "edi %u\n", &state->edi);
}
*/

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
    if (INTERRUPT_DIVIDE_BY_ZERO == irq) {
        panic(stack, irq, "division by zero");
    }
    if (INTERRUPT_INVALID_OPCODE == irq) {
        panic(stack, irq, "invalid opcode");
    }
    if (PIC_MASTER_OFFSET + 0x7 == irq) {
        /* Is this spurious? */
        uint16 isr = pic_get_in_service_register();
        if (!(isr & (1 << 7))) {
            /* We have a spurious interrupt. Don't acknowledge. */
            return;
        }
    }
    if (PIC_SLAVE_OFFSET + 0x7 == irq) {
        /* Is this spurious? */
        uint16 isr = pic_get_in_service_register();
        if (!(isr & (1 << 15))) {
            /* We have a spurious interrupt. Tell the master PIC */
            pic_acknowledge((uint8) (PIC_MASTER_OFFSET + 0x2));
            return;
        }
    }
    panic(stack, irq, "unknown interrupt");
    // only for interrupts from pic
    //pic_acknowledge((unsigned char)irq);
}

