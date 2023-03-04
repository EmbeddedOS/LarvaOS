#pragma once
#include <types.h>

#define PAGING_CACHE_DISABLED 0b00010000
#define PAGING_WRITE_THROUGH 0b00001000
#define PAGING_ACCESS_FROM_ALL 0b00000100
#define PAGING_IS_WRITEABLE 0b00000010
#define PAGING_IS_PRESENT 0b00000001

#define PAGING_TOTAL_ENTRIES_PER_TABLE 1024
#define PAGING_PAGE_SIZE 4096

struct paging_4GB_chunk
{
    uint32_t *directory_entry;
};

extern void enable_paging();

struct paging_4GB_chunk *make_new_4GB_virtual_memory_address_space(uint8_t flags);

void switch_to_page(struct paging_4GB_chunk *directory);

void release_4GB_virtual_memory_address_space(struct paging_4GB_chunk *page);

int paging_map_virtual_memory(struct paging_4GB_chunk *page,
                              void *virt,
                              void *phys,
                              void *phys_end,
                              int flags);

void *paging_align_address(void *ptr);