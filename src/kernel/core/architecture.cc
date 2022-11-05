#include "architecture.hh"

extern "C"
{
#include <interrupt.h>
}
using namespace lava;

void arch::init()
{
    init_interrupt_descriptor_table();
}

void arch::enable_interrupt()
{
    enable();
}

void arch::disable_interrupt()
{
    disable();
}