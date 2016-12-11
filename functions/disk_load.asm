; load dh sectors to es:bx from drive dl
[bits 16]
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
	; dx won't be restored on this branch
	jc disk_error

	pop dx
	cmp al, dh
	jne disk_error
	ret

	disk_error:
		mov bx, DISK_ERROR_MESSAGE
		call print
		mov bx, 0
		mov bl, ah
		call print_hex
		mov bx, DISK_ERROR_NEW_LINE
		call print
		ret

FIRST_DISK_ERROR_MESSAGE:
	db 'failed to read anything from disk', 0xa, 0xd, 0
DISK_ERROR_MESSAGE:
	db 'disk read error: ', 0
DISK_ERROR_NEW_LINE:
	db 0xa, 0xd, 0
