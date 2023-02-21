#include "file.h"
#include "fat16.h"
#include "path_parser.h"
#include <types.h>
#include <string.h>
#include <stdbool.h>
#include <mm/kheap.h>
#include <disk/disk.h>
#include <errno.h>
#include <video.h>

struct filesystem *filesystems[MAX_FILESYSTEMS];

struct file_descriptor *file_descriptors[MAX_FILE_DESCRIPTORS];

static void fs_static_load()
{
    fs_insert_filesystem(fat16_init());
}

static struct filesystem **fs_get_free_filesystem()
{
    for (int i = 0; i < MAX_FILESYSTEMS; i++)
    {
        if (filesystems[i] == NULL)
        {
            return &filesystems[i];
        }
    }

    return NULL;
}

static int make_new_file_descriptor(struct file_descriptor **fd_out)
{
    int res = -ENOMEM;
    for (int i = 0; i < MAX_FILE_DESCRIPTORS; i++)
    {
        if (file_descriptors[i] == NULL)
        {
            struct file_descriptor *fd = kzalloc(sizeof(struct file_descriptor));
            // File descriptor will start at 1.
            fd->index = i + 1;
            file_descriptors[i] = fd;
            *fd_out = fd;
            res = 0;
            break;
        }
    }

out:
    return res;
}

static struct file_descriptor *get_file_descriptor(int index)
{
    if (index <= 0 || index >= MAX_FILE_DESCRIPTORS)
    {
        return NULL;
    }

    return file_descriptors[index - 1];
}

static file_mode get_file_mode_by_string(const char *str)
{
    file_mode mode = FILE_MODE_INVALID;

    if (strncmp(str, "r", 1) == 0)
    {
        mode = FILE_MODE_READ;
    }
    else if (strncmp(str, "w", 1) == 0)
    {
        mode = FILE_MODE_WRITE;
    }
    else if (strncmp(str, "a", 1) == 0)
    {
        mode = FILE_MODE_APPEND;
    }

    return mode;
}

void fs_load()
{
    memset(filesystems, 0, sizeof(filesystems));
    fs_static_load();
}

void fs_init()
{
    memset(file_descriptors, 0, sizeof(file_descriptors));
    fs_load();
}

void fs_insert_filesystem(struct filesystem *fs)
{
    print("Inserting filesystem: ");
    print(fs->name);
    print(".\n");

    struct filesystem **free_fs;
    free_fs = fs_get_free_filesystem();

    if (free_fs == NULL)
    { // Failed to inserting the file system.
        while (true)
        {
        }
    }

    *free_fs = fs;
    print("Inserting filesystem: ");
    print(fs->name);
    print(" successfully.\n");
}

struct filesystem *fs_resolve(struct disk *disk)
{
    struct filesystem *fs = NULL;
    for (int i = 0; i < MAX_FILESYSTEMS; i++)
    {
        if (filesystems[i] != NULL && filesystems[i]->resolve != NULL && filesystems[i]->resolve(disk) == NULL)
        {
            print("The disk is resolved with filesystem: ");
            print(filesystems[i]->name);
            print(".\n");
            fs = filesystems[i];
            break;
        }
    }

    return fs;
}

int fopen(const char *file_name, const char *mode_of_operation)
{
    int res = 0;

    // Parse filename.
    struct path_part *root = path_parse(file_name);
    if (root == NULL)
    {
        res = -EINVAL;
        goto out;
    }

    // Ensure the disk exists.
    // We only use the disk with a drive number of 0.
    struct disk *disk = get_disk(0);
    if (disk == NULL)
    {
        res = -EIO;
        goto out;
    }

    // The disk have a bound filesystem.
    if (disk->fs == NULL)
    {
        res = -EIO;
        goto out;
    }

    file_mode mode = get_file_mode_by_string(mode_of_operation);
    if (mode == FILE_MODE_INVALID)
    {
        res = -EINVAL;
        goto out;
    }

    print("Trying to open: ");
    print(file_name);
    print(" using fs: ");
    print(disk->fs->name);
    print(" of the disk: ");
    print_number(disk->id);
    print("\n");

    void *fd_private_data = disk->fs->open(disk, root, mode);
    if (IS_ERR(fd_private_data))
    {
        res = PTR_ERR(fd_private_data);
        goto out;
    }

    struct file_descriptor *fd = NULL;
    res = make_new_file_descriptor(&fd);
    if (res < 0)
    {
        goto out;
    }

    fd->fs = disk->fs;
    fd->p = fd_private_data;
    fd->disk = disk;
    res = fd->index;
out:
    if (res < 0)
    { // fopen should not return negative values,
        // return 0 indicate a error.
        res = 0;
    }
    // If success, it return a file descriptor.
    return res;
}

/**
 * nmemb: number of memory block.
 */
int fread(int fd, void *ptr, uint32_t size, uint32_t nmemb)
{
    int res = 0;
    if (fd < 1 || size == 0 || nmemb == 0)
    {
        res = -EINVAL;
        goto out;
    }

    struct file_descriptor *desc = get_file_descriptor(fd);
    if (desc == NULL)
    {
        res = -EINVAL;
        goto out;
    }

    res = desc->fs->read(desc->disk,
                         desc->p,
                         size,
                         nmemb,
                         (char *)ptr);

out:
    return res;
}

int fseek(int fd, int offset, file_seek_mode whence)
{
    int res = 0;
    struct file_descriptor *desc = get_file_descriptor(fd);
    if (desc == NULL)
    {
        res = -EINVAL;
        goto out;
    }

    res = desc->fs->seek(desc->p, offset, whence);
out:
    return res;
}