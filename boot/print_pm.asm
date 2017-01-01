; print routine in 32 bit protected mode
[bits 32]

VIDEO_MEMORY equ 0xb8000
WHITE_ON_BLACK equ 0x0f

; print a null terminated string at address ebx
print_pm:
	pusha
	mov edx, VIDEO_MEMORY
	print_pm_loop:
		mov al, [ebx]
		mov ah, WHITE_ON_BLACK
		cmp al, 0
		jle print_pm_end_loop
		mov [edx], ax
		add ebx, 1 ; next char in string
		add edx, 2 ; next cell in video memory
		jmp print_pm_loop

	print_pm_end_loop:
		popa
		ret

