section .asm
global tss_load

tss_load:
    push ebp
    mov ebp, esp
    mov ax, [ebp + 8]   ; TSS segment.
                        ; The descriptor of the TSS in the GDT 
                        ; (e.g. 0x28 if the sixths entry in your GDT describes your TSS)
    ltr ax              ; The actual load.
    pop ebp
    ret