#include "heap.h"
#include "kheap.h"

struct heap g_kernel_heap;
struct heap_table g_kernel_heap_table;

void kheap_init()
{
    int total_table_entries = KERNEL_HEAP_SIZE_BYTES / HEAP_BLOCK_SIZE_BYTES;

    g_kernel_heap_table.entries = (heap_block_table_entry *)(HEAP_TABLE_ADDRESS);
    g_kernel_heap_table.total = total_table_entries;

    int res = heap_create(&g_kernel_heap,
                          (void *)(KERNEL_HEAP_START_ADDRESS),
                          (void *)(KERNEL_HEAP_START_ADDRESS + KERNEL_HEAP_SIZE_BYTES),
                          &g_kernel_heap_table);

    if (res < 0)
    {
        
    }
}