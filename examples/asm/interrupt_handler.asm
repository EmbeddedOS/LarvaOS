ORG 0       ; Assembly origin. 
BITS 16     ; 16 bit instruction.         

setup_BPB:
    jmp short _start 
    nop

times 33 db 0   ; Set up BIOS parameter block to avoid corrupt data. 


_start:
    jmp 0x7c0:start



interrupt_0_handler:
    mov ah, 0eh 
    mov al, '0'
    mov bx, 0x00
    int 0x10
    iret


interrupt_1_handler:
    mov si, message				; Copy address of message to si register.
    call print
    iret



start:
    ; Set up memory start.
    cli                         ; Clear Interrupts.
    ; Set data segment, extra segment to 0x7C0.
    mov ax, 0x7c0
    mov ds, ax
    mov es, ax 
    
    ; Set stack segment to 0x00.
    mov ax, 0x00
    mov ss, ax 

    ;  Setting stack pointer points to 0x7c00 address.
    mov sp, 0x7c00
    sti                         ; Enable Interrupts.
    ; Set up memory end.

    ; Set up interrup handler 0 start.
    mov word[ss:0x00], interrupt_0_handler  ; 2 bytes for offset.
    mov word[ss:0x02], 0x7c0                ; 2 bytes for segment.
    ; Set up interrup handler 0 end.
    ; Set up interrup handler 1 start.
    mov word[ss:0x04], interrupt_1_handler  ; 2 bytes for offset.
    mov word[ss:0x06], 0x7c0                ; 2 bytes for segment.
    ; Set up interrup handler 1 end.
    int 0x00                                ; call interrupt 0.
    int 0x01                                ; call interrupt 1.
    jmp $


print:  
    mov bx, 0
.loop:
    lodsb
    cmp al, 0
    je .done
    call print_char
    jmp .loop
.done:
    ret 

print_char:
    mov ah, 0eh
    int 0x10          ; Interrupt 10/AH=0Eh - VIDEO - TELETYPE OUTPUT. 
    ret

message: db 'Hello world!', 0

times 510-($ - $$) db 0
dw 0xAA55

