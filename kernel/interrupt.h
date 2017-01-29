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

void interrupt_handler(const struct interrupt_stack*, uint32 irq);

void interrupt_print_descriptor(struct printer*,
        const struct interrupt_descriptor*);

void interrupt_initialize(struct printer*);
void interrupt_enable(void);
void interrupt_disable(void);

void interrupt_print_stack(struct printer*, const struct interrupt_stack*);

extern void* interrupt_handler_0;
extern void* interrupt_handler_1;
extern void* interrupt_handler_3;
extern void* interrupt_handler_4;
extern void* interrupt_handler_5;
extern void* interrupt_handler_6;
extern void* interrupt_handler_7;
extern void* interrupt_handler_8;
extern void* interrupt_handler_9;
extern void* interrupt_handler_10;
extern void* interrupt_handler_11;
extern void* interrupt_handler_12;
extern void* interrupt_handler_13;
extern void* interrupt_handler_14;
extern void* interrupt_handler_16;

extern void* interrupt_handler_32;
extern void* interrupt_handler_33;

void interrupt_0(void);
void interrupt_6(void);
void interrupt_39(void);

#endif

