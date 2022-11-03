#include "kernel.hh"
#include "iostream.hh"
extern "C"
{
    #include <types.h>
}

extern "C"
{
    void kernel_main()
    {
        lava::cout << lava::ostream::color::green << "Hello world " << sizeof(uint64_t) << lava::endl;
        while(1) {}
    }
}
