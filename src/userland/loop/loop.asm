[BITS 32]
section .asm
global start

start:
    mov eax, 0 ; First argument, syscall number.
    int 0x80

loop:
    jmp loop