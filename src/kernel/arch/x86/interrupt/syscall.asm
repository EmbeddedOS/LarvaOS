[BITS 32]
section .asm

global syscall_wrapper
extern syscall_handler

syscall_wrapper:
    ; INTERRUPT FRAME START
    ; Already pushed to us automatically by processor upon entry to this interrupt.
    ; uint32_t ip;
    ; uint32_t cs;
    ; uint32_t flags;
    ; uint32_t sp;
    ; uint32_t ss;

    ; Pushes the user land general purpose registers to the stack.
    ; PUSHAD command - Push All General-Purpose Registers (Push EAX, ECX, EBX, original ESP, EBP, ESI and EDI).
    pushad

    ; INTERRUPT FRAME END

    ; Push the stack pointer, so that we are pointing to the interrupt frame.
    push esp

    ; Push the syscall number.
    push eax
    call syscall_handler
    mov dword[res], eax ; Push Integer that return from C function to a temporary variable.

    add esp, 8  ; We pushed stack pointer (4 bytes), syscall number (4bytes),
                ; so we can add to the stack pointer by 8,
                ; in 32 bit OS, a word is 4 bytes in size.
                ; So we add to the stack pointer by eight, 
                ; which well return our stack poiner to as 
                ; it was before we push these two elements to the stack.

    ; Restore general purpose registers for user land.
    ; POPAD command - POP All General-Purpose Registers.

    popad

    ; Move the result back into the eax register, the result will be returned to the user land.
    mov eax, [res]
    iretd

section .data
; Stored the return result from syscall_handler.
res: dd 0