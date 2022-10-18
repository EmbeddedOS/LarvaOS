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


; This will simply get replaced bythe contents of the file.
%include "src/kernel/arch/x86/boot/gdt.asm" 
%include "src/kernel/arch/x86/utils/print_string_pm.asm"
%include "src/kernel/arch/x86/boot/enable_A20_line.asm"

[BITS 32]           ; We need to use the [bits 32] directive to tell our the assembler that,
                    ; from that point onwards, it should encode in 32-bit mode instructions.
start_protected_mode:
    ; By now we are assuredly in 32 - bit protected mode.
    ; Initialize registers and the stack once in Protected Mode.
    mov ax, DATA_SEG                ; Now in PM, our old segments are meaningless,
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

    jmp $



MSG_PROT_MODE db "Successfully landed in 32 - bit Protected Mode.", 0

times 510-($ - $$) db 0             ; When compiled, our program must fit into 512 bytes,
                                    ; with the last two bytes being the magic number,
                                    ; so here, tell our assembly compiler t pad out our
                                    ; program with enough zeros bytes (db 0) to bring us 
                                    ; to the 510th byte.

dw 0xAA55                           ; Last two byte (one word) form the magic number,
                                    ; So BIOS knows we are a boot sector (as bootable).

