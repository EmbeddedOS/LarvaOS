#pragma once

#define DISK_SECTOR_SIZE 512
#define PHYSICAL_HARD_DISK_TYPE 0
typedef unsigned int disk_t;

struct filesystem;
struct disk
{
    disk_t type;
    int sector_size;
    int id;
    struct filesystem *fs;
    void* fs_private_data;
};

void disk_init();
struct disk *get_disk(int index);
int disk_read_blocks(struct disk *idisk, int lba, int sectors, void *buf);
