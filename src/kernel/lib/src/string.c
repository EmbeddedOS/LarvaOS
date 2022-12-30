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

int memcmp(const void *p1, const void *p2, size_t c)
{
    const unsigned char *su1, *su2;
    signed char res = 0;

    for (su1 = p1, su2 = p2; 0 < c; ++su1, ++su2, c--)
    {
        if ((res = *su1 - *su2) != 0)
        {
            break;
        }
    }

    return res;
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

size_t strnlen(const char *s, size_t count)
{
    const char *sc;

    for (sc = s; count-- && (*sc != 0); ++sc)
    {
    }

    return (sc - s);
}
