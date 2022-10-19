# Rolling your own boot-loader

Some people prefer to use their own software for everything, or wish to try their hand at coding a boot-loader.

This page attempts to describe what steps to take when you write your own boot-loader.

Before you start writing one, it is best that you know the background [boot-loader](bootloader.md).

## 1. What and why?

### 1.1. Disclaimer

Okay. You are here because you don't want to use a mainstream boot-loader. You may also want to code your own boot-loader as a learning experience to better understand how they function.

Whether or not you will use your own boot-loader or reuse an existing tool is completely up to you. If you get the feeling you don't understanding a thing, make sure you read our page about the [Boot Sequence](boot_sequence.md) first.

A good reason to have a custom boot-loader would be a custom filesystem, though you could add support for your filesystem to GRUB or perhaps some other boot-loader.

### 1.2. What you need to do?

The boot-loader ultimately has to bring the kernel (and all the kernel needs to bootstrap) in memory, switch to an environment that the kernel will like and then transfer control to the kernel.

As the scope of this article is protected mode C kernels, I'll assume that "an environment the kernel will like" means **Protected Mode**, with kernel and additional components being stored at their 'favorite', compile-time known locations, with a wide-enough stack ready and BSS section cleared.

### 1.3. What you could wish to add

Since the boot-loader runs in **Real Mode**, it has easier access to BIOS resources and functions. Therefore it's a good place to perform **memory map detection**, **detecting available video modes**, **loading additional files** etc. The boot-loader will collect this information and present it in a way the kernel will be able to understand.

## 2. Loading... Please wait

### 2.1. Where will you load your kernel?

You will have to decide where in memory you are going to load your kernel. Your kernel generally depends on it.

In **Real Mode**, the easiest is to stay below the `1MB` barrier, which means you practically have `512KB` of memory to load things. You may wish the kernel to be loaded at a **well-known position**, say `0x10000` physical (es=0x1000, bx=0 when calling INT 13h).

If your kernel is bigger (or is expected to grow bigger) than this, you'll probably prefer to have the kernel above the 1MB barrier, which means you need to activate **A20 gate** and switch to **Unreal Mode** to load the kernel (with A20 alone, you cannot have more than 64K above 1MB).

Note that BIOS will still be **unable** to write to memory above 1MB, so you need to read stuff in a buffer below `1MB` and then perform a rep `movsd` to place the data where they ultimately should go.

### 2.2. How will you find your kernel?

The bits of your kernel are **somewhere** on **some disk** (presumably the booting disk, but this is not mandatory). Question is: `Where on the disk?`, `Is it a regular file on a FAT-formatted partition?`, `Is it a collection of consecutive sectors in the "reserved area" of the FAT file system (in which case you may need a dedicated tool to format the disk and install the kernel on it)?`, `Or is the disk/partition simply left unformatted and the kernel pasted directly with a disk image tool`.

All the above options are possible. Maybe the one I would choose myself would be to reserve enough space on a FAT file system to store the list of sectors used by the kernel file. The field reserved sectors in the BPB is a perfect place for this. The **"advantage"** of being fully-FAT is that you don't need to re-write the boot-sector every time you rewrite the kernel.

### 2.3. What else could you need to load?

That mainly depends on what's in your kernel. Linux, for instance, requires an additional `initrd` file that will contain the 'initialization process' (as user level).

### 2.4. What if I get beyond the 512 bytes of the boot sector?

**Make sure** the first `512 bytes` are able to load the rest of your loader and you're safe. Some do this with a separate "**second stage**" loader, others by really inserting a `512-bytes` break in their ASM code, making sure the rest of the loader is put after the boot-sector (that is, starting at 0x7e00).

### 2.5. What if I wish to offer the user the option to boot several OSes?

TODO: complete me, please!

### 2.6. How do I actually load bytes

TODO: complete me, please!

## 3. Loaded. Gathering Information

TODO: complete me, please!

## 4. Ready. Entering Protected Mode

To enter protected mode you should first disable interrupts and set global descriptor table. After it set `PE` bit of `CR0`:

```asm
mov eax,cr0
or eax,1
mov cr0,eax
```

After it set registers and do a far jump to kernel. If data selector is `10h`, code selector is `8h` and kernel offset is `10000h` do:

```asm
mov ax,10h
mov ds,ax
mov es,ax
mov fs,ax
mov gs,ax
mov ss,ax
jmp 8:10000h
```

Notes:

* that in this case, the **GDT** will be temporary. Indeed, the loader has no idea of what the kernel wants to do with the **GDT**, so all it can do is providing a minimal and let the kernel reload **GDTR** with an appropriate **GDT** later.
* It's common for the loader to keep interrupts disabled (the kernel will enable them later when an IDT is properly set up).
* Give yourself the time about thinking whether you'll enable paging now or not. **Keep in mind** that debugging paging initialization code without the help of exception handlers may quickly become a nightmare!
* It is possible to perform more initialization once protected mode is enabled and before kernel is loaded. This will, however, require that you mix `16bits` and `32bits` code in a single object file, which can quickly become a nightmare too...
* It is very likely that your kernel does not start with an executable code, rather it has an ELF or PE header at `10000h`. You'll have to parse that to get the entry point to jump to.

## 5. You have long way to continue

Now, you are very distant from using extern and call function for C Code. You will need to enable **A20**, make something which reads images (so you can actually boot any .bin or .sys file), and so on.

## 6. A list of things you might want to do

* Setup 16-bit segment registers and stack.
* Print startup message.
* Check presence of PCI, CPUID, MSRs.
* Enable and confirm enabled A20 line.
* Load GDTR.
* Inform BIOS of target processor mode.
* Get memory map from BIOS.
* Locate kernel in filesystem.
* Allocate memory to load kernel image.
* Load kernel image into buffer.
* Enable graphics mode.
* Check kernel image ELF headers.
* Enable long mode, if 64-bit.
* Allocate and map memory for kernel segments.
* Setup stack.
* Setup COM serial output port.
* Setup IDT.
* Disable PIC.
* Check presence of CPU features (NX, SMEP, x87, PCID, global pages, TCE, WP, MMX, SSE, SYSCALL), and enable them.
* Assign a PAT to write combining.
* Setup FS/GS base.
* Load IDTR.
* Enable APIC and setup using information in ACPI tables.
* Setup GDT and TSS.
