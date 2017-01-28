#ifndef _interrupt_h_
#define _interrupt_h_

#include "integer.h"

struct printer;

enum privilege_level {
    INTERRUPT_PRIVILEGE_KERNEL = 0
};

struct interrupt_descriptor {
    unsigned short offset_low;
    unsigned short segment_selector;
    unsigned char reserved;
    unsigned char attributes;
    unsigned short offset_high;
} __attribute__((packed));

/* This is loaded with interrupt_load_descriptor_table() */
struct interrupt_descriptor_table {
    /* Length of the interrupt descriptor table in bytes - 1. */
    unsigned short limit;
    /* Start address of the interrupt descriptor table. */
    struct interrupt_descriptor* base;
} __attribute__((packed));

/**
 * interrupt handling in c is apparently a bad idea because gcc with
 * optimization can corrupt the stack
 */
struct cpu_state {
    uint32 eax;
    uint32 ecx;
    uint32 edx;
    uint32 ebx;
    uint32 ebp;
    uint32 esi;
    uint32 edi;
} __attribute__((packed));

struct interrupt_stack {
    uint32 error_code;
    uint32 eip;
    uint32 cs;
    uint32 eflags;
} __attribute__((packed));

void interrupt_set_descriptor(struct interrupt_descriptor*,
        unsigned int offset, unsigned short segment_selector,
        enum privilege_level);

void interrupt_load_descriptor_table(struct interrupt_descriptor_table*);

void interrupt_handler(struct cpu_state cpu, struct interrupt_stack stack,
        unsigned int interrupt);

void interrupt_print_descriptor(struct printer*,
        const struct interrupt_descriptor*);

void interrupt_enable(void);

void interrupt_print_registers(struct printer*, const struct cpu_state*);
void interrupt_print_stack(struct printer*, const struct interrupt_stack*);

extern void* interrupt_handler_0;
extern void* interrupt_handler_1;
extern void* interrupt_handler_6;

#endif

