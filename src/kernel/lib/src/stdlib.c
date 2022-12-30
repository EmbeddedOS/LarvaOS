#include <stdlib.h>

char *itoa(int value, char *str, int base)
{
    int tmp;
    int i, j;

    tmp = value;
    i = 0;

    do
    {
        tmp = value % base;
        str[i++] = (tmp < 10) ? (tmp + '0') : (tmp + 'a' - 10);
    } while (value /= base);

    str[i--] = 0;

    for (j = 0; j < i; j++, i--)
    {
        tmp = str[j];
        str[j] = str[i];
        str[i] = tmp;
    }

    return str;
}

int atoi(const char *s)
{
    int v = 0;
    int sign = 0;

    while ((*s == ' ') || ((unsigned int)(*s - 9) < 5u))
    {
        ++s;
    }

    switch (*s)
    {
    case '-':
        sign = -1;
    case '+':
        ++s;
    }

    while ((unsigned int)(*s - '0') < 10u)
    {
        v = v * 10 + *s - '0';
        ++s;
    }

    return sign ? -v : v;
}
