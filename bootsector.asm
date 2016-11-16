; infinite loop boot sector

loop1:
	jmp loop1

; pad to the 510th byte
times 510-($-$$) db 0

; end of boot sector magic number
dw 0xaa55
