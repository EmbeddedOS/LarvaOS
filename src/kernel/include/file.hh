#pragma once
#pragma once

namespace lava
{
    enum class open_file_mode {
        READ,
        WRITE,
        APPEND
    };

    class file
    {
    public:
        explicit file(const char *filename, open_file_mode mode);
        virtual ~file();

        virtual void open();
        virtual void write();
        virtual void seekg();
        virtual void read();
        virtual void close();

    protected:
        char* _name;
        open_file_mode _mode;
    };
}