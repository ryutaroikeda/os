#include "idle.h"
#include "interrupt.h"
#include "panic.h"
#include "print.h"

void panic(const char* message) {
    struct printer p;
    p.target = PRINT_FRAMEBUFFER;
    print(&p, "%s\n", message);
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


