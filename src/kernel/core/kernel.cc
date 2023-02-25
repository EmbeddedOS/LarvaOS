#include "kernel.hh"
#include <iostream.hh>
#include "architecture.hh"

extern "C"
{
    void kernel_main()
    {
        lava::cout << lava::ostream::color::green << "Welcome to Larva OS." << lava::endl;
        lava::arch ar;
        ar.init();
        int x = 0;
        int y = 0;
        while (1)
        {
            x++;
            if ((x % 100000000) == 0)
            {
                lava::cout << ".";
                y++;
            }
            if ((y % 10 == 0) && y != 0)
            {
                int a = 0;
                int z = 2;
                z /= a;
            }
        }
    }
}
