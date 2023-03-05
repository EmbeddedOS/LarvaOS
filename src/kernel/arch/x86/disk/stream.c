#include <memory/kheap.h>
#include <stdbool.h>
#include <disk/stream.h>
#include <video.h>

struct disk_stream *create_disk_stream(int disk_id)
{
    struct disk *disk = get_disk(disk_id);
    if (disk == NULL)
    {
        return NULL;
    }

    struct disk_stream *disk_streamer = kzalloc(sizeof(struct disk_stream));
    disk_streamer->pos = 0;
    disk_streamer->disk = disk;
    return disk_streamer;
}

int disk_stream_seek(struct disk_stream *stream, int pos)
{
    stream->pos = pos;
    return 0;
}

int disk_stream_read(struct disk_stream *stream, void *buf, int total)
{
    bool need_to_read_more_than_one_sector = (total > DISK_SECTOR_SIZE);
    int sector = stream->pos / DISK_SECTOR_SIZE;
    int offset = stream->pos % DISK_SECTOR_SIZE;
    char tmp_buf[DISK_SECTOR_SIZE];
    int result = 0;

    // Read a block.
    result = disk_read_blocks(stream->disk, sector, 1, tmp_buf);
    if (result < 0)
    {
        goto out;
    }

    int total_to_read = need_to_read_more_than_one_sector ? DISK_SECTOR_SIZE : total;

    for (int i = 0; i < total_to_read; i++)
    {
        *(char *)buf++ = tmp_buf[offset + i];
    }

    // Adjust the stream position.
    stream->pos += total_to_read;

    // Recursive until read full `total` bytes.
    if (need_to_read_more_than_one_sector)
    {
        result = disk_stream_read(stream, buf, (total - total_to_read));
    }

out:
    return result;
}

void release_disk_stream(struct disk_stream *stream)
{
    kfree(stream);
}