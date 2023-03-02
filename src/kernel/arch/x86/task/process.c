#include <task/process.h>
#include <task/task.h>
#include <memory/kheap.h>
#include <memory/paging.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <fs/file.h>
#include <video.h>

struct process *current_process = NULL;

struct process *processes[MAX_PROCESSES];

struct process *get_current_process()
{
    return current_process;
}

static void process_init(struct process *proc)
{
    memset(proc, 0, sizeof(struct process));
}

static struct process *get_process_by_id(int id)
{
    if (id < 0 || id >= MAX_PROCESSES)
    {
        return NULL;
    }

    return processes[id];
}

static int process_load_binary(const char *filename,
                               struct process *process)
{ // Load the binary file.
    int res = 0;
    int fd = fopen(filename, "r");
    if (fd <= 0)
    {
        res = -EIO;
        goto out;
    }

    struct file_stat stat;
    res = fstat(fd, &stat);
    if (res < 0)
    {
        goto out;
    }

    void *program_data_ptr = kzalloc(stat.filesize);
    if (program_data_ptr == NULL)
    {
        res = -ENOMEM;
        goto out;
    }

    if (fread(fd, program_data_ptr, stat.filesize, 1) < 0)
    {
        res = -EIO;
        goto out;
    }

    process->physical_pointer = program_data_ptr;
    process->size = stat.filesize;
out:
    return res;
}

static int process_load_data(const char *filename,
                             struct process *process)
{
    int res = 0;
    res = process_load_binary(filename, process);
out:
    return res;
}

static int process_map_binary(struct process *process)
{
    int res = 0;
    res = paging_map_virtual_memory(process->task->page_directory->directory_entry, (void *)PROGRAM_VIRTUAL_ADDRESS,
                                    process->physical_pointer,
                                    paging_align_address(process->physical_pointer + process->size),
                                    PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL | PAGING_IS_WRITEABLE);
out:
    return res;
}

static int process_map_virtual_memory(struct process *process)
{
    int res = 0;
    res = process_map_binary(process);
out:
    return res;
}

int process_load_for_slot(const char *filename,
                          struct process **process,
                          int slot)
{
    int res = 0;
    struct task *task = NULL;
    struct process *tmp_process = NULL;
    void *program_stack_ptr = NULL;

    if (get_process_by_id(slot))
    {
        res = -ESRCH;
        goto out;
    }

    tmp_process = kzalloc(sizeof(struct process));
    if (tmp_process == NULL)
    {
        res = -ENOMEM;
        goto out;
    }

    process_init(tmp_process);
    res = process_load_data(filename, tmp_process);
    if (res < 0)
    {
        goto out;
    }

    program_stack_ptr = kzalloc(USER_PROGRAM_STACK_SIZE);
    if (program_stack_ptr == NULL)
    {
        res = -ENOMEM;
        goto out;
    }

    // Init process properties.
    strncpy(tmp_process->binary_file, filename, sizeof(tmp_process->binary_file));
    tmp_process->stack_pointer = program_stack_ptr;
    tmp_process->id = slot;

    // Create a new task for the process.
    task = make_new_task(tmp_process);
    if (IS_ERR(task))
    {
        res = PTR_ERR(task);
        goto out;
    }

    tmp_process->task = task;

    res = process_map_virtual_memory(tmp_process);
    if (res < 0)
    {
        goto out;
    }

    processes[slot] = tmp_process;
    *process = tmp_process;

out:
    if (res < 0)
    {
        if (tmp_process != NULL && tmp_process->task)
        {
            release_task(tmp_process->task);
        }

        kfree(tmp_process);
    }

    return res;
}

static int process_get_free_slot()
{
    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        if (processes[i] == NULL)
        {
            return i;
        }
    }

    return -ENOMEM;
}

int load_process(const char *filename, struct process **process)
{
    int res = 0;
    int slot = process_get_free_slot();

    if (slot < 0)
    {
        res = slot;
        goto out;
    }


    res = process_load_for_slot(filename, process, slot);

out:
    return res;
}