[bits 32]

extern interrupt_handler

%macro interrupt_handler_without_error_code 1

global interrupt_handler_%1

interrupt_handler_%1:
    push dword 0
    ; push the interrupt number
    push dword %1
    jmp common_interrupt_handler

%endmacro

%macro interrupt_handler_with_error_code 1

global interrupt_handler_%1

interrupt_handler_%1:
    ; The error code has already been pushed.
    ; Push the interrupt number.
    push dword %1
    jmp common_interrupt_handler

%endmacro

; fake interrupts for testing
%macro interrupt 1
global interrupt_%1
interrupt_%1:
    int %1
    ret
%endmacro

;global interrupt_handler_32
;interrupt_handler_32:
;	push ax
;	push dx
;	mov al, 0x20
;	mov dx, 0x20
;	out dx, al
;	pop ax
;	pop dx
;	add esp, 4
;	iret

; fault -> eip points to instruction that caused the exception
; trap -> eip points to instruction dynamically after offending instruction
; abort -> cannot locate cause of exception, severe error

; divide by zero fault
interrupt_handler_without_error_code 0 
; debug fault or trap
interrupt_handler_without_error_code 1
; break point trap
interrupt_handler_without_error_code 3
; arithmetic overflow trap
interrupt_handler_without_error_code 4
; bounds check fault
interrupt_handler_without_error_code 5
; invalid opcode fault
interrupt_handler_without_error_code 6
; coprocessor not available
interrupt_handler_without_error_code 7
; double fault
interrupt_handler_with_error_code 8
; coprocessor overrun
interrupt_handler_without_error_code 9
; invalid tss fault
interrupt_handler_with_error_code 10
; segment not present fault
interrupt_handler_with_error_code 11
; stack fault
interrupt_handler_with_error_code 12
; general protection fault
interrupt_handler_with_error_code 13
; page fault
interrupt_handler_with_error_code 14
; coprocessor fault
interrupt_handler_without_error_code 16
; hopefully a programming error
interrupt_handler_with_error_code 15

; system timer
interrupt_handler_without_error_code 32
; keyboard controller
interrupt_handler_without_error_code 33
; cascaded signals from irq 8 - 15
interrupt_handler_without_error_code 34
; serial port controller for serial port 2
interrupt_handler_without_error_code 35
; serial port controller for serial port 1
interrupt_handler_without_error_code 36
; parallel port 2 and 3 or sound card
interrupt_handler_without_error_code 37
; floppy disk
interrupt_handler_without_error_code 38
; parallel port 1
interrupt_handler_without_error_code 39
; real time clock
interrupt_handler_without_error_code 40
; advanced configuration and power interface
interrupt_handler_without_error_code 41
; available for peripherals
interrupt_handler_without_error_code 42
; available for peripherals
interrupt_handler_without_error_code 43
; mouse
interrupt_handler_without_error_code 44
; fpu
interrupt_handler_without_error_code 45
; primary ATA channel
interrupt_handler_without_error_code 46
; secondary ATA channel
interrupt_handler_without_error_code 47
; interrupt doesn't exist
interrupt_handler_without_error_code 255

interrupt 0
interrupt 1
interrupt 2
interrupt 3
interrupt 4
interrupt 5
interrupt 6
interrupt 7
interrupt 8
interrupt 9
interrupt 10
interrupt 11
interrupt 12
interrupt 13
interrupt 14
interrupt 16
interrupt 32
interrupt 33
interrupt 34
interrupt 35
interrupt 36
interrupt 37
interrupt 38
interrupt 39
interrupt 40
interrupt 41
interrupt 42
interrupt 43
interrupt 44
interrupt 45
interrupt 46
interrupt 47
interrupt 255

; The number of registers pushed by pushad
REGISTER_NUM equ 8

global common_interrupt_handler

common_interrupt_handler:
    pushad

    ; Make new call frame
    mov ebp, esp

    ; Push the irq
    mov ebx, [ebp + REGISTER_NUM * 4]
    push ebx

    ; Push pointer to interrupt stack.
	; This should be safer than pushing registers into C because otherwise the
	; compiler might corrupt the stack.
    mov ebx, ebp
    add ebx, (REGISTER_NUM + 1) * 4
    push ebx

    ; This is defined in kernel/interrupt.c
    call interrupt_handler

    ; Clear arguments
    add esp, 2 * 4

    popad

    ; Assume that this was called with the error code and the interrupt number
    ; on the stack.
    ; iret expects the stack to be in the same state as it was called, so
    ; restore.
    add esp, 8

    ; Return to the code that triggered the interrupt.
    ; iret will restore eflags by popping it.
    iret

global interrupt_load_descriptor_table

; @param [ebp + 8] The address of the first entry in the interrupt
; descriptor table.
interrupt_load_descriptor_table:
    push ebp
    mov ebp, esp
    push eax
    mov eax, [ebp + 8]
    lidt [eax]
    pop eax
    pop ebp
    ret

global interrupt_enable
interrupt_enable:
    sti
    ret

global interrupt_disable
interrupt_disable:
    cli
    ret

