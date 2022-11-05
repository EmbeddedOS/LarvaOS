#include "kernel.hh"
#include "iostream.hh"
#include "architecture.hh"

extern "C"
{
    extern void problem();

    void kernel_main()
    {
        lava::cout << lava::ostream::color::green << "Hello world " << lava::endl;
        lava::arch ar;
        ar.init();
        lava::cout << lava::ostream::color::green << "Hello world " << lava::endl;
        lava::cout << lava::ostream::color::green << "Hello world " << lava::endl;

        problem();

        while (1)
        {
        }
    }
}
