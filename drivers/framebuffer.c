#include "framebuffer.h"
#include "kernel/memory.h"
#include "port.h"

enum {
    MAX_ROWS = 25,
    MAX_COLUMNS = 80,
    SEND_HIGH = 14,
    SEND_LOW = 15,
    CONTROL_PORT = 0x3d4,
    DATA_PORT = 0x3d5
};

static char* const VIDEO_MEMORY = (char*) 0xb8000;

static const struct framebuffer_attribute WHITE_ON_BLACK = {
    FRAMEBUFFER_WHITE,
    FRAMEBUFFER_BLACK
};

static char get_attribute_bits(struct framebuffer_attribute attribute) {
    return (char) (((attribute.background & 0xff) << 4) |
        (attribute.foreground & 0xff));
}

static void print_char(char c, struct framebuffer_offset offset,
        struct framebuffer_attribute attribute) {
    VIDEO_MEMORY[2 * (offset.row * MAX_COLUMNS + offset.column)] = c;
    VIDEO_MEMORY[2 * (offset.row * MAX_COLUMNS + offset.column) + 1] =
        get_attribute_bits(attribute);
}

void framebuffer_clear_screen(void) {
    for (int i = 0; i < MAX_ROWS * MAX_COLUMNS; i++) {
        VIDEO_MEMORY[2 * i] = ' ';
        VIDEO_MEMORY[(2 * i) + 1] = get_attribute_bits(WHITE_ON_BLACK);
    }
}

void framebuffer_set_cursor(struct framebuffer_offset offset) {
    int position = offset.row * MAX_COLUMNS + offset.column;
    unsigned char high = (unsigned char) ((position >> 8) & 0x00ff);
    unsigned char low = (unsigned char) (position & 0x00ff);

    port_byte_out(CONTROL_PORT, SEND_HIGH);
    port_byte_out(DATA_PORT, high);
    port_byte_out(CONTROL_PORT, SEND_LOW);
    port_byte_out(DATA_PORT, low);
}

struct framebuffer_offset framebuffer_get_cursor(void) {
    struct framebuffer_offset offset;

    port_byte_out(CONTROL_PORT, SEND_HIGH);
    unsigned char high = port_byte_in(DATA_PORT);
    port_byte_out(CONTROL_PORT, SEND_LOW);
    unsigned char low = port_byte_in(DATA_PORT);

    int position = (high << 8) | low;
    offset.column = position % MAX_COLUMNS;
    offset.row = position / MAX_COLUMNS;

    return offset;
}

void framebuffer_write(const char* s, int len) {
    struct framebuffer_offset offset = framebuffer_get_cursor();
    for (int i = 0; i < len; i++) {
        if (!s[i]) {
            break;
        }
        if ('\n' == s[i]) {
            offset.column = 0;
            offset.row += 1;
            if (offset.row >= MAX_ROWS) {
                framebuffer_scroll(1);
                offset.row -= 1;
            }
            continue;
        }
        print_char(s[i], offset, WHITE_ON_BLACK);
        offset.column += 1;
        if (offset.column >= MAX_COLUMNS) {
            offset.column = 0;
            offset.row += 1;
            if (offset.row >= MAX_ROWS) {
                framebuffer_scroll(1);
                offset.row -= 1;
            }
        }
    }
    framebuffer_set_cursor(offset);
}

void framebuffer_scroll(int rows) {
    memory_copy(VIDEO_MEMORY + (MAX_COLUMNS * rows),
            VIDEO_MEMORY, MAX_COLUMNS * (MAX_ROWS - rows));
}

