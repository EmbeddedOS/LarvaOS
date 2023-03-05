#include <memory/paging.h>
#include <memory/kheap.h>
#include <stdbool.h>
#include <errno.h>

extern void paging_load_directory(uint32_t *directory);

static struct paging_4GB_chunk *kernel_directory = NULL;
static uint32_t *current_directory = NULL;

void switch_to_kernel_page()
{
    switch_to_page(kernel_directory);
}

void paging_install_kernel_page(struct paging_4GB_chunk *kernel_page)
{
    if (kernel_directory == NULL)
    {// We only load kernel page one time.
        kernel_directory = kernel_page;
    }
}


static bool paging_check_address_is_aligned(void *addr)
{
    return ((uint32_t)addr % PAGING_PAGE_SIZE) == 0;
}

struct paging_4GB_chunk *make_new_4GB_virtual_memory_address_space(uint8_t flags)
{
    /* 1. Creating a Blank Page Directory. The page directory should have exactly 1024 entries.*/
    uint32_t *directory = kzalloc(sizeof(uint32_t) * PAGING_TOTAL_ENTRIES_PER_TABLE);

    /* 2. Creating Page Tables, each entry in the Page Directory is a pointer to a Page Table.*/
    int offset = 0;
    for (int i = 0; i < PAGING_TOTAL_ENTRIES_PER_TABLE; i++)
    { // this For loop make 1024 page tables,
      // initialise them with the flags,
      // and causes the Page Directory Entries point to them.

        uint32_t *entry = kzalloc(sizeof(uint32_t) * PAGING_TOTAL_ENTRIES_PER_TABLE);
        for (int j = 0; j < PAGING_TOTAL_ENTRIES_PER_TABLE; j++)
        { // We will fill all 1024 entries in the table, mapping 4 megabytes.

            // Bits 31-12 of address, 11-0 of flags.
            // As the address is page aligned, it will always leave 12 bits zeroed.
            entry[j] = (offset + (j * PAGING_PAGE_SIZE)) | flags;
        }

        offset += (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE);

        // Put the Page Table in the Page Directory.
        directory[i] = (uint32_t)entry | flags | PAGING_IS_WRITEABLE;
    }

    /* 3. Return 4GB chunk object for the caller. */
    struct paging_4GB_chunk *chunk_4GB = kzalloc(sizeof(struct paging_4GB_chunk));
    chunk_4GB->directory_entry = directory;
    return chunk_4GB;
}

void switch_to_page(struct paging_4GB_chunk *directory)
{
    paging_load_directory(directory->directory_entry);
    current_directory = directory->directory_entry;
}

static int paging_get_indexes(void *virtual_address, uint32_t *directory_index_out, uint32_t *table_index_out)
{
    int result = 0;
    if (!paging_check_address_is_aligned(virtual_address))
    {
        result = -EINVAL;
        goto out;
    }

    *directory_index_out = ((uint32_t)virtual_address / (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE));
    *table_index_out = (((uint32_t)virtual_address % (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE)) / PAGING_PAGE_SIZE);

out:
    return result;
}

static int paging_set_virtual_address(struct paging_4GB_chunk *page, void *virtual_address, uint32_t val)
{
    int result = 0;
    if (!paging_check_address_is_aligned(virtual_address))
    {
        result = -EINVAL;
        goto out;
    }

    uint32_t directory_index = 0;
    uint32_t table_index = 0;
    int res = paging_get_indexes(virtual_address, &directory_index, &table_index);
    if (res < 0)
    {
        return res;
    }

    uint32_t entry = page->directory_entry[directory_index];
    uint32_t *table = (uint32_t *)(entry & 0xfffff000);
    table[table_index] = val;

out:
    return result;
}

void release_4GB_virtual_memory_address_space(struct paging_4GB_chunk *page)
{
    for (int i = 0; i < 1024; i++)
    {
        uint32_t entry = page->directory_entry[i];
        uint32_t *table = (uint32_t *)(entry & 0xFFFFF000);
        kfree(table);
    }
    kfree(page->directory_entry);
    kfree(page);
}
static int paging_map_page(struct paging_4GB_chunk *page,
                           void *virt,
                           void *phys,
                           int flags)
{
    if ((uint32_t)virt % PAGING_PAGE_SIZE || (uint32_t)phys % PAGING_PAGE_SIZE)
    {
        return -EINVAL;
    }

    return paging_set_virtual_address(page, virt, (uint32_t)phys | flags);
}

static int paging_map_memory_by_range(struct paging_4GB_chunk *page,
                                      void *virt,
                                      void *phys,
                                      uint32_t total_pages,
                                      int flags)
{
    int res = 0;
    for (int i = 0; i < total_pages; i++)
    {
        res = paging_map_page(page, virt, phys, flags);
        if (res == 0)
        {
            break;
        }

        virt += PAGING_PAGE_SIZE;
        phys += PAGING_PAGE_SIZE;
    }
out:
    return res;
}

int paging_map_virtual_memory(struct paging_4GB_chunk *page,
                              void *virt,
                              void *phys,
                              void *phys_end,
                              int flags)
{
    int res = 0;

    // Check memory is aligned or not.
    if ((uint32_t)virt % PAGING_PAGE_SIZE)
    {
        res = -EINVAL;
        goto out;
    }

    if ((uint32_t)phys % PAGING_PAGE_SIZE)
    {
        res = -EINVAL;
        goto out;
    }

    if ((uint32_t)phys_end % PAGING_PAGE_SIZE)
    {
        res = -EINVAL;
        goto out;
    }

    // Check valid of physical memory.
    if ((uint32_t)phys_end < (uint32_t)phys)
    {
        res = -EINVAL;
        goto out;
    }

    uint32_t total_bytes = (uint32_t)phys_end - (uint32_t)phys;
    int total_pages = total_bytes / PAGING_PAGE_SIZE;

    res = paging_map_memory_by_range(page, virt, phys, total_pages, flags);
out:
    return res;
}

void *paging_align_address(void *ptr)
{
    int unaligned_addresses_size = (uint32_t)ptr % PAGING_PAGE_SIZE;
    if (unaligned_addresses_size > 0)
    {
        return (void *)((uint32_t)ptr + PAGING_PAGE_SIZE - unaligned_addresses_size);
    }

    return ptr;
}

