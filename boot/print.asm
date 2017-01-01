; print string up to the null byte
; bx is the address of the start of string
[bits 16]
print:
	pusha
	mov ah, 0x0e

	print_loop:
		; assume little endian
		mov cl, [bx]
		cmp cl, 0
		je end_print
		; mov al, ch
		mov al, cl
		int 0x10
		add bx, 1
		jmp print_loop

	; clean up
	end_print:
		popa
		ret

