[BITS 32]           ; We need to use the [bits 32] directive to tell our the assembler that,
                    ; from that point onwards, it should encode in 32-bit mode instructions.

global kernel_entry_point   ; We make a global symbol 'kernel entry point',
                            ; so the linker can find and jump to it first.

extern kernel_main, __CTOR_LIST__, __CTOR_END__, __DTOR_LIST__, __DTOR_END__

KERNEL_DATA_SEG equ 0x10
; NOTE: this symbol will be loaded at address 0x0100000 by linker, so
; DO NOT push any codes in front of 'kernel_entry_point' symbol.
; If can not jump to this symbol, my OS will be crashed.

kernel_entry_point:
    ; By now we are assuredly in 32 - bit protected mode.
    ; Initialize registers and the stack once in Protected Mode.
   mov ax, KERNEL_DATA_SEG          ; Now in PM, our old segments are meaningless,
   mov ds, ax                       ; so we point our segment registers to the 
   mov es, ax                       ; data selector we defined in our GDT.
   mov fs, ax
   mov gs, ax 
   mov ss, ax
   mov ebp, 0x00200000              ; Update our stack position so it is right 
   mov esp, ebp                     ; at the top of the free space.

   call enable_A20_line             ; Enable A20 line.

   mov ebx , MSG_PROT_MODE
   call print_string_pm             ; Use our 32 - bit print routine.

call_global_constructors:           ; Call all global constructors of static, global object.
   mov ebx, __CTOR_LIST__
   jmp check_constructor_list
call_constructor:
   call [ebx]
   add ebx, 0x04
check_constructor_list:
   cmp ebx, __CTOR_END__
   jb call_constructor

   call remap_master_PIC            ; Remap master PIC.
   call kernel_main                 ; jump to kernel main function.

call_global_destructors:            ; Call all global destructors of static, global object.
   mov ebx, __DTOR_LIST__
   jmp check_destructor_list
call_destructor:
   call [ebx]
   add ebx, 0x04
check_destructor_list:
   cmp ebx, __DTOR_END__
   jb call_destructor

   jmp $

%include "./utils/print_string_pm.asm"
%include "./boot/enable_A20_line.asm"
%include "./boot/remap_master_PIC.asm"


MSG_PROT_MODE db "Entered to 32 - bit Protected Mode.", 0

times 512-($ - $$) db 0             ; Align kernel entry point into a complete sector.