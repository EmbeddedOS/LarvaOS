#pragma once

#define MAX_FILESYSTEMS 12
#define MAX_FILE_DESCRIPTORS 512

typedef unsigned int file_seek_mode;
typedef unsigned int file_mode;

enum
{
    SEEK_SET = 0,
    SEEK_CUR,
    SEEK_END
};

enum
{
    FILE_MODE_READ = 0,
    FILE_MODE_WRITE,
    FILE_MODE_APPEND,
    FILE_MODE_INVALID
};

struct disk;
struct path_part;
typedef void *(*FS_OPEN_FUNCTION)(struct disk *disk, struct path_part *path, file_mode mode);
typedef int (*FS_RESOLVE_FUNCTION)(struct disk *disk);

struct filesystem
{
    char name[10];
    // Filesystem should return zero from resolve if the provided disk is using its filesystem.
    FS_RESOLVE_FUNCTION resolve;
    FS_OPEN_FUNCTION open;
};

struct file_descriptor
{
    int index;
    struct filesystem *fs;
    struct disk *disk; // The disk that the file descriptor should be on.
    void *p;           // Private data for internal file descriptor.
};

void fs_init();
void fs_insert_filesystem(struct filesystem *fs);
struct filesystem *fs_resolve(struct disk *disk);

int fopen(const char *file_name, const char *mode_of_operation);