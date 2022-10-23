#include "kernel.hh"
#include "iostream.hh"

extern "C"
{
    void kernel_main()
    {
        lava::cout << lava::ostream::color::green << "Hello world " << 22 << lava::endl;
        while(1) {}
    }
}
