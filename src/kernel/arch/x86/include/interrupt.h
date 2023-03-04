#pragma once
#include <types.h>

#define TOTAL_INTERRUPTS 512
#define KERNEL_CODE_SELECTOR 0x08
#define KERNEL_DATA_SELECTOR 0x10

typedef void(*INTERRUPT_CALLBACK_FUNCTION)();

/**
 * Interrupt Descriptor Table (32-bit).
 * The entries in the IDT are 8 bytes long and form a table like this:
 * | Address             | Content   |
 * | IDTR Offset + 0     | Entry 0   |
 * | IDTR Offset + 8     | Entry 1   |
 * | IDTR Offset + 16    | Entry 2   |
 * | ...                 | ...       |
 * | IDTR Offset + 2040  | Entry 256 |
 * Each entry in the table has a complex structure:
 **/
struct idt_desc
{
    uint16_t offset_1;       // Offset bits 0 - 15.
    uint16_t selector;       // A code segment selector in our GDT or LDT.
    uint8_t zero;            // Unused, set to zero.
    uint8_t type_attributes; // Descriptor type and attributes.
    uint16_t offset_2;       // Offset bits 16 - 31.
} __attribute__((packed));

struct idtr_desc
{
    uint16_t limit; // Size of descriptor table - 1.
    uint32_t base;  // Base address of the start of the interrupt descriptor table.
} __attribute__((packed));

/**
 * Init IDT after kernel is loaded.
 **/
void init_interrupt_descriptor_table();

void enable_interrupt();
void disable_interrupt();