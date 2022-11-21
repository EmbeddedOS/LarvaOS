#include "architecture.hh"

extern "C"
{
#include <interrupt.h>
#include <mm/kheap.h>
}
using namespace lava;

void arch::init()
{
    init_interrupt_descriptor_table();
    kheap_init();
}

void arch::enable_interrupt()
{
    enable();
}

void arch::disable_interrupt()
{
    disable();
}