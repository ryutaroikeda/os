#include "kernel/panic.h"
#include "keyboard.h"
#include "lib/bool.h"
#include "lib/integer.h"
#include "lib/memory.h"
#include "lib/print.h"
#include "lib/queue.h"
#include "port.h"

enum keyboard_response {
    ERROR_1 = 0x00,
    ERROR_2 = 0xff,
    TEST_PASSED = 0xaa,
    TEST_FAILED_1 = 0xfc,
    TEST_FAILED_2 = 0xfd,
    ECHO = 0xee,
    ACKNOWLEDGED = 0xfa,
    RESEND = 0xfe
};

enum {
    DATA_BYTE_NONE = 0xff
};

enum {
    KEYBOARD_SCAN_PORT = 0x60,
    KEYBOARD_DATA_PORT = 0x60,
    KEYBOARD_STATUS_PORT = 0x64
};

struct keyboard keyboard_global;

QUEUE_DEFINE(keyboard_command, 256);

QUEUE_DEFINE(keyboard_scan_code, 256);

/*
static uint8 keyboard_get_scan_code(void) {
}
*/

static bool keyboard_has_data_byte(enum keyboard_command_byte command) {
    return (GET_SCAN_CODE_SET == command);
}

static void keyboard_byte_out(uint8 byte) {
    // wait until the ps/2 controller is ready
    while (port_byte_in(KEYBOARD_STATUS_PORT) & 0x2) {}
    port_byte_out(KEYBOARD_DATA_PORT, byte);
}

static uint8 keyboard_byte_in(void) {
    // wait until data is available for reading
    while (! (port_byte_in(KEYBOARD_STATUS_PORT) & 0x1)) {}
    return port_byte_in(KEYBOARD_DATA_PORT);
}

static void keyboard_do_send(struct keyboard_command command) {
    keyboard_byte_out(command.command_byte);
    if (keyboard_has_data_byte(command.command_byte)) {
        port_wait();
        keyboard_byte_out(command.data_byte);
    }
}

void keyboard_send(struct keyboard* keyboard,
        struct keyboard_command command) {
    if (keyboard_command_queue_is_empty(&keyboard->commands)) {
        keyboard_do_send(command);
        keyboard_byte_out(command.command_byte);
    }
    keyboard_command_queue_enqueue(&keyboard->commands, command);
}

void keyboard_receive(struct keyboard* keyboard) {
    uint8 response = keyboard_byte_in();
    switch (response) {
        case ERROR_1:
        case ERROR_2:
        case TEST_PASSED:
        case TEST_FAILED_1:
        case TEST_FAILED_2:
        case ECHO:
            break;
        case RESEND:
            if (keyboard_command_queue_is_empty(&keyboard->commands)) {
                panic("invalid resend response");
            }
            struct keyboard_command com =
                keyboard_command_queue_dequeue(&keyboard->commands);
            keyboard_send(keyboard, com);
            break;
        case ACKNOWLEDGED:
            keyboard_command_queue_dequeue(&keyboard->commands);
            break;
        default:

            keyboard_scan_code_queue_enqueue(&keyboard->scan_codes,
                    (struct keyboard_scan_code) { .byte = response });
            break;
    }
}

void keyboard_initialize(struct keyboard* keyboard,
        struct printer* printer) {
    memory_set(keyboard, 0, sizeof(struct keyboard));
    keyboard->printer = printer;
    keyboard_send(keyboard, (struct keyboard_command)
            { ENABLE_SCAN_CODE, DATA_BYTE_NONE });
    keyboard_send(keyboard, (struct keyboard_command)
            { SET_SCAN_CODE_SET, 2 });
}

void keyboard_interrupt_handler(const struct interrupt_stack* stack,
        uint32 irq) {
    (void) stack;
    if (33 != irq) {
        panic("bad irq");
    }
    keyboard_receive(&keyboard_global);
}

