#pragma once
extern "C"
{
#include <mm/paging.h>
}

namespace lava
{
    class arch
    {
        paging_4gb_chunk *kernel_chunk{nullptr};

    public:
        explicit arch();
        void init();              /* Start the processor interface. */
        void enable_interrupt();  /* Enable the interruption. */
        void disable_interrupt(); /* Disable the interruption. */
    };
}