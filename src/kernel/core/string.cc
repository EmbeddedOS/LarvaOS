#include <string.hh>
#include <memory.hh>

extern "C"
{
#include <string.h>
}

namespace lava
{
    string::string()
        : _str{nullptr}
    {
        _str = new char[1];
        _str[0] = '\0';
    }

    string::string(const char *val)
    {
        if (val == nullptr)
        {
            _str = new char[1];
            _str[0] = '\0';
        }
        else
        {
            _str = new char[strlen(val) + 1];
            strcpy(_str, val);
            _str[strlen(val)] = '\0';
        }
    }

    string::string(string &&source)
    {
        _str = source._str;
        source._str = nullptr;
    }

    string::string(const string &source)
    {
        _str = new char[strlen(source._str) + 1];
        strcpy(_str, source._str);
        _str[strlen(source._str)] = '\0';
    }

    string::~string()
    {
        delete[] _str;
    }

    ostream &operator<<(ostream &os, const string &str)
    {
        os << str._str;
        return os;
    }

    string &string::operator=(const char *val)
    {
        if (_str != nullptr)
        {
            delete[] _str;
        }

        if (val == nullptr)
        {
            _str = new char[1];
            _str[0] = '\0';
        }
        else
        {
            _str = new char[strlen(val) + 1];
            strcpy(_str, val);
            _str[strlen(val)] = '\0';
        }

        return *this;
    }

    const char *string::data() const
    {
        return _str;
    }

}