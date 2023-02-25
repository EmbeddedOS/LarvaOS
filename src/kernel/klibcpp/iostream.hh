#pragma once

namespace lava
{
    class ostream
    {
    public:
        enum class color
        {
            black = 0,
            blue = 1,
            green = 2,
            cyan = 3,
            red = 4,
            magenta = 5,
            orange = 6,
            light_grey = 7,
            dark_grey = 8,
            light_blue = 9,
            light_green = 10,
            light_cyan = 11,
            light_red = 12,
            light_magenta = 13,
            yellow = 14,
            white = 15
        };

        explicit ostream();
        ~ostream() = default;
        ostream &operator<<(const char *str);
        ostream &operator<<(const int &num);
        ostream &operator<<(const lava::ostream::color &color);
        ostream &operator<<(ostream &(*func)(ostream &out));
    };

    inline ostream &endl(ostream &out)
    {
        out << "\n";
        return out;
    }

    extern ostream cout;
}