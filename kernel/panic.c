#include "idle.h"
#include "interrupt.h"
#include "panic.h"
#include "lib/print.h"

void panic(const char* message) {
    struct printer p;
    p.target = PRINT_SERIAL_COM_1;
    p.port = (struct serial_port) { SERIAL_COM_1_BASE, false };
    print(&p, "%s\n", message);
    while (1) {
        idle();
    }
}

void panic_with_printer(struct printer* printer, const char* message) {
    print(printer, "%s\n", message);
    while (1) {
        idle();
    }
}

void panic_with_stack(const char* message,
        const struct interrupt_stack* stack, uint32 irq) {
    struct printer p;
    p.target = PRINT_FRAMEBUFFER;
    interrupt_print_stack(&p, stack);
    print(&p, "[kernel panic] irq %u: %s\n", &irq, message);
    while (1) {
        idle();
    }
}

