# Bare Bones

In this tutorial you write a simple kernel for 32-bit x86 and boot it. This is the **FIRST STEP** in creating your own OS. This tutorial serves as an example of how to create a minimal system, but not as an example of how to properly structure your project.

You are about to begin development of a new OS. Perhaps one day, your new OS can be developed under itself. This is a process known as **bootstrapping** or going self-hosted. Today, you will simply set up a system that can compile your new OS from an existing OS. This process is known as [cross-compiling](cross_compiler.md) and it is the first step in OS development.

This tutorial uses existing technology to get you started and straight into kernel development, rather than developing your own programming language, your own compiler, and your own boot-loader. In this tutorial, you will use:

* The [GNU linker](linker_script.md) from **Binutils** to link your objects files into final kernel.
* The **GNU Assembler** form **Binutils** (or optionally **NASM**) to assemble instructions into object files containing machine code.
* The **GCC** to compile your high level code into assembly.
* The C/C++ programming language to write the high level parts of your kernel.
* The **GRUB** boot-loader to boot-load your kernel using **Multi-boot** boot protocol that loads us into 32-bit protected mode with paging disable.
* The **ELF** as the **executable format** that gives us control of where and how the kernel is loaded.

Succeed at OS development requires **BECOMING AN EXPERT**, having patience, and reading all the instruction very carefully. You need to read everything in this article before proceeding.

## 1. Building a Cross-Compiler

The first thing you should do is set up a [GCC Cross-compiler](cross_compiler.md). You have not yet modified your compiler to know about the existence of your OS, so you will use a generic target called i686-elf, which provides you with a toolchain targeting the System V ABI.

You will **not** be able to correctly compile your OS without a cross-compiler.

You will **not** be able to complete this tutorial with a x86_64-elf cross-compiler, as GRUB is only able to load 32-bit multi-boot kernels. If this is your first OS project, you should do a 32-bit kernel first. If you use a x86_64 compiler instead and somehow bypass the later sanity check, you will end up with a kernel that GRUB doesn't know how to boot.

## 2. Overview

By now, you should have set up your [cross-compiler](cross_compiler.md) for i686-elf. This tutorial provides a minimal solution for creating an OS for x86. It doesn't serve as a recommend skeleton for project structure, but rather as an example of minimal kernel. In this simple case, you just need three input files:

* `boot.s` - kernel entry point that sets up the processor environment.
* `kernel.c` - your actual kernel routines.
* `linker.ld` - for linking the above files.

## 3. Booting the OS

To start the OS, an existing piece of software will be needed to load it. This is called the boot-loader and in this tutorial you will be using **GRUB**. Writing your own boot-loader is an **advanced subject**, but it is commonly done. We will later configure the boot-loader, but the OS needs to handle when the boot-loader passes control to it.

The kernel is passed a very minimal environment, in which the stack is not set up yet, virtual memory is not yet enabled, hardware is not initialized, and so on.

The first task you will deal with is how the boot-loader starts the kernel. OS-developers are lucky because there exists a Multi-boot standard, which describes an easy interface between the boot-loader and the OS kernel. **It works by putting a few MAGIC VALUES in some global variables(known as a multi-boot header)**, which is **searched for by the boot-loader**. When it sees these values, it recognizes the kernel as multi-boot compatible and it knows how to load us, and it can even forward us important information such as memory maps, but you won't need that yet.

Since there is no stack yet and you need to make sure the global variables are set correctly, you will do this in assembly.

### 3.1. Bootstrap Assembly

You will now create a file called `boot.s` and discuss its contents. In this example, you are using GNU assembler, which is part of the cross-compiler toolchain you built earlier. This assembler integrates very well with the rest of the GNU toolchain.

The **VERY MOST IMPORTANT** piece to create is the multi-boot header, as it must be **VERY EARLY** in the kernel binary, or the boot-loader will fail to recognize us.

