#include "architecture.hh"
#include "iostream.hh"

extern "C"
{
#include <interrupt.h>
#include <memory/kheap.h>
#include <disk/disk.h>
#include <fs/path_parser.h>
#include <fs/file.h>
#include <string.h>
}

using namespace lava;

arch::arch() : m_kernel_chunk{nullptr},
               m_structured_gdt_runtime{
                   {.base = 0x00, .limit = 0x00, .type = 0x00},                     // NULL segment.
                   {.base = 0x00, .limit = 0xFFFFFFFF, .type = 0x9A},               // Kernel code segment.
                   {.base = 0x00, .limit = 0xFFFFFFFF, .type = 0x92},               // Kernel data segment.
                   {.base = 0x00, .limit = 0xFFFFFFFF, .type = 0xf8},               // User code segment.
                   {.base = 0x00, .limit = 0xFFFFFFFF, .type = 0xf2},               // User data segment.
                   {.base = (uint32_t)&m_tss, .limit = sizeof(m_tss), .type = 0xE9} // Task state segment.
               }
{
    memset(m_gdt_runtime, 0, sizeof(m_gdt_runtime));
    extract_structured_gdt(m_gdt_runtime, m_structured_gdt_runtime, TOTAL_GDT_SEGMENTS);
}

void arch::init()
{
    disable_interrupt();

    gdt_load_runtime(m_gdt_runtime, sizeof(m_gdt_runtime));

    /* 1. Initialize the HEAP. */
    kheap_init();

    /* 2. Initialize the filesystem. */
    fs_init();

    /* 3. Initialise the disk controller, bind the filesystem to the disk. */
    disk_init();

    /* 4. Initialize the interrupt descriptor table. */
    init_interrupt_descriptor_table();

    /* 5. Setup the TSS. */
    memset(&m_tss, 0x00, sizeof(m_tss));
    m_tss.esp0 = 0x600000;
    m_tss.ss0 = KERNEL_DATA_SELECTOR;
    tss_load(0x28);

    /* 5. make 4GB virtual memory address space for the kernel. */
    m_kernel_chunk = make_new_4GB_virtual_memory_address_space(PAGING_IS_WRITEABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);
    switch_to_paging_mode(m_kernel_chunk->directory_entry);
    enable_paging();

    int fd = fopen("/data.txt", "r");
    char buf[20];
    fseek(fd, 3, SEEK_SET);
    fread(fd, buf, 20, 1);
    lava::cout << "content of the file: " << buf << lava::endl;

    file_stat stat;

    fstat(fd, &stat);
    lava::cout << "stat file: " << stat.filename
               << ", extension: " << stat.ext
               << ", filesize: " << stat.filesize
               << lava::endl;
    fclose(fd);
    lava::cout << "Close the file\n";

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