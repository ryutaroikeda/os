CC = gcc
CFLAGS=-Wextra -Wall -pedantic -Werror -Wshadow \
       -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wmissing-prototypes \
       -Wconversion -fno-stack-protector \
	   -m32 -march=i386 -ffreestanding \
	   -fno-builtin

C_SOURCES = $(wildcard kernel/*.c drivers/*.c)
OBJ = $(C_SOURCES:.c=.o)

boot: os.img
	qemu-system-i386 -drive format=raw,file=$<

os.img: boot_strap.img kernel.bin
	cat $^ > $@
	qemu-img resize -f raw $@ 100M

kernel.bin: kernel/kernel_entry.o $(OBJ)
	ld -m elf_i386 -o $@ -Ttext 0x1000 --oformat binary -entry=main $^

kernel/kernel_entry.o: kernel/kernel_entry.asm
	nasm $< -f elf -o $@

boot_strap.img: boot/boot_strap.asm boot/*
	nasm -f bin -i 'boot/' -o $@ $<

hextobin: bin bin/hextobin

bin:
	@mkdir -p bin

bin/hextobin: src/hextobin.cpp
	$(CC) $(CFLAGS) -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.dis: %.bin
	ndisasm -b 32 $< > $@

%.dump: %.o
	objdump -m i386 -d $< > $@

clean:
	@rm -f *.bin *.dis kernel/*.o drivers/*.o *.img *.dump

tags:
	ctags -R kernel/ drivers/

.PHONY: boot
.PHONY: clean
.PHONY: tags
.PRECIOUS: %.o
.PRECIOUS: %.bin

