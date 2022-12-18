[BITS 32]
section .asm

global enable_paging
global paging_load_directory

; Enabling paging is actually very simple. All that is needed is to load CR3
; with the address of the page directory and to set the paging (PG) and protection 
; (PE) bits of CR0.

paging_load_directory:
    push ebp
    mov ebp, esp

    mov eax, [ebp+8]
    mov cr3, eax        ; Load CR3 with the address of the page directory.

    pop ebp
    ret

enable_paging:
    push ebp
    mov ebp, esp

    or eax, 0x80000000
    mov cr0, eax

    pop ebp
    ret