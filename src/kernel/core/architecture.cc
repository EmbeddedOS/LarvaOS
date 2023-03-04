#include <architecture.hh>
#include <iostream.hh>
#include <memory.hh>
#include <string.hh>

extern "C"
{
#include <interrupt.h>
#include <memory/kheap.h>
#include <disk/disk.h>
#include <fs/path_parser.h>
#include <fs/file.h>
#include <string.h>
#include <task/process.h>
}

namespace lava
{
    arch::global_descriptor_table::global_descriptor_table(const arch &ar) : _structured_gdt_runtime{
                                                                                 {.base = 0x00, .limit = 0x00, .type = 0x00},                         // NULL segment.
                                                                                 {.base = 0x00, .limit = 0xFFFFFFFF, .type = 0x9A},                   // Kernel code segment.
                                                                                 {.base = 0x00, .limit = 0xFFFFFFFF, .type = 0x92},                   // Kernel data segment.
                                                                                 {.base = 0x00, .limit = 0xFFFFFFFF, .type = 0xf8},                   // User code segment.
                                                                                 {.base = 0x00, .limit = 0xFFFFFFFF, .type = 0xf2},                   // User data segment.
                                                                                 {.base = (uint32_t)&ar._tss, .limit = sizeof(ar._tss), .type = 0xE9} // Task state segment.
                                                                             }
    {
    }

    void arch::global_descriptor_table::initialize()
    {
        memset(_gdt_runtime, 0, sizeof(_gdt_runtime));
        extract_structured_gdt(_gdt_runtime, _structured_gdt_runtime, TOTAL_GDT_SEGMENTS);
        gdt_load_runtime(_gdt_runtime, sizeof(_gdt_runtime));
    }

    arch::arch() : _gdt{*this} {};

    void arch::initialize()
    {
        disable_interrupt();

        _gdt.initialize();

        /* 1. Initialize the HEAP. */
        kheap_init();

        /* 2. Initialize the filesystem. */
        fs_init();

        /* 3. Initialise the disk controller, bind the filesystem to the disk. */
        disk_init();

        /* 4. Initialize the interrupt descriptor table. */
        init_interrupt_descriptor_table();

        /* 5. Setup the TSS. */
        memset(&_tss, 0x00, sizeof(_tss));
        _tss.esp0 = 0x600000;
        _tss.ss0 = KERNEL_DATA_SELECTOR;
        tss_load(0x28);

        /* 5. make 4GB virtual memory address space for the kernel and switch to it. */
        vm::get_instance().initialize();

        // Test virtual file system.
        // int fd = fopen("/data.txt", "r");
        // char buf[20];
        // fseek(fd, 3, SEEK_SET);
        // fread(fd, buf, 20, 1);
        // lava::cout << "content of the file: " << buf << lava::endl;

        // file_stat stat;

        // fstat(fd, &stat);
        // lava::cout << "stat file: " << stat.filename
        //            << ", extension: " << stat.ext
        //            << ", filesize: " << stat.filesize
        //            << lava::endl;
        // fclose(fd);
        // lava::cout << "Close the file\n";

        // Test process.
        struct process *proc = NULL;
        int res = load_process("/loop.bin", &proc);
        if (res < 0)
        {
            lava::cout << "Failed to load loop.bin\n";
        }
    }

    void arch::enable_int() const
    {
        enable_interrupt();
    }

    void arch::disable_int() const
    {
        disable_interrupt();
    }

    arch &arch::get_instance()
    {
        static arch instance;
        return instance;
    }

}