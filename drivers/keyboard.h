#ifndef _keyboard_h_
#define _keyboard_h_

#include "lib/integer.h"
#include "lib/queue.h"

struct interrupt_stack;
struct printer;

enum {
    MAX_COMMAND_SIZE = 256,
    MAX_SCAN_CODE_SIZE = 256
};

enum keyboard_command_byte {
    ENABLE_SCAN_CODE = 0xf4
    ,GET_SCAN_CODE_SET = 0xf0
    ,SET_SCAN_CODE_SET = 0xf0
};

struct keyboard_command {
    enum keyboard_command_byte command_byte;
    uint8 data_byte;
};

QUEUE_DECLARE(keyboard_command, 256);

struct keyboard_scan_code {
    uint8 byte;
};

QUEUE_DECLARE(keyboard_scan_code, 256);

struct keyboard {
    struct keyboard_command_queue commands;
    struct keyboard_scan_code_queue scan_codes;
    struct printer* printer;
};

extern struct keyboard keyboard_global;

void keyboard_send(struct keyboard*, struct keyboard_command);

void keyboard_receive(struct keyboard*);

void keyboard_initialize(struct keyboard*, struct printer*);

void keyboard_interrupt_handler(const struct interrupt_stack*, uint32 irq);

#endif

