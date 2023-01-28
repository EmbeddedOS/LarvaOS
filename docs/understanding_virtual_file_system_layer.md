# Understanding the Virtual File System Layer

- The virtual filesystem layer allows a kernel to support an infinite amount of file systems.
- The virtual filesystem layer allows us to abstract out low level filesystem code.
- Allows filesystem functionality to be loaded or unloaded to the kernel at anytime.

## Unlimited File-systems

- Filesystem drivers can be loaded or unloaded on demand.
- The programming interface to the file-systems remains the same for all file-systems.

## WHat happens when a disk gets inserted?

- We poll each filesystem and ask if the disk holds a filesystem it can manage.
- We call this resolving the filesystem.
- When a filesystem that can used with the disk is found then the disk binds its self to its implementation.

## Communication ?

User programs <---> Kernel <---> FAT 16
                           <---> FAT 32
                           <---> NTFS
                           <---> etc.

## Overview

- The Virtual filesystem layer allows infinite amount of file-systems.
- All the filesystem functionality uses the same interface.
- The caller of file routines does not have to care about which filesystem to use.
