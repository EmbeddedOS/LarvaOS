# Preparing to read from the hard disk

## What is a PCI IDE controller?

* IDE refers to the electrical specification of cables which connect ATA drives to another device.
* IDE allows up to 4 drives to be connected:
  * ATA (Serial): Used for modern hard drives.
  * ATA (Parallel): Used for hard drives.
  * ATAPI (Serial): Used for modern optical drives.
  * ATAPI (Parallel): Commonly used for optical drives.
* kernel programmers do not have to care if the drive is serial or parallel.

Possible Drive types:

* Primary master drive.
* Primary slave drive.
* Secondary master drive.
* Secondary slave drive.
