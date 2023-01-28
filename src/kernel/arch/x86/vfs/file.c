#include "file.h"
#include <types.h>
#include <string.h>
#include <stdbool.h>
#include <mm/kheap.h>
#include <errno.h>

struct filesystem *filesystems[MAX_FILESYSTEMS];

struct file_descriptor *file_descriptors[MAX_FILE_DESCRIPTORS];

static void fs_static_load()
{
    // TODO: init Fat 16 file system.
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

static int make_new_file_descriptor(struct file_descriptor *fd_out)
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
            fd_out = fd;
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
    struct filesystem **free_fs;
    free_fs = fs_get_free_filesystem();

    if (free_fs == NULL)
    { // Failed to inserting the file system.
        while (true)
        {
        }
    }

    *free_fs = fs;
}

struct filesystem *fs_resolve(struct disk *disk)
{
    struct filesystem *fs = NULL;
    for (int i = 0; i < MAX_FILESYSTEMS; i++)
    {
        if (filesystems[i] != NULL && filesystems[i]->resolve(disk) == NULL)
        { // TODO: Need to check resolve() function pointer is not NULL.
            fs = filesystems[i];
            break;
        }
    }

    return fs;
}

int fopen(const char *file_name, const char *mode_of_operation)
{
    return -EIO;
}