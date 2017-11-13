%include "const.inc"

section .text   ; start of the text (code) section
align 4         ; the code must be 4 byte aligned

extern exception_handler
extern irq_handler
;------------------------------------------------
; CPU exceptions
%macro exception_nocode 1
global _exception_%1
_exception_%1:
    cli          ; disable interrupts
    push    0    ; dummy error code
    push    %1  ; exception number
    jmp     exception_wrapper
%endmacro

%macro exception_code 1
global _exception_%1
_exception_%1:
    cli          ; disable interrupts
    push    %1  ; exception number
    jmp     exception_wrapper
%endmacro

exception_nocode 0
exception_nocode 1
exception_nocode 2
exception_nocode 3
exception_nocode 4
exception_nocode 5
exception_nocode 6
exception_nocode 7
exception_nocode 9
exception_nocode 15
exception_nocode 16
exception_nocode 18
exception_nocode 19
exception_nocode 20
exception_code 8
exception_code 10
exception_code 11
exception_code 12
exception_code 13
exception_code 14
exception_code 17
;------------------------------------------------
; IRQ
%macro _irq 1
global _irq_%1
_irq_%1:
    cli          ; disable interrupts
    push    0    ; dummy error code
    push    %1  ; irq number
    jmp     irq_wrapper
%endmacro

_irq 0
_irq 1
_irq 2
_irq 3
_irq 4
_irq 5
_irq 6
_irq 7
_irq 8
_irq 9
_irq 10
_irq 11
_irq 12
_irq 13
_irq 14
_irq 15
;------------------------------------------------
; Wrapper for exceptions
%macro wrapper 1

%1_wrapper:
    ; Save all registers
    push    eax
    push    ebx
    push    ecx
    push    edx
    push    esi
    push    edi
    push    ebp
    push    ds
    push    es
    push    fs
    push    gs

    ; Load kernel data descriptor into all segments
    mov     ax,GDT_KERNEL_DATA_SELECTOR
    mov     ds,ax
    mov     es,ax
    mov     fs,ax
    mov     gs,ax

    ; Pass the stack pointer (which gives the CPU context) to the C function
    mov     eax,esp
    push    eax
    call    %1_handler  ; implemented in idt.c
    pop     eax  ; only here to balance the "push eax" done before the call

    ; Restore all registers
    pop     gs
    pop     fs
    pop     es
    pop     ds
    pop     ebp
    pop     edi
    pop     esi
    pop     edx
    pop     ecx
    pop     ebx
    pop     eax
    
	; Fix the stack pointer due to the 2 push done before the call to
	; exception_wrapper: error code and exception/irq number
    add     esp,8
    iret
%endmacro
wrapper exception
wrapper irq
global idt_load
idt_load:
    mov     eax,[esp+4]  ; Get the pointer to the IDT, passed as a parameter.
    lidt    [eax]        ; Load the new IDT pointer
    ret
