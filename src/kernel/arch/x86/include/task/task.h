#pragma once
#include <types.h>

#define PROGRAM_VIRTUAL_ADDRESS 0x400000
#define USER_PROGRAM_STACK_SIZE 1024 * 16
#define PROGRAM_VIRTUAL_STACK_ADDRESS_START 0x3FF000
#define PROGRAM_VIRTUAL_STACK_ADDRESS_END (PROGRAM_VIRTUAL_STACK_ADDRESS_START - USER_PROGRAM_STACK_SIZE)

#define USER_DATA_SEGMENT 0x23
#define USER_CODE_SEGMENT 0x1b

struct registers
{ // structure represents the CPU registers.
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;

    uint32_t ip;
    uint32_t cs;
    uint32_t flags;
    uint32_t esp;
    uint32_t ss;
};
struct interrupt_frame;
struct paging_4GB_chunk;
struct process;
struct task
{
    struct paging_4GB_chunk *page_directory; // Page directory of the task.
    struct registers registers;              // Register state of the task when it is not running.
    struct task *next;                       // Next task in the task linked list.
    struct task *prev;                       // Previous task in the task linked list.
    struct process *proc;                    // Process of the task.
};

struct process;
struct task *make_new_task(struct process *proc);
void release_task(struct task *task);

struct task *get_current_task();
struct task *get_next_task();

extern void task_return(struct registers *regs);
extern void restore_general_purpose_registers(struct registers *regs);
extern void load_user_data_segment_registers();
extern void load_kernel_data_segment_registers();

int load_user_task_page();

void task_save_state(struct task *task, struct interrupt_frame *frame);
