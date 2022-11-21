#include "heap.h"
#include <stdbool.h>
#include <errno.h>
#include <string.h>

static bool heap_validate_alignment(void *ptr)
{
    return ((unsigned int)ptr % HEAP_BLOCK_SIZE_BYTES) == 0;
}

static int heap_validate_table(void *start, void *end, struct heap_table *table)
{
    int result = 0;
    size_t table_size = (size_t)(end - start);
    size_t total_block = table_size / HEAP_BLOCK_SIZE_BYTES;
    if (table->total != total_block)
    {
        result = -EINVAL;
        goto out;
    }

out:
    return result;
}

static uint32_t align_value_to_upper(uint32_t value)
{
    uint32_t remainder = value % HEAP_BLOCK_SIZE_BYTES;
    if (remainder == 0)
    {
        return value;
    }
    value -= remainder;
    value += HEAP_BLOCK_SIZE_BYTES;
    return value;
}

static unsigned char heap_get_entry_type(heap_block_table_entry entry)
{
    return entry & 0x0F;
}

static bool block_entry_is_free(heap_block_table_entry entry)
{
    return heap_get_entry_type(entry) == HEAP_BLOCK_TABLE_ENTRY_FREE;
}

static bool block_entry_is_taken(heap_block_table_entry entry)
{
    return heap_get_entry_type(entry) == HEAP_BLOCK_TABLE_ENTRY_TAKEN;
}

static bool block_has_next_block(heap_block_table_entry entry)
{
    return entry & HEAP_BLOCK_HAS_NEXT;
}

int heap_create(struct heap *heap, void *start, void *end, struct heap_table *table)
{
    int result = 0;
    if (!heap_validate_alignment(start) || !heap_validate_alignment(end))
    {
        result = -EINVAL;
        goto out;
    }

    memset(heap, 0, sizeof(struct heap));
    heap->start_address = start;
    heap->table = table;

    result = heap_validate_table(start, end, table);
    if (result < 0)
    {
        goto out;
    }

    size_t table_size = sizeof(heap_block_table_entry) * table->total;
    memset(table->entries, HEAP_BLOCK_TABLE_ENTRY_FREE, table_size);

out:
    return result;
}

void *get_physical_address_from_block_number(struct heap *heap, int num)
{
    void *address = NULL;
    struct heap_table *table = heap->table;

    if ((num < 0) || (num > (table->total - 1)))
    {
        goto out;
    }

    address = heap->start_address + (num * HEAP_BLOCK_SIZE_BYTES);

out:
    return address;
}

int get_block_number_from_physical_address(struct heap *heap, void *address)
{
    return ((int)(address - heap->start_address)) / HEAP_BLOCK_SIZE_BYTES; 
}

int mark_blocks_taken(struct heap *heap, int start_block_number, int size)
{
    int result = 0;
    struct heap_table *table = heap->table;

    if (start_block_number > (table->total - 1))
    { // Invalid argument, start of block number out of entries range.
        result = -EINVAL;
        goto out;
    }

    int end_block_number = (start_block_number + size) - 1;

    if (end_block_number > (table->total - 1))
    {
        end_block_number = table->total - 1;
    }

    table->entries[start_block_number] = HEAP_BLOCK_TABLE_ENTRY_TAKEN | HEAP_BLOCK_IS_FIRST;

    if (size > 1)
    {
        table->entries[start_block_number] |= HEAP_BLOCK_HAS_NEXT;
    }

    for (int i = start_block_number + 1; i <= end_block_number; i++)
    {
        table->entries[i] = HEAP_BLOCK_TABLE_ENTRY_TAKEN;
        if (i != end_block_number)
        {
            table->entries[i] |= HEAP_BLOCK_HAS_NEXT;
        }
    }

out:
    return result;
}

void mark_blocks_free(struct heap *heap, int num)
{
    struct heap_table *table = heap->table;
    int first_block_number = num;
    while (first_block_number)
    {
        bool is_first_block = table->entries[num] & HEAP_BLOCK_IS_FIRST;
        if (is_first_block)
        {
            break;
        }

        // If number is not first block,
        // we need to determine the first block of the blocks.
        first_block_number--;
    }

    for (int i = first_block_number; i < table->total; i++)
    {
        heap_block_table_entry entry = table->entries[i];
        table->entries[i] = HEAP_BLOCK_TABLE_ENTRY_FREE;

        if (!(entry & HEAP_BLOCK_HAS_NEXT))
        {
            break;
        }
    }
}

int get_free_blocks(struct heap *heap, uint32_t size)
{
    struct heap_table *table = heap->table;

    int free_block_count = 0;
    int block_number = -ENOMEM;
    for (size_t i = 0; i < table->total; i++)
    {
        if (block_entry_is_free(table->entries[i]))
        {
            if (block_number == -ENOMEM)
            { // Assign number of first block.
                block_number = i;
            }

            free_block_count++;
            if (free_block_count == size)
            {
                break;
            }
        }
        else
        { // If block is taken, reset the counter.
            free_block_count = 0;
            block_number = -ENOMEM;
        }
    }

    if (free_block_count != size)
    { // End of loop with free blocks, however, number
      // of blocks is not enough as requested by the caller.
        block_number = -ENOMEM;
    }

    return block_number;
}

void *malloc_blocks(struct heap *heap, uint32_t size)
{
    void *address = NULL;
    int start_block = get_free_blocks(heap, size);
    if (start_block < 0)
    {
        goto out;
    }

    address = get_physical_address_from_block_number(heap, start_block);

    // Mark the blocks as taken, in here,
    // we do not need validate result of this function,
    // it can not return a error number.
    mark_blocks_taken(heap, start_block, size);
out:
    return address;
}

void *malloc(struct heap *heap, size_t size)
{
    size_t aligned_size = align_value_to_upper(size);
    uint32_t total_blocks = aligned_size / HEAP_BLOCK_SIZE_BYTES;
    return malloc_blocks(heap, total_blocks);
}

void free(struct heap *heap, void *ptr)
{
    mark_blocks_free(heap, get_block_number_from_physical_address(heap, ptr));
}