ARCH=x86
KERNEL=kernel.elf
IMAGE=../../bin/LavaOS.img

include arch/$(ARCH)/config.make
include arch/$(ARCH)/Makefile

# Adding architecture-independent code.
OBJS+= ./core/kernel.o

all: $(FIRST_SECTOR) $(KERNEL)
	rm -rf $(IMAGE)
# BOOT SECTOR will be loaded first (0-512 bytes).
	dd if=$(FIRST_SECTOR) >> $(IMAGE)
# The next sector will be kernel code.
	dd if=$(KERNEL) >> $(IMAGE)
# WHY?
	dd if=/dev/zero bs=1048576 count=16 >> $(IMAGE)
	@echo "Make new kernel image in $(IMAGE)."
	make clean

$(KERNEL): $(OBJS)
	$(LD) $(LD_FLAG) -o link.o $^
	$(SCC) link.o $(FLAGS) -T $(LINKER_SCRIPT) -o $@
	rm link.o

# Build the first sector in binary format.
# A Binary file is a pure binary file with no memory fix-ups or relocations,
# it has explicit instructions to be loaded at a specific memory address. 
# So, we need the boot sector in 'raw' format.	
# FIXME: The first sector is architecture-dependent code (x86), 
# it should be responsible of x86 code.
%.bin: %.S
	$(ASM) $(ASM_BIN_FLAG) $(INCLUDES) $< -o  $@
	
%.o: %.asm
	$(ASM) $(ASM_ELF_FLAG) $(INCLUDES) $< -o  $@

%.o: %.c
	$(SC) $(FLAGS) $(INCLUDES) -c $< -o  $@

%.o: %.cc
	$(SCC) $(FLAGS) $(INCLUDES) -c $< -o  $@

clean:
	rm -f $(FIRST_SECTOR) $(OBJS) $(KERNEL)