```asm
/* Declare constants for the multiboot header. */
.set ALIGN,    1<<0             /* align loaded modules on page boundaries */
.set MEMINFO,  1<<1             /* provide memory map */
.set FLAGS,    ALIGN | MEMINFO  /* this is the Multiboot 'flag' field */
.set MAGIC,    0x1BADB002       /* 'magic number' lets bootloader find the header */
.set CHECKSUM, -(MAGIC + FLAGS) /* checksum of above, to prove we are multiboot */
 
/* 
Declare a multiboot header that marks the program as a kernel. These are magic
values that are documented in the multiboot standard. The bootloader will
search for this signature in the first 8 KiB of the kernel file, aligned at a
32-bit boundary. The signature is in its own section so the header can be
forced to be within the first 8 KiB of the kernel file.
*/
.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM
 
/*
The multiboot standard does not define the value of the stack pointer register
(esp) and it is up to the kernel to provide a stack. This allocates room for a
small stack by creating a symbol at the bottom of it, then allocating 16384
bytes for it, and finally creating a symbol at the top. The stack grows
downwards on x86. The stack is in its own section so it can be marked nobits,
which means the kernel file is smaller because it does not contain an
uninitialized stack. The stack on x86 must be 16-byte aligned according to the
System V ABI standard and de-facto extensions. The compiler will assume the
stack is properly aligned and failure to align the stack will result in
undefined behavior.
*/
.section .bss
.align 16
stack_bottom:
.skip 16384 # 16 KiB
stack_top:
 
/*
The linker script specifies _start as the entry point to the kernel and the
bootloader will jump to this position once the kernel has been loaded. It
doesn't make sense to return from this function as the bootloader is gone.
*/
.section .text
.global _start
.type _start, @function
_start:
    /*
    The bootloader has loaded us into 32-bit protected mode on a x86
    machine. Interrupts are disabled. Paging is disabled. The processor
    state is as defined in the multiboot standard. The kernel has full
    control of the CPU. The kernel can only make use of hardware features
    and any code it provides as part of itself. There's no printf
    function, unless the kernel provides its own <stdio.h> header and a
    printf implementation. There are no security restrictions, no
    safeguards, no debugging mechanisms, only what the kernel provides
    itself. It has absolute and complete power over the
    machine.
    */
    
    /*
    To set up a stack, we set the esp register to point to the top of the
    stack (as it grows downwards on x86 systems). This is necessarily done
    in assembly as languages such as C cannot function without a stack.
    */
    mov $stack_top, %esp
    
    /*
    This is a good place to initialize crucial processor state before the
    high-level kernel is entered. It's best to minimize the early
    environment where crucial features are offline. Note that the
    processor is not fully initialized yet: Features such as floating
    point instructions and instruction set extensions are not initialized
    yet. The GDT should be loaded here. Paging should be enabled here.
    C++ features such as global constructors and exceptions will require
    runtime support to work as well.
    */
    
    /*
    Enter the high-level kernel. The ABI requires the stack is 16-byte
    aligned at the time of the call instruction (which afterwards pushes
    the return pointer of size 4 bytes). The stack was originally 16-byte
    aligned above and we've pushed a multiple of 16 bytes to the
    stack since (pushed 0 bytes so far), so the alignment has thus been
    preserved and the call is well defined.
    */
    call kernel_main
    
    /*
    If the system has nothing more to do, put the computer into an
    infinite loop. To do that:
    1) Disable interrupts with cli (clear interrupt enable in eflags).
       They are already disabled by the bootloader, so this is not needed.
       Mind that you might later enable interrupts and return from
       kernel_main (which is sort of nonsensical to do).
    2) Wait for the next interrupt to arrive with hlt (halt instruction).
       Since they are disabled, this will lock up the computer.
    3) Jump to the hlt instruction if it ever wakes up due to a
       non-maskable interrupt occurring or due to system management mode.
    */
    cli
1:  hlt
    jmp 1b
 
/*
Set the size of the _start symbol to the current location '.' minus its start.
This is useful when debugging or when you implement call tracing.
*/
.size _start, . - _start
```

You can then assemble `boot.s` using:

```asm
i686-elf-as boot.s -o boot.o

```

## 2. Implementing the kernel

So far you have written the bootstrap assembly stub that sets up the processor such that high level language such as C can be used. It is also possible to use other language such as C++.

## 2.1. Freestanding and Hosted Environments

If you have done C or C++ programming in user-space, you have used a so-called **Hosted Environment**. **Hosted** means that there is C standard library and other useful runtime features.

Alternatively, there is the **Freestanding** version, which is what you are using here. **Freestanding** means that there is **no** C standard library, only what you provide yourself.

However, some header files are actually not part of the C standard library, but rather the compiler. These remain available even in freestanding C source code. In this case you use `<stdbool.h>` to get bool datatype, `<stddef.h>` to get size_t and NULL, and `<stdint.h>` to get the `intx_t` and `uint_x` data types which are invaluable for OS development, where you need to make sure that the variable is of an exact size.

Additional you can access the `<float.h>`, `<iso646.h>`, `<limits.h>`, and `<stdarg.h>` headers, as they are also freestanding. GCC actually ships a few more headers, but these are special purpose.

### 2.2. Writing a kernel in C

The following shows how to create a simple kernel in C. This kernel uses the VGA text mode buffer (located at 0xB8000) as the output device. It sets up a simple driver that remembers the location of the next character in this buffer and provides a primitive for adding a new character. Adding this will be your first task. Please take a few moments to understand the code.

