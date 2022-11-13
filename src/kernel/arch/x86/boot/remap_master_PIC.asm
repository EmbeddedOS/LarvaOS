; In protected mode, the IRQs 0 to 7 conflict with the CPU exception
; which are reserved by Intel up until 0x1F. (It was an IBM design mistake.) 
; Consequently it is difficult to tell the difference between an IRQ or an software error.
; It is thus recommended to change the PIC's offsets (also known as remapping the PIC) so that IRQs use non-reserved vectors. 
; A common choice is to move them to the beginning of the available range (IRQs 0..0xF -> INT 0x20..0x2F).
; For that, we need to set the master PIC's offset to 0x20 and the slave's to 0x28.

[BITS 32]
remap_master_PIC:
    mov al, 0x11    ; First, send initialize command to PIC.     
    out 0x20, al

    mov al, 0x20    ; Send its vector offset to Master PIC Data, 
                    ; interrupt 0x20 is where master ISR should start.
    out 0x21, al

    mov al, 0x01
    out 0x21, al
    ret