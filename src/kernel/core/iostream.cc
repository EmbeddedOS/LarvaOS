#include "iostream.hh"
extern "C"
{
#include <string.h>
#include <video.h>
}

namespace lava
{
    ostream cout{};
}

using namespace lava;

ostream &ostream::operator<<(const char *str)
{
    write(str, strlen(str));
    return *this;
}

ostream::ostream()
{
    clear();
}