**IMPORTANT NOTE**: the VGA text mode (as well as **BIOS**) is deprecated on newer machines, and **UEFI** only support pixel buffers. For forward compatibility you might want to start with that.

```cpp
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
 
/* Check if the compiler thinks you are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif
 
/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif
 
/* Hardware text mode color constants. */
enum vga_color {
    VGA_COLOR_BLACK = 0,
    VGA_COLOR_BLUE = 1,
    VGA_COLOR_GREEN = 2,
    VGA_COLOR_CYAN = 3,
    VGA_COLOR_RED = 4,
    VGA_COLOR_MAGENTA = 5,
    VGA_COLOR_BROWN = 6,
    VGA_COLOR_LIGHT_GREY = 7,
    VGA_COLOR_DARK_GREY = 8,
    VGA_COLOR_LIGHT_BLUE = 9,
    VGA_COLOR_LIGHT_GREEN = 10,
    VGA_COLOR_LIGHT_CYAN = 11,
    VGA_COLOR_LIGHT_RED = 12,
    VGA_COLOR_LIGHT_MAGENTA = 13,
    VGA_COLOR_LIGHT_BROWN = 14,
    VGA_COLOR_WHITE = 15,
};
 
static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) 
{
    return fg | bg << 4;
}
 
static inline uint16_t vga_entry(unsigned char uc, uint8_t color) 
{
    return (uint16_t) uc | (uint16_t) color << 8;
}
 
size_t strlen(const char* str) 
{
    size_t len = 0;
    while (str[len])
        len++;
    return len;
}
 
static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
 
size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;
 
void terminal_initialize(void) 
{
    terminal_row = 0;
    terminal_column = 0;
    terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    terminal_buffer = (uint16_t*) 0xB8000;
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            terminal_buffer[index] = vga_entry(' ', terminal_color);
        }
    }
}
 
void terminal_setcolor(uint8_t color) 
{
    terminal_color = color;
}
 
void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) 
{
    const size_t index = y * VGA_WIDTH + x;
    terminal_buffer[index] = vga_entry(c, color);
}
 
void terminal_putchar(char c) 
{
    terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
    if (++terminal_column == VGA_WIDTH) {
        terminal_column = 0;
        if (++terminal_row == VGA_HEIGHT)
            terminal_row = 0;
    }
}
 
void terminal_write(const char* data, size_t size) 
{
    for (size_t i = 0; i < size; i++)
        terminal_putchar(data[i]);
}
 
void terminal_writestring(const char* data) 
{
    terminal_write(data, strlen(data));
}
 
void kernel_main(void) 
{
    /* Initialize terminal interface */
    terminal_initialize();
 
    /* Newline support is left as an exercise. */
    terminal_writestring("Hello, kernel World!\n");
}
```

Notice how in the code you wished to use the common C function `strlen()`, but this function is part of the C standard library that you don't have available. Instead, you relied on the freestanding header `<stddef.h>` to provide `size_t` and you simply declared your own implementation of `strlen()`. You have to do this for every function you wish to use (as the freestanding headers only provide macros and data types.).

Compile using:

```bash
i686-elf-gcc -c kernel.c -o kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
```

### 2.3. Writing a kernel in C++

Writing a kernel in C++ is easy. Note that not all features from the language is available. For instance, exception support requires special runtime support and so does memory allocation. To write a kernel in C++, simply adopt code above: Add an `extern "C"` declaration to the main method.

Notice how the `kernel_main` function has to be declared with C linkage, as otherwise the compiler would include information in the assembly name (name mangling). this complicates calling the function from your above assembly stub and you therefore use C linkage, where the symbol name is the same as the name of the function. Save the code as `kernel.c++` (or what your favorite C++ filename extension is).

You can compile the file `kernel.c++` using:

```bash
i686-elf-g++ -c kernel.c++ -o kernel.o -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti
```

Note that you must have also built a cross C++ compiler for this work.

## 3. Linking the kernel

You can now assemble `boot.s` and compile `kernel.c`. This produces two object files that each contain part of the kernel. To create the full and final kernel you will have to **link** these object files into the final kernel program, usable by the boot-loader. when developing user-space programs, your toolchain ships with default scripts for linking such programs. However, these are unsuitable for kernel development and you need to provide your own customized linker script. Save the following in `linker.ld`:

