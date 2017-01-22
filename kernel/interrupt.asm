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

global interrupt_hander_%1

interrupt_handler_%1:
    ; The error code has already been pushed.
    ; Push the interrupt number.
    push dword %1
    jmp common_interrupt_handler

%endmacro

interrupt_handler_without_error_code 0
interrupt_handler_without_error_code 1
interrupt_handler_without_error_code 2

global common_interrupt_handler

common_interrupt_handler:
    pushad

    ; This is defined in kernel/interrupt.c
    ; Hope that our stack doesn't get corrupted.
    call interrupt_handler

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

global interrupt_test

interrupt_test:
    int 1
    ret

