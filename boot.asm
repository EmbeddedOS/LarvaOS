ORG 0x7c00  ; Assembly origin. 
BITS 16     ; 16 bit instruction.         

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

setup_BPB:
    jmp short _start 
    nop

times 33 db 0   ; Set up BIOS parameter block to avoid corrupt data. 


_start:
    jmp 0:start



start:
    ; Set up memory start.
    cli                         ; Clear Interrupts.
    ; Set data segment, extra segment to 0x00.
    mov ax, 0x00
    mov ds, ax
    mov es, ax 
    
    ; Set stack segment to 0x00.
    mov ss, ax 

    ; Setting stack pointer points to 0x7c00 address.
    mov sp, 0x7c00
    sti                         ; Enable Interrupts.
    ; Set up memory end.
    


.load_protected:
    cli         ; Clear Interrupts.
    lgdt[gdt_descriptor]
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax 
    jmp CODE_SEG:load32



; Global descriptor table setup.
gdt_start:
gdt_null:
    dd 0x0
    dd 0x0

; offset 0x8
gdt_code:           ; CS SHOULD POINT TO THIS.
    dw 0xffff       ; Segment limit 0-15 bits.
    dw 0            ; Base first 0-15 bits.
    db 0            ; Base 16-23 bits.
    db 0x9a         ; Access byte.
    db 11001111b    ; High 4 bit flags and the low 4 bit flags.
    db 0            ; Base 24-31 bits.

; offset 0x10
gdt_data:           ; DS, SS, ES, FS, GS.
    dw 0xffff       ; Segment limit first 0-15 bits.
    dw 0            ; Base first 0-15 bits.
    db 0            ; Base 16-23 bits.
    db 0x92         ; Access byte.
    db 11001111b    ; High 4 bit flags and the low 4 bit flags.
    db 0            ; Base 24-31 bits.

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

[BITS 32]
load32:
    mov ax, DATA_SEG
    mov ds, ax 
    mov es, ax
    mov fs, ax
    mov gs, ax 
    mov ss, ax
    mov ebp, 0x00200000
    jmp $

times 510-($ - $$) db 0
dw 0xAA55

