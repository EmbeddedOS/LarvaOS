#pragma once
#include <types.h>

struct segment_descriptor
{ // Segment descriptor structure.
    uint16_t limit0_15;
    uint16_t base0_15;
    uint8_t base_16_23;
    uint8_t access;
    uint8_t limit16_19 : 4;
    uint8_t other : 4;
    uint8_t base24_31;
} __attribute__((packed));

struct structured_segment_descriptor
{
    uint32_t base;
    uint32_t limit;
    uint8_t type;
};

extern void gdt_load_runtime(struct segment_descriptor *gdt, size_t size);
void extract_structured_gdt(struct segment_descriptor *gdt,
                            struct structured_segment_descriptor *structured_gdt,
                            int number_of_entry);