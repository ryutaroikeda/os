; this depends on functions/print.asm
; print bx as hex
[bits 16]
print_hex:
	pusha

	mov dx, bx
	shr bx, 12
	call convert_hex_to_char
	mov byte [print_hex_result + 2], bl

	mov bx, dx
	shr bx, 8
	call convert_hex_to_char
	mov byte [print_hex_result + 3], bl

	mov bx, dx
	shr bx, 4
	call convert_hex_to_char
	mov byte [print_hex_result + 4], bl

	mov bx, dx
	call convert_hex_to_char
	mov byte [print_hex_result + 5], bl

	mov bx, print_hex_result
	call print

	popa
	ret

; input: bx the byte to convert
; output: bl the char to print
convert_hex_to_char:
	and bx, 0x000f
	cmp bx, 10
	jge case_alpha
	add bx, '0'
	jmp end_convert_hex_to_char
	case_alpha:
		sub bx, 10
		add bx, 'a'

	end_convert_hex_to_char:
		ret

print_hex_result:
	db '0x0000', 0

