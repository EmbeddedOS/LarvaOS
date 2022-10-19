# Boot Sequence

## 1. POST

When a computer is switched on or reset, it runs through a series a diagnostics called **POST** `Power-On-Self-Test`. This sequence culminates in locating a boot-able device, such as a Floppy disk, CD-ROM or a Hard disk in the order that the firmware is configured to.

## 2. Master Boot Record

The (legacy) BIOS checks boot-able devices for a boot signature, a so called magic number.

The boot signature is in a boot sector (sector number 0) and it contains the the byte sequence `0x55`, `0xAA` at byte offsets `510` and `511` respective.

When the BIOS finds such a boot sector, it is loaded into memory `0x0000:0x7c00` (segment 0, address 0x7c00). (However, some BIOS's load to `0x7c0:0x0000` (segment 0x07c0, offset 0), which resolves to the same physical address, but can be surprising. A good practice is to enforce CS:IP at the **VERY START** of your boot sector.)

Execution is then transferred to the freshly loaded boot record. On a hard drive, **the Master Boot Record (MBR)** holds executable code at offset `0x0000-0x01bd`, followed by table entries for the four primary partitions, using **sixteen bytes** per entry (`0x01be - 0x01fd`).

## 3. Early Environment

This early execution environment is highly implementation defined, meaning the implementation of your particular BIOS.

Never make any assumptions on the contents of registers: They might be initialized to 0, but they might contain a spurious value just as well. This includes the FLAGS register and the **SP** register, you may not have a valid stack either! The only thing that is certain, is that the **DL** register holds the drive code from where your boot code was loaded.

The CPU is currently in real mode.

## 4. Kernel
  
Finally, the boot-loader loads the kernel into memory and passes control to it.

## 5. Loading

Now we know what we have to load, let's see how we get it loaded. If booting from hard drive, You have only `446` bytes available for your boot record. Looking at the list of things to do before your kernel image can run, you will agree that this not much:

* Determine which partition to boot from (either by looking for the active partition, or by presenting the user with a selection of installed OS to chose from);

* Determine where your kernel image is located on the boot partition. (either by interpreting the file system, or by loading the image from a fixed position);

* Load the kernel image into memory (requires basic disk I/O);

* Enable protected mode.

* Preparing the runtime environment for the kernel (e.g setting up stack space);

You don't have to do things in this order, but all if this has to be one before you can call `kmain()`.

To make things worse, GCC generates protected mode executables only, So the code for this early environment is one of the [Things You Cannot Do With C](things_you_cannot_do_with_C.md).

There are several approaches to this problem:

* **Geek loading**: Squeeze everything from the above list into the boot record. This is next to impossible, and does not leave room for any special-case handling or useful error messages.

* **One-stage loading**: Write a stub program for making the switch, and link that in front of your kernel image. Boot record loads kernel image (below the 1MB memory mark, because in real mode that is the upper memory limit!), jumps into the stub, stub makes the switch to Protected Mode and runtime preparations, jumps into proper.

* **Two-stage loading**: Write a separate stub program which is loaded below the 1MB memory mark, and does everything from the above list.

### 5.1. The traditional way

Traditionally, the MBR relocates itself to `Ox0000:0x0600`, determines the active partition from the partition table, loads the first sector of that partition (the "**partition boot record**") to `0x0000:0x7c00` (hence the previous relocation), and jumps to that address.

This is called "**chain loading**". If you want your self-written boot record to be capable of dual-booting.

### 5.2. Easy Way Out

Unless you really want to be [Rolling Your Own Boot-loader](rolling_your_own_bootloader.md) (records/ stubs) for the educational value, I **RECOMMEND** using readily available boot-loaders.

The most prominent one is **GRUB**, a **two-stage boot-loader** that not only provides a boot menu with chain-loading capability, but initializes the early environment to a well-defined state (Protected mode and reading various interesting information from the BIOS), can load generic executables as kernel images (instead of requiring flat binaries like most other boot-loaders), supports optional kernel modules, various file systems, and if ./configure'd correctly, Diskless Booting.

### 5.3. Some methods

There are many possible variants to boot. Below is a list of methods but it is possible that there are even more methods:

* You could take an unused partition and load the stage 2 "**raw**".
* You could place the stage 2 between **MBR** and start of the first partition.
* You could (as Lilo did) write a kernel file, then use a tool to detect the sectors (or clusters). Then let stage 1 load the sectors from the list.
* DOS and Windows do it this way: Create an empty filesystem (format it) and then place the kernel in the first file, and the shell in the second file in the empty **rootdir**. So the loader simply loads the first entry in the **rootdir** and then the second.
* Old Linux was booting from floppy disk. The first sector ("boot") loaded the second stage in "**raw**" mode = without filesystem (The second stage was"setup", in the sectors directly behind "boot") The second stage did setup the system (video mode, memory map, etc.) and then loaded the real kernel image (packed in tgz/ bz).
