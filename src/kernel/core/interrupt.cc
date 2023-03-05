#include <interrupt.hh>

extern "C"
{
#include <interrupt.h>
}

namespace lava
{
    void interrupt::initialize()
    { // Initialize the interrupt descriptor table.
        init_interrupt_descriptor_table();
    }

    interrupt &interrupt::get_instance()
    {
        static interrupt instance;
        return instance;
    }

    void interrupt::disable() const
    {
        disable_interrupt();
    }

    void interrupt::enable() const
    {
        enable_interrupt();
    }
}