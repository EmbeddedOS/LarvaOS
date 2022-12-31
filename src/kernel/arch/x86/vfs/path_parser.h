#pragma once

#define FILESYSTEM_MAX_PATH_LENGTH 100
#define FILESYSTEM_ROOT_PATH "/"

struct path_part
{
    char *part;
    struct path_part *next;
};

struct path_part *path_parse(const char *path);
