# Communication with kernel from a process

- User program calls interrupt using the interrupt instruction.
- Kernel interrupt routine is executed and extracts arguments pushed by the user program.
- Kernel interrupt routine returns the result.

## User program calls an interrupt using the interrupt instruction

- Lets pretend we have a kernel operation that's represented by code 1. Code 1 simply prints a message to the screen.

- Telling the kernel to print is easy from user land we do the following:
  - The user program begins by setting the `EAX` register to 1 this is kernel operation code for print operations.
  - The user program then pushes the address of the message that should be printed to the screen.
  - The user program issues an interrupt to the kernel. The interrupt number used is 0x80 as in this hypothetical kernel implementation we have decided to user interrupt 0x80 handling commands to the kernel.

- Example Code to Print From User land:

    ```asm
    ; void print(const char* message);

    print:
        push ebp
        mov ebp, esp

        mov eax, 1          ; Command 1 = Print
        mov ebx, [ebp+8]    ; String to print
        push dword ebx      ; Push it to the stack

        int 0x80            ; Invoke kernel to print
        add esp, 3          ; Restore stack
        pop ebp
        ret
    ```

## Kernel interrupt routine is executed

- The processor pushes the same information we pushed to get into user land in the first place to the stack.

```asm
isr80h_wrapper:
    ; INTERRUPT FRAME START
    ; ALREADY PUSHED TO US IS THE USER LAND STACK INFORMATION
    ; uint32_t ip;
    ; uint32_t cs;
    ; uint32_t flags;
    ; uint32_t sp;
    ; uint32_t ss;
    ; Save user land registers
    pushad

    ; INTERRUPT FRAME START

    ; Push a pointer to the user land registers, and the stack tat was passed to us. Essentially push a pointer to the interrupt frame.
    
    push exp

    ; EAX holds our command lets push it

    push eax 
    call isr80h_handler     ; Return value to EAX.
    mov dword[tmp_res], eax
    pop ebx
    pop ebx

    ; Restore user land registers
    popad

    ; Set the EAX register to the return result stored in tmp_res.
    mov eax, [tmp_res]
    iretd
```

- The interrupt 0x80 kernel routine begins execution.
- We save all the user land registers ready for when we handle the interrupt handler.
- We push the stack pointer so the interrupt handle has something to work with.
- The command number in the EAX register is also pushed.
- The isr80h_handler is called.

- The interrupt frame that we pushed a pointer for:

```C
struct interrupt_frame
{
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t reserved;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;

    uint32_t ip;
    uint32_t cs;
    uint32_t flags;
    uint32_t esp;
    uint32_t ss;
} __attribute__((packed));
```

- The isr80h_handler function is called:

```C
void* isr80h_handler(int command, struct interrupt_frame * frame)
{
    void *res = 0;

    // Our interrupt handler may only be called by programs and not the kernel.
    kernel_page();
    task_current_save_state();
    res = isr80h_handle_command(command, frame);

    // If the task is now in a paused state we should switch to the next task.
    if (!task_current()->awake)
    {
        task_next();
        // We can never execute past this line, execution is changed with task_next();
    }
    task_page();

    return res;
}
```

- A pointer to the interrupt frame is also present.
- Finally we handle the command provided to us.

```C
void *isr80h_command1_print()
{
    // The message to print is the first element on the user stack.
    void* msg_user_space_addr = task_current_get_stack_item(0);

    char buf[1024];
    ASSERT(copy_string_from_task(task_current(), msg_user_space_addr, buf, sizeof(buf)) == 0);

    task_print(buf);
    return 0;
}
```

- We get the user land address from the tasks stack.
- We then copy the string from that address into a kernel buffer.
- Next, we call our print function providing our buffer.

- The message is outputted to the screen.
- The kernel commands return 0.
- The EAX register is populated with the return result upon returning from int 0x80.

- We are now executing in user land again just after our interrupt call to the kernel. EAX is equal to zero as the routine returned zero.

