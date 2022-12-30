#include "path_parser.h"
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <kheap.h>

static bool path_format_is_valid(const char *filename)
{
    int length = strnlen(filename, FILESYSTEM_MAX_PATH_LENGTH);
    int root_path_length = strlen(FILESYSTEM_ROOT_PATH);

    return (length >= root_path_length) && (memcmp(filename[0], FILESYSTEM_ROOT_PATH, root_path_length) == 0);
}

static struct path_part *create_root_path()
{
    struct path_part *root = (struct path_part *)kzalloc(sizeof(struct path_part));
    root->next = NULL;
    root->part = FILESYSTEM_ROOT_PATH;
    return root;
}

static const char* get_path_part(const char** path)
{

} 

struct path_part* parse_path_part(struct path_part* last_part, const char** path)
{

}

void free_root_path(struct path_part *root)
{
    struct path_part *part = root->next;
    while (part != NULL)
    {
        struct path_part *next = part->next;
        kfree(part->part);
        kfree(part);
        part = next;
    }

    kfree(root);
}

struct path_part *path_parse(const char *path, const char *current_directory_path)
{
    int result = 0;
    const char *tmp_path = path;
    struct path_part *root = NULL;
    if (!path_format_is_valid(path))
    {
        goto out;
    }

    root = create_root_path();
    if (root == NULL)
    {
        goto out;
    }

out:
    return root;
}