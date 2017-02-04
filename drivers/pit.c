#include "pit.h"
#include "port.h"

enum {
    CHANNEL_0_DATA_PORT = 0x40,
    COMMAND_PORT = 0x43
};

void pit_initialize(void) {
    // Select channel 0 with access mode lobyte/hibyte and operating mode 0
    // (interrupt on terminal count).
    port_byte_out(COMMAND_PORT, 0x18);
    port_byte_out(CHANNEL_0_DATA_PORT, 0xff);
    port_byte_out(CHANNEL_0_DATA_PORT, 0xff);
}

