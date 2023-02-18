#include "path_parser.h"
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <mm/kheap.h>

static bool path_format_is_valid(const char *filename)
{
    int length = strnlen(filename, FILESYSTEM_MAX_PATH_LENGTH);
    int root_path_length = strlen(FILESYSTEM_ROOT_PATH);

    return (length >= root_path_length) && (memcmp((void *)&filename[0], FILESYSTEM_ROOT_PATH, root_path_length) == 0);
}

static char *get_and_remove_prefix_path(const char **parent_path)
{ /* Function return prefix of the path and remove the prefix, for example:
   * Input: path_part_1/path_part_2/file_name
   * Output:
   * + Argument 0 becomes path_part_2/file_name
   * + return value: path_part_1
   */

    char *suffix_path = strchr(*parent_path, '/');
    size_t length_of_parent_path = strlen(*parent_path);

    if (suffix_path == NULL && length_of_parent_path == 0)
    { // End of parts.
        return NULL;
    }
    else if (suffix_path == NULL && length_of_parent_path)
    { // Get the last file name if any.
        suffix_path = (char *)(*parent_path) + length_of_parent_path;
    }

    int position = suffix_path - (*parent_path);

    if (position == 0)
    { // Skip the forward slash.
        *parent_path += 1;
        return get_and_remove_prefix_path(parent_path);
    }

    char *path = kzalloc((position + 1));
    memcpy(path, *parent_path, position);
    path[position] = '\0';
    *parent_path += position;

    return path;
}

struct path_part *parse_one_path_part(const char **path)
{
    char *prefix_path = get_and_remove_prefix_path(path);
    if (prefix_path == NULL)
    {
        return NULL;
    }

    struct path_part *part = kzalloc(sizeof(struct path_part));
    part->next = NULL;
    part->part = prefix_path;

    return part;
}

void free_root_path(struct path_part *root)
{
    struct path_part *part = root->next;
    while (part != NULL)
    {
        struct path_part *next = part->next;
        kfree((void *)part->part);
        kfree(part);
        part = next;
    }

    kfree(root);
}

struct path_part *path_parse(const char *path)
{
    const char *tmp_path = path;
    struct path_part *root = NULL;

    if (!path_format_is_valid(path))
    {
        goto out;
    }

    root = parse_one_path_part(&tmp_path);
    if (root == NULL)
    {
        goto out;
    }

    struct path_part *part = parse_one_path_part(&tmp_path);
    root->next = part;

    while (part)
    {
        struct path_part *next = parse_one_path_part(&tmp_path);
        part->next = next;
        part = next;
    }

out:
    return root;
}