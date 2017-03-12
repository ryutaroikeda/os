CC = gcc
CFLAGS=-Wextra -Wall -pedantic -Werror -Wshadow \
       -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wmissing-prototypes \
       -Wconversion -fno-stack-protector \
	   -m32 -march=i386 -ffreestanding \
	   -fno-builtin \
	   -I . \
	   -O0 \
	   -std=c99

C_SOURCES = $(wildcard kernel/*.c drivers/*.c lib/*.c)
ASM_SOURCES = $(wildcard kernel/*.asm drivers/*.asm)
OBJ = $(C_SOURCES:.c=.o)
ASM_OBJ = $(ASM_SOURCES:.asm=.asmo)

boot: os.img
	qemu-system-i386 -drive format=raw,file=$< \
		-serial file:log

os.img: boot_strap.img kernel.bin
	cat $^ > $@
	qemu-img resize -f raw $@ 100M

kernel.bin: kernel/kernel_entry.o $(OBJ) $(ASM_OBJ)
	ld -m elf_i386 -o $@ -Ttext 0x1000 --oformat binary -entry=main $^

boot_strap.img: boot/boot_strap.asm boot/*
	nasm -f bin -i 'boot/' -o $@ $<

hextobin: bin bin/hextobin

bin:
	@mkdir -p bin

bin/hextobin: src/hextobin.cpp
	$(CC) $(CFLAGS) -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.asmo: %.asm
	nasm $< -f elf -o $@

%.dis: %.bin
	ndisasm -b 32 $< > $@

%.dump: kernel/%.o
	objdump -m i386 -d $< > $@

%.p: %.c
	$(CC) $(CFLAGS) -E $< > $@

clean:
	@rm -f *.bin *.dis *.img *.dump $(OBJ) $(ASM_OBJ)

tags:
	ctags -R kernel/ drivers/

.PHONY: boot
.PHONY: clean
.PHONY: tags
.PRECIOUS: %.o
.PRECIOUS: %.bin

