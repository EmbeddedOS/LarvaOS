#pragma once
#include <iostream.hh>

namespace lava
{
    class string
    {

    public:
        string();

        string(const char *val);

        string(const string &source);

        string(string &&source);

        ~string();

        const char *data() const;

        friend ostream &operator<<(ostream &os, const string &str);
        string &operator=(const char *str);

    private:
        char *_str;
    };
}
