#ifndef _panic_h_
#define _panic_h_

#include "integer.h"

struct interrupt_stack;

void panic(const struct interrupt_stack*, uint32 irq,
        const char* message);

#endif

