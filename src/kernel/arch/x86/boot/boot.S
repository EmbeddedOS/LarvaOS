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

    mov si, MSG_REAL_MODE	
    call print_string_rm

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
%include "./gdt.asm" 
%include "./utils/print_string_pm.asm"
%include "./utils/print_string_rm.asm"
%include "./enable_A20_line.asm"
%include "./ATA_read_sector.asm"

[BITS 32]           ; We need to use the [bits 32] directive to tell our the assembler that,
                    ; from that point onwards, it should encode in 32-bit mode instructions.
start_protected_mode:
    mov eax, 0x01           ; @param EAX - Logical Block Address of sector.
                            ; We will read from sector 1,
                            ; because sector 0 is boot sector.
    mov ecx, 0x64           ; @param ECX - Number of sectors to read.
                            ; Read 100 sectors (1-100).
    mov edi, 0x0100000      ; @param EDI - The address of buffer to put data obtained from disk.
                            ; Load them into address 0x0100000 (kernel code).
    call ATA_read_sector    ; Read sectors in LBA mode.
    
    jmp CODE_SEG:0x0100000  ; Jump to kernel entry point.


MSG_REAL_MODE db "Entered to 16 - bit Real Mode.", 0

times 510-($ - $$) db 0             ; When compiled, our program must fit into 512 bytes,
                                    ; with the last two bytes being the magic number,
                                    ; so here, tell our assembly compiler t pad out our
                                    ; program with enough zeros bytes (db 0) to bring us 
                                    ; to the 510th byte.

dw 0xAA55                           ; Last two byte (one word) form the magic number,
                                    ; So BIOS knows we are a boot sector (as bootable).

