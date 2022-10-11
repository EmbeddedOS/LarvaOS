; -- This is a Boot sector. BIOS will look for this as the first thing!


ORG 0x7C00  ; Why do we need to select the MBR(Master Boot Record) at 0x7C00(32KB - 1KB) address of the memory?
            ; Minimum requirement for DOS - Disk Operating System version 1.0 was 32KB (0x0000 to 0x7FFF). 
            ; When we subtract 1KB (1024 bytes) from 32KB we will ge the value 0x7C00.
            ; Even though the length of MBR is 512 bytes,
            ; the reason for keeping 1024 bytes for MBR is to have extra memory for Stack/Data area of MBR.
            ; For more details: https://www.glamenv-septzen.net/en/view/6
            ;
            ; Early version of x86 has the memory architecture was like below:
            ; 0x0000    +---------------------------------------+
            ;           | Interrupts vector table (1024 bytes). |
            ; 0x0400    +---------------------------------------+
            ;           | BIOS Data area (256 bytes).           |
            ; 0x0500    +---------------------------------------+
            ;           | OS load area (30,464 bytes).          |
            ; 0x7C00    +---------------------------------------+
            ;           | MBR (512 bytes).                      |
            ; 0x7E00    +---------------------------------------+
            ;           | Boot Stack/Data area (512 bytes).     |
            ; 0x7FFF    +---------------------------------------+


            ; The ORG directive assigns a specific offset to the very next label or instruction following the
            ; directive. The addresses of all subsequent labels are calculated from that point onward.
            ; In our case, the address 0x7C00 will be asigned to 'start' label.

BITS 16     ; 16 bit instruction.      
            ; Since all intel based start up with 16-bit instructions,
            ; we have to start with 16-bit instructions before switch to protected mode to use 32-bit instructions.

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

; -- Entry point.
jmp short _start
nop

; -- OEM Parameter Block / BIOS Parameter Block.
times 33 db 0   ; Set up BIOS parameter block, this is necessary to avoid corrupt data. 

_start:
    jmp 0:start ; by jumping to segment 0 and address offset 0x7C00, 
                ; we ensure that absolute address calculation will always get the correct address.


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
    lgdt[gdt_descriptor]    ; Load Global Descriptor Table register with start address of GDT,
                            ; which defines the protected mode segments (e.g for code and data).
    
    mov eax, cr0            ; To make the switch to protected mode, we set
    or al, 0x1              ; the first bit Protection Enable - PE bit in CR0 (Control register 0).
    mov cr0, eax 
    jmp CODE_SEG:start_protected_mode   ; Perform far jump to selector CODE_SEG (offset into GDT, pointing at a 32 bit PM code segment descriptor)
                                        ; to load CS with proper PM32.
                                        ; Using far jump also forces the CPU to flush its cache of pre-fetched and
                                        ; real-mode decoded instructions, which can cause problems.



; Global Descriptor Table memory layout.
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
start_protected_mode:
    ; By now we are assuredly in 32 - bit protected mode.
    ; load DS, ES, FS, GS, SS, ESP
    mov ax, DATA_SEG
    mov ds, ax 
    mov es, ax
    mov fs, ax
    mov gs, ax 
    mov ss, ax
    mov ebp, 0x00200000
    jmp $

times 510-($ - $$) db 0             ; When compiled, our program must fit into 512 bytes,
                                    ; with the last two bytes being the magic number,
                                    ; so here, tell our assembly compiler t pad out our
                                    ; program with enough zeros bytes (db 0) to bring us 
                                    ; to the 510th byte.

dw 0xAA55                           ; Last two byte (one word) form the magic number,
                                    ; So BIOS knows we are a boot sector (as bootable).

