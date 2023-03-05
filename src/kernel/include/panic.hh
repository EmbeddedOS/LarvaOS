#pragma once
#include <string.hh>

namespace lava
{
    class kernel_panic
    {
        static void panic(const string& msg);
    };
}