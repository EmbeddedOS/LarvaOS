#include <ctype.h>

int isupper(int c)
{
    return ((c >= 'A') && (c <= 'Z'));
}

int islower(int c)
{
    return ((c >= 'a') && (c <= 'z'));
}

int isdigit(int c)
{
    return ((c >= '0') && (c <= '9'));
}

int tolower(int c)
{
    if (isupper(c))
    {
        return c | 0x20;
    }
    else
    {
        return c;
    }
}

int toupper(int c)
{
    if (islower(c))
    {
        return c & ~0x20;
    }
    else
    {
        return c;
    }
}