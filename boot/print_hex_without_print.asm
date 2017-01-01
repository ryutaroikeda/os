; dx the hex value to print
print_hex:
	pusha

	; optimization is the root of all evil
	mov bx, dx
	shr bx, 12
	call print_hex_byte
	mov bx, dx
	shr bx, 8
	call print_hex_byte
	mov bx, dx
	shr bx, 4
	call print_hex_byte
	mov bx, dx
	call print_hex_byte

	popa
	ret

; bx the byte to print
print_hex_byte:
	pusha
	mov ah, 0x0e

	and bx, 0x000f
	cmp bx, 10
	jge case_alpha
	add bx, '0'
	mov al, bl
	jmp end_print_hex_byte
	case_alpha:
		sub bx, 10
		add bx, 'a'
		mov al, bl

	end_print_hex_byte:
		int 0x10

	popa
	ret

