#pragma once

#define DISK_SECTOR_SIZE 512
#define PHYSICAL_HARD_DISK_TYPE 0
typedef unsigned int disk_t;

struct disk
{
    disk_t type;
    int sector_size;
    int id;
};

void disk_init();
int disk_read_blocks(struct disk *idisk, int lba, int sectors, void *buf);
