#include "fat16.h"
#include <types.h>
#include <errno.h>
#include <string.h>
#include <disk/stream.h>
#include <mm/kheap.h>
#include <stdbool.h>

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

struct fat_item_descriptor
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
        resolve : fat16_resolve
    };

static void fat16_init_private_data(struct disk *disk, struct fat_private_data *private)
{
    memset(private, 0, sizeof(struct fat_private_data));
    private->cluster_read_stream = create_disk_stream(disk->id);
    private->fat_read_stream = create_disk_stream(disk->id);
    private->directory_stream = create_disk_stream(disk->id);
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

struct filesystem *fat16_init()
{
    strcpy(fat16_fs.name, "FAT16");
    return &fat16_fs;
}

void *fat16_open(struct disk *disk, struct path_part *path, file_mode mode)
{
    return NULL;
}

int fat16_resolve(struct disk *disk)
{
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

out:
    if (stream)
    { // Release our stream.
        release_disk_stream(stream);
    }

    if (res < 0)
    {
        kfree(fat_private);
        disk->fs_private_data = NULL;
    }

    return res;
}