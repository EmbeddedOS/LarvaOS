# Paging

Paging is a system which allows each process to see a full virtual address space, without actually requiring the full amount of physical memory to be available or present. 32-bit x86 processors support 32 bit virtual addresses and 4 GiB virtual address spaces, and current 64-bit processors support 48-bit virtual addressing and 256-TiB virtual address spaces.

In addition to this, paging introduces the benefit of page-level protection. In this system, user processes can only see and modify data which is paged in on their own address space, providing hardware-based isolation. System pages are also protected from user processes. On the x86-64 architecture, page-level protection now completely supersedes Segmentation as the memory protection mechanism. On the IA-32 architecture, both paging and segmentation exist, but segmentation is now considered `legacy`.

Once an Operating System has paging, it can also make use of other benefits and workarounds, such as linear frame-buffer simulation for memory-mapped IO and paging out to disk, where disk storage space is used to free up physical RAM.

## 32-bit Paging (protected mode)

### MMU

Paging is achieved though the [Memory Management Unit (MMU)](memory_management.md). On the x86, the MMU maps memory though a series of tables, two to be exact. They are the paging directory (PD), and the paging table (PT).

Both tables contain 1024 4-bute entries, making them 4KiB each. In the paging directory, each entry points to a page table. In the page table, each entry points to a 4 KiB  physical page frame. Additional, each entry has bits controlling access protection and caching features of the structure to which it points. The entire system consisting of a page directory and page tables represents a linear 4-GiB virtual memory map.
