#include <kernel.hh>
#include <iostream.hh>
#include <architecture.hh>

extern "C"
{
    void kernel_main()
    {
        lava::cout << lava::ostream::color::green << "Welcome to Larva OS." << lava::endl;
        lava::arch::get_instance().initialize();
        while (1)
        {
        }
    }
}
