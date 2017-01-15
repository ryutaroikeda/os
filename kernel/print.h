#ifndef _print_h_
#define _print_h_

enum printer_target {
    PRINT_FRAMEBUFFER,
    PRINT_SERIAL_COM_1
};

enum {
    PRINT_BUFFER_SIZE = 1024
};

struct printer {
    enum printer_target target;
    struct serial_port port;
    char buffer[PRINT_BUFFER_SIZE];
    int buffer_offset;
    int precision;
};

/*
enum print_type {
    PRINT_INT,
    PRINT_FLOAT,
    PRINT_BOOL,
    PRINT_CHAR,
    PRINT_STRING,
    PRINT_HEX,
    PRINT_BINARY
};
*/

struct print_argument {
    //enum print_type type;
    union {
        int d;
        float f;
        char c;
        bool b;
        char* s;
    } value;
};

int print_0(struct printer*, const char* format, int len);

int print_1(struct printer*, const char* format, struct print_argument*,
        int arg_len);

#endif

