[bits 16]
switch_to_protected_mode:
	; switch off interrupts until we set up protected mode interrupt vector
	cli
	; load global descriptor table
	lgdt [gdt_descriptor]

	; set the first bit of the control register to enter 32 bit protected mode
	mov eax, cr0
	or eax, 0x1
	mov cr0, eax

	; far jump to 32 bit code and also flush the CPU instruction pipeline
	jmp CODE_SEG:init_protected_mode

[bits 32]

init_protected_mode:
	; update segment registers
	mov ax, DATA_SEG
    ; do we want to use lss? Is mov ss okay in real mode?
    mov ss, ax
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	mov ebp, 0x90000
	mov esp, ebp

	call BEGIN_PM

