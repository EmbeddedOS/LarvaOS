#pragma once

#include "disk.h"

struct disk_stream
{
    int pos;
    struct disk *disk;
};
