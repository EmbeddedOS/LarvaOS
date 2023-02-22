#include "fat16.h"
#include "path_parser.h"
#include <types.h>
#include <errno.h>
#include <string.h>
#include <disk/stream.h>
#include <mm/kheap.h>
#include <stdbool.h>
#include <strings.h>
#include <video.h>
#include <stdlib.h>

typedef unsigned int FAT_ITEM_TYPE;

// The boot record extensions introduced with DOS 4.0 start with a magic 40 (0x28) or 41 (0x29).
#define FAT16_SIGNATURE 0x29
#define FAT16_FAT_ENTRY_SIZE 0x02
#define FAT16_BAD_SECTOR 0xFF7
#define FAT16_UNUSED 0x00

#define FAT_ITEM_TYPE_DIRECTORY 0
#define FAT_ITEM_TYPE_FILE 1

// FAT directory entry attributes bitmask
#define FAT_FILE_READ_ONLY 0x01
#define FAT_FILE_HIDDEN 0x02
#define FAT_FILE_SYSTEM 0x04
#define FAT_FILE_VOLUME_LABEL 0x08
#define FAT_FILE_SUBDIRECTORY 0x10
#define FAT_FILE_ARCHIVED 0x20
#define FAT_FILE_DEVICE 0x40
#define FAT_FILE_RESERVED 0x80

struct fat_extended_header
{ // Extended BIOS Parameter Block (Dos 4.0)
    uint8_t drive_number;
    uint8_t win_nt_bit;
    uint8_t signature;
    uint32_t volume_id;
    uint8_t volume_id_string[11];
    uint8_t system_id_string[8];
} __attribute__((packed));

struct fat_header
{                             // Common structure of the Boot Sector used by most FAT versions for IBM compatible x86 machines.
    uint8_t short_jmp_ins[3]; // First 3 bytes is jump instruction.
    uint8_t oem_identifier[8];
    uint16_t bytes_per_sector;
    uint8_t sectors_per_cluster;
    uint16_t reserved_sectors;
    uint8_t fat_copies;
    uint16_t root_dir_entries;
    uint16_t number_of_sectors;
    uint8_t media_type;
    uint16_t sectors_per_fat;
    uint16_t sectors_per_track;
    uint16_t number_of_heads;
    uint32_t hidden_sectors;
    uint32_t sectors_big;
} __attribute__((packed));

struct fat_h
{
    struct fat_header primary_header;
    union fat_e_h
    {
        struct fat_extended_header extended_header;
    } shared;
};

struct fat_directory_item
{
    uint8_t filename[8];
    uint8_t ext[3];
    uint8_t attribute;
    uint8_t reserved;
    uint8_t creation_time_tenths_of_a_sec;
    uint16_t creation_time;
    uint16_t creation_date;
    uint16_t last_access;
    uint16_t high_16_bits_first_cluster;
    uint16_t last_mod_time;
    uint16_t last_mod_date;
    uint16_t low_16_bits_first_cluster;
    uint32_t filesize;
} __attribute__((packed));

struct fat_directory
{
    struct fat_directory_item *item;
    int total;
    int sector_pos;
    int ending_sector_pos;
};

struct fat_item
{
    union
    {
        struct fat_directory_item *item;
        struct fat_directory *directory;
    };

    FAT_ITEM_TYPE type;
};

struct fat_file_descriptor
{
    struct fat_item *item;
    uint32_t pos;
};

struct fat_private_data
{
    struct fat_h header;
    struct fat_directory root_dir;

    // Used to stream data clusters.
    struct disk_stream *cluster_read_stream;

    // Used to stream the file allocation table.
    struct disk_stream *fat_read_stream;

    // Used to stream the directory.
    struct disk_stream *directory_stream;
};

struct filesystem fat16_fs =
    {
        open : fat16_open,
        resolve : fat16_resolve,
        read : fat16_read,
        seek : fat16_seek,
        stat : fat16_stat
    };

static void fat16_init_private_data(struct disk *disk, struct fat_private_data *private)
{
    memset(private, 0, sizeof(struct fat_private_data));
    private->cluster_read_stream = create_disk_stream(disk->id);
    private->fat_read_stream = create_disk_stream(disk->id);
    private->directory_stream = create_disk_stream(disk->id);
    print("FAT16 filesystem is initializing private dat with the disk. \n");
}

