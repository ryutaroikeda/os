[org 0x7c00]

mov bx, 0xdead
call print_hex
mov bx, 0xbeef
call print_hex

jmp $

%include "functions/print.asm"
%include "functions/print_hex.asm"

times 510 - ($ - $$) db 0
dw 0xaa55
