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

global port_wait:

; Do a dummy write to port 0x80 to "flush" the bus
; Port 0x80 is not available on qemu-system-i386 version 2.5
port_wait:
    mov al, 0
    mov dx, 0x80
    out dx, al

