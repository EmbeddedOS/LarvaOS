[BITS 32]
section .asm

global restore_general_purpose_registers
global task_return
global load_user_data_segment_registers
global load_kernel_data_segment_registers

; C prototype: void task_return(struct registers* regs);
task_return:
    mov ebp, esp

    ; 1. Push the data segment: uint32_t ss;
    mov ebx, [ebp + 4]
    push dword [ebx + 44] ; Push the data/stack selector.

    ; 2. Push the stack address: uint32_t esp;
    push dword [ebx + 40] ; Push the stack pointer.

    ; 3. Push the flags: uint32_t flags;
    pushf
    pop eax
    or eax, 0x200
    push eax

    ; 4. Push the code segment: uint32_t cs;
    push dword [ebx + 32]

    ; 5. Push the IP to execute.
    push dword [ebx + 28]

    ; 6. Setup some segment registers.
    mov  ax, [ebx + 44]
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; 7. Restore general purpose registers.
    push dword [ebp + 4]
    call restore_general_purpose_registers
    
    add esp, 4  ; Restore the stack, we are unable to pop from the stack because 
                ; we risk corrupting a register.
                ; We just pushed 4 bytes when `Restore general purpose registers.` 
                ; when we pass `registers structure` to it.
            
    ; 8. Leave Kernel land and execute in user land.
    ireqd

; C prototype: void restore_general_purpose_registers(struct registers* regs);
restore_general_purpose_registers:
    push ebp
    mov ebp, esp

    mov ebx, [ebp + 8]      ; Restore uint32_t ebx;
    mov edi, [ebx]          ; Restore uint32_t edi;          
    mov esi, [ebx + 4]      ; Restore uint32_t esi;  
    mov ebp, [ebx + 8]      ; Restore uint32_t ebp;
    mov edx, [ebx + 16]
    mov ecx, [ebx + 20]     ; Restore uint32_t edx;
    mov eax, [ebx + 24]     ; Restore uint32_t ecx;
    mov ebx, [ebx + 12]     ; Restore uint32_t eax;

    add esp, 4
    ret

; C prototype: void load_user_data_segment_registers();
; Change all the segment registers to the user data segment registers.

load_user_data_segment_registers:
    mov ax, 0x23        ; #define USER_DATA_SEGMENT 0x23
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    ret

; C prototype: void load_kernel_data_segment_registers();
; Change all the segment registers to the kernel data segment registers.

load_kernel_data_segment_registers:
    mov ax, 0x10        ; #define KERNEL_DATA_SELECTOR 0x10
    mov ds, ax
    mov es, ax
    mov gs, ax
    mov fs, ax
    ret