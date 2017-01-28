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
interrupt_handler_without_error_code 6
interrupt_handler_without_error_code 39

REGISTER_NUM equ 8
global common_interrupt_handler

common_interrupt_handler:
    pushad

    ; Push the irq
    mov eax, esp
    mov ebx, [eax + (REGISTER_NUM * 4)]
    push ebx

    ; Push pointer to interrupt stack. Assume ss is code segment with base 0
    ; This should be safer than pushing registers into C because the compiler
    ; might corrupt the stack.
    mov ebx, eax
    add ebx, (REGISTER_NUM + 1) * 4
    push ebx

    ; This is defined in kernel/interrupt.c
    call interrupt_handler

    ; Clean up the arguments
    pop eax
    pop eax

    popad
    ;pop edi
    ;pop esi
    ;pop ebp
    ;pop ebx
    ;pop edx
    ;pop ecx
    ;pop eax

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

