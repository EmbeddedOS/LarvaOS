# Interrupt Service Routines

The x86 architecture is an interrupt driven system. External events trigger an interrupt -- the normal control flow is interrupted and an Interrupt Service Routine (ISR) is called.

Such events can be triggered by hardware or software. An example of a hardware interrupt is the keyboard: every time you press a key, the keyboard triggers IRQ1 (Interrupt Request 1), and the corresponding interrupt handler is called. Timers and disk request completion are other possible sources of hardware interrupts.

For the system to know which interrupt service routine to call when a certain interrupt occurs, offsets to the ISRs are stored in the [Interrupt Descriptor Table](interrupt_descriptor_table.md) when you're in Protected mode, or in the Interrupt Vector Table when you're in Real Mode.

An ISR is called directly by the CPU, and the protocol for calling an ISR differs from calling e.g. a C function.

Most importantly, an ISR has to end with the `iret` opcode (or iretq in long mode—yes, even when using intel syntax), whereas usual C functions end with `ret` or `retf`.

## 1. When the Handlers are Called?
