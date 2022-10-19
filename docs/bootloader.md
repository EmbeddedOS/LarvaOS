# Boot-loader

A boot-loader is a program written to load a more complex kernel. Implementation details are gathers in [Rolling your Own Boot-loader](rolling_your_own_bootloader.md).

## 1. What does a boot loader do?

The boot loader ultimately has to:

* Bring the kernel (and all the kernel needs to bootstrap) into memory.
* Provide the kernel with the information it needs to work correctly.
* Switch to an environment that the kernel will like.
* Transfer control to the kernel.

On the x86, the boot loader runs in **real mode**. Consequently it has easy access to BIOS resources and functions. Therefore it's a good place to perform memory map detection, detection of available video modes, loading of additional files, etc. The boot loader will collect this information and present it a way the kernel will be able to understand.

### 1.1. Loading your kernel

The bits of your kernel are **somewhere** on **some disk** (presumably the booting disk, but this is not mandatory). Question is: `Where on the disk?`, `Is it a regular file on a FAT-formatted partition?`, `Is it a collection of consecutive sectors in the "reserved area" of the FAT file system (in which case you may need a dedicated tool to format the disk and install the kernel on it)?`, `Or is the disk/partition simply left unformatted and the kernel pasted directly with a disk image tool`.

All the above options are possible. Maybe the one I would choose myself would be to reserve enough space on a FAT file system to store the list of sectors used by the kernel file. The field reserved sectors in the BPB is a perfect place for this. The **"advantage"** of being fully-FAT is that you don't need to re-write the boot-sector every time you rewrite the kernel.

What needs to be loaded mainly depends on what's in your kernel. Linux, for instance, requires an additional `'initrd'` file that will contain the `'initialization process'` (as user level). If your kernel is modular and if File-systems are understood by some modules, you need to load the modules along with kernel. Same goes for `micro-kernel services` like this `/disk/files/memory` services, etc.

### 1.2. Giving the kernel its information

Some kernel require some extra information to run. For example, you will need to tell Linux the root partition to start from. Pretty useful information to have is a map of the address space - effectively a map of where physical memory is and where it is not. Other popular queries regard video modes.

In general, anything that involves a BIOS call is easier to to in **REAL MODE**, so better do them while in real mode than trying to comeback to real mode for a trip later.

### 1.3. Establishing an environment

Most kernels require protected mode. For these kernels you will have to:

* Enable A20.
* Load a GDT.
* Enter Protected mode.

before giving control to the kernel.
It's common for the loader to keep the interrupts disabled (the kernel will enable them later when an IDT is properly setup.)

**NOTE**: take time to think about whether or not you will enable paging here. **KEEP IN MIND THAT** debugging paging initialization code without the help of exception handlers may quickly become a nightmare!!

## 2. Boot-loader design

Virtually any boot-loader follows a common design.

* **Single Stage Boot-loader**: A single stage boot-loader consists of a single file that is loaded entirely by the **BIOS**. However, on the x86 you are usually limited to 512 bytes for a first stage, which is not much. Also, a lot of this size may be dedicated to BIOS structures and FAT headers, which leaves even less space to work with.

* **Two-Stage Boot-loader**: A two-stage boot-loader actually consists of two boot-loader after each other. the first being small with the sole purpose of the loading the second one. The second one can then contain all the code needed for loading the kernel. **GRUB** uses two stages.

* **Mixed Boot-loader**: Another way to avoid the 512 byte barrier is to **split** the boot-loader in two parts, where the first half (512 bytes) can load the rest.This can be achieved by inserting a '512-bytes' break in the ASM code, making sure the rest of the loader is put after the boot-sector.

## 3. Booting multiple OSes

The easiest way to boot another OS is a mechanism called `chain-loading`.
