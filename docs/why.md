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

## What is exactly memory map of our OS?

After boot-loader find kernel code and load them into RAM. Physical Address Space will look like:

0x00000000  +---------------------------------------+
            | Interrupts vector table (1024 bytes). |
0x00000400  +---------------------------------------+
            | BIOS Data area (256 bytes).           |
0x00000500  +---------------------------------------+
            | OS load area (30,464 bytes).          |
0x00007C00  +---------------------------------------+
            | MBR (512 bytes).                      |
0x00007E00  +---------------------------------------+
            | Boot Stack/Data area (512 bytes).     |
0x00007FFF  +---------------------------------------+
            |                                       |
0x000A0000  +---------------------------------------+
            | Video memory.                         |
0x000C0000  +---------------------------------------+
            |                                       |
0x00010000  +---------------------------------------+
            | Kernel Image (.text, .data, .ro, etc.)|
            +---------------------------------------+
            | Free Low memory.                      |
0xC0000000  +---------------------------------------+
            | Free High memory.                     |
0xFFFFFFFF  +---------------------------------------+

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

## Why can't we make a file with name greater than 8 characters that will be mounted to our disk?

We are using FAT 16 as filesystem which support maximum filename is 8 characters and extension is 3 characters:

```C
struct fat_directory_item
{
    uint8_t filename[8];
    uint8_t ext[3];
    uint8_t attribute;
    uint8_t reserved;
    uint8_t creation_time_tenths_of_a_sec;
    uint16_t creation_time;
    uint16_t creation_date;
    uint16_t last_access;
    uint16_t high_16_bits_first_cluster;
    uint16_t last_mod_time;
    uint16_t last_mod_date;
    uint16_t low_16_bits_first_cluster;
    uint32_t filesize;
} __attribute__((packed));

```

For detailed, visit [link](https://www.keil.com/pack/doc/mw/FileSystem/html/fat_fs.html)
**Long and Short File Name handling**
File System Component is provided with the library with long (LFN) and short (SFN or 8.3) filename support. Since there is no compulsory algorithm for creating the 8.3 name from an LFN, File System Component uses convention described below.

Upper-cased or lowercased name which is 8.3 valid, is stored upper-cased in a single SFN entry
Example: "TEXTFILE.TXT" is stored as "TEXTFILE.TXT" (SFN)
Example: "textfile.txt" is stored as "TEXTFILE.TXT" (SFN)
Mixed-case name which is 8.3 valid, is stored mixed-case in LFN entry and SFN entry is created, which is concluded with tilde and a numeric value.
Example: "TextFile.txt" is stored as "TextFile.txt" (LFN) and "TEXTFI~1.TXT" (SFN)
Name which is not 8.3 valid is stored in LFN entry and SFN entry. Name written in SFN entry is upper-cased, stripped of invalid 8.3 characters which are replaced with underscore "_" and concluded with tilde and a numeric value.
Example: "Tex+File.txt" is stored as "Tex+File.txt" (LFN) and "TEX_FI~1.TXT (SFN)"

## Add symbols and flags to build Origin CPP program

- Because we don't use the C++ standard library built-in, so we need to add something to able to build CPP program. Add symbols:

```Cpp


extern "C"
{

    int __cxa_atexit(void (*Destructor)(void *), void *Parameter, void *HomeDSO);
    void __cxa_finalize(void *);
    void __cxa_pure_virtual();
    void __stack_chk_guard_setup();
    void __attribute__((noreturn)) __stack_chk_fail();
    void _Unwind_Resume();
}

void *__dso_handle;
void *__stack_chk_guard(0);

namespace __cxxabiv1
{
    __extension__ typedef int __guard __attribute__((mode(__DI__)));

    extern "C"
    {
        int __cxa_guard_acquire(__guard *Guard) { return !*(char *)(Guard); }
        void __cxa_guard_release(__guard *Guard) { *(char *)Guard = 1; }
        void __cxa_guard_abort(__guard *) {}
    }
}

int __cxa_atexit(void (*)(void *), void *, void *)
{
    return 0;
}

void _Unwind_Resume()
{
}

void __cxa_finalize(void *)
{
}

void __cxa_pure_virtual()
{
}

void __stack_chk_guard_setup()
{
    unsigned char *Guard;
    Guard = (unsigned char *)&__stack_chk_guard;
    Guard[sizeof(__stack_chk_guard) - 1] = 255;
    Guard[sizeof(__stack_chk_guard) - 2] = '\n';
    Guard[0] = 0;
}

void __attribute__((noreturn)) __stack_chk_fail()
{
    for (;;)
        ;
}

void *__gxx_personality_v0 = (void *)0x00;
```

- Error `undefined reference to '_ZTVN10__cxxabiv120__si_class_type_infoE'` add flag: -fno-rtti

## Global overloading of new and delete operator

```Cpp
[[gnu::visibility("default")]] void *operator new(size_t size);
[[gnu::visibility("default")]] void operator delete(void *ptr);
[[gnu::visibility("default")]] void *operator new[](size_t size);
[[gnu::visibility("default")]] void operator delete[](void *ptr);
[[gnu::visibility("default")]] void operator delete(void *ptr, size_t size);
[[gnu::visibility("default")]] void operator delete[](void *ptr, size_t size);
```

- Some architecture using delete operator with two arguments, so we need to define two functions for this operator:

```Cpp
[[gnu::visibility("default")]] void operator delete(void *ptr);
[[gnu::visibility("default")]] void operator delete(void *ptr, size_t size);
```
