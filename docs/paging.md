# Paging

Paging is a system which allows each process to see a full virtual address space, without actually requiring the full amount of physical memory to be available or present. 32-bit x86 processors support 32 bit virtual addresses and 4 GiB virtual address spaces, and current 64-bit processors support 48-bit virtual addressing and 256-TiB virtual address spaces.

In addition to this, paging introduces the benefit of page-level protection. In this system, user processes can only see and modify data which is paged in on their own address space, providing hardware-based isolation. System pages are also protected from user processes. On the x86-64 architecture, page-level protection now completely supersedes Segmentation as the memory protection mechanism. On the IA-32 architecture, both paging and segmentation exist, but segmentation is now considered `legacy`.

Once an Operating System has paging, it can also make use of other benefits and workarounds, such as linear frame-buffer simulation for memory-mapped IO and paging out to disk, where disk storage space is used to free up physical RAM.

## Understanding paging

### What is Paging?

- Paging allows us to remap memory addresses to point to other memory addresses.
- Can be used to provide the illusion we have the maximum amount of RAM installed.
- Can be used to hide memory from other processes.

### Remapping memory

- Paging allows to remap one memory address to another, so 0x100000 could point to 0x200000.
- Paging works in 4096 byte block sizes by default. The  blocks are called pages.
- When paging is enabled the MMU (Memory Management Unit) will look at your allocated page tables to resolve virtual addresses into physical addresses.
- Paging allows us to pretend memory exists when it does not.

### Virtual addresses vs Physical addresses?

- Virtual addresses are addresses that are not pointing to the address in memory that their value says they are. Virtual address 0x100000 might point to physical address 0x200000 as an example.
- Physical addresses are absolute addresses in memory whose value points to the same address in memory. For example if physical address 0x100000 points to address 0x100000 then this is a physical address.
- Essentially virtual address and physical address are just terms we used to explain how a piece of memory is being accessed.

### Structure Overview

- 1024 page directories that point to 1024 page tables.
- 1024 page table entries per page table.
- Each page table entry covers 4096 bytes of memory.
- Each `4096` byte block of memory is called a page.
- `1024 * 1024 * 4096 = 4,294,967,296 bytes` ($GB of addressable memory).

### Page directory structure

- Holds a pointer to a page table.
- Holds attributes.

Page directory structure:

|31           11|10     9|                0|
|Page table 4-KB|Avail.  |G|S|0|A|D|W|U|R|P|
|Aligned Address|        | | | | | | | | | |

Description:

- Page table 4-KB Aligned Address: The physical 4096 byte aligned address to the start of a page table.
- S: Set to 0 for 4KB pages, set to 1 for 4 MB pages.
- A: Set to 1 by the CPU if this page is accessed.
- W: If this bit is set then write though caching is enabled if not then write back is enabled instead.
- U (User supervisor bit): If this is set then this page can be accessed by all privilege ring levels. If it not set then only supervisor ring can access this page.
- R: If this bit is set the page readable and writable if its not set then this page is only readable. Note the WP bit in the CR) register can allow writing in all cases for supervisor.
- P: This bit is set if the page exist in real memory, if this page is not actually available then we the kernel developer should set this bit to zero. If someone accesses this memory a "Page fault" will occur and we are expected to resolve it.

Page table entry structure;

|31           11|10     9|                0|
|Physical   4-KB|Avail.  |G|0|D|A|C|W|U|R|P|
|Aligned Address|        | | | | | | | | | |

Description:

- G: Set to prevent TLB from updating the address in its cache if the CR3 register is reset.
- D: If set this indicates the page has been written to.
- C: Cache disable bit. Set to 1 to disable page caching.

### Page fault exception

The CPU will call the page fault interrupt 0x14 when their was a problem with paging.
The exception is invoked:

- If you access a page on memory that does not have its `P (Present)` bit set.
- Invoked of you access a page that is for supervisor but you are not supervisor.
- Invoked if you write to a page that is read only and you are not supervisor.

### Hiding memory from processes

- If we give each process its own page directory table then we can map the memory for the process however we want it to be. We can make it so the process can only see its self.
- Hiding memory can be achieved by switching the page directories when moving between processes.
- All processes can access the same virtual memory addresses but they will point to different physical addresses.

### Illusion of more memory

- We can pretend we have the maximum amount of memory even if we do not.
- This is achieved by creating page tables that are not present. Once a process accesses this non-present address a page fault will occur. We can then load the page back into memory and the process had no idea.
- 100MB system can act as if it has access to the full 4GB on a 32 bit architecture.

### Benefits to paging

- Each process can access the same virtual memory addresses, never writing over each other.
- Security is is an added benefit as we can map out physical memory that we don't want processes to see.
- Can be used to prevent overwriting of sensitive data such as program code.
- many more benefits exist.

## 32-bit Paging (protected mode)

### MMU

Paging is achieved though the [Memory Management Unit (MMU)](memory_management.md). On the x86, the MMU maps memory though a series of tables, two to be exact. They are the paging directory (PD), and the paging table (PT).

Both tables contain 1024 4-bute entries, making them 4KiB each. In the paging directory, each entry points to a page table. In the page table, each entry points to a 4 KiB  physical page frame. Additional, each entry has bits controlling access protection and caching features of the structure to which it points. The entire system consisting of a page directory and page tables represents a linear 4-GiB virtual memory map.


