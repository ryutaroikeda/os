#include "drivers/framebuffer.h"
#include "drivers/serial.h"

void main(void);

void main(void) {
	framebuffer_clear_screen();
	struct framebuffer_offset offset = {0, 0};
	framebuffer_set_cursor(offset);
    framebuffer_write("hello world!", 120);
    framebuffer_write("hola mundo!", 120);

    struct serial_line_configuration config = { 1, 0x3, 0, 0, 0 };
    serial_configure_line(SERIAL_COM_1_BASE, config);

    struct serial_buffer_configuration buffer_config = { 3, 0, 1, 1, 1 };
    serial_configure_buffer(SERIAL_COM_1_BASE, buffer_config);

    serial_write(SERIAL_COM_1_BASE, "really bad error!!!!! abort!!!", 100);

    framebuffer_write("exitting", 100);
}

