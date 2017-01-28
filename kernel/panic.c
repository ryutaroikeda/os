#include "interrupt.h"
#include "panic.h"
#include "print.h"

void panic(const struct cpu_state* cpu, const struct interrupt_stack* stack,
        uint32 irq, const char* message) {
    struct printer p;
    p.target = PRINT_FRAMEBUFFER;
    interrupt_print_registers(&p, cpu);
    interrupt_print_stack(&p, stack);
    print(&p, "[kernel panic] irq %u: %s\n", &irq, message);
    while (1) {}
}

