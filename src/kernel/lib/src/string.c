#include <string.h>

void *memset(void *ptr, int c, size_t size)
{
    char *c_ptr = (char *)ptr;
    for (int i = 0; i < size; i++)
    {
        c_ptr[i] = (char)c;
    }
    return ptr;
}

size_t strlen(const char *s)
{
    size_t i = 0;
    while (*s != 0)
    {
        i++;
        s += 1;
    }

    return i;
}
