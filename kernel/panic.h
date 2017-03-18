#ifndef _panic_h_
#define _panic_h_

#include "lib/integer.h"

struct interrupt_stack;
struct printer;

void panic(const char* message);

void panic_with_printer(struct printer*, const char*);

void panic_with_stack(const char* message,
        const struct interrupt_stack* stack, uint32 irq);

#endif

