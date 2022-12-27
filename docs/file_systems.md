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
