section .asm

global load_interrupt_descriptor_table
global enable_interrupts
global disable_interrupts
global int21h
extern int21h_handler
global no_interrupt
extern no_interrupt_handler

load_interrupt_descriptor_table:
    ; Make new call frame.
    ; Some compilers may produce an 'enter' instruction instead.
    push ebp            ; Save old call frame.
    mov ebp, esp        ; Initialize new call frame.

    mov ebx, [ebp+8]    ; Get first argument that is passed by the caller.
                        ; In this case, it is address of IDT register. 
    lidt [ebx]          ; Load Interrupt Descriptor Table.

    pop ebp             ; Restore old call frame.
    ret                 ; Return.

enable_interrupts:
    push ebp
    mov ebp, esp
    sti
    pop ebp
    ret

disable_interrupts:
    push ebp
    mov ebp, esp
    cli
    pop ebp
    ret

; Interrupt wrapper.
int21h:
    cli
    pushad
    call int21h_handler
    popad
    sti
    iret

; Interrupt wrapper.
no_interrupt:
    cli
    pushad
    call no_interrupt_handler
    popad
    sti
    iret