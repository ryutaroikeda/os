#include "kernel/integer.h"
#include "pic.h"
#include "port.h"
#include "kernel/print.h"

/**
 * PIC stands for programmable interrupt controller.
 *
 * Interrupt request numbers (IRQ), Vector offset, interrupt numbers
 * master: 0 to 7, 0x8, 0x8 to 0xf
 */

enum pic_command {
    PIC_ACKNOWLEDGE = 0x20
};

enum pic_initialization_command_word {
    /* Require ICW if set. */
    ICW_1_NEED_ICW_4 = 0x01,
    /* Cascade mode if not set. */
    ICW_1_IS_SINGLE = 0x02,
    /* Call address interval 4 if set, 8 otherwise. */
    ICW_1_CALL_ADDRESS_INTERVAL = 0x04,
    /* Level triggered mode if set, edge triggered mode otherwise. */
    ICW_1_LEVEL_TRIGGERED_MODE = 0x08,
    /* Required flag. */
    ICW_1_INITIALIZE = 0x10,

    /* 8086/88 mode if set, MCS-80/85 mode otherwise. */
    ICW_4_8086 = 0x01,
    ICW_4_AUTO_END_OF_INTERRUPT = 0x02,
    ICW_4_BUFFERED_SLAVE_MODE = 0x08,
    ICW_4_BUFFERED_MASTER_MODE = 0x0c,
    ICW_4_SPECIAL_FULLY_NESTED_MODE = 0x10
};

enum pic_operation_command_word {
    OCW_3_READ_INTERRUPT_REQUEST_REGISTER = 0xa,
    OCW_3_READ_IN_SERVICE_REGISTER = 0xb
};

#define PIC_COMMAND(base) (base)
#define PIC_DATA(base) ((base) + 1)

/**
 * By default, PIC uses numbers 0 to 15 for hardware interrupts. This
 * conflicts with the numbers used for software interrupts, so the numbers for
 * PIC are remapped to use a different range here.
 * @param master_offset Vector offset for master PIC.
 * @param slave_offset Vector offset for slave PIC.
 */
void pic_remap(void) {
    uint8 master_offset = PIC_MASTER_OFFSET;
    uint8 slave_offset = PIC_SLAVE_OFFSET;

    /* Save data ports. */
    uint8 master_data = port_byte_in(PIC_DATA(PIC_MASTER_PORT));
    uint8 slave_data = port_byte_in(PIC_DATA(PIC_SLAVE_PORT));

    /* Start initialization in cascade mode. */
    port_byte_out(PIC_COMMAND(PIC_MASTER_PORT),
            ICW_1_NEED_ICW_4 | ICW_1_INITIALIZE);
    port_byte_out(PIC_COMMAND(PIC_SLAVE_PORT),
            ICW_1_NEED_ICW_4 | ICW_1_INITIALIZE);

    port_byte_out(PIC_DATA(PIC_MASTER_PORT), master_offset);
    port_byte_out(PIC_DATA(PIC_SLAVE_PORT), slave_offset);

    /* Tell master there's a slave at IRQ 2.
     * The position of the bit corresponds to the IRQ (0000 0100) */
    port_byte_out(PIC_DATA(PIC_MASTER_PORT), 0x04);
    /* Tell slave its cascade identity (IRQ 2). */
    port_byte_out(PIC_DATA(PIC_SLAVE_PORT), 2);

    port_byte_out(PIC_DATA(PIC_MASTER_PORT), ICW_4_8086);
    port_byte_out(PIC_DATA(PIC_SLAVE_PORT), ICW_4_8086);

    /* Restore data ports. */
    port_byte_out(PIC_DATA(PIC_MASTER_PORT), master_data);
    port_byte_out(PIC_DATA(PIC_SLAVE_PORT), slave_data);
}

/**
 * Acknowledge an interrupt from master PIC or slave PIC after handling. We
 * won't get new interrupts until we acknolwedge (also called
 * end-of-interrupt).
 */
void pic_acknowledge(uint8 irq) {
    if ((irq < PIC_MASTER_OFFSET) || (PIC_SLAVE_END < irq)) {
        /* This shouldn't happen. */
        return;
    }
    /* If the interrupt is from slave, send to both master and slave. */
    if (PIC_SLAVE_OFFSET <= irq) {
        port_byte_out(PIC_COMMAND(PIC_SLAVE_PORT), PIC_ACKNOWLEDGE);
    }
    port_byte_out(PIC_COMMAND(PIC_MASTER_PORT), PIC_ACKNOWLEDGE);
}

/**
 * The PIC has an 8 bit register called the interrupt mask register IMR. 
 * Each bit corresponds to an irq. When the bit is set, the irq is disabled.
 */

void pic_set_mask(uint8 irq) {
    if ((irq < PIC_MASTER_OFFSET) || (PIC_SLAVE_END < irq)) {
        /* This shouldn't happen. */
        return;
    }
    Port port;
    if (PIC_SLAVE_OFFSET <= irq) {
        port = PIC_SLAVE_PORT;
        irq = (uint8) (irq - PIC_SLAVE_OFFSET);
    } else {
        port = PIC_MASTER_PORT;
        irq = (uint8) (irq - PIC_MASTER_OFFSET);
    }
    uint8 value = (uint8) (port_byte_in(port) | (1 << irq));
    port_byte_out((uint8)PIC_DATA(port), value);
}

void pic_set_all_mask() {
    port_byte_out(PIC_DATA(PIC_SLAVE_PORT), 0xff);
    port_byte_out(PIC_DATA(PIC_MASTER_PORT), 0xff);
}

void pic_unset_mask(uint8 irq) {
    if ((irq < PIC_MASTER_OFFSET) || (PIC_SLAVE_END < irq)) {
        struct printer p;
        p.target = PRINT_FRAMEBUFFER;
        print(&p, "invalid irq %u\n", &irq);
        return;
    }
    Port port;
    if (PIC_SLAVE_OFFSET <= irq) {
        port = PIC_SLAVE_PORT;
        irq = (uint8) (irq - PIC_SLAVE_OFFSET);
    } else {
        port = PIC_MASTER_PORT;
        irq = (uint8) (irq - PIC_MASTER_OFFSET);
    }
    // is this the right port?
    uint8 value = (uint8) (port_byte_in(port) & ~(1 << irq));
    port_byte_out((uint8)PIC_DATA(port), value);
}

static uint16 get_interrupt_register(uint8 ocw3) {
    port_byte_out(PIC_COMMAND(PIC_MASTER_PORT), ocw3);
    port_byte_out(PIC_COMMAND(PIC_SLAVE_PORT), ocw3);
    return port_byte_in((Port)((PIC_DATA(PIC_MASTER_PORT) << 8) |
        port_byte_in(PIC_DATA(PIC_SLAVE_PORT))));
}

uint16 pic_get_in_service_register(void) {
    return get_interrupt_register(OCW_3_READ_IN_SERVICE_REGISTER);
}

uint16 pic_get_interrupt_request_register(void) {
    return get_interrupt_register(OCW_3_READ_INTERRUPT_REQUEST_REGISTER);
}

