#pragma once
#include <iostream.hh>

namespace lava
{
    class string
    {

    public:
        explicit string();

        string(const char *val);

        string(const string &source);

        string(string &&source);

        ~string();

        friend ostream &operator<<(ostream &os, const string &str);
        string &operator=(const char *str);

    private:
        char *str;
    };
}
