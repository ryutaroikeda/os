; global descriptor table
; we use a "flat model" with overlapping code and data segments.

gdt_start:

gdt_null_descriptor:
	dd 0x0
	dd 0x0

; segment descriptor for code
gdt_code:
	dw 0xffff ; limit
	dw 0x0    ; base
	db 0x0
	; the access byte
	; present=1, privilige=00, descriptor-type=1
	; type=1 is for code, conforming=0 memory protection, readable=1,
	; accessed=0 this is set when cpu accesses segment
	db 10011010b
	; limit and flags
	; granularity=1 offset limit by 2^12
	; 32 bit mode=1
	; 64 bit mode =0
	; available = 0
	; limit = 1111b
	db 11001111b; 
	; base
	db 0x0

; segment descriptor for data
gdt_data:
	dw 0xffff
	dw 0x0
	db 0x0
	db 10010010b
	db 11001111b; 
	db 0x0

gdt_end:

gdt_descriptor:
	; size of the gdt minus one
	dw gdt_end - gdt_start - 1
	dd gdt_start

; define constants for start of segments
CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

