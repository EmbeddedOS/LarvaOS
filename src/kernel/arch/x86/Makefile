ARCH=x86
ASM=nasm
ASM_BIN_FLAGS=-f bin
ASM_ELF_FLAGS=-f elf -g
LD=i686-elf-ld
SC=i686-elf-gcc
SCC=i686-elf-g++
FLAGS=-g -ffreestanding -falign-jumps -falign-functions -falign-labels -falign-loops -fstrength-reduce -fomit-frame-pointer -finline-functions -Wno-unused-function -fno-builtin -Werror -Wno-unused-label -Wno-cpp -Wno-unused-parameter -nostdlib -nostartfiles -nodefaultlibs -Wall -O0 -Iinc

FIRST_SECTOR:=arch/$(ARCH)/boot/boot.bin 
OBJS:=arch/$(ARCH)/start.o \
	arch/$(ARCH)/video/video.o \
	arch/$(ARCH)/panic/panic.o \
	arch/$(ARCH)/irq/interrupt.o \
	arch/$(ARCH)/irq/idt.o \
	arch/$(ARCH)/gdt_runtime/gdt.o \
	arch/$(ARCH)/gdt_runtime/gdt_load_runtime.o \
	arch/$(ARCH)/io/io.o \
	arch/$(ARCH)/disk/disk.o \
	arch/$(ARCH)/disk/stream.o \
	arch/$(ARCH)/memory/heap.o \
	arch/$(ARCH)/memory/kheap.o \
	arch/$(ARCH)/memory/paging.o \
	arch/$(ARCH)/memory/enable_paging.o \
	arch/$(ARCH)/fs/path_parser.o \
	arch/$(ARCH)/fs/file.o \
	arch/$(ARCH)/fs/fat16.o \
	arch/$(ARCH)/task/task.o \
	arch/$(ARCH)/task/tss_load.o \
	arch/$(ARCH)/task/process.o \
	arch/$(ARCH)/task/switch.o


LINKER_SCRIPT:=./arch/$(ARCH)/linker.ld
INCLUDES:=-I./arch/$(ARCH)/include/ -I./arch/$(ARCH)/ -I../utils_libc/include/

$(FIRST_SECTOR): arch/$(ARCH)/boot/boot.asm
	$(ASM) $(ASM_BIN_FLAG) $(INCLUDES) $< -o  $@
