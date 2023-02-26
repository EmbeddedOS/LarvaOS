#include <task/task.h>
#include <string.h>
#include <memory/paging.h>
#include <errno.h>
#include <memory/kheap.h>
#include <task/process.h>

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