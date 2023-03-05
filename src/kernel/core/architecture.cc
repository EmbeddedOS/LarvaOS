#include <architecture.hh>
#include <iostream.hh>
#include <memory.hh>
#include <string.hh>
#include <file.hh>
#include <utility.hh>
#include <interrupt.hh>
#include <process.hh>

extern "C"
{
#include <string.h>
}

namespace lava
{
    arch::global_descriptor_table::global_descriptor_table(const arch &ar) : _structured_gdt_runtime{
                                                                                 {.base = 0x00, .limit = 0x00, .type = 0x00},       // NULL segment.
                                                                                 {.base = 0x00, .limit = 0xFFFFFFFF, .type = 0x9A}, // Kernel code segment.
                                                                                 {.base = 0x00, .limit = 0xFFFFFFFF, .type = 0x92}, // Kernel data segment.
                                                                                 {.base = 0x00, .limit = 0xFFFFFFFF, .type = 0xf8}, // User code segment.
                                                                                 {.base = 0x00, .limit = 0xFFFFFFFF, .type = 0xf2}, // User data segment.
                                                                                 {.base = (uint32_t)proc::get_instance().get_task_state_segment(),
                                                                                  .limit = sizeof(*proc::get_instance().get_task_state_segment()),
                                                                                  .type = 0xE9} // Task state segment.
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
        // Disable interrupt.
        interrupt::get_instance().disable();

        // Initialize global descriptor table.
        _gdt.initialize();

        // Initialize the HEAP.
        kernel_heap::initialize_kernel_heap();

        // Initialize the file system.
        vfs::get_instance().initialize();

        // Initialize the interrupt descriptor table.
        interrupt::get_instance().initialize();

        // Initialize the task state segment.
        proc::get_instance().initialize();

        // Initialize virtual memory and switch to kernel virtual memory.
        vm::get_instance().initialize();

        // Test process.
        proc::get_instance().load_proc("/loop.bin");
    }

    arch &arch::get_instance()
    {
        static arch instance;
        return instance;
    }

}