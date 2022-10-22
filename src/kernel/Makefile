ARCH=x86
include arch/$(ARCH)/Makefile

KERNEL=kernel.elf
DISK=../../bin/LavaOS.img
KERNEL_LIB_DIR=../../bin/kernel_lib/


# Adding architecture-independent code.
OBJS+= ./core/kernel.o \
	   ./core/iostream.o \

INCLUDES+= -I.arch/$(ARCH)/ -I./ -I./core/ -I./runtime
LIBC:=$(wildcard $(KERNEL_LIB_DIR)*.a)

all: $(FIRST_SECTOR) $(KERNEL)
	rm -rf $(DISK)
	dd if=$(FIRST_SECTOR) >> $(DISK)
	dd if=$(KERNEL) >> $(DISK)
	dd if=/dev/zero bs=512 count=100 >> $(DISK)
	@echo "Make new disk in $(DISK)."

$(KERNEL): $(OBJS)
	$(SCC) $^ $(LIBC) $(FLAGS) -T $(LINKER_SCRIPT) -o $@ 

%.o: %.S
	$(ASM) $(ASM_BIN_FLAGS) $(INCLUDES) $< -o  $@
	
%.o: %.asm
	$(ASM) $(ASM_ELF_FLAGS) $(INCLUDES) $< -o  $@

%.o: %.c
	$(SC) $(FLAGS) $(INCLUDES) -c $< -o  $@

%.o: %.cc
	$(SCC) $(FLAGS) $(INCLUDES) -c $< -o  $@

%.o: %.cpp
	$(SCC) $(FLAGS) $(INCLUDES) -c $< -o  $@

clean:
	rm -f $(FIRST_SECTOR) $(OBJS) $(KERNEL)

