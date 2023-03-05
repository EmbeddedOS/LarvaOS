#pragma once
#include <string.hh>

namespace lava
{
    class kernel_panic
    {
    public:
        static void panic(const string &msg);
    };
}