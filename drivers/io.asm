[bits 32]

global port_byte_out

; send a byte to an IO port
; @param [esp + 8] the data
; @param [esp + 4] the IO port
; @param [esp] the return address
port_byte_out:
	mov al, [esp + 8]
	mov dx, [esp + 4]
	out dx, al
	ret

global port_byte_in

; read a byte from an IO port
; @param [esp + 4] the IO port
port_byte_in:
	mov dx, [esp + 4]
	in al, dx
	ret