static int fat16_get_total_items_for_directory(struct disk *disk, int directory_start_sector)
{
    struct fat_directory_item item;
    struct fat_directory_item empty_item;
    memset(&empty_item, 0, sizeof(empty_item));
    struct fat_private_data *private = (struct fat_private_data *)disk->fs_private_data;

    int res = 0;
    int total = 0;
    int directory_start_pos = directory_start_sector * disk->sector_size;
    struct disk_stream *stream = private->directory_stream;
    if (disk_stream_seek(stream, directory_start_pos) != 0)
    {
        res = -EIO;
        goto out;
    }

    while (true)
    {
        if (disk_stream_read(stream, &item, sizeof(item)) != 0)
        {
            res = -EIO;
            goto out;
        }

        if (item.filename[0] == 0xE5)
        { // Directory[0] = 0xE5 indicates the directory entry is free (available).
            continue;
        }

        if (item.filename[0] == 0x00)
        { // Directory[0] = 0x00 also indicates the directory entry is free (available).
            // However, this is an additional indicator that `all directory entries following
            // the current free entry are also free. So, we done!!`
            break;
        }
        total++;
    }

    res = total;

out:
    return res;
}

static int fat16_sector_to_absolute(struct disk *disk, int sector)
{
    return sector * disk->sector_size;
}

static int fat16_get_root_directory(struct disk *disk, struct fat_private_data *private, struct fat_directory *directory)
{
    int res = 0;
    struct fat_header *primary_header = &private->header.primary_header;
    int root_dir_sector_pos = (primary_header->fat_copies * primary_header->sectors_per_fat) + primary_header->reserved_sectors;
    int root_dir_entries = private->header.primary_header.root_dir_entries;
    int root_dir_size = root_dir_entries * sizeof(struct fat_directory_item);
    int total_sectors = root_dir_size / disk->sector_size;

    if (root_dir_size % disk->sector_size)
    { // Plus a sector (512 bytes) if root dir size remainder few (< 512 byte).
        total_sectors += 1;
    }

    int total_items = fat16_get_total_items_for_directory(disk, root_dir_sector_pos);

    struct fat_directory_item *dir = kzalloc(root_dir_size);
    if (dir == NULL)
    {
        res = -ENOMEM;
        goto out;
    }

    // Seek to root directory position.
    struct disk_stream *stream = private->directory_stream;
    if (disk_stream_seek(stream, fat16_sector_to_absolute(disk, root_dir_sector_pos)) != 0)
    {
        res = -EIO;
        goto out;
    }

    // Read Root directory data from disk.
    if (disk_stream_read(stream, dir, root_dir_size) != 0)
    {
        res = -EIO;
        goto out;
    }

    directory->item = dir;
    directory->total = total_items;
    directory->sector_pos = root_dir_sector_pos;
    directory->ending_sector_pos = root_dir_sector_pos + (root_dir_size / disk->sector_size);
out:
    return res;
}

void fat16_to_proper_string(char **out, const char *in, size_t size)
{
    int i = 0;
    while (*in != 0x00 && *in != 0x20)
    {
        **out = *in;
        *out += 1;
        in += 1;
        // We cant process anymore since we have exceeded the input buffer size
        if (i >= size - 1)
        {
            break;
        }
        i++;
    }

    **out = 0x00;
}

static void fat16_get_full_relative_filename(struct fat_directory_item *item, char *out, int max_len)
{ // For example: filename = test, file extension = .txt -> output: test.txt
    memset(out, 0x00, max_len);
    char *out_tmp = out;
    fat16_to_proper_string(&out_tmp, (const char *)item->filename, sizeof(item->filename));
    if (item->ext[0] != 0x00 && item->ext[0] != 0x20)
    {
        *out_tmp++ = '.';
        fat16_to_proper_string(&out_tmp, (const char *)item->ext, sizeof(item->ext));
    }
}

static int fat16_get_first_cluster(struct fat_directory_item *item)
{
    return (item->high_16_bits_first_cluster) | (item->low_16_bits_first_cluster);
}

static int fat16_cluster_to_sector(struct fat_private_data *private, int cluster)
{ // Convert clusters to sectors.
    return private->root_dir.ending_sector_pos + ((cluster - 2) * private->header.primary_header.sectors_per_cluster);
}

static uint32_t fat16_get_first_fat_sector(struct fat_private_data *private)
{
    return private->header.primary_header.reserved_sectors;
}

