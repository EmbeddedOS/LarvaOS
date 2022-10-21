
[BITS 32]           ; We need to use the [bits 32] directive to tell our the assembler that,
                    ; from that point onwards, it should encode in 32-bit mode instructions.

; ATA read sectors (LBA mode) 
;
; @param EAX - Logical Block Address of sector.
; @param ECX - Number of sectors to read.
; @param EDI - The address of buffer to put data obtained from disk.
;
; @return None
;
; Reference: ./docs/ATA_specifications.md

ATA_read_sector:
    mov ebx, eax    ; Save LBA in EBX.
    
; Send the highest 8 bits of the LBA 
; to hard disk controller.
    shr eax, 24         ; Get bit 24 - 27 in EAX.
    or eax, 0xE0        ; Set bit 6 in EAX for LBA mode,
                        ; select the master drive.
    mov dx, 0x1F6       ; Port to send drive and bit 24 - 27 of LBA.
    out dx, al

; Send the total sectors to read.
    mov eax, ecx        ; Get number of sectors from ECX.
    mov dx, 0x1F2       ; Port to send number of sectors.
    out dx, al

; Send more bits(0-7) of the LBA.
    mov eax, ebx        ; Get LBA from EBX.
    mov dx, 0x1F3       ; Port to send bit 0 - 7 of LBA.
    out dx, al

; Send more bits(8-15) of the LBA.
    mov dx, 0x1F4       ; Port to send bit 8 - 15 of LBA.
    mov eax, ebx        ; Get LBA from EBX.
    shr eax, 8          ; Get bit 8 - 15 in AL.
    out dx, al

; Send more bits(16-23) of the LBA.
    mov dx, 0x1F5       ; Port to send bit 16 - 23 of LBA.
    mov eax, ebx        ; Get LBA from EBX.
    shr eax, 16         ; Get bit 16 - 23 in AL.
    out dx, al

    mov dx, 0x1f7       ; Command port.
    mov al, 0x20        ; Read with retry.
    out dx, al

    
; Start reading all sectors into memory.
.next_sector:
    push ecx

; Checking if we need to read.
.try_again:
    mov dx, 0x1f7        ; Command port.
    in al, dx
    test al, 8          ; the sector buffer requires servicing.
    jz .try_again       ; until the sector buffer is ready.

; We need to read 256 words at a time.
    mov ecx, 256        ; to read 256 words = 1 sector.
    mov dx, 0x1F0       ; Data port, in and out.
    rep insw            ; in to [RDI].
    pop ecx
    loop .next_sector

; End of reading sectors into memory.
    ret


