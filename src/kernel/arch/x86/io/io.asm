section .asm

global inb
global inw
global outb
global outw

; Basic I/O Functions.
inb:
    push ebp
    mov ebp, esp

    xor eax, eax            ; Clear AX register.
    mov edx, [ebp+8]        ; Get first parameter and mov it to DX register.
    in al, dx               ; Read from port that is specified by DX register,
                            ; and write data to AX register.
                            ; Return value will be get from AX register.
    pop ebp
    ret

inw:
    push ebp
    mov ebp, esp

    xor eax, eax
    mov edx, [ebp+8]
    in ax, dx

    pop ebp
    ret

outb:
    push ebp
    mov ebp, esp

    mov eax, [ebp+12]       ; Get second parameter and mov it to AX register.
    mov edx, [ebp+8]        ; Get first parameter and mov it to DX register.
    out dx, al              ; Write data from AX register to port 
                            ; that is specified by DX register,

    pop ebp
    ret

outw:
    push ebp
    mov ebp, esp

    mov eax, [ebp+12]
    mov edx, [ebp+8]
    out dx, ax

    pop ebp
    ret