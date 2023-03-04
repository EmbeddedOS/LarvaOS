#pragma once
extern "C"
{
#include <memory/paging.h>
#include <gdt.h>
#include <task/task_state.h>
}

namespace lava
{
    constexpr int TOTAL_GDT_SEGMENTS = 6;
    class arch
    {
        paging_4GB_chunk *m_kernel_chunk{nullptr};
        segment_descriptor m_gdt_runtime[TOTAL_GDT_SEGMENTS];
        structured_segment_descriptor m_structured_gdt_runtime[TOTAL_GDT_SEGMENTS];
        task_state_segment m_tss;

    public:
        explicit arch();
        void init();              /* Start the processor interface. */
        void enable_interrupt();  /* Enable the interruption. */
        void disable_interrupt(); /* Disable the interruption. */
    };
}