#include <panic.hh>

extern "C"
{
#include <panic.h>
}

namespace lava
{

    /**
     *	panic - halt the system
     *	@msg: The text string to print
     *
     *	Display a message, then perform cleanups.
     *
     *	This function never returns.
     */
    void kernel_panic::panic(const string &msg)
    {
        // Cleanup something.

        // Call architecture panic.
        arc_panic(msg.data());
    }
}