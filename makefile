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

%.o: %.c
	gcc -Wall -pedantic -m32 -march=i386 -nostartfiles -ffreestanding -c $< -o $@

%.bin: %.o
	ld -m elf_i386 --entry=my_function -o $@ -Ttext 0x0 --oformat binary $<

%.dis: %.bin
	ndisasm -b 32 $< > $@

%.dump: %.o
	objdump -m i386 -d $< > $@

.PRECIOUS: %.o
.PRECIOUS: %.bin
