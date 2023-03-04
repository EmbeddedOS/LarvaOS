ARCH=x86
include arch/$(ARCH)/Makefile

KERNEL=kernel_image.o
DISK_IMAGE=../../bin/LavaOS.img
KLIB=../../bin/kernel_lib/


# Adding architecture-independent code.
OBJS+= ./core/kernel.o \
	   ./core/iostream.o \
	   ./core/architecture.o \
	   ./core/panic.o \
   	   ./core/file.o \
	   ./core/memory.o \
	   ./core/string.o \
	   ./core/exception.o

FLAGS+= -fno-exceptions

INCLUDES+= -I.arch/$(ARCH)/include -I./include -I./libc/include
LIBS:=$(wildcard $(KLIB)*.a)

all: os-image
	@echo "Make new disk image in $(DISK_IMAGE)."


# This is the actual disk image that the computer loads
# which is the combination of our compiled bootsector and kernel.
os-image: $(FIRST_SECTOR) $(KERNEL)
	cat $^ > $(DISK_IMAGE)
	dd if=/dev/zero bs=1048576 count=16 >> $(DISK_IMAGE)

$(KERNEL): $(OBJS)
	$(SCC) $^ $(LIBS) $(FLAGS) -T $(LINKER_SCRIPT) -o $@ 

%.o: %.S
	$(ASM) $(ASM_BIN_FLAGS) $(INCLUDES) $< -o $@
	
%.o: %.asm
	$(ASM) $(ASM_ELF_FLAGS) $(INCLUDES) $< -o $@

%.o: %.c
	$(SC) $(FLAGS) $(INCLUDES) -c $< -o $@

%.o: %.cc
	$(SCC) $(FLAGS) $(INCLUDES) -c $< -o $@

%.o: %.cpp
	$(SCC) $(FLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(FIRST_SECTOR) $(OBJS) $(KERNEL) $(DISK_IMAGE)

