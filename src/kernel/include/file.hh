#pragma once
#include <architecture.hh>
#include <string.hh>

extern "C"
{
#include <fs/file.h>
}

namespace lava
{
    enum class open_file_mode
    {
        READ = 0,
        WRITE,
        APPEND
    };

    enum class seek_file_mode
    {
        SET = 0,
        CUR,
        END
    };

    class file
    {
    public:
        file(const string &filename = "", const open_file_mode &mode = open_file_mode::READ);
        virtual ~file();

        virtual void open();
        bool is_open() const;

        int size() const;

        virtual void write(const string &str);
        virtual void seekg(const int &pos, const seek_file_mode &mode);
        virtual string read(const int &size, const int &nmemb = 1);
        virtual void close();

    protected:
        int _fd;
        string _name;
        open_file_mode _mode;
        file_stat _stat;
    };

    class vfs : public arch_interface
    { // Virtual file system.
    public:
        void initialize() override final;
        static vfs &get_instance();

        vfs(const vfs &) = delete;
        vfs(vfs &&) = delete;
        void operator=(vfs const &) = delete;
        void operator=(vfs &&) = delete;

    private:
        vfs() = default;
        ~vfs() = default;
    };
}