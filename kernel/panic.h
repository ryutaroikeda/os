#ifndef _panic_h_
#define _panic_h_

#include "integer.h"

struct cpu_state;
struct interrupt_stack;

void panic(const struct cpu_state*, const struct interrupt_stack*, uint32 irq,
        const char* message);

#endif

