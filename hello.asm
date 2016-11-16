; hello world boot sector

mov ah, 0x0e ; scrolling teletype bios routines

mov al, 'h'
int 0x10
mov al, 'e'
int 0x10
mov al, 'l'
int 0x10
mov al, 'l'
int 0x10
mov al, 'o'
int 0x10

jmp $ ; jump to the current address. This is an infinite loop.

; padding and magic BIOS number

times 510-($-$$) db 0

dw 0xaa55
