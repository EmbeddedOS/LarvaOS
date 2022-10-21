ARCH=x86
KERNEL=kernel.o
IMAGE=../../bin/LavaOS.img
KERNEL_LIB_DIR=../../bin/kernel_lib/
TMP_OBJ_FILE=tmp.o
include arch/$(ARCH)/config.make
include arch/$(ARCH)/Makefile

# Adding architecture-independent code.
OBJS+= ./core/kernel.o
INCLUDES+= -I./ -I./core/ -I./runtime
LIBC:=$(wildcard $(KERNEL_LIB_DIR)*.a)


all: $(FIRST_SECTOR) $(KERNEL)
	rm -rf $(IMAGE)
# BOOT SECTOR will be loaded first (0-512 bytes).
	dd if=$(FIRST_SECTOR) >> $(IMAGE)
# The next sector will be kernel code.
	dd if=$(KERNEL) >> $(IMAGE)
# What if own kenrel does not fill up to a sector
# ( < 512 bytes or not divisible by 512).
# So, we need to fill null data to kernel image, and
# ATA driver in the boot-loader will can read all the kernel code.
# We are going to load 100 sectors into memory 
# (ATA drive read at least 100 sectors a time).
	dd if=/dev/zero bs=512 count=100 >> $(IMAGE)
	@echo "Make new kernel image in $(IMAGE)."
	make clean

$(KERNEL): $(OBJS)
	$(LD) $(LD_FLAG) -o $(TMP_OBJ_FILE) $^
	$(SCC) $(TMP_OBJ_FILE) $(LIBC) $(FLAGS) -T $(LINKER_SCRIPT) -o $@

# Build the first sector in binary format.
# A Binary file is a pure binary file with no memory fix-ups or relocations,
# it has explicit instructions to be loaded at a specific memory address. 
# So, we need the boot sector in 'raw' binary format.	
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
	rm -f $(FIRST_SECTOR) $(OBJS) $(KERNEL) $(TMP_OBJ_FILE)

