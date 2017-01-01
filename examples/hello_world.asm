[org 0x7c00]

mov bx, my_string
call print

jmp $

%include "functions/print.asm"

my_string:
	db 'hello world!', 0

times 510 - ($ - $$) db 0
dw 0xaa55
