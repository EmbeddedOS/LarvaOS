# Task State Segment

- A Task State Segment (TSS) is a binary data structure specific to the `IA-32` and `x86-64` architectures. It hold information about a task. In Protected Mode the TSS os primary suited for Hardware Task Switch, where each individual Task has its own TSS. For use in software multitasking, one or two are also generally used, as they allow for entering Ring 0 code after an interrupt.
- You'll have to update the TSS yourself in the multitasking function, as it apparently does not save registers automatically.

## Protected Mode

- For its use in hardware task switching, a TSS contains a program's state, including `General Purpose Registers`, `Segment Selectors`, the `Instruction Pointer`, the `EFLAGS Register` and `Control Register 3`. It contains certain other fields described below.

|0x3    |0x2    |0x1    |0x0    |Offset|
|      SSP                      |0x68  |
|IOBP           |Reserved       |0x64  |
|Reserved       |LDTR           |0x60  |
|Reserved       |GS             |0x5C  |
|Reserved       |FS             |0x58  |
|Reserved       |DS             |0x54  |
|Reserved       |SS             |0x50  |
|Reserved       |CS             |0x4C  |
|Reserved       |ES             |0x48  |
|EDI                            |0x44  |
|ESI                            |0x40  |
|EBP                            |0x3C  |
|ESP                            |0x38  |
|EBX                            |0x34  |
|EDX                            |0x30  |
|ECX                            |0x2C  |
|EAX                            |0x28  |
|EFLAGS                         |0x24  |
|EIP                            |0x20  |
|CR3                            |0x1C  |
|Reserved       |SS2            |0x18  |
|ESP2                           |0x14  |
|Reserved       |SS1            |0x10  |
|ESP1                           |0x0C  |
|Reserved       |SS0            |0x08  |
|ESP0                           |0x04  |
|Reserved       |LINK           |0x00  |

- In here:
  - `LINK`: Previous Task Link Field. Contains the `Segment Selector` for the `TSS` of the previous task.
  - `SS0`, `SS1`, `SS2`: The `Segment Selectors` used to load the stack when a privilege level change occurs from a lower privilege level to a higher one.
  - `ESP0`, `ESP1`, `ESP2`: The stack pointers used to load the stack when a privilege level change  occurs from a lower privilege level to a higher one.
  - `IOPB`: I/O Map Base Address Field. Contains a 16-bit offset from the base of the `TSS` to the `I/O Permission Bit Map`.
  - `SSP`: Shadow Stack Pointer.

## TSS in software multitasking

- For each CPU which executes processes possibly wanting to do system calls via interrupts, one TSS is required. The only interesting fields are `SS0` and `ESP0`.
- Whenever a system call occurs, the CPU gets the `SS0` and `ESP0` value in its TSS and assigns the stack-pointer to it. So one or more kernel-stacks need to be set up for processes doing system calls. Be aware that a thread's/process's time-slice may end during a system call, passing control to another thread/process which may as well perform a system call, ending up in the same stack. Solutions are to create  private kernel-stack for each/process and re-assign `ESP0` at any task-switch or to disable scheduling during a system-call.
- Setting up a TSS is straight-forward. An entry in the GDT is needed, specifying the TSS' address as `base`, TSS' size as `limit`, , `0x89` (Present|Executable|Accessed) as `access byte` and `0x40` (Size-bit) as `flags`. In the TSS itself, the members `SS0`, `ESP0` and `IOPB offset` are to be set:
  - `SS0` gets the kernel data segment descriptor (e.g. 0x10 if the third entry in your GDT describes your kernel's data)
  - `ESP0` gets the value the stack-pointer shall get at a system call.
  - `IOPB` may get the value sizeof(TSS) (which is 104) if you don't plan to use this io-bitmap further.

- The actual loading of the `TSS` must take place in protected mode and after the GDT has been loaded. The loading is simple as:

```asm
mov ax, 0x??  ;The descriptor of the TSS in the GDT (e.g. 0x28 if the sixths entry in your GDT describes your TSS)
ltr ax        ;The actual load
```
