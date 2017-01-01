jmp main

my_print_function:
pusha
mov ah, 0x0e
int 0x10
popa
ret

main:
mov al, 'H'
call my_print_function

jmp $

times 510 - ($ - $$) db 0
dw 0xaa55
