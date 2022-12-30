# File systems

File system are the operating system's method of ordering data on persistent store device like disks. They provide an abstracted interface to access data on these devices in such a way that it can be read or modified efficiently. Which file system is convenient depends on the target application of the OS.
For example, Windows uses FAT32 or NTFS file system. If a disk has a large capacity, FAT32 is inconvenient, because the FAT system was designed considering the smaller disks available at that time. At the same time, an NTFS file system is not convenient on a tiny disk, because it was designed to work with large volumes of data - there would be excessive overhead when using devices such as a 1.44MB floppy disk.

## What is a file system?

* A filesystem is a structure that describes how information is laid on disk.
* Disks are not aware of files.
* The OS knows the filesystem structure so knows how to read files from the disk.

## Disks

* Hard disks can be thought of as just a giant array of information split into sectors.
* Each sector can be read into memory and is given a LBA (Logic Block Address) number.
* Files do not exist on the disk.
* Disks have no concept of files.

## Filesystem structure

* Contains raw data for files (Remember the disk isn't aware of this).
* Contains the filesystem structure header which can explain things such as how many files are on the disk, where the root directory is located and so on...
* The way files are laid on disk is different depending on the filesystem you are using for example "FAT16", "FAT32", "NTFS", and more ...
* without filesystem we would be forced to read and write data though the use of sector numbers, structure would not exist and corruption would be likely.

## FAT16(File Allocation Table) 16 bits

* The first sector in this filesystem format is the boot sector on a disk. Fields also exist in this first sector that describe the filesystem such as how many reversed sectors follow this sector.
* Then follows the reversed sectors these are sectors ignored by the filesystem. There is a field in the boot sector that specifies how many reversed sectors there are. (Remember the OS must ignore these its not automatic! the disk has no idea)
* Now we have our first file allocation table, this table contains values that represent which clusters on the disk are taken and which are free. (A cluster is just a certain number of sectors joined together to represent one cluster)
* Next comes our second file allocation table it's optional though and depends on the FAT6 header in the boot sector.
* Now comes our root directory this explains what files/directories are in the root directory of the filesystem. Each entry has relative name that represents the file or directory name, attributes such as read only, the address of the first cluster representing the data on the disk and more. And more ...
* Finally we have our data region, all the data is here.

## What to take away from this

* Disks don't understand files they are just giant flat arrays of bytes.
* Operating systems need file system drivers so they can read from the disk.
* Every filesystem is different.
* Without implementing a filesystem in your OS you can not have files.

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

The allocation table is comparable to the bitmap approach. However instead of just having a field free or occupied, it may contain other information. Advantage is that the use of this structure is simple. Disadvantage is that this approach is relatively slow, and a separate set of data is needed to define the which sections are ued, and in which order. FAT for example, combines a linked list and an allocation table in the same structure.

### Separate file and system areas

Some filesystem keep metadata and actual contents in separate areas on the disk. This makes specific sorts of data easy to find, as well as allowing for a special 'free' area. Downside is that you have to keep track of the boundary or boundaries between these areas, as the usage can differ and the disk has to adapt for that (big files: more data, less metadata; small files: less data, more metadata). This method is used prominently in SFS.

## Network File Systems

All these file systems are a way to create a large, distributed storage system from a collection of "back end" systems. That means you can not (for instance) format a disk in 'NFS' but you instead mount a 'virtual' NFS partition that will reflect what's on another machine. Note that a new generation of file systems is under heavy research, basing on latest P2P, cryptography and error correction techniques.

## File Systems of OSDevers

while you could pick `<insert favorite filesystem here>`as your OS main filesystem, you might want to consider all options. Commonly, you will want to have a filesystem that is operational very quickly so that you can concentrate on the rest before implementing a 'real filesystem'.

### "Beginners" filesystems

There are only five filesystems that are both relatively easy to implement and worth to consider.

 There is no general recommendation as the choice depends largely on style and OS design. Instead you can read the comparison and make your own educated decision.

* USTAR
* RAMFS/TMPFS
* FAT
  * Can be read and written by virtually all OSes.
  * The 'standard' for floppies.
  * Relatively easy to implement.
  * Part of it involving long filenames and compatibility.
  * Large overhead.
  * No support for large (>4GB) files.
  * No support for Unix permissions.
* Ext2
* BMFS

### Rolling your own

There are many different kinds of filesystems around, from the well-known to the more obscure ones. The most unfortunate thing about filesystems is that every hobbyist OS programmer thinks that the filesystem they design is the ultimate technology, when in reality it's usually just a copy of FAT with change here and there, perhaps because it is one of the easiest to implement. The world doesn't need another FAT-like filesystem. Investigate all the possibilities before you decide to roll your own.

IF despite of this warning you decide to create your own file system, then you should start that by implementing a FUSE driver for it.

### Guideline if you do decide to roll your own

* Consider carefully what it will be used for.
* Use a program to figure out the layout (e.g. a spreadsheet). The basic areas needed are:
  * Bootsector: Unless you are booting with UEFI, this is a must. Even then, it's recommended to include it in the specs for compatibility with older file systems. This section should contain at a minimum the disk size, location of the file table, hidden sectors for multiple partition disks, and a version number. I'd be leaving plenty of reserved space for features you don't think of. Don't forget to leave space for a jmp instruction and the boot code!.
  * File table: Don't think of this as just a simple table containing a list of files and their locations. One idea is, instead of storing files, the system would store file parts, and the file table would list the parts in each file. This would be useful for saving space if many files on the disk are the same or similar (for example, license agreements).
  * Data area: Files will be stored here.
* Create a program to read and write disk images with your filesystem. Parts of this will be portable into the fs driver.
* It is strongly recommended to create a FUSE driver for your file system.
* Implement the fs into your OS.
