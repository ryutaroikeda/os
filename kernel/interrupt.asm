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

interrupt_handler_without_error_code 32
interrupt_handler_without_error_code 33
interrupt_handler_without_error_code 39

; The number of registers pushed by pushad
REGISTER_NUM equ 8 + 5

global common_interrupt_handler

common_interrupt_handler:
    pushad
    ; do we need to do this?
    push ss
    push ds
    push es
    push fs
    push gs

    mov ax, 0x10
    mov ss, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Make new call frame
    mov ebp, esp

    ; Push the irq
    mov ebx, [ebp + (REGISTER_NUM * 4)]
    push ebx

    ; Push pointer to interrupt stack. Assume ss is code segment with base 0
    ; This should be safer than pushing registers into C because the compiler
    ; might corrupt the stack.
    mov ebx, ebp
    add ebx, (REGISTER_NUM + 1) * 4
    push ebx

    ; This is defined in kernel/interrupt.c
    call interrupt_handler

    ; Clear arguments
    add esp, 2 * 4
    
    pop gs
    pop fs
    pop es
    pop ds
    pop ss

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

; @param [esp + 4] The address of the first entry in the interrupt
; descriptor table.
interrupt_load_descriptor_table:
    mov eax, [esp + 4]
    lidt [eax]
    ret

global interrupt_enable
interrupt_enable:
    sti
    ret

global interrupt_disable
interrupt_disable:
    cli
    ret

global interrupt_0

interrupt_0:
    int 0
    ret

global interrupt_6
interrupt_6:
    int 6
    ret

global interrupt_39
interrupt_39:
    int 39
    ret