## Calling the Kernel Overview

- The processor pushes the same information we pushed to get into user land in the first place to the stack.
- Interrupt 0x80 kernel routine begins execution, the command number is extracted from the EAX register.
- The C interrupt handler for 0x80 is called.
- The task that executed the interrupt has its state saved, all registers for that task are saved in the tasks register structure. These are extracted these are extracted from the interrupt frame.
- Execution flow is passed to the correct handler for the command number that was provided to the kernel in the EAX register.
- Kernel does the action that it was instructed to do from the user land i.e print to the screen.
- Kernel command handler returns a value.
- Execution continues after the user lands `int 0x80` instruction.
- The EAX register is populated with the return result from the kernel.

## Kernel Command numbers

- Can be represented by an enum
- Can also be represented by definitions #define.

```Cpp
enum SystemCommands
{
    SYSTEM_COMMAND_EXIT,
    SYSTEM_COMMAND_PRINT,
    SYSTEM_COMMAND_GET_KEY,
    SYSTEM_COMMAND_GET_KERNEL_INFO,
    SYSTEM_COMMAND_PUTCHAR,
    SYSTEM_COMMAND_MALLOC,
    SYSTEM_COMMAND_INVOKE,
    SYSTEM_COMMAND_SLEEP,
    SYSTEM_COMMAND_VIDEO_RECTANGLE_NEW,
    SYSTEM_COMMAND_VIDEO_RECTANGLE_SET_PIXEL,
    SYSTEM_COMMAND_VIDEO_RECTANGLE_FILL,
    SYSTEM_COMMAND_VIDEO_RECTANGLE_DRAW_BLOCK,
    SYSTEM_COMMAND_VIDEO_RECTANGLE_DRAW_BLOCKS,
    SYSTEM_COMMAND_VIDEO_FONT_GET,
    SYSTEM_COMMAND_VIDEO_FONT_DRAW,
    SYSTEM_COMMAND_VIDEO_FONT_MAKE_EMPTY_STRING,
    SYSTEM_COMMAND_VIDEO_RECTANGLE_DRAW_FONT_DATA
};
```

## Registering the kernel commands

```cpp
void isr80h_register_commands()
{
    isr80h_register_command(SYSTEM_COMMAND0_SUM, isr80h_command0_sum);
    isr80h_register_command(SYSTEM_COMMAND1_PRINT, isr80h_command1_print);
    isr80h_register_command(SYSTEM_COMMAND2_GETKEY, isr80h_command2_getkey);
    isr80h_register_command(SYSTEM_COMMAND3_PUTCHAR, isr80h_command3_putchar);
    isr80h_register_command(SYSTEM_COMMAND4_MALLOC, isr80h_command4_malloc);
    isr80h_register_command(SYSTEM_COMMAND5_FREE, isr80h_command5_free);
    isr80h_register_command(SYSTEM_COMMAND6_PROCESS_LOAD_START, isr80h_command6_process_load_start);
    isr80h_register_command(SYSTEM_COMMAND7_INVOKE_SYSTEM_COMMAND, isr80h_command7_invoke_system_command);
    isr80h_register_command(SYSTEM_COMMAND8_GET_PROGRAM_ARGUMENTS, isr80h_command8_get_program_arguments);
    isr80h_register_command(SYSTEM_COMMAND9_EXIT, isr80h_command9_exit);
}
```

- Responsible for registering kernel commands in the system.

```Cpp
typedef void* (*ISR80H_COMMAND)(struct interrupt_frame* frame);

void isr80h_register_command(int command_id, ISR80H_COMMAND command)
{
    // ...
    isr80h_command[command_id] = command;
}
```

## ISR80H handle command function

- This function is called by our isr80h_handler function shown previous:

```Cpp
void *isr80h_handle_command(int command, struct interrupt_frame* frame)
{
    // ...
    ISR80H_COMMAND command_func = isr80h_commands[command];

    result = command_func(frame);
    return result;
}
```
