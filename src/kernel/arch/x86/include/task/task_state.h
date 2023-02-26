#pragma once
#include <types.h>

struct task_state_segment
{
    uint16_t previous_task, __previous_task_reserved; // Contains the `Segment Selector`
                                                      // for the `TSS` of the previous task.
    uint32_t esp0;                                    // Kernel stack pointer.
    uint16_t ss0, __ss0_reserved;                     // Kernel stack segment.
    uint32_t esp1;
    uint16_t ss1, __ss1_reserved;
    uint32_t esp2;
    uint16_t ss2, __ss2_reserved;
    uint32_t cr3;
    uint32_t eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
    uint16_t es, __es_reserved;
    uint16_t cs, __cs_reserved;
    uint16_t ss, __ss_reserved;
    uint16_t ds, __ds_reserved;
    uint16_t fs, __fs_reserved;
    uint16_t gs, __gs_reserved;
    uint16_t ldt_selector, __ldt_sel_unused;
    uint16_t debug_flag, io_map; //  I/O Map Base Address Field.
} __attribute__((packed));

extern void tss_load(int tss_segment);