# LarvaOS

A multitasking operating system and kernel with an interactive shell.

## Using bless command to see your own kernel image

bless - graphical hexadecimal Gtk# editor.
Edit  the  FILEs  as a `sequence of bytes`, allowing read/write, search, pattern finding,  efficient  query-replace,  multi-tabbing,  customized data-views, plugins, and many other features. Using bless command:

```bash
bless ./bin/LavaOS.img
```

## Run your OS using qemu

```bash
qemu-system-x86_64 -hda ./bin/LavaOS.img
```

## Using cross compiler

```bash
./bin/cross/bin/$TARGET-gcc --version   # For compile C files.
./bin/cross/bin/$TARGET-g++ --version   # For compile C++ files.
./bin/cross/bin/$TARGET-ld              # Linker.
```

## Assemble a assembly file with nasm

```bash
nasm -f bin file_name.asm -o file_name.bin
```

To disassemble it, simply using `ndisasm` command:

```bash
ndisasm boot.binemu-system-x86_64 -hda file_name.bin -S -gdb stdio
```

## Debug my kernel image with gdb

NOTE: We should use kernel.elf (kernel image) to add symbol instead of using disk image.

First, entering to gdb mode:

```bash
gdb
```

Adding traceable symbol file:

```bash
add-symbol-file /bin/kernel.o 0x0100000
```

Set break point in symbol:

```bash
break kernel_entry_point
```

Start debugging with remote target:

```bash
target remote | qemu-system-x86_64 -hda ./bin/LavaOS.img -S -gdb stdio
```

Switching to assembly:

```bash
layout asm
```

And step by step.
prints out registers in both raw format (hex) and natural format in gdb:

```bash
info registers
```

## debug variable

```bash
gdb
```

In gdb:

```bash
add-symbol-file ./src/kernel/core/kernel.o 0x0100000
target remote | qemu-system-x86_64 -hda ./bin/LavaOS.img -S -gdb stdio
# Set break point in file-line
break kernel.cc:21
# continue
c
# print value of ptr variable
print ptr
```
