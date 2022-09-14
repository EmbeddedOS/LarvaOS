ORG 0x7c00 ; Assembly origin. 
BITS 16    ; 16 bit instruction.         

start:  
    mov ah, 0eh
    mov al, 'A'
    mov bx, 0
    int 0x10          ; Interrupt 10/AH=0Eh - VIDEO - TELETYPE OUTPUT.

times 510-($ - $$) db 0
dw 0xAA55

