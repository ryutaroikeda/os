; bootsector that boots a C kernel in 32 bit protected mode
[org 0x7c00]
[bits 16]
; we'll load the kernel here
KERNEL_OFFSET equ 0x1000

; BIOS stores the boot drive in dl
mov [BOOT_DRIVE], dl

; set up the stack
mov bp, 0x9000
mov sp, bp

mov bx, MSG_REAL_MODE
call print
call load_kernel
call switch_to_protected_mode
jmp $

load_kernel:
	mov bx, MSG_LOAD_KERNEL
	call print
	mov bx, 0
	mov bl, [BOOT_DRIVE]
	call print_hex
	mov bx, NEW_LINE
	call print

	mov bx, KERNEL_OFFSET
	mov dh, 20
	mov dl, [BOOT_DRIVE]
	call disk_load
	ret

[bits 32]
BEGIN_PM:
	mov bx, MSG_PROTECTED_MODE
	call print_pm
	call KERNEL_OFFSET
	jmp $

%include "print.asm"
%include "print_hex.asm"
%include "disk_load.asm"
%include "global_descriptor_table.asm"
%include "protected_mode.asm"
%include "print_pm.asm"

BOOT_DRIVE:
	db 0
MSG_REAL_MODE:
	db 'started in 16 bit real mode', 0xa, 0xd, 0 
MSG_PROTECTED_MODE:
	db 'landed in 32 protected mode', 0
MSG_LOAD_KERNEL:
	db 'loading kernel in memory from drive ', 0
NEW_LINE:
	db 0xa, 0xd, 0
test:
	db 'testing testing', 0xa, 0xd, 0

times 510 - ($ - $$) db 0
dw 0xaa55

