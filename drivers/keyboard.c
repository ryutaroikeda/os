#include "keyboard.h"
#include "kernel/memory.h"

enum keyboard_response {
    ACKNOWLEDGED = 0xfa,
    RESEND = 0xfe
};

enum {
    DATA_BYTE_NONE = 0xff,
    QUEUE_SIZE = 256
};

enum {
    KEYBOARD_SCAN_PORT = 0x60,
    KEYBOARD_STATUS_PORT = 0x64
};

struct keyboard_command command_queue[QUEUE_SIZE];

uint8 command_num;

void keyboard_send(struct keyboard_command* command) {
    command_queue[command_num++] = *command;
}

void keyboard_initialize(void) {
    memory_set(command_queue, 0, QUEUE_SIZE);
    command_num = 0;
}

