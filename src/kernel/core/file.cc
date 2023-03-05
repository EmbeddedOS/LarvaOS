#include <file.hh>

extern "C"
{
#include <disk/disk.h>
#include <fs/path_parser.h>
#include <fs/file.h>
}

namespace lava
{
    file::file(const string &filename, const open_file_mode &mode) : _fd{0}, _name{filename}, _mode{mode}
    {
    }

    void file::open()
    {
        if (is_open() == true)
        {
            return;
        }

        string mode{"r"};
        if (_mode == open_file_mode::READ)
        {
            mode = "r";
        }
        else if (_mode == open_file_mode::WRITE)
        {
            mode = "w";
        }
        else if (_mode == open_file_mode::APPEND)
        {
            mode = "a";
        }

        _fd = fopen(_name.data(), mode.data());

        if (is_open() == true)
        {
            fstat(_fd, &_stat);
        }
    }

    int file::size() const
    {
        return _stat.filesize;
    }

    bool file::is_open() const
    {
        return _fd > 0;
    }

    void file::write(const string &str)
    { // TODO: Implement write function.
        return;
    }

    void file::seekg(const int &pos, const seek_file_mode &mode)
    {
        if (is_open() == false)
        {
            return;
        }

        int c_mode{SEEK_SET};
        if (mode == seek_file_mode::SET)
        {
            c_mode = SEEK_SET;
        }
        else if (mode == seek_file_mode::CUR)
        {
            c_mode = SEEK_CUR;
        }
        else if (mode == seek_file_mode::END)
        {
            c_mode = SEEK_END;
        }

        fseek(_fd, pos, c_mode);
    }

    string file::read(const int &size, const int &nmemb)
    {
        if (is_open() == false)
        {
            return string{""};
        }

        char buf[size];
        int res = fread(_fd, buf, size, nmemb);
        if (res < 0)
        {
            return string{""};
        }

        return string{buf};
    }

    void file::close()
    {
        if (is_open() == false)
        {
            return;
        }

        fclose(_fd);
        _fd = 0;
    }

    file::~file()
    {
        if (is_open() == true)
        {
            close();
        }
    }

    void vfs::initialize()
    { // Initialize the filesystem. Then,
      // initialise the disk controller, bind the filesystem to the disk.
        fs_init();
        disk_init();
    }

    vfs &vfs::get_instance()
    {
        static vfs instance;
        return instance;
    }

}