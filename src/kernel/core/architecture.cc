#include "architecture.hh"

extern "C"
{
#include <interrupt.h>
#include <mm/kheap.h>
}

using namespace lava;

arch::arch() : kernel_chunk{nullptr}
{
}

void arch::init()
{
    disable_interrupt();

    /*1. Initialize the interrupt descriptor table.*/
    init_interrupt_descriptor_table();

    /*2. Initialize the HEAP.*/
    kheap_init();

    /* 3. Setup paging, switch to kernel paging chunk, and enable paging.*/
    kernel_chunk = paging_new_4gb(PAGING_IS_WRITEABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);
    paging_switching(kernel_chunk->directory_entry);
    enable_paging();


    enable_interrupt();
}

void arch::enable_interrupt()
{
    enable();
}

void arch::disable_interrupt()
{
    disable();
}