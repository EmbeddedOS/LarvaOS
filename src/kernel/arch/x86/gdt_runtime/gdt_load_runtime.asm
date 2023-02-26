section .asm
global gdt_load_runtime

gdt_load_runtime:
    mov eax, [esp + 4]              ; Address of segment descriptor.
    mov [gdt_descriptor + 2], eax
    mov ax, [esp + 8]               ; Size of segment descriptor.
    mov [gdt_descriptor], ax
    lgdt [gdt_descriptor]           ; Load GDT descriptor.
    ret

section .data
; GDT descriptor.
gdt_descriptor:
    dw 0x00      ; Size of our GDT, always less one of the true size.
    dd 0x00      ; Start address of our GDT.