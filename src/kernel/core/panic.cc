#include "panic.hh"
#include <iostream.hh>

using namespace lava;

/**
 *	panic - halt the system
 *	@msg: The text string to print
 *
 *	Display a message, then perform cleanups.
 *
 *	This function never returns.
 */
void panic(const char *msg)
{
    lava::cout << msg << lava::endl;
    while (true)
    {
    }
}