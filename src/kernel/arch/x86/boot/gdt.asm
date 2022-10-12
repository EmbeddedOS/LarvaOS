; Segment descriptor is an 8-byte structure that define the following properties of a protected-mode segment:
; + Base address (32 bits), which define where the segment begins in physical memory.
; + Segment Limit (20 bits), which define the size of the segment.
; + Various flags, which affect how the CPU interprets the segment, such as privilige level of code, read- or write-only.

; Structure of the segment descriptor:
;   |31					24|23		20|19		16|15		12|11		8|7					0|
;   |Base address (24-31) |G |DB|  |A |Limit 16-19|P |DPL  |S | Type	 |Base address(16-23)|
;   |			Base address ( Bit 0-15)		  |				Segment Limit (Bit 0-15)	 |
;
; G -- Granularity.
; P -- Segment Present.
; S -- Descriptor type (0 = system; 1 = code or data).
; A -- Available for use by system software.
; DPL -- Descriptor privilege level.
; DB -- Defaulr operation size (0 = 16-bit segment; 1 = 32-bit segment).
; Type -- Segment type.
; For code:
;   + Code: 1 for code, since this is a code segment.
;   + Conforming: 0, by not corming it means code in a segment with a lower privilege may not call code in this segment,
;                 this a key to memory protection.  
;   + Readable: 1, 1 if readable, 0 if execute-only. Readable allows us to read constants defined in the code.
;   + Accessed: 0 This is often used for debugging and virtual memory techniques,
;               since the CPU sets the bit when it accesses the segment.
; For data: 
;   + Code: 0 for data
;   + Expand down: 0. This allows the segment to expand down - TODO: explain this.
;   + Writable: 1. This allows the data segment to be written to, otherwise it would be read only.
;   + Accessed: 0 This is often used for debugging and virtual memory techniques, 
;               since the CPU sets the bit when it accesses the segment.




; Global Descriptor Table.
gdt_start:

gdt_null:           ; The mandatory null descriptor.
    dd 0x0          ; 'dd' means define double word (4 bytes).
    dd 0x0

; offset 0x8
gdt_code:           ; The code segment descriptor.
    ; base = 0x0, limit = 0xfffff,
    ; 1st flags: (present)1 (privilege)00 (descriptor type)1 -> 1001b.
    ; type flags : (code)1 (conforming)0 (readable)1 (accessed)0 -> 1010b.
    ; 2nd flags : (granularity)1 (32 - bit default)1 (64 - bit seg)0 (AVL)0 -> 1100b.
    dw 0xffff       ; Limit (bits 0-15).
    dw 0            ; Base (bits 0-15).
    db 0            ; Base (bits 16-23).
    db 10011010b    ; 1st flags, type flags.
    db 11001111b    ; 2nd flags, Limit (bits 16-19).
    db 0            ; Base (bits 24-31).

; offset 0x10
gdt_data:           ; The data segment descriptor.
    ; Same as code segment except for the type flags :
    ; type flags : ( code )0 ( expand down )0 ( writable )1 ( accessed )0 -> 0010 b
    dw 0xffff       ; Limit (bits 0-15).
    dw 0            ; Base (bits 0-15).
    db 0            ; Base (bits 16-23).
    db 10010010b    ; 1st flags, type flags.
    db 11001111b    ; 2nd flags, Limit (bits 16-19).
    db 0            ; Base (bits 24-31).

gdt_end:            ; The reason for putting a label at the end of GDT is so we can have
                    ; the assembler calculate the size of the GDT for the GDT descriptor.

; The GDT is pointed to by the GDTR register,
; This is loaded using the LGDT assembly instruction, whose argument is a pointer to a GDT Descriptor structure:
; GDT Descriptor (GDTR)
; |79 (64-bit mode)     | 
; |48 (32-bit mode)   16|15    0|
; |Offset               |Size   |
; |63 (64-bit mode)     |       |
; |31 (32-bit mode)   16|Size   |


; GDT descriptor.
gdt_descriptor:
    dw gdt_end - gdt_start - 1      ; Size of our GDT, always less one of the true size.
    dd gdt_start                    ; Start address of our GDT.

; Define some handy constants for the GDT segment descriptor offsets, which
; are what segment registers must contain when in protected mode. For example,
; when we set DS = 0x10 in PM, the CPU knows that we mean it to use the
; segment described at offset 0x10 (i.e. 16 bytes) in our GDT, which in our
; case is the DATA segment (0x0 -> NULL; 0x08 -> CODE; 0x10 -> DATA )
CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start