static int fat16_get_fat_entry(struct disk *disk, int cluster)
{
    int res = 0;
    struct fat_private_data *private = disk->fs_private_data;
    struct disk_stream *stream = private->fat_read_stream;
    if (stream == NULL)
    {
        res = -EIO;
        goto out;
    }

    uint32_t fat_table_postion = fat16_get_first_fat_sector(private) * disk->sector_size;
    res = disk_stream_seek(stream, fat_table_postion * (cluster * FAT16_FAT_ENTRY_SIZE));
    if (res < 0)
    {
        goto out;
    }

    uint16_t result = 0;
    res = disk_stream_read(stream, &result, sizeof(result));
    if (res < 0)
    {
        goto out;
    }

    res = result;
out:
    return res;
}

static int fat16_get_cluster_for_offset(struct disk *disk, int starting_cluster, int offset)
{ // Gets the correct cluster to use based on the starting cluster and the offset.
    int res = 0;
    struct fat_private_data *private = disk->fs_private_data;
    int size_of_cluster_bytes = private->header.primary_header.sectors_per_cluster * disk->sector_size;
    int cluster_to_use = starting_cluster;
    int clusters_ahead = offset / size_of_cluster_bytes;
    for (int i = 0; i < clusters_ahead; i++)
    {
        int entry = fat16_get_fat_entry(disk, cluster_to_use);
        if (entry == 0xFF8 || entry == 0xFFF)
        { // Last entry in the file.
            res = -EIO;
            goto out;
        }

        if (entry == FAT16_BAD_SECTOR)
        { // Sector is marked as BAD.
            res = -EIO;
            goto out;
        }

        if (entry == 0xFF0 || entry == 0xFF6)
        { // Reserved sector.
            res = -EIO;
            goto out;
        }

        if (entry == 0x00)
        {
            res = -EIO;
            goto out;
        }

        cluster_to_use = entry;
    }

    res = cluster_to_use;
out:
    return res;
}

static int fat16_read_internal_from_stream(struct disk *disk, struct disk_stream *stream, int cluster, int offset, int total, void *out)
{
    int res = 0;
    struct fat_private_data *private = disk->fs_private_data;
    int size_of_cluster_bytes = private->header.primary_header.sectors_per_cluster * disk->sector_size;
    int cluster_to_use = fat16_get_cluster_for_offset(disk, cluster, offset);
    if (cluster_to_use < 0)
    {
        res = cluster_to_use;
        goto out;
    }

    int offset_from_cluster = offset % size_of_cluster_bytes;
    int starting_sector = fat16_cluster_to_sector(private, cluster_to_use);
    int starting_pos = (starting_sector * disk->sector_size) + offset_from_cluster;
    int total_to_read = (total > size_of_cluster_bytes) ? size_of_cluster_bytes : total;
    res = disk_stream_seek(stream, starting_pos);
    if (res < 0)
    {
        goto out;
    }

    res = disk_stream_read(stream, out, total_to_read);
    if (res < 0)
    {
        goto out;
    }

    total -= total_to_read;
    if (total > 0)
    { // Need to read more!!
        res = fat16_read_internal_from_stream(disk,
                                              stream,
                                              cluster,
                                              (offset + total_to_read),
                                              total,
                                              (out + total_to_read));
    }

out:
    return res;
}

static int fat16_read_internal(struct disk *disk, int starting_cluster, int offset, int total, void *out)
{
    struct fat_private_data *private = disk->fs_private_data;
    struct disk_stream *stream = private->cluster_read_stream;
    return fat16_read_internal_from_stream(disk, stream, starting_cluster, offset, total, out);
}

static void fat16_free_directory(struct fat_directory *directory)
{
    if (directory == NULL)
    {
        return;
    }

    if (directory->item != NULL)
    {
        kfree(directory->item);
    }

    kfree(directory);
}

static struct fat_directory *fat16_load_fat_directory(struct disk *disk, struct fat_directory_item *item)
{
    int res = 0;
    struct fat_directory *directory = NULL;
    struct fat_private_data *private = disk->fs_private_data;
    if (!(item->attribute & FAT_FILE_SUBDIRECTORY))
    {
        res = -EINVAL;
        goto out;
    }

    directory = kzalloc(sizeof(struct fat_directory));

    if (directory == NULL)
    {
        res = -ENOMEM;
        goto out;
    }

    int cluster = fat16_get_first_cluster(item);
    int cluster_sector = fat16_cluster_to_sector(private, cluster);
    int total_items = fat16_get_total_items_for_directory(disk, cluster_sector);
    directory->total = total_items;

