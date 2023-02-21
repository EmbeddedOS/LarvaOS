#pragma once
#include "file.h"

struct filesystem *fat16_init();

void *fat16_open(struct disk *disk, struct path_part *path, file_mode mode);
int fat16_read(struct disk *disk, void *p, uint32_t size, uint32_t nmemb, char *out);
int fat16_resolve(struct disk *disk);