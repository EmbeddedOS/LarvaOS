# Interrupts

The **Interrupts** are signals from a device, such as a keyboard or a hard drive, to the CPU, telling it to immediately **STOP** whatever it is currently doing and do something else.

For example, a keyboard controller can send an interrupt when a character key was pressed. Then the OS can display the character on screen immediately, even if the CPU was doing something completely unrelated before, and return to what it was afterwards.

When a specific interrupt arises, the CPU looks up an entry for that specific interrupt from a table provided by the OS. In x86 protected mode the table is called the [Interrupt Descriptor Table (IDT)](interrupt_descriptor_table.md) and can have up to **256** entries, but the name of this table and the maximum number of entries it can have can differ based on the CPU architecture.

After the CPU finds the entry for the interrupt, it jumps to the code the entry points to. This code that is run in response to the interrupt is known as [Interrupt Service Routine (ISR or an Interrupt Handler)](interrupt_service_routines.md).

## 1. Type of interrupts

There are generally **three** classes of interrupts on most platform:

* [Exceptions](exceptions.md):
* **Interrupt Request (IRQ)** or **Hardware Interrupt**:
* **Software Interrupt**:
