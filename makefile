CFLAGS=-g -std=c++11 -lc++ -Wextra -Wall -pedantic -Werror -Wshadow \
       -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wmissing-prototypes \
       -Wconversion

hextobin: bin bin/hextobin

bin:
	@mkdir -p bin

bin/hextobin: src/hextobin.cpp
	$(CC) $(CFLAGS) -o $@ $<

bootsector0.img: bootsector hextobin
	cat $< | ./bin/hextobin $@

bootsector.img: bootsector.asm
	nasm -f bin -o $@ $<

hello.img: hello.asm
	nasm -f bin -o $@ $<

%.img: %.asm
	nasm -f bin -o $@ $<

basic.o: basic.c
	gcc -ffreestanding -c $< -o $@

basic.bin: basic.o
	ld -o $@ -Ttext 0x0 --oformat binary $<
