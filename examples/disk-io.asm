; test disk_load works

[org 0x7c00]

; bios stores the boot drive in dl
mov bx, 0
mov bl, dl

mov [BOOT_DRIVE], dl

mov bx, [BOOT_DRIVE]
;call print_hex
;call new_line

; move the stack out of the way
mov bp, 0x8000
mov sp, bp

; read sectors
mov bx, 0x9000
; number of sectors to read
mov dh, 1
mov dl, [BOOT_DRIVE]
call disk_load

mov bx, [0x9000]
call print_hex
call new_line
mov bx, [0x9000 + 512]
call print_hex
call new_line

jmp $

%include "functions/print.asm"
%include "functions/print_hex.asm"
%include "functions/disk_load.asm"

new_line:
	pusha
	mov bx, NEW_LINE
	call print
	popa
	ret

NEW_LINE:
	db 0xa, 0xd, 0

BOOT_DRIVE:
	db 0

times 510 - ($ - $$) db 0
dw 0xaa55

; We know that BIOS will load only the first 512-byte sector from the disk,
; so if we purposely add a few more sectors to our code by repeating some
; familiar numbers, we can prove to ourselves that we actually loaded those
; additional two sectors from the disk we booted from.
times 256 dw 0xdead
times 256 dw 0xbeef
;times 256 dw 0xface
