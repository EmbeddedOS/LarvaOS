ORG 0x7C00  ; Assembly origin. 
BITS 16     ; 16 bit instruction.         

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

; -- Entry point.
jmp short _start 
nop


times 33 db 0   ; Set up BIOS parameter block, this is necessary to avoid corrupt data. 

_start:
    jmp 0:start

start:
    ; -- Set up memory:
    ; data segment, extra segment, stack segment to 0x00.
    ; stack pointer points to 0x7C00.
    cli                         
    mov ax, 0x00
    mov ds, ax
    mov es, ax 
    mov ss, ax 
    mov sp, 0x7C00
    sti                         

    ; -- Entering Protected mode: https://wiki.osdev.org/Protected_Mode
    ; 1. Disable interrupts.
    ; 2. Enable A20 Line.
    ; 3. Load the Global Descriptor Table with segment descriptors suitable for code, data and stack.
    cli                     ; Disable interrupts.
    lgdt[gdt_descriptor]    ; Load Global Descriptor Table register with start address of GDT.
    
    mov eax, cr0
    or al, 0x1              ; Set Protection Enable - PE bit in CR0 (Control register 0).
    mov cr0, eax 
    jmp CODE_SEG:protect_mode_main  ; Perform far jump to selector CODE_SEG (offset into GDT, pointing at a 32 bit PM code segment descriptor)
                                    ; to load CS with proper PM32.



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
protect_mode_main:
    ; -- Entered Protected Mode.
    ; load DS, ES, FS, GS, SS, ESP
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

