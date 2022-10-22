#include "kernel.hh"
#include "iostream.hh"

extern "C"
{
    void kernel_main()
    {
        lava::cout << "Hello world";
        while(1) {}
    }
}
