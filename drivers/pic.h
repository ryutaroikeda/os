#ifndef _pic_h_
#define _pic_h_

enum pic_vector_offset {
    PIC_MASTER_OFFSET = 0x20,
    PIC_SLAVE_OFFSET = 0x28,
    PIC_SLAVE_END = PIC_SLAVE_OFFSET + 0x07
};

void pic_remap(unsigned char master_offset, unsigned char slave_offset);

void pic_acknowledge(unsigned char interrupt);

void pic_set_mask(unsigned char irq);

void pic_set_all_mask(void);

void pic_unset_mask(unsigned char irq);

/** to do: handle spurious irq */
#endif

