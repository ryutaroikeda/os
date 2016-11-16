; [org 0x7c00] ; this adds the address of this sector to labels.

mov ah, 0x0e ; scrolling teletype BIOS routine

mov al, the_secret
int 0x10

mov al, [the_secret]
int 0x10 ; this prints X if we have [org ...]

mov bx, the_secret
add bx, 0x7c00
mov al, [bx]
int 0x10 ; this prints X (if we don't have [org ...]

mov al, [0x7c1d]
int 0x10 ; this prints X 

jmp $

the_secret:
db 'X'

times 510-($-$$) db 0
dw 0xaa55
