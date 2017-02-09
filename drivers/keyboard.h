#ifndef _keyboard_h_
#define _keyboard_h_

#include "kernel/integer.h"

enum keyboard_command_byte {
    ENABLE_SCAN_CODE = 0xf4,
    SCAN_CODE = 0xf0
};

struct keyboard_command {
    enum keyboard_command_byte command_byte;
    uint8 data_byte;
};

void keyboard_send(struct keyboard_command*);

void keyboard_initialize(void);

#endif