```linker
/* The bootloader will look at this image and start execution at the symbol
   designated as the entry point. */
ENTRY(_start)
 
/* Tell where the various sections of the object files will be put in the final
   kernel image. */
SECTIONS
{
    /* Begin putting sections at 1 MiB, a conventional place for kernels to be
       loaded at by the bootloader. */
    . = 1M;
 
    /* First put the multiboot header, as it is required to be put very early
       early in the image or the bootloader won't recognize the file format.
       Next we'll put the .text section. */
    .text BLOCK(4K) : ALIGN(4K)
    {
    *(.multiboot)
    *(.text)
    }
 
    /* Read-only data. */
    .rodata BLOCK(4K) : ALIGN(4K)
    {
        *(.rodata)
    }
 
    /* Read-write data (initialized) */
    .data BLOCK(4K) : ALIGN(4K)
    {
        *(.data)
    }
 
    /* Read-write data (uninitialized) and stack */
    .bss BLOCK(4K) : ALIGN(4K)
    {
        *(COMMON)
        *(.bss)
    }
 
    /* The compiler may produce other sections, by default it will put them in
       a segment with the same name. Simply add stuff here as needed. */
}
```

With these components tou can now actually build the final kernel. We use the compiler as the linker as it allows it greater control over the link process.

Note that if your kernel is written in C++, you should use the C++ compiler instead.

You can then link your kernel using:

```bash
i686-elf-gcc -T linker.ld -o myos.bin -ffreestanding -O2 -nostdlib boot.o kernel.o -lgcc
```

Note: Some tutorials suggest linking with i686-elf-ld rather than the compiler, however this prevents the compiler from performing various tasks during linking.

The file `myos.bin` is now your kernel (all other files are no longer needed).

Note that we are linking against `libgcc` which implements various runtime routines that your cross-compiler depends on. Leaving it out will give you problems in the future. If you did not build and install `libgcc` as part of your cross-compiler, you should go back now and build a cross-compiler with `libgcc`. The compiler depends on this library and will use it regardless of whether you provide it or not.

## 4. Verifying Multi-boot

If you have **GRUB** installed, you can check whether a file has a valid Multi-boot version 1 header, which is the case for your kernel.

It's important that the Multi-boot header is within the first `8 KiB` of the actual program file at 4 byte alignment.  This can potentially break later if you make a mistake in the boot assembly, the linker script, or anything else that might go wrong. If the header isn't valid, GRUB will give an error that it can't find a Multi-boot header when you try to boot it. This code fragment will help you diagnose such cases:

```bash
grub-file --is-x86-multiboot myos.bin
```

## 5. Booting the kernel

In a few moments, you will see your kernel in action.

### 5.1. Building a bootable cdrom image

You can easily create a bootable CD-ROM image containing the **GRUB** bootloader and your kernel using the program **grub-mkrescue**.

You may need to install the GRUB utility programs and the program `xorriso` (version 0.5.6 or higher). First you should create a file called grub.cfg containing the contents:

```text
menuentry "myos" {
    multiboot /boot/myos.bin
}
```

Note that the braces must be placed as shown here. You can now create a bootable image of your operating system by typing these commands:

```bash
mkdir -p isodir/boot/grub
cp myos.bin isodir/boot/myos.bin
cp grub.cfg isodir/boot/grub/grub.cfg
grub-mkrescue -o myos.iso isodir
```

Congratulations! You have now created a file called `myos.iso` that contains your Hello World operating system. If you don't have the program **grub-mkrescue** installed, now is a good time to install **GRUB**. It should already be installed on Linux systems.

### 5.2. Testing your operating system (QEMU)

Virtual Machines are very useful for development operating systems, as they allow you to quickly test your code and have access to the source code during the execution. They start very quickly, especially combined with small operating systems such as yours.

Install QEMU from your repositories, and then use the following command to start your new operating system.

```bash
qemu-system-i386 -cdrom myos.iso
```

Additionally, QEMU supports booting multiboot kernels directly without bootable medium:

```bash
qemu-system-i386 -kernel myos.bin
```

### 5.3. Testing your operating system (Real Hardware)

The program grub-mkrescue is nice because it makes a bootable ISO that works on both real computers and virtual machines. You can then build an ISO and use it everywhere. To boot your kernel on your local computer you can install `myos.bin` to your `/boot` directory and configure your bootloader appropriately.

Or alternatively, you can burn it to an USB stick (erasing all data on it!). To do so, simply find out the name of the USB block device, in my case `/dev/sdb` but this may vary, and using the wrong block device (your harddisk, gasp!) may be disastrous. If you are using Linux and `/dev/sdx` is your block name, simply:

```bash
sudo dd if=myos.iso of=/dev/sdx && sync
```

Your operating system will then be installed on your USB stick. If you configure your BIOS to boot from USB first, you can insert the USB stick and your computer should start your operating system.

Alternatively, the `.iso` is a normal cdrom image. Simply burn it to a CD or DVD if you feel like wasting one of those on a few kilobytes large kernel.
