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
	arch/$(ARCH)/video.o \
	arch/$(ARCH)/interrupt.o \
	arch/$(ARCH)/idt.o

LINKER_SCRIPT:=./arch/$(ARCH)/linker.ld
INCLUDES:=-I./arch/$(ARCH)/ -I./arch/$(ARCH)/boot/ -I./arch/$(ARCH)/utils -I./lib/include/

$(FIRST_SECTOR): arch/$(ARCH)/boot/boot.asm
	$(ASM) $(ASM_BIN_FLAG) $(INCLUDES) $< -o  $@
