#pragma once

// We use 100MB for kernel heap and start 
// at address 0x01000000 (RAM -- free for use).
#define KERNEL_HEAP_SIZE_BYTES          0x06400000
#define KERNEL_HEAP_START_ADDRESS       0x01000000

void kheap_init();