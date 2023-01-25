# File Allocation Table

## What is the file allocation table?

- The File allocation table is a file system developed by Microsoft.
- It consists of a series of clusters that hold data and a table that determines the state of a cluster.
- The boot sectors stores information about the filesystem.

## FAT 16 Filesystem

- Uses clusters to represent data subdirectories.
- Each cluster uses a fixed amount of sectors which is specified in the boot sector.
- Every file in FAT 16 needs to use at least one cluster for its data this means a lot of storage is wasted for small files.
- FAT 16 cannot store files large than 2GB without large file support. With large file support 4GB is the maximum.

## FAT 16 disk layout

- We assume sector size of 512 bytes.

|Name|Size|
|-------------------|-----------------------------------|
|Boot sector        | 512 bytes                         |
|Reversed sectors   | Fat_header.reversed_sectors * 512 |
|FAT 1              | Fat_header.sectors_per_fat * 512  |
|FAT 2 (Optional)   | Fat_header.sectors_per_fat * 512  |
|Root Directory     | Fat_header.root_dir_entries *     |
|                   | sizeof(struct fat_directory_item) |
|                   | rounded to next sector if needed  |
|Data Clusters      | X                                 |

## FAT 16 File Allocation Table Explained

- Each entry in the table is 2 bytes long and represents a cluster in the data clusters region that is available or taken.
- Clusters can chain together, for example a file large than one cluster will two clusters. The value that represents the first cluster in the file allocation table will contain the value of the next sector. The final cluster will contain a value of `0xFFFF` signifying that there are no more clusters.
- The size of a cluster is represented in the boot sector.
- Example FAT table:

    |0x0003|0xffff|0xffff|0xffff|
    |0x0000|0x0000|0x0000|0x0000|

  - cluster 0x0000 contain 0x0003 index.

## FAT 16 Root Directory
  
- Filesystem directories/folders. FAT16 is no different.
- FAT16 has what's known as a root directory, this is the top most directory in the system.
- Directories contain directory entries of a fixed size.

- FAT 16 Directory Entry
  - Attribute field contains flags that determine if this directory item is a file, or a directory. If it is a read only and so on...
  - If the directory item represents a file, then the first cluster points to the start of the file data. If it is representing a directory, then its first cluster will point to a cluster that has directory entries.

## Iterating though directories

- In the boot sector contains the maximum number of root directory entries we should not exceed this value when iterating through the root directory.
- We know when we have finished iterating though the root directory or a subdirectory because the first byte of the filename will be equal to zero.

## Directory entry attribute flags

- 0x01 - Read Only.
- 0x02 - File Hidden.
- 0x04 - System file do not move the clusters.
- 0x08 - Volume label.
- 0x10 - This is not a regular file it's a subdirectory. (If this bit is not set then this directory entry represents a regular file.)
- 0x20 - Archived.
- 0x40 - Device.
- 0x80 - Reserved must not be changed by disk tools.

## FAT 16 file name and extension

- The filename is 8 bytes wide and unused bytes are padded with spaces (0x20).
- The extension is 3 bytes wide and unused bytes are padded with spaces (0x20).

## CLuster

- Each cluster represents a certain amount of sectors linearly to each other.
- The amount of sectors that represents a cluster is stored in the boot sector.
- the data clusters section in the filesystem contains all the clusters that make up the subdirectories and file data of files throughout the FAT filesystem.

## Useful tips

- Use `__attribute__ ((packed))` with all structures that are to be stored or read from disk. The C compiler can do clever optimization's on structures and this is not what we want when working with raw data from the disk. Setting the packed attribute ensures that never happens.
