#pragma once
#include <types.h>

#define HEAP_BLOCK_TABLE_ENTRY_FREE     0x00
#define HEAP_BLOCK_TABLE_ENTRY_TAKEN    0x01
#define HEAP_BLOCK_HAS_NEXT       0b10000000
#define HEAP_BLOCK_IS_FIRST       0b01000000
#define HEAP_BLOCK_SIZE_BYTES     0x00001000

// For the heap table, we store it at address
// 0x00007E00 that is a Conventional Memory Region
// size of the region upto 480.5 KiB, so, we can
// use it to manage all heap block entries. For
// more information, check the standard x86 memory 
// map.
#define HEAP_TABLE_ADDRESS        0x00007E00

typedef unsigned char heap_block_table_entry;

struct heap_table
{
    heap_block_table_entry *entries;
    size_t total;
};

struct heap 
{
    struct heap_table *table;
    void *start_address;
};

int heap_create(struct heap *heap, void *start, void *end, struct heap_table *table);

void *malloc(struct heap *heap, size_t size);

void free(struct heap *heap, void *ptr);