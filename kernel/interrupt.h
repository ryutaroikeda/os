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
extern void* interrupt_handler_15;

extern void* interrupt_handler_32;
extern void* interrupt_handler_33;
extern void* interrupt_handler_34;
extern void* interrupt_handler_35;
extern void* interrupt_handler_36;
extern void* interrupt_handler_37;
extern void* interrupt_handler_38;
extern void* interrupt_handler_39;
extern void* interrupt_handler_40;
extern void* interrupt_handler_41;
extern void* interrupt_handler_42;
extern void* interrupt_handler_43;
extern void* interrupt_handler_44;
extern void* interrupt_handler_45;
extern void* interrupt_handler_46;
extern void* interrupt_handler_47;

void interrupt(uint32 irq);

void interrupt_0(void);
void interrupt_1(void);
void interrupt_2(void);
void interrupt_3(void);
void interrupt_4(void);
void interrupt_5(void);
void interrupt_6(void);
void interrupt_7(void);
void interrupt_8(void);
void interrupt_9(void);
void interrupt_10(void);
void interrupt_11(void);
void interrupt_12(void);
void interrupt_13(void);
void interrupt_14(void);
void interrupt_15(void);
void interrupt_16(void);
void interrupt_32(void);
void interrupt_33(void);
void interrupt_34(void);
void interrupt_35(void);
void interrupt_36(void);
void interrupt_37(void);
void interrupt_38(void);
void interrupt_39(void);
void interrupt_40(void);
void interrupt_41(void);
void interrupt_42(void);
void interrupt_43(void);
void interrupt_44(void);
void interrupt_45(void);
void interrupt_46(void);
void interrupt_47(void);

#endif

