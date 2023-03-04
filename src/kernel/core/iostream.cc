#include "iostream.hh"
extern "C"
{
#include <string.h>
#include <stdlib.h>
#include <video.h>
}

namespace lava
{
    ostream cout{};
}

using namespace lava;

ostream &ostream::operator<<(const char *str)
{
    print(str);
    return *this;
}

ostream &ostream::operator<<(const int &num)
{
    char str[32];
    itoa(num, str, 10);
    print(str);
    return *this;
}

ostream &ostream::operator<<(const lava::ostream::color &color)
{
    set_color(vga_color(color));
    return *this;
}

ostream &ostream::operator<<(ostream &(*func)(ostream &out))
{
    return func(*this);
}

ostream::ostream()
{
    clear();
}