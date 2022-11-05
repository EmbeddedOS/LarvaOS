# Why?

## Why do we need to build file boot.asm in binary format?

First, need to understand what is a binary file? A Binary file is a pure binary file with no memory fix-ups or relocations, it has explicit instructions to be loaded at a specific memory address.

The [boot.asm](../src/kernel/arch/x86/boot/boot.asm) file is written as a raw data, will be build in 'raw' binary format and become **First sector** in our disk image (size of this file is alway 512 bytes).

The (legacy) BIOS checks boot-able devices for a boot signature a so called magic number. So, when it checks our OS across devices, it will find the **First sector**.

The **Master Boot Record (MBR)** is the information in the first sector of any hard disk or diskette that identifies how and where an operating system is located so that it can be boot(loaded) into the computer's main storage or random access memory.

## Why while building our kernel image, we need to use dd or cat command to copy files to final image?

We are emulating a OS that is booted from a disk. We are actually building a disk image, that contains **MBR** partition, and multiple another partitions.

The disk format for our OS:

```text
|   sector 1    |    sector 2   |   sector 3   |...         |
|0           511|512        1023|1024      1535|...         |
|MBR            |Our kernel     | Our kernel   |...         |
|               |                                           |    
```

We only simply copy kernel image to the disk image, put it right behind the **First Sector** (our boot-sector). So, as a result, the kernel image will become **Second Sector** and the next sectors (depend on size of kernel image) in our disk image.

We set entry point is `0x0100000`, so boot loader will jump from MBR code to kernel code based on information of **Global Descriptor Table**.

## Why do we copy 100 sectors to our disk image?

What if our kernel does not fill up to a sector (lesser than 512 bytes or not divisible by 512). So, we need to fill null data to our disk image, and ATA driver in the boot-loader will can read all the kernel code (ATA drive read at least 100 sectors a time). If our kernel image increase size in future, we will continue increase this number.

## Why do we describe '.ctor' and '.dtor' sections linker script file? what are they?

We are using C++ as main programming language, these sections store addresses of constructors and destructors of static, global objects.

```linker
.data : ALIGN (0x1000)
{ /** GNU C++ will will normally arrange to put the addresses of 
    * global constructors and destructors into the .ctors and .dtors 
    * sections. We need to get the lists to call constructors and
    * destructors of global objects before and after using main().
    */
   __CTOR_LIST__ = .;
   *(.ctor*)
   __CTOR_END__ = .;
   __DTOR_LIST__ = .;
   *(.dtor*)
   __DTOR_END__ = .;
   *(.data)
}
```

We need to get and call them before starting and ending kernel main function:

```asm
call_global_constructors:           ; Call all global constructors of static, global object.
   mov ebx, __CTOR_LIST__
   jmp check_constructor_list
call_constructor:
   call [ebx]
   add ebx, 0x04
check_constructor_list:
   cmp ebx, __CTOR_END__
   jb call_constructor

    jmp kernel_main                 ; jump to kernel main function.

call call_global_destructors:       ; Call all global destructors of static, global object.
   mov ebx, __DTOR_LIST__
   jmp check_destructor_list
call_destructor:
   call [ebx]
   add ebx, 0x04
check_destructor_list:
   cmp ebx, __DTOR_END__
   jb call_destructor
```

## Why do we use 'extern "C"' keyword multiple times?

Any object file built by gcc, when linking against object file built by g++, needs to use the external "C" keyword. Because symbol name formats are different.

## Why do we align sections (.data, .text, etc.) to 0x1000? what is 0x1000?

`0x1000` is size of a typical memory page. We are aligning sections to the page size.

## Why do not we use inline assembly function in C code instead of using bare assembly function in assembly file?

Yeah, I know. They are the same. However, I want C and assembly should be kept separate. For example, using inline assembly:

```c
__asm__ ("lidt %0" :: "m"(idtr_descriptor));
```

Or writes into assembly function:

```asm
load_interrupt_descriptor_table:
    push ebp            ; Push current base pointer to stack.
    mov ebp, esp        ; Move the stack pointer into the base pointer,
                        ; So we get reference to our frame.

    mov ebx, [ebp+8]
    lidt [ebx]

    pop ebp            ; Pop current base pointer.
    ret
```
