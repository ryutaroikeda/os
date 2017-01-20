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

struct print_argument {
    const void* value;
};

int print_n(struct printer*, const char* format, const struct print_argument*,
        int arg_len);

int print_0(struct printer*, const char* format);
int print_1(struct printer*, const char* format, const void*);
int print_2(struct printer*, const char* format, const void*, const void*);

#define GET_MACRO(_0, _1, _2, NAME, ...) NAME

#define print(printer, ...) \
    GET_MACRO(__VA_ARGS__, print_2, print_1, print_0, print_) \
    (printer, __VA_ARGS__)

#endif

