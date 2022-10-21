[BITS 32]           ; We need to use the [bits 32] directive to tell our the assembler that,
                    ; from that point onwards, it should encode in 32-bit mode instructions.

global kernel_entry_point   ; We make a global symbol 'kernel entry point',
                            ; so the linker can find and jump to it first.

extern kernel_main

KERNEL_DATA_SEG equ 0x10


; NOTE: this symbol will be loaded at address 0x0100000 by linker, so
; DO NOT push any codes in front of 'kernel_entry_point' symbol.
; If can not jump to this symbol, my OS will be crashed.

kernel_entry_point:
    ; By now we are assuredly in 32 - bit protected mode.
    ; Initialize registers and the stack once in Protected Mode.
    mov ax, KERNEL_DATA_SEG         ; Now in PM, our old segments are meaningless,
    mov ds, ax                      ; so we point our segment registers to the 
    mov es, ax                      ; data selector we defined in our GDT.
    mov fs, ax
    mov gs, ax 
    mov ss, ax
    mov ebp, 0x00200000             ; Update our stack position so it is right 
    mov esp, ebp                    ; at the top of the free space.

    call enable_A20_line            ; Enable A20 line.

    mov ebx , MSG_PROT_MODE
    call print_string_pm            ; Use our 32 - bit print routine.

    jmp kernel_main
    jmp $

%include "./print_string_pm.asm"
%include "./enable_A20_line.asm"

MSG_PROT_MODE db "Entered to 32 - bit Protected Mode.", 0
