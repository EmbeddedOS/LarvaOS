# File systems

File system are the operating system's method of ordering data on persistent store device like disks. They provide an abstracted interface to access data on these devices in such a way that it can be read or modified efficiently. Which file system is convenient depends on the target application of the OS.
For example, Windows uses FAT32 or NTFS file system. If a disk has a large capacity, FAT32 is inconvenient, because the FAT system was designed considering the smaller disks available at that time. At the same time, an NTFS file system is not convenient on a tiny disk, because it was designed to work with large volumes of data - there would be excessive overhead when using devices such as a 1.44MB floppy disk.

## File system theory

A filesystem provides a generalized structure over persistent storage, allowing the low-level structure of the devices (e.g., disk, tape, flash memory storage) to be abstracted away. Generally speaking, the goal of a filesystem is *allowing logical groups of data to be organized into files*, which can be manipulated as a unit. In order to do this, the filesystem must provide some sort of index of the locations of files in the actual secondary storage:

* Tracking the available storage space.
* Tracking which block blocks of data belong to which files.
* Creating new files.
* Reading data from existing files into memory.
* Updating the data in the files.
* Deleting existing files.

(Perceptive readers will note that the last four operations - Create, Read, Update, and Delete, or CRUD - are also applicable to many other data structures, and are fundamental to databases as well as filesystem.)

Additionally, there are other features which go along with a practice filesystem:

* Assigning human-readable names to files, and renaming files after creation.
* Allowing files to be divided among non-contiguous blocks, in storage, and tracking the parts of files even when they are *fragmented* across the medium.
* Providing some form of hierarchical structure, allowing the files to be divided into directories or folder.
* Buffering reading and writing to reduce the number of actual operation on the physical medium.
* Caching frequently accessed files or parts of files to speed up access.
* Allowing files to be marked as 'read-only' to prevent unintentional corruption of critical.
* Providing a mechanism for preventing unauthorized access to a user's files.

Additional features may be found on some filesystem as well, such as automatic encryption, or journalling of read/write activity.

### Indexing Methods

There are several methods of indexing the contents of files, with the most commonly used being *i-nodes* and *File Allocation Tables*.

### Inodes

Inodes (information nodes) are a crucial design element in most Unix file systems: Each file is made of data blocks (the sectors that contains your your raw data bits), index blocks (containing pointers to data blocks so that you know which sector is the nth in the sequence), and one inode block.

The inode is the root of the index blocks, and can also be sole index block of the file is small enough. Moreover, as Unix file systems support hard links (the same file may appear several times in the directory tree), inodes are a natural place to store Metadata such as file size, owner, creation/access/modification times, locks, etc.

### FAT

The File Allocation Table (FAT) is the primary indexing mechanism of MS-DOS and it's descendants. There are several variants on FAT, but the general design is to have a table (actually a pair of tables, one serving as a backup for the first in case it is corrupted) which holds a list of blocks of a given size, which map to the whole capacity of the disk.

## Workings of File Systems

There are several common approaches to storing disk information. However, in comparison to memory management, there are some key differences in managing disk media:

* Data can only be written in fixed size chunks.
* Access times are different for different locations on the disk. Seeking is usually a costly operation.
* Data throughput is very small compared to RAM.

Hence some file systems have specialized structures, algorithms, ot combinations thereof to improve speed ratings.

### Allocation table

The allocation table is comparable to the bitmap approach. However instead of just having a field free or occupied, it may contain other information.