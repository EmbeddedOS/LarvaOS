#include <string.hh>
#include <memory.hh>

extern "C"
{
#include <string.h>
}

namespace lava
{
    string::string()
        : str{nullptr}
    {
        str = new char[1];
        str[0] = '\0';
    }

    string::string(const char *val)
    {
        if (val == nullptr)
        {
            str = new char[1];
            str[0] = '\0';
        }
        else
        {
            str = new char[strlen(val) + 1];
            strcpy(str, val);
            str[strlen(val)] = '\0';
        }
    }

    string::string(string &&source)
    {
        str = source.str;
        source.str = nullptr;
    }

    string::string(const string &source)
    {
        str = new char[strlen(source.str) + 1];
        strcpy(str, source.str);
        str[strlen(source.str)] = '\0';
    }

    string::~string()
    {
        delete[] str;
    }

    ostream &operator<<(ostream &os, const string &str)
    {
        os << str.str;
        return os;
    }

    string &string::operator=(const char *val)
    {
        if (str != nullptr)
        {
            delete[] str;
        }

        if (val == nullptr)
        {
            str = new char[1];
            str[0] = '\0';
        }
        else
        {
            str = new char[strlen(val) + 1];
            strcpy(str, val);
            str[strlen(val)] = '\0';
        }

        return *this;
    }
}