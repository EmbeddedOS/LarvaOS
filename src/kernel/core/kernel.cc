#include "kernel.hh"
extern "C"
{
#include "../arch/x86/video.h"
}

extern "C"
{
    void kernel_main()
    {
        video_clear();
    }
}
