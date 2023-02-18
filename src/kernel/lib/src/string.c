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

void *memcpy(void *d, const void *s, size_t n)
{
    char *dest;
    char *src;

    dest = (char *)d;
    src = (char *)s;

    while (n--)
    {
        *dest++ = *src++;
    }

    return d;
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

char *strchr(const char *s, int c)
{
    for (; *s != (char)c; s++)
    {
        if (*s == 0)
        {
            return NULL;
        }
    }

    return (char *)s;
}

char *strcpy(char *d, const char *s)
{
    char *tmp = d;

    while ((*d++ = *s++) != 0) ;

    return tmp;
}
