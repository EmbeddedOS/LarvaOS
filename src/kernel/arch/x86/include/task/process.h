#pragma once
#include <types.h>
#include <fs/path_parser.h>

#define MAX_PROGRAM_ALLOCATIONS 1024
#define MAX_PROCESSES 20
struct process
{
    uint16_t id;

    char binary_file[FILESYSTEM_MAX_PATH_LENGTH];      // Filename of binary file.
    struct task *task;                                 // Main process task.
    void *memory_allocations[MAX_PROGRAM_ALLOCATIONS]; // The memory allocations (malloc) of the process.
    void *physical_pointer;                            // Physical memory pointer to the process memory.
    uint32_t size;                                     // The size of the data pointed by `physical_pointer`.
    void *stack_pointer;                               // Physical memory pointer to the stack memory.
};

int load_process(const char *filename, struct process **process);