    int directory_size = directory->total * sizeof(struct fat_directory_item);
    directory->item = kzalloc(directory_size);
    if (directory->item == NULL)
    {
        res = -ENOMEM;
        goto out;
    }

    res = fat16_read_internal(disk, cluster, 0x00, directory_size, directory->item);

out:
    if (res < 0)
    {
        fat16_free_directory(directory);
    }

    return directory;
}

static struct fat_directory_item *fat16_clone_directory_item(struct fat_directory_item *item)
{
    struct fat_directory_item *item_copy = kzalloc(sizeof(struct fat_directory_item));
    if (item_copy == NULL)
    {
        goto out;
    }
    memcpy(item_copy, item, sizeof(struct fat_directory_item));

out:
    return item_copy;
}

static struct fat_item *fat16_make_new_fat_item_for_directory_item(struct disk *disk, struct fat_directory_item *item)
{
    struct fat_item *f_item = kzalloc(sizeof(struct fat_item));
    if (f_item == NULL)
    {
        return NULL;
    }

    if (item->attribute & FAT_FILE_SUBDIRECTORY)
    {
        f_item->directory = fat16_load_fat_directory(disk, item);
        f_item->type = FAT_ITEM_TYPE_DIRECTORY;
        return f_item;
    }

    f_item->type = FAT_ITEM_TYPE_FILE;
    f_item->item = fat16_clone_directory_item(item);
    return f_item;
}

static struct fat_item *fat16_find_item_in_directory(struct disk *disk, struct fat_directory *directory, const char *name)
{
    struct fat_item *item = NULL;
    char tmp_filename[FILESYSTEM_MAX_PATH_LENGTH];

    for (int i = 0; i < directory->total; i++)
    {
        // Remove space, etc.
        // file name origin format: DATA    TXT
        fat16_get_full_relative_filename(&directory->item[i], tmp_filename, sizeof(tmp_filename));

        if (strcasecmp(tmp_filename, name) == 0)
        {   // After remove space, the filename that get from the disk
            // has the upcase format, for example: DATA.TXT
            // Found the file, let's create a new fat item.
            print("FAT16 filesystem is found the file: ");
            print(name);
            print("\n");
            item = fat16_make_new_fat_item_for_directory_item(disk, &directory->item[i]);
        }
    }

    return item;
}

static void fat16_fat_item_free(struct fat_item *item)
{
    if (item == NULL)
    {
        return;
    }

    if (item->type == FAT_ITEM_TYPE_DIRECTORY)
    {
        fat16_free_directory(item->directory);
    }
    else if (item->type == FAT_ITEM_TYPE_FILE)
    {
        kfree(item->item);
    }

    kfree(item);
}

static struct fat_item *fat16_get_directory_entry(struct disk *disk, struct path_part *path)
{
    print("FAT16 filesystem is finding the file.\n");

    struct fat_private_data *private = disk->fs_private_data;
    struct fat_item *current_item = NULL;
    struct fat_item *root_item = fat16_find_item_in_directory(disk, &private->root_dir, path->part);
    if (root_item == NULL)
    {
        goto out;
    }

    struct path_part *next = path->next;
    current_item = root_item;

    while (next != NULL)
    {
        if (current_item->type != FAT_FILE_SUBDIRECTORY)
        {
            current_item = NULL;
            break;
        }
        struct fat_item *tmp_item = fat16_find_item_in_directory(disk, current_item->directory, next->part);
        fat16_fat_item_free(current_item);
        current_item = tmp_item;
        next = next->next;
    }

out:
    return current_item;
}

struct filesystem *fat16_init()
{
    print("Initializing FAT16 filesystem...\n");
    strcpy(fat16_fs.name, "FAT16");
    return &fat16_fs;
}

void *fat16_open(struct disk *disk, struct path_part *path, file_mode mode)
{
    struct fat_file_descriptor *fat_fd = NULL;
    int error_code = 0;

    if (mode != FILE_MODE_READ)
    { // Currently, we only support reading files.
        error_code = -EROFS;
        goto error_out;
    }

    fat_fd = kzalloc(sizeof(struct fat_file_descriptor));
    if (fat_fd == NULL)
    {
        error_code = -ENOMEM;
        goto error_out;
    }

    print("FAT16 filesystem is opening the file.\n");

    fat_fd->item = fat16_get_directory_entry(disk, path);
    if (fat_fd->item == NULL)
    {
        error_code = -EIO;
        goto error_out;
    }

    fat_fd->pos = 0;
    return fat_fd;

error_out:
    print("FAT16 filesystem is failed to open the file.\n");
    if (fat_fd != NULL)
    {
        kfree(fat_fd);
    }
    return ERR_PTR(error_code);
}

int fat16_resolve(struct disk *disk)
{
    print("FAT16 filesystem is trying to resolve the disk: ");
    print_number(disk->id);
    print(".\n");

    int res = 0;
    struct fat_private_data *fat_private = kzalloc(sizeof(struct fat_private_data));
    fat16_init_private_data(disk, fat_private);

    disk->fs_private_data = fat_private;
    disk->fs = &fat16_fs;

    struct disk_stream *stream = create_disk_stream(disk->id);
    if (stream == NULL)
    {
        res = -ENOMEM;
        goto out;
    }

    // Get FAT filesystem header of the Boot Sector.
    if (disk_stream_read(stream, &fat_private->header, sizeof(fat_private->header)) != 0)
    {
        res = -EIO;
        goto out;
    }
    print("FAT16 filesystem reads headers of boot sector successfully.\n");

    // Check FAT 16 signature is present or not.
    if (fat_private->header.shared.extended_header.signature != FAT16_SIGNATURE)
    {
        res = -EIO;
        goto out;
    }

    // Check the root directory is load successfully or not.
    if (fat16_get_root_directory(disk, fat_private, &fat_private->root_dir) != 0)
    {
        res = -EIO;
        goto out;
    }

    print("FAT16 filesystem loaded root directory successfully with total entry: ");
    print_number(fat_private->root_dir.total);
    print(".\n");

out:
    if (stream)
    { // Release our stream.
        release_disk_stream(stream);
    }

    if (res < 0)
    {
        kfree(fat_private);
        disk->fs_private_data = NULL;
        print("FAT16 filesystem is failed to resolve the disk.\n");
    }

    return res;
}

int fat16_read(struct disk *disk, void *p, uint32_t size, uint32_t nmemb, char *out)
{
    int res = 0;
    struct fat_file_descriptor *fat_desc = p;
    struct fat_directory_item *item = fat_desc->item->item;
    int offset = fat_desc->pos;

    for (uint32_t i = 0; i < nmemb; i++)
    {
        res = fat16_read_internal(disk, fat16_get_first_cluster(item), offset, size, out);
        if (res < 0)
        {
            goto out;
        }

        out += size;
        offset += size;
    }

    res = nmemb;
out:
    return res;
}

int fat16_seek(void *p, uint32_t offset, file_seek_mode seek_mode)
{
    int res = 0;
    struct fat_file_descriptor *fat_desc = p;
    struct fat_item *desc_item = fat_desc->item;
    if (desc_item->type != FAT_ITEM_TYPE_FILE)
    {
        res = -EINVAL;
        goto out;
    }

    struct fat_directory_item *item = desc_item->item;
    if (offset >= item->filesize)
    {
        res = -EIO;
        goto out;
    }

    switch (seek_mode)
    {
    case SEEK_SET:
        fat_desc->pos = offset;
        break;
    case SEEK_CUR:
        fat_desc->pos += offset;
        break;
    case SEEK_END: // Not implemented.
                   // TODO: Implement seek end mode.
    default:
        res = -EINVAL;
        break;
    }

out:
    return res;
}

int fat16_stat(struct disk *disk, void *p, struct file_stat *stat)
{
    int res = 0;

    struct fat_file_descriptor *fat_desc = p;
    struct fat_item *desc_item = fat_desc->item;
    if (desc_item->type != FAT_ITEM_TYPE_FILE)
    {
        res = -EINVAL;
        goto out;
    }

    struct fat_directory_item *item = desc_item->item;
    strcpy(stat->filename, (const char *)item->filename);
    strcpy(stat->ext, (const char *)item->ext);

    if (item->attribute & FAT_FILE_READ_ONLY)
    {
        stat->attribute |= FILE_STAT_READ_ONLY;
    }
    else
    {
        stat->attribute = 0x00;
    }

    stat->reserved = item->reserved;
    stat->creation_time = item->creation_time;
    stat->creation_date = item->creation_date;
    stat->last_access = item->last_access;
    stat->last_mod_time = item->last_mod_time;
    stat->last_mod_date = item->last_mod_date;
    stat->filesize = item->filesize;

out:
    return res;
}
