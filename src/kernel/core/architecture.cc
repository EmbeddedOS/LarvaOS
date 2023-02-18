#include "architecture.hh"
#include "iostream.hh"

extern "C"
{
#include <interrupt.h>
#include <mm/kheap.h>
#include <disk/disk.h>
#include <fs/path_parser.h>
#include <fs/file.h>
}

using namespace lava;

arch::arch() : kernel_chunk{nullptr}
{
}

void arch::init()
{
    disable_interrupt();

    /* 1. Initialize the filesystem. */
    fs_init();

    /* 2. Initialise the disk controller, bind the filesystem to the disk. */
    disk_init();

    /* 3. Initialize the interrupt descriptor table. */
    init_interrupt_descriptor_table();

    /* 4. Initialize the HEAP. */
    kheap_init();

    /* 5. make 4GB virtual memory address space for the kernel. */
    kernel_chunk = make_new_4GB_virtual_memory_address_space(PAGING_IS_WRITEABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);
    switch_to_paging_mode(kernel_chunk->directory_entry);
    enable_paging();

    enable_interrupt();

    struct path_part *root = path_parse("/home/larva/os");
    struct path_part *part = root;
    while (part)
    {
        lava::cout << part->part << lava::endl;
        part = part->next;
    }
}

void arch::enable_interrupt()
{
    enable();
}

void arch::disable_interrupt()
{
    disable();
}