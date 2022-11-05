#pragma once

namespace lava
{
    class arch
    {
    public:
        void init();              /* Start the processor interface. */
        void enable_interrupt();  /* Enable the interruption. */
        void disable_interrupt(); /* Disable the interruption. */
    };
}