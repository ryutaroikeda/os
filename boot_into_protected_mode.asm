; boot sector that switches to 32 bit protected mode
[org 0x7c00]
[bits 16]
mov bp, 0x9000
mov sp, bp

mov bx, MSG_REAL_MODE
call print
; we never return from here
call switch_to_protected_mode 
jmp $

%include "protected_mode.asm"
%include "global_descriptor_table.asm"
%include "functions/print.asm"
%include "functions/print_pm.asm"

[bits 32]

BEGIN_PM:
mov ebx, MSG_PROT_MODE
call print_pm

jmp $

MSG_REAL_MODE:
	db "starting in real mode", 0
MSG_PROT_MODE:
	db "landed in 32 bit protected mode!", 0

times 510 - ($ - $$) db 0
dw 0xaa55
