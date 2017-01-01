mov bx, 40

cmp bx, 4
jle less_than_or_equal_to_four

cmp bx, 40
jl less_than_fourty

default:
mov al, 'C'
jmp end_switch

less_than_or_equal_to_four:
mov al, 'A'
jmp end_switch

less_than_fourty:
mov al, 'B'

end_switch:
mov ah, 0x0e
int 0x10

jmp $

times 510-($-$$) db 0
dw 0xaa55
