#include <strings.h>

int strcasecmp(const char *s1, const char *s2)
{
    int result;

    while (1)
    {
        result = tolower(*s1) - tolower(*s2++);

        if ((result != 0) || (*s1++ == 0))
        {
            break;
        }
    }

    return result;
}

int strncasecmp(const char *s1, const char *s2, size_t c)
{
    int result = 0;

    while (c)
    {
        result = toupper(*s1) - toupper(*s2++);

        if ((result != 0) || (*s1++ == 0))
        {
            break;
        }

        c--;
    }

    return result;
}
