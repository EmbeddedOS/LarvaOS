#pragma once

#define FILESYSTEM_MAX_PATH_LENGTH 100
#define FILESYSTEM_ROOT_PATH "/"

struct path_part
{
    const char *part;
    struct path_part *next;
};