#ifndef _print_h_
#define _print_h_

enum printer_target {
    PRINT_FRAMEBUFFER,
    PRINT_SERIAL_COM_1
};

struct printer {
    enum printer_target target;
    struct serial_port port;
};

int print0(struct printer p, const char* format);

#endif

