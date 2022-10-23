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
