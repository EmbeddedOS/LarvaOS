# HEAP

A heap is a vital component of both application programs and the kernel. It is also generally superseded by a higher level of memory management that deals with larger chunks of memory. Fot most operating systems memory will be allocated based on pages or other large chunks. A page on the x86 and x64 architectures is generally 4Kb, but can be larger. However, for smaller allocations the entire page would be wasted. For example, of you needed 24 bytes and you allocate an entire 4KB page you will have wasted a lot of memory. So most applications and kernels will implemented a second memory management scheme that uses memory allocated in 4KB chunks (or large) and break these strips of pages or individual pages into smaller parts as they are requested.

## Standard Library, Applications, And Kernel

Applications in most OSes have their own heap implementation as a part of the standard library and do not use the implementation that the kernel uses. So therefore not only is the heap for the kernel separate from the heap of each application, but it may very well uses a different heap implementation. Also, this allows each application to use it's own implementation depending on which standard library is was built against or with. However. there is no reason why a kernel could not provide heap services directly to applications. It must be noted too that there is no way to enforce an application to use a specific heap or implementation because a clever programmer could simply use the memory allocated to supply it's own implementation.

## Writing a memory management

### First fit MM

It's not that hard to implementation a basic first fit MM.  By memory management, I don't mean Paging, but rather a simple library you can uses in user-space and in your kernel.

#### `malloc()`

Okay, the first step is to get out a pen and paper, I don't mean a keyboard and text editor since you can not draw diagrams.

Anyway, my point is. Do you understand the basic concept? Sketch how the memory will be laid out on paper. Don't start writing your allocate function straight away. That should be one of the last things you should do. Actually design how you want your memory to be managed, where you want to store the information (in a header before, in a global bitmap, etc). What do you want to store in the header/bitmap? If it's a header, what sort of data will be saved in it (pointer to the next free space, if it's free or used).

Remember this is your OS! Plan it how you would like.

The important thing is to start from  an OVERALL view. Not sitting at a keyboard with a blank malloc function expecting to know what to do.

Then write a basic structure for the header/bitmap.

When you know how memory is stored, then you can go on to your allocation function. Close that text editor, we are not done with that pen yet :)). Again, start with a top down view. What, exactly, does allocator do?

1. Find free block
2. Mark it as used
3. Return its address

Then break it up.

How do we find a free block?

* Start with a pointer to the beginning of the free memory.
* Does this header have an  End Of Memory status?
  * Yes - Allocate a new page.
  * No - Return a error (such as a pointer to NULL).
* Does this header say this memory is free.
  * Yes - Is large than the size wanted ?
    * Yes - WE FOUND A FREE BLOCK.
    * NO - point to the next block an go back to step 2.
  * No - Point to the next block an go back to step 2.

Once you have found one, set it's used bit to used. If the space found is significantly larger than the space needed, then it is generally a good idea to split it up, then return the address!

`free()`

Don't program it yet! Now you need to write your free function. You need to basically go through the same process. You need a way to find out where about your bitmap or which header is associated with the address you are given. With a header it's simply address - size of header. Set it's bit to free.

The next step is to scan through the memory and merge free blocks (e.g. if next pointer's header is free, search the next pointer's header, then the next, until you find a used block, then set the current block's next pointer to that used block, skipping over the free blocks - if any of that made sense :D).

Just keep breaking it into smaller, and smaller blocks. Make sure you understand 100% how each process works. The key is to understand it fully before you start coding! Write down every equation for everything. It is essential you know how your code works, and have it in readable form on paper, so when an error occurs it's easy to debug.

Don't copy someone else's code! You can copy their basic theory, but plan how you want it to work in YOUR OS on paper, and implement it YOUR way. That way, you know exactly how it works, and you can debug it and extend on it yourself.

### Best fit MM

A basic idea of a best fit memory manager is same as a first fit one. Having blocks in memory. But the problem with first fit is the fragmentation which occurs when splitting blocks way bigger than we need, when there are smaller fitting blocks.

#### Keeping track of free blocks

You need to know where each free block is in memory, and how big it is. You can store this in an array, or binary tree, or whatever. The way you sort the list is important. If we sort the list on size, small to big, we can look for free block starting at first entry, and as long as the current node is not big enough, going to next node. This way you will find the best fitting block. If no block is found, you need to ask the virtual memory manager for more memory.

#### Merging and splitting blocks

Merging and splitting free blocks becomes a bit more work because you have to keep your free block list updated.

Good luck making a best fit memory allocator!
