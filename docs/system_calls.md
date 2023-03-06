# System Calls

- System calls are used to call a kernel service from user land. The goal is to be able to switch from user mode to kernel mode, with the associated privileges. Provided system calls depends on the nature of your kernel.

## Possible methods to make a system call

### Interrupt

- The most common way to implement system calls is using a software [interrupt](Interrupts.md). It is probably the most portable way to implement system calls. Linux traditionally uses interrupt 0x80 for this purpose on 0x86. Other systems may have fixed system call vector.

- To do this, you will have to create your interrupt handler in assembly. This is because your system call ABI will likely not correspond to the normal ABI the compiler supports. It is therefore necessary to translate from one to the other.

- For example, on `i386`, the Linux kernel gets its  arguments in `eax`, `ebx`, `ecx`, `edx`, `esi`, `edi` and `ebp` in that order. The ABI however places all arguments in reverse order on the stack. Linux proceeds to construct a `pt_regs` structure on the stack and passes a pointer to it to a C function to handle the call itself. This can be simplified into something like this:

```s
Int128Handler:
    ; already on stack: ss, sp, flags, cs, ip.
    ; need to push ax, gs, fs, es, ds, -ENOSYS, bp, di, si, dx, cx, and bx
    push eax
    push dword gs
    push dword fs
    push dword es
    push dword ds
    push dword -ENOSYS
    push ebp
    push edi
    push esi
    push edx
    push ecx
    push ebx
    push esp
    call do_syscall_in_C
    add esp, 4
    pop ebx
    pop ecx
    [...]
    pop es
    pop fs
    pop gs
    add esp, 4
    iretd
```

- Many protected mode OSes use EAX to hold the function code. DOS uses the AX register to store the function code - `AH` for the service snd `AL` for functions of the service, or AH for the functions if there are no services. For example, let's say you have `read()` and `write()`. The codes are 1 for `read()` and 2 for `write()` from the interrupt `0A9h` (an arbitrary choice, possibility wrong). You can write():

```s
 IntA9Handler:
     CMP AH, 1
     JNE .write
     CALL _read
     JMP .done
 .write:
     CMP AH, 2
     JNE .badcode
     CALL _write
     JMP .done
 .badcode:
     MOV EAX, 0FFFFFFFFh
 .done:
     IRETD
```

- However, if all function codes are small contiguous numbers, a better option might be a function table, such as:

```s
dispatch_syscall:
    cmp eax, NR_syscalls
    ja .badcode
    jmp [syscall_table+4*eax]
.badcode:
    mov eax, -ENOSYS
    ret
```

- Note that this assumes the system call table to be NULL free. If there is a hole in the table, fill it with a pointer to a function returning an error code!.

## Passing Arguments

### Registers

- The easiest way to pass arguments to a system call handler are the registers. the BIOS takes arguments this way.

- Pros:
  - Very fast.
- Cons:
  - Limited to the number of available registers.
  - Caller has to save/restore the used registers if it needs their old values after the System Call.
  - Insecure (if the caller passes more/less arguments than the callee assumes to get).

### Stack

- It is also possible to pass arguments through the stack.

- Pros:
  - Nested System calls are possible.
  - It is easy to implement a System Call handler in C because C uses the stack to pass arguments to functions, too.
  - Not limited.

- Cons:
  - Insecure (if the caller passes more/less arguments than the callee assumes to get).

### Memory

- The last common way to pass arguments is to store them in memory. Before making the System Call the caller must store a pointer to the argument's location in a register for the System Call handler.

- Pros:
  - Not limited.
  - Secure.

- Cons:
  - One register is still needed.
  - Nested System Calls are not possible without copying arguments.
  - Insecure (if the caller passes more/less arguments than the callee assumes to get).

## On the user land side

- While the developer can trigger a system call manually,  it is probably a good idea to provide a library to encapsulate such call. Therefore you will be able to switch the system call technique without impacting user applications.

- Another way is to have a stub somewhere in memory that the kernel places there, then once your registers are set up, call that stub to do the actual system call for you. Then you can swap methods at load time rather than compile time.

- Note that whatever library you provide, you cannot assume the user to call the system with that stub. They can, and will, call the system directly if given half the chance.
