mov ah, 0x0e

mov al, [secret]
int 0x10 ; this prints giberish

; we can't set ds directly
mov bx, 0x7c0
mov ds, bx
mov al, [secret]
int 0x10 ; this prints X

; use the es segment
mov al, [es:secret]
int 0x10 ; this prints giberish

mov bx, 0x7c0
mov es, bx
mov al, [secret]
int 0x10 ; this prints X

jmp $

secret:
	db 'X'

times 510 - ($ - $$) db 0
dw 0xaa55
