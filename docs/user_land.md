# Understanding User land

- User land is a term used to describe when the processor is in a limited privileged state.
- User land is what OS processes run in.
- Kernel land is the state that the kernel runs. It is a high privileged state.

## User land term

- Describes a limited processor state.
- Processes generally run in user land.
- User land is safe because if something goes wrong the kernel is able to intervene.
- User land is when the processor is in ring 3.
- IMPORTANT: User land is not a special place where processes run, its simply a term to describe the processor when its in a privilege limited state.

## Kernel land term

- Kernel land is when the processor is in its maximum privileged state.
- Whilst in kernel land any area in memory can be changed.
- Any CPU instruction can be run.
- Theirs also a high risk of damage to the system if things go wrong.
- Kernel land is when the processor is in a privileged protection ring such as ring 0.

## USer land restrictions

- Access to certain locations in memory can be restricted for user land processes.
- Access to certain CPU instructions are restricted from user land.
- Using paging the kernel can ensure all processes cannot access each others memory. User land code is unable to override this because its running in an unprivileged state. The instructions for switching pages disabled.
- Attempting to run privileged instructions whilst in user land will cause a protection fault. The protection fault exception interrupt handler will then be responsible for solving the problem.

## Getting to user land

1. Setup user code and data segments.
2. Setup a TSS (Task Switch Segment).
3. Pretend we are returning from an interrupt pushing appropriate flags, and data to the stack before executing an `iret` instruction to change the processors privilege state.

## Setup the user segment registers

## What is a TSS (task switch segment)

- The task switch segment is a way for the processor to get back to kernel land when we have a system interrupt.
- The task switch segment explains things such as where the kernel stack is located.
- Upon receiving interrupt when the processor is in user land state, the processor will switch to the kernel code and data segments. It will then restore the stack pointer located in the TSS (Task Switch Segment) before then invoking the kernel interrupt handler.

## Preventing we are returning from an interrupt

- We should set our segment registers to the user data segment that we created in previous steps. This is likely `0x23`. `ds`, `es`, `fs`, and `gs` registers should be changed but not the stack segment.
- Next we save our stack pointer in the `EAX` register as we are about to modify the stack.
- Now we push our user data segment to the stack `0x23`.
- Now we push our stack pointer we saved in EAX earlier.
- Next we push our current flags to the stack but not before we bitwise OR the bit that re-enables interrupts. This is important as our interrupts are cleared at this moment in time and we only want to re-enable them when we `iret`.
- Now we push the user code segment which should be `0x1B`.
- Finally we push the address of the function we want to run in user land.
- The last step is to call an `iret` which should force the processor into a user land unprivileged state.

## Getting back to user land when in a kernel interrupt

- When an interrupt is invoked whilst the processor is in the user land state the processor will push the same registers that we pushed to get to the user land in the first place. This way getting back to user land is very easy you just `iret` at the end of your kernel interrupt routine, causing the kernel to go back to the user program and just after the user programs interrupt instruction.

- In a multi-tasking system user land register will need to be salvaged when entering kernel land, this is important so we can switch to the next process task if wanted too. When ever we want to switch back to the old task we just swap the old registers of the task back to the real CPU registers again and then finally we drop the processor back into user land the task will then continue executing as if nothing happened.
