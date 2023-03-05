#include <task/task.h>
#include <string.h>
#include <memory/paging.h>
#include <errno.h>
#include <memory/kheap.h>
#include <task/process.h>
#include <panic.h>
#include <interrupt.h>

struct task *current_task = NULL;

// Task linked list.
struct task *tail_task = NULL;
struct task *head_task = NULL;

struct task *get_current_task()
{
    return current_task;
}

struct task *get_next_task()
{
    if (current_task->next == NULL)
    {
        return head_task;
    }

    return current_task->next;
}

static void remove_task_from_linked_list(struct task *task)
{
    if (task == NULL)
    {
        return;
    }

    if (task->prev)
    {
        task->prev->next = task->next;
    }

    if (task == head_task)
    {
        head_task = task->next;
    }

    if (task == tail_task)
    {
        tail_task = task->prev;
    }

    if (task == current_task)
    {
        current_task = get_next_task();
    }
}

void release_task(struct task *task)
{
    if (task == NULL)
    {
        return;
    }

    release_4GB_virtual_memory_address_space(task->page_directory);
    remove_task_from_linked_list(task);
    kfree(task);
}

static int task_init(struct task *task, struct process *proc)
{
    int res = 0;
    memset(task, 0, sizeof(struct task));

    // Map the entire 4GB address space to its self.
    task->page_directory = make_new_4GB_virtual_memory_address_space(PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);
    if (task->page_directory == NULL)
    {
        res = -EIO;
        goto out;
    }

    task->registers.ip = PROGRAM_VIRTUAL_ADDRESS;
    task->registers.ss = USER_DATA_SEGMENT;
    task->registers.cs = USER_CODE_SEGMENT;
    task->registers.esp = PROGRAM_VIRTUAL_STACK_ADDRESS_START;

    task->proc = proc;
out:
    return res;
}

struct task *make_new_task(struct process *proc)
{
    int res = 0;
    struct task *task = kzalloc(sizeof(struct task));
    if (task == NULL)
    {
        res = -ENOMEM;
        goto out;
    }

    res = task_init(task, proc);
    if (res != 0)
    {
        goto out;
    }

    if (head_task == NULL)
    { // If the task linked list is not initializing.
        head_task = task;
        tail_task = task;
        current_task = task;
        goto out;
    }

    tail_task->next = task;
    task->prev = tail_task;
    tail_task = task;

out:
    if (res < 0)
    {
        release_task(task);
        return ERR_PTR(res);
    }

    return task;
}

int task_switch(struct task *task)
{
    int res = 0;
    current_task = task;
    // Change the page directories of the process to point to the new task.
    switch_to_page(task->page_directory);

out:
    return res;
}

int load_user_task_page()
{ // Takes us out of the kernel page,
  // page directory and loads us into the task page directory.
  // Use this function when we want to change from kernel page to task,
  // Do not use it when switch task by task.
    int res = 0;

    load_user_data_segment_registers();
    task_switch(current_task);

out:
    return res;
}

void run_first_task()
{
    if (current_task == NULL)
    {
        arc_panic("No current task exist!");
    }

    task_switch(head_task);
    task_return(&head_task->registers);
}

void task_save_state(struct task *task, struct interrupt_frame *frame)
{
    task->registers.ip = frame->ip;
    task->registers.cs = frame->cs;
    task->registers.flags = frame->flags;
    task->registers.esp = frame->esp;
    task->registers.ss = frame->ss;
    task->registers.eax = frame->eax;
    task->registers.ebp = frame->ebp;
    task->registers.ebx = frame->ebx;
    task->registers.ecx = frame->ecx;
    task->registers.edi = frame->edi;
    task->registers.edx = frame->edx;
    task->registers.esi = frame->esi;
}
