; load dh sectors to es:bx from drive dl
disk_load:
	push dx

	; bios read sector function
	mov ah, 0x02

	; read dh sectors
	mov al, dh

	; select cylinder 0
	mov ch, 0x00 

	; sector head 0
	mov dh, 0x00 

	; read from the second sector. This is after the boot sector.
	mov cl, 0x02

	int 0x13
	jc disk_error

	pop dx
	cmp al, dh
	jne disk_error
	ret

	disk_error:
		mov bx, DISK_ERROR_MESSAGE
		call print
		ret

DISK_ERROR_MESSAGE:
	db 'disk read error', 0
