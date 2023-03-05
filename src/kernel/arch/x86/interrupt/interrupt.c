#include <interrupt.h>
#include <string.h>
#include <video.h>
#include <io.h>

// Assembly functions.
extern void load_interrupt_descriptor_table(void *ptr);

extern void enable_interrupts();

extern void disable_interrupts();

struct idt_desc idt_descriptors[TOTAL_INTERRUPTS];
struct idtr_desc idt_register;

void idt_divide_by_zero()
{ /* Handle IRQ 0 interrupt number,
   * This is not good enough,
   * interrupt handler should be returned by `iret` command.
   * FIXME: call me with interrupt wrapper.
   */
    write("Divide by Zero.", 14);
}

extern void no_interrupt();

void no_interrupt_handler()
{
    outb(0x20, 0x20);
}

void init_pic(void)
{
    /* Initialization of ICW1 */
    outb(0x20, 0x11);
    outb(0xA0, 0x11);

    /* Initialization of ICW2 */
    outb(0x21, 0x20); /* start vector = 32 */
    outb(0xA1, 0x70); /* start vector = 96 */

    /* Initialization of ICW3 */
    outb(0x21, 0x04);
    outb(0xA1, 0x02);

    /* Initialization of ICW4 */
    outb(0x21, 0x01);
    outb(0xA1, 0x01);

    /* mask interrupts */
    outb(0x21, 0x0);
    outb(0xA1, 0x0);
}

void set_interrupt_handler(int pos, void *address)
{
    struct idt_desc *desc = &idt_descriptors[pos];

    desc->offset_1 = (uint32_t)address & 0x0000ffff; // Lower part of interrupt function's offset address.
    desc->selector = KERNEL_CODE_SELECTOR;
    desc->zero = 0x00;
    desc->type_attributes = 0xEE;             // 32-bit Interrupt Gate.
    desc->offset_2 = (uint32_t)address >> 16; // Higher part of interrupt function's offset address.
}

void init_interrupt_descriptor_table()
{
    size_t size_of_descriptor_table = sizeof(idt_descriptors);

    memset(idt_descriptors, 0, size_of_descriptor_table);

    idt_register.limit = size_of_descriptor_table - 1; // One less than the size of the IDT in bytes.
    idt_register.base = (uint32_t)idt_descriptors;     // The linear address of the Interrupt Descriptor Table (not the physical address, paging applies).

    for (int i = 0; i < TOTAL_INTERRUPTS; i++)
    {
        set_interrupt_handler(i, &no_interrupt);
    }

    set_interrupt_handler(DIVIDE_BY_ZERO_INTERRUPT_NUMBER, &idt_divide_by_zero);
    set_interrupt_handler(SYSTEM_CALL_INTERRUPT_NUMBER, &syscall_wrapper);

    load_interrupt_descriptor_table((void *)&idt_register);
}

void enable_interrupt()
{
    enable_interrupts();
}

void disable_interrupt()
{
    disable_interrupts();
}