#include <exception.hh>

// If we use exception on the kernel, enable this:
// void *__gxx_personality_v0 = (void *)0x00;
// And ignore flag on the build system: -fno-exceptions
// For detailed: https://stackoverflow.com/questions/329059/what-is-gxx-personality-v0-for 