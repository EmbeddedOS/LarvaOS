# Memory Management

**Memory management** is a critical part of any operating system kernel. Providing a quick way for programs to allocate and free memory on a regular basis a major responsibility of the kernel. There are many implementations for allocating physical memory including bitmaps, buddy allocation and using tree structures or queues/stacks.

*For an overview of memory allocation models, and methods of allocating memory, see [Program Memory Allocation Types](program_memory_allocation_types.md). If you are looking for heap type memory management, which is the allocation of smaller chunks of memory not on large boundaries then see the [heap](heap.md). A heap is commonly implemented (in the popular way of thinking) not only in the kernel, but also in applications - in the form of a standard library.*

## 1. Address Spaces

Many platforms, including x86, use a memory management unit (MMU) to handle translation between the virtual and physical address spaces. Some architectures have the MMU built-in, while others have a separate chip. Having multiple address spaces separate allows each task to ave its own memory space to work in. In modern systems this is a major part of memory protection. Keeping processes' memory spaces separate allows them to run without causing problems in another process's memory space.

### 1.2. Physical Address Space

The physical address space is the direct memory address used to access a real location in RAM. The addresses used in this space are the bit patterns used to identify a memory location on the address bus.

In this memory model, every executable or library must either use PIC (Position-Independent Code), or come with relocation tables so jump and branch targets can be adjusted by the loader.

### 1.3. Virtual Address Space

The advent of MMUs (Memory Management Units) allows virtual addresses to be used. A virtual address can be mapped to any physical address. It is possible to provide each executable with its own address space, so that memory always starts at 0x0000 0000. This relieves the executable loader of some relocation work, and solves the memory fragmentation problem - you no longer need physical continuous blocks of memory. And since the kernel in control of the `virtual-to-physical mapping`, processes cannot access each other's memory unless allowed to do so by the kernel.

## 2. Memory Translation Systems

The x86 platform is unique in modern computer systems in that it has two methods for handling the mapping between virtual and physical addresses. The two methods, paging and segmentation, each use a very different system to manage memory mapping.

### 2.1. Segmentation

Segmentation is not commonly available in mainstream systems except for the x86. In protected mode this method involves separating each area of memory for a process into units. This is handled by the segment registers: CS, DS, SS, ES, FS, GS (CodeSegment, DataSegment, StackSegment, the rest are ExtraSegments).

### 2.2. Paging

Having an individual virtual-to-physical mapping for each address is of course **ineffective**. The traditional approach to virtual memory is to split up the available physical memory into chunks (pages), and to map virtual to physical addresses page-wise. This task is largely handled by the MMU, so the performance impact is low, and generally accepted as an appropriate price to pay for memory protection.

## 3. Virtual memory

The next step is, instead of reporting an "out of memory" once the physical memory runs out, is to take pages that are not actually accessed currently, and write them to the hard disk (swap-file or partition) - freeing up the physical memory page. This is referred to as "**paging out**" memory.

This requires additional bookkeeping and scheduling, introduces a **severe performance hit** when a process accesses a page that's currently swapped out and must be swapped in again from hard disk, and requires some smart design to run efficiently at all. Do it wrong, and this one part of your OS can severely impact your performance.

On the other hand, your "virtual address space" grows to whatever your CPU and hard drive can handle. In concept, CPU caches and RAM simply become cache layers on top of your hard drive, which represents your "real" memory limitation.

Page swapping systems relies on the assumption that, at a given time, **a process does not need all of its memory to work properly, but only a subset of it** (like, if you're copying a book, you certainly don't need the whole book and a full set of blank pages: the current chapter and a bunch of blank page can be enough if someone can bring you new blank pages and archive the pages you've just written when you come short on blank pages, or bring you the next chapter when you're almost done with the current one). This is known as the working set abstraction. **In order to run correctly, a process requires at least its working set of physical pages:** if less pages are provided to the process, there's a high risk of thrashing, which means the process will be constantly requiring pages to be swapped in -- which forces other pages from this process's working set to be swapped out while they should have remained present.
