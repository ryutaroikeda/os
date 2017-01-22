#ifndef _interrupt_h_
#define _interrupt_h_

struct printer;

enum privilege_level {
    PRIVILEGE_KERNEL = 0
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
 * optimization will mangle the stack
 */
struct cpu_state {
    unsigned int eax;
    unsigned int ecx;
    unsigned int edx;
    unsigned int ebx;
    unsigned int esp;
    unsigned int ebp;
    unsigned int esi;
    unsigned int edi;
};

struct interrupt_stack {
    unsigned int error_code;
    unsigned int eip;
    unsigned int cs;
    unsigned int eflags;
};

void interrupt_set_descriptor(struct interrupt_descriptor*,
        unsigned int offset, unsigned short segment_selector,
        enum privilege_level);

void interrupt_load_descriptor_table(struct interrupt_descriptor_table*);

void interrupt_handler(struct cpu_state cpu, struct interrupt_stack stack,
        unsigned int interrupt);

void interrupt_print_descriptor(struct printer*,
        const struct interrupt_descriptor*);

void interrupt_enable(void);

void interrupt_test(void);

extern void* interrupt_handler_0;
extern void* interrupt_handler_1;
extern void* interrupt_handler_2;

#endif

