[bits 32]

global port_byte_out

; send a byte to an IO port
; @param [ebp + 8] the IO port
; @param [ebp + 12] the data
port_byte_out:
    push ebp
    mov ebp, esp
    push dx
    push ax
	mov dx, [ebp + 8]
	mov al, [ebp + 12]
	out dx, al
    pop ax
    pop dx
    pop ebp
	ret

global port_byte_in

; read a byte from an IO port
; @param [ebp + 8] the IO port
port_byte_in:
    push ebp
    mov ebp, esp
    push dx
	mov dx, [ebp + 8]
	in al, dx
    pop dx
    pop ebp
	ret

global port_wait:

; Do a dummy write to port 0x80 to "flush" the bus
port_wait:
    mov al, 0
    mov dx, 0x80
    out dx, al
    ret

