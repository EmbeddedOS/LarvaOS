# Understanding the heap

## What is the heap?

Heap is giant memory region that can be shared in controlled manner. You can ask the heap for memory and tell the heap when your done with that memory. Heap implementation are essentially system memory managers.

## C program language

In the C programming language you can point to **any memory area in RAM** regardless if you can access it or not.

## Malloc in C

Returns a memory address of memory that we can write too (it becomes ours).

Ensure that any other time your program calls `malloc` it does not return a memory address that is unavailable.

## Free in C

Accepts the memory address that we want to free. Marks that memory address and all associated blocks as free. Next time someone calls `malloc` we can safely end up with a previous address that was used.

## Memory limits for a 32 bit kernel

While in protected mode we have certain restrictions, the processor is in a 32 bit state.

As we are running in a 32 bit mode we have **access only** to 32 bit memory address allowing us to address to a maximum of 4.29GB or 4294967296 bytes (0x00000000 - 0xffffffff) of RAM **regardless of how much system RAM os installed**.

## Memory of An Uninitialized System

* Video memory takes up portions of RAM.
* Hardware memory takes up portions of RAM.
* Unused parts of RAM are available for use.
* An array of uninitialized memory is available to us from address `0x01000000` (can be a lot of RAM or very little depending on the installed memory).

Note address: `0xC0000000` is reserved this means the memory array we have at address `0x01000000` can give us a maximum of 3.22GB for a machine with 4GB or higher installed.

## The heap

* Can be pointed at an address unused by hardware that is also big enough for us to use.
* The heap data size can be defined for example 100MB of heap memory.
* So long as we have 100MB of memory available our heap will work fine.
* We need a heap implementation to make our heap work properly.
* The heap will be responsible for managing this giant chunk of memory that we call the heap.

## Simplest possible heap implementation

* Start with a start address and call it a `current address` point it somewhere free i.e (0x01000000)
* And call to malloc gets the current address stores it in a temporary variable called `tmp`.
* Now the current address is incremented by the size provided to `malloc`.
* Temporary variable called "tmp" that contains the allocated address is returned.
* Current address now contains the next address for `malloc` to return when `malloc` is called again.

Benefits: Easy to implement.
Negatives: Memory can never be released which may eventually lead to the system being unusable and requiring a reset.

```c
void* current_address = (void*)(0x01000000);
void* malloc(int size)
{
    void* tmp = current_address;
    current_address += size;
    return tmp;
}

void free(void* ptr)
{
    // Our implementation can not free any memory due to its design :((
}  
```

## Our heap implementation

* Will consist of a giant table which describes a giant piece of free memory in the system. This table will describe which memory is taken, which memory is free and so on. **We will call this the "entry table".**
* Will have another pointer to a giant piece of free memory, this will be the actual heap data its self that users of "malloc" call use. **We will call this the "DATA POOL"**. If our heap can allocate 100MB of RAM then the heap data pool will be 100MB in size.
* Our heap implementation will be block based, each address returned from `malloc` will be aligned to 4096 and will at least be 4096 in size.
* If you request to have "50" bytes, 5096 bytes of memory will be returned to you.

## The entry table

* Composes of an array of 1 byte values that represent an entry in our heap data pool.
* Array size is calculated by taking the heap data pool size and dividing it by our block size of 4096 bytes. We are left with the total number of entries we need in our array.

We want a 100MB heap then the math is 100 MB /4096 = 25600 bytes in our entry table. if our heap data pool is at address 0x01000000 then entry zero in our table will represent address 0x01000000. Entry one will represent address 0x01001000. Entry two will represent address 0x01002000.

## The entry structure

But structure of 8 bit entry in our table
7-Upper 4 buts are flags           Lower 4 bits are the entry type-0
|HAS_N  |IS_FIRST|0       |0      |ET_3     |ET_2   |ET_1   |ET_0   |

HAS_N - Set if the entry to the right of us is part of our allocation.
IS_FIRST - Set if this is the first entry of our allocation.

Each entry byte describes 4096 bytes of data in the heap data pool.

## Entry types

* HEAP_BLOCK_TABLE_ENTRY_TAKEN - The entry is taken and the address cannot be used.
* HEAP_BLOCK_TABLE_ENTRY_FREE - The entry is free and may be used.

## The data pool

Simply a raw flat array of thousands or millions of bytes that our heap implementation can give to people who need memory.

## Malloc example

* Firstly we assume we set our heap data pool to point to address 0x01000000.
* We assume our heap is 100MB in size.
* We assume we have 25600 entries in our entry table that describe our 100MB of data in the data pool. (100MB /4096 = 25600)

## Malloc example - Memory Allocation process

* Take the size from malloc and calculate how many blocks we need to allocate for this size. If the user asks for "5000" bytes we will need to allocate 8192 bytes because our implementation works on 4096 byte blocks. 8192 bytes is two blocks.
* Check the entry table for the first entry we can find that has a type of HEAP_BLOCK_TABLE_ENTRY_FREE, meaning that the 4096 block that this entry represents is free for use.
* Since we require two blocks we also need to ensure the next entry is also free for use otherwise we will need to discard the first block we found and look further in our table until we find at least two free blocks that are next to each other.
* Once we have two blocks we mark those blocks as taken.
* We now return absolute address that the start block represents. Calculation: heap_data_pool_start_address + (block_number * block_size).

## Malloc step 1 - Finding the total blocks

* block_size = 4096
* Get the size provided to malloc. For example value "5000" we then align it to "4096" we get the value 8192.
* Now we just divide 8192 by our block size of "4096" which gives us 8192/4096 = 2. **Two Blocks To Allocate**.

## Malloc step 2 - Find two free blocks in the table

## Malloc step 3 - Calculate the absolute address the programmer can use

## Free example

* Calculate the block number based on the address provided to us to free.
* Go through the entry table starting at the block number we have calculated, set each entry to "0x00" until we reach the last block of the allocation.
* We know how many blocks we need to free because the current block we are freeing will not have the "HAS_N" bit set in the entry byte.

## Advantages of our implementation

* Fast to allocate blocks of memory.
* Fast to free blocks of memory.
* Can be written in under 200 lines of code (easy to implement).

## Dis-advantages of our implementation

* We allocate in memory blocks meaning misaligned sizes requested from our heap will result in wasted lost bytes.
* Memory fragmentation is possible.
