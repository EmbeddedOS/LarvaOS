# Linker scripts

**GNU ld** supports configuration of the linking process via the use of linker scripts. Linker scripts are written in a specialized scripting language specific to the GNU ld application.

A Linker script's principal use is specifying the format and layout of the final executable binary. This is of particular relevance to OS development, where executable binaries will often require specific file layouts in order to be recognized by certain [boot-loaders](bootloader.md). **GNU GRUB** is one such boot-loader.

Linker scripts are typically invoked via the use of the `-T script.ld` command line argument when calling the `ld` application.

## 1. Keywords

Listed below are a selection of significant keywords used within linker scripts.

### 1.1. ENTRY

```linker
ENTRY(main)
ENTRY(MultibootEntry)
```

The `ENTRY` keyword is used for defining the **entry point** of an application, that being specifically the first executable instruction in the output file. This keyword accepts the symbol name for the entry point of the linked program/kernel as a single argument. The code pointed to by the symbol name provided will be the first byte of the `.text` section in ELF and PE binaries.

### 1.3. OUTPUT_FORMAT

```linker
OUTPUT_FORMAT(elf64-x86-64)
OUTPUT_FORMAT("pe-i386")
```

The `OUTPUT_FORMAT` directive takes a single argument. It specifies the output format of the executable. To find out what output formats are supported by your system binutils and GCC, the `objdump -i` command may be used.

Some commonly used formats are detailed below:

|Format|Description|
|------|-----------|
|binary|A flat binary with no formatting at all.|
|elf32-i386|32-bit ELF format for the i386 architecture.|
|elf64-x86-64|64-bit ELF format for the x86-64 architecture.|
|pe-i386|32-bit PE format for the i386 architecture.|

### 1.4. STARTUP

```linker
STARTUP(Boot.o)
STARTUP(crt0.o)
```

`STARTUP` takes one argument. It is which file you want to be linked to the very beginning of the executable. For user-land programs, this is usually `crt0.o` or `crtbegin.o`. For kernels, it is usually the file that contains your assembly boilerplate that initiates the stack and in some cases GDT and such and then calls your `kmain()`.

### 1.5. SEARCH_DIR

```linker
SEARCH_DIR(Directory)
```

This will add a path to your library search directory.

The `-nostdlib` flag will cause any library found in this path to be effectively ignored. I'm not sure why, it just seems to be how `ld` works. It treats linker script specified search directories as standard directories, and therefore ignores them with `-no-default-libs` and such flags.

### 1.6. INPUT

```linker
INPUT(File1.o File2.o File3.o ...)
INPUT
(
    File1.o
    File2.o
    File3.o
    ...
)
```

`INPUT` is a **'in-linker script'** replacement for adding object files to the command line. Where you would usually specify something like `ld File1.o File2.o`, the `INPUT` section can be used to do this inside the linker script instead.

### 1.7. OUTPUT

```linker
OUTPUT(Kernel.bin)
```

The `OUTPUT` command specifies the file to be generated as the output of the linking process. This is the name of the final binary created. The effect of this command is identical to the effect of the `-o filename` command line flag, which overrides it.

### 1.8. MEMORY

```linker
MEMORY
{
    ROM (rx) : ORIGIN = 0, LENGTH = 256k
    RAM (wx) : org = 0x00100000, len = 1M
}
```

`MEMORY` declares one or more memory regions with attributes specifying whether the region can be written to, read from or executed. This is mostly used in embedded system where different regions of address space may contain different access permission.

The example script above tells the linker that there are two memory regions:

* "**ROM**" starts at address `0x00000000`, is `256kB` in length, can be read and executed.
* "**RAM**" starts at address `0x00100000`, is `1MB` in length, can be written, read and executed.
  
### 1.9. SECTIONS

```linker
SECTIONS
{
  .text.start (_KERNEL_BASE_) : {
    startup.o( .text )
  }

  .text : ALIGN(0x1000) {
_TEXT_START_ = .;
    *(.text)
_TEXT_END_ = .;
  }

  .data : ALIGN(0x1000) {
_DATA_START_ = .;
    *(.data)
_DATA_END_ = .;
  }

  .bss : ALIGN(0x1000) {
_BSS_START_ = .;
    *(.bss)
_BSS_END_ = .;
  }
}
```

This script tell the linker to place code from `.text` section in `startup.o` at the beginning, starting al logical address `_KERNEL_BASE_`. This is then followed by paging-aligned sections for all `.text`, `.data` and `.bss` sections of all the other input files.

**Linker symbols** are defined holding the **start** and **end address** of each section. These symbols have external linkage within the application and are accessible as pointers within the code.

### 1.10. KEEP

TODO: complete me, please!

## 2. Symbols

It is possible to define arbitrary symbols within a linker script. These symbols are added into the program's symbol table. . Each symbol in the table has a **name** and an **associated address**. Symbols within a linker script that have been assigned a **value** will be given **external linkage**, and are accessible within the program's code as pointers.

An example script showing three different places that symbol assignment may be used can be seen below:

```linker
floating_point = 0;
SECTIONS
{
  .text :
    {
      *(.text)
      _etext = .;
    }
  _bdata = (. + 3) & ~ 3;
  .data : { *(.data) }
}
```

In the above example, the symbol `floating_point` has been defined as zero. The symbol `_etext` has been defined as the address following the last `.text` input section. The symbol `_bdata` has been defined as the address following the `.text` output section aligned upward to a 4 byte boundary.

An example usage of these symbols in C code can be seen below:

```c
/** Externally linked symbol */
extern uintptr_t _etext;
// ...
/** Pointer to the binary data at the address stored in the symbol expression. */
uint32_t* item = &_etext;
```
