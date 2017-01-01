#CFLAGS=--Wextra -Wall -pedantic -Werror -Wshadow \
       -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wmissing-prototypes \
       -Wconversion

hextobin: bin bin/hextobin

bin:
	@mkdir -p bin

bin/hextobin: src/hextobin.cpp
	$(CC) $(CFLAGS) -o $@ $<

bootsector0.img: bootsector hextobin
	cat $< | ./bin/hextobin $@

%.img: %.asm
	nasm -f bin -o $@ $<

kernel_entry.o: kernel_entry.asm
	nasm $< -f elf -o $@

%.o: %.c
	gcc -Wall -pedantic -m32 -march=i386 -nostartfiles -ffreestanding -c $< -o $@

kernel.bin: kernel_entry.o kernel.o
	ld -m elf_i386 -o $@ -Ttext 0x1000 --oformat binary -entry=main $^

%.bin: %.o
	ld -m elf_i386 -o $@ -Ttext 0x1000 --oformat binary $<

%.dis: %.bin
	ndisasm -b 32 $< > $@

%.dump: %.o
	objdump -m i386 -d $< > $@

os.img: boot_strap.img kernel.bin
	cat $^ > $@
	qemu-img resize -f raw $@ 1M

boot: os.img
	qemu-system-i386 -drive format=raw,file=$<

clean:
	rm kernel.bin
	rm boot_strap.img

.PHONY: boot
.PHONY: clean
.PRECIOUS: %.o
.PRECIOUS: %.bin

