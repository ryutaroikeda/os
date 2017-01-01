#CFLAGS=--Wextra -Wall -pedantic -Werror -Wshadow \
       -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wmissing-prototypes \
       -Wconversion

C_SOURCES = $(wildcard kernel/*.c drivers/*.c)
OBJ = $(C_SOURCES:.c=.o)

boot: os.img
	qemu-system-i386 -drive format=raw,file=$<

os.img: boot_strap.img kernel.bin
	cat $^ > $@
	qemu-img resize -f raw $@ 1M

kernel.bin: kernel/kernel_entry.o $(OBJ)
	ld -m elf_i386 -o $@ -Ttext 0x1000 --oformat binary -entry=main $^

kernel/kernel_entry.o: kernel/kernel_entry.asm
	nasm $< -f elf -o $@

boot_strap.img: boot/boot_strap.asm
	nasm -f bin -i 'boot/' -o $@ $<

hextobin: bin bin/hextobin

bin:
	@mkdir -p bin

bin/hextobin: src/hextobin.cpp
	$(CC) $(CFLAGS) -o $@ $<

%.o: %.c
	gcc -Wall -pedantic -m32 -march=i386 -nostartfiles -ffreestanding -c $< -o $@

%.bin: %.o
	ld -m elf_i386 -o $@ -Ttext 0x1000 --oformat binary $<

%.dis: %.bin
	ndisasm -b 32 $< > $@

%.dump: %.o
	objdump -m i386 -d $< > $@

clean:
	rm -r *.bin *.dis *.o *.img *.dump

.PHONY: boot
.PHONY: clean
.PRECIOUS: %.o
.PRECIOUS: %.bin

