#include <architecture.hh>
#include <iostream.hh>
#include <memory.hh>
#include <string.hh>
#include <file.hh>
#include <utility.hh>
#include <interrupt.hh>

extern "C"
{
#include <string.h>
#include <task/process.h>
#include <interrupt.h>
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
        interrupt::get_instance().disable();

        _gdt.initialize();

        // Initialize the HEAP.
        kernel_heap::initialize_kernel_heap();

        // Initialize the file system.
        vfs::get_instance().initialize();

        // Initialize the interrupt descriptor table.
        interrupt::get_instance().initialize();

        // Setup the TSS.
        memset(&_tss, 0x00, sizeof(_tss));
        _tss.esp0 = 0x600000;
        _tss.ss0 = KERNEL_DATA_SELECTOR;
        tss_load(0x28);

        vm::get_instance().initialize();

        // Test file system.
        file f {"/data.txt", open_file_mode::READ};
        f.open();
        f.seekg(3, seek_file_mode::SET);
        lava::cout << "Content of file: " << f.read(20) << " file size " << f.size() << lava::endl;
        f.close();

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