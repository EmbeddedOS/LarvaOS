# Kernel Panic

- Unrecoverable system errors are called kernel panics.

## Purpose

- The main purpose of a kernel panic screen is to inform the user that an unrecoverable error happened, and they have to reset the computer to continue. Quite often the crash screen contains low-level information for developers to help debugging.
- Such low-level information often includes a textual error message, stack trace and register dumps.

## Implementation

- To implement a kernel panic in your kernel, you'll have to write a special function.
  - This function must be dependency-free, and must not rely on other parts of your kernel.
  - It must be self-contained, so that it can display the screen regardless which parts of your OS failed.
  - This function is commonly called `kpanic`, but you can give any name to it you'd like.
  
- A few notes to consider:
  - make sure that this memory allocated for this module does not rely upon any part of the kernel.
  - make sure it is not part of the paging process.  i.e: it is identity mapped as physical memory outside of the paging system.
  - save such information as the current video mode's frame buffer, pixel depth, bytes per scan line, etc.
  - with modern hardware, you will have to render font characters to the screen, since Legacy VGA hardware is no longer present.
  - remember to update this video information if you ever change the screen mode.
  - load this module as soon as possible allowing the kernel to panic during boot if needed.

## Example

- Linux - As much information as possible, important details often lost in the noise. Includes register dumps and stack trace, and requires a screen pager. Totally frightening to average users, no clear instructions for non-experts what to do.

- Source code of panic() function in V6 UNIX:[7]

```C
/*
 * In case console is off,
 * panicstr contains argument to last
 * call to panic.
 */
char    *panicstr;

/*
 * Panic is called on unresolvable
 * fatal errors.
 * It syncs, prints "panic: mesg" and
 * then loops.
 */
panic(s)
char *s;
{
        panicstr = s;
        update();
        printf("panic: %s\n", s);
        for(;;)
                idle();
}
```

## Causes

- A panic may occur as a result of a `hardware failure` or a `software bug` in the operating system. In many cases, the operating system is capable of continued operation after an error has occurred. However, the system is in an unstable state and rather than risking security breaches and data corruption, the operating system stops to prevent further damage and facilitate diagnosis of the error and, in usual cases, restart.

- After recompiling a kernel binary image from source code,
  - A kernel panic while booting the resulting kernel is a common problem if the `kernel was not correctly configured, compiled or installed`.
  - Add-on hardware or malfunctioning RAM could also be sources of fatal kernel errors during start up, due to `incompatibility with the OS or a missing device driver`.
  - A kernel may also go into panic() if it is `unable to locate a root file system`.
  - During the final stages of kernel user space initialization, a panic is typically triggered if the `spawning of init fails`.
  - A panic might also be triggered if the `init process terminates`, as the system would then be unusable.

- The following is an implementation of the Linux kernel final initialization in kernel_init():

```C
static int __ref kernel_init(void *unused)
{

         ...

        /*
         * We try each of these until one succeeds.
         *
         * The Bourne shell can be used instead of init if we are
         * trying to recover a really broken machine.
         */
        if (execute_command) {
                if (!run_init_process(execute_command))
                        return 0;
                pr_err("Failed to execute %s.  Attempting defaults...\n",
                        execute_command);
        }
        if (!run_init_process("/sbin/init") ||
            !run_init_process("/etc/init") ||
            !run_init_process("/bin/init") ||
            !run_init_process("/bin/sh"))
                return 0;

        panic("No init found.  Try passing init= option to kernel. "
              "See Linux Documentation/init.txt for guidance.");
}
```
