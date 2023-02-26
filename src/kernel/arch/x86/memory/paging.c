#include <memory/paging.h>
#include <memory/kheap.h>
#include <stdbool.h>
#include <errno.h>

extern void paging_load_directory(uint32_t *directory);

static uint32_t *current_directory = 0;

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

void switch_to_paging_mode(uint32_t *directory)
{
    paging_load_directory(directory);
    current_directory = directory;
}

int paging_get_indexes(void *virtual_address, uint32_t *directory_index_out, uint32_t *table_index_out)
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

int paging_set_virtual_address(uint32_t *directory, void *virtual_address, uint32_t val)
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

    uint32_t entry = directory[directory_index];
    uint32_t *table = (uint32_t *)(entry & 0xfffff000);
    table[table_index] = val;

out:
    return result;
}