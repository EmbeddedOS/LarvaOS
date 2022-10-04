# LarvaOS
A multitasking operating system and kernel with an interactive shell.

Assembles a binary file from assembly file.
nasm -f bin boot.asm -o boot.bin

Disassembles a binary file:
ndisasm boot.binemu-system-x86_64 -hda boot.bin -S -gdb stdio

run OS:
    qemu-system-x86_64 -hda boot.bin

run gdb with qemu remote target:
    gdb
    target remote | qemu-system-x86_64 -hda boot.bin -S -gdb stdio
    
switching to assembly in gdb:
    layout asm

prints out registers in both raw format (hex) and natural format in gdb:
    info registers


