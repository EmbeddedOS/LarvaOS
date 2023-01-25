#pragma once

#include "disk.h"

struct disk_stream
{
    int pos;
    struct disk *disk;
};

struct disk_stream *create_disk_stream(int disk_id);
int disk_stream_seek(struct disk_stream *stream, int pos);
int disk_stream_read(struct disk_stream *stream, void *buf, int total);
void release_disk_stream(struct disk_stream *stream);
