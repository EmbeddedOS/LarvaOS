# 8259 Programmable Interrupt Controller (PIC)

8259 PIC is one the most importance chips making up the x86 architecture. Without it, the x86 architecture would not be an **interrupt driven architecture**. The function of the 8269A is to manage hardware interrupts and send them to appropriate system interrupt. This allows the system to respond to devices needs without loss of time (from polling the device, for instance).

It is important to note the **APIC** has replaced the 8269 in more modern systems, especially those with multiple cores/processors.

## 1. What does the 8259 PIC do?

The 8259 PIC controls the CPU's interrupt mechanism, by accepting several interrupt requests and feeding them to the processor in order.

For instance, when a keyboard registers a key-hit, it sends a pulse along its interrupt line (IRQ 1) to the PIC chip, which then translates the IRQ into a system interrupt, and sends a message to interrupt the CPU from whatever it is doing. Part of the kernel's job is to either handle these IRQs and perform the necessary procedures (poll the keyboard for the scan-code) or alert a user program to the interrupt (send a message to the keyboard driver).

## 2. Programming with the 8259 PIC?

Each chip (master and slave) has a command port and a data port (given in the table below). When no command is issued, the data port allows us to access the interrupt mask of the 8259 PIC.

|Chip-Purpose|I/O port|
|-|-|
|Master PIC - Command|0x0020|
|Master PIC - Data|0x0021|
|Slave PIC - Command|0x00A0|
|Slave PIC - Data|0x00A1|

* Each PIC vector offset must be divisible by 8, as the 8259A uses the lower 3 bits for the interrupt number of a particular interrupt (0..7).
* The only way to change the vector offsets used by the 8269 PIC is to re-initialize it, which explains why the code is "so long" anf plenty of things that have apparently no reasons to be here.
* if you plan to return to real mode protected mode (for any purpose), you really must restore the PIC to its former configuration.

### 2.1. Real Mode

|Chip|Interrupt numbers (IRQ)|Vector offset|Interrupt Numbers|
|-|-|-|-|
|Master PIC|0 to 7|0x08|0x08 to 0x0F|
|Slave PIC|8 to 15|0x70|0x70 to 0x77|

These default BIOS values suit real mode programming quite well; they do not conflict with any CPU exceptions like they do in protected mode.

### 2.2. Protected Mode

In protected mode, the IRQs 0 to 7 **conflict with the CPU exception which are reserved by Intel** up until `0x1F` (It was an IBM design mistake.). Consequently it is difficult to tell the difference between an IRQ or an software error. It is thus recommended to **change the PIC's offsets** so that IRQs use-non-reserved vectors. A common choice is to move them to the beginning of the available range (IRQs 0..0xF -> INT 0x20..0x2F). For that, we need to set the master PIC's offset to 0x20 and the slave's to 0x28.

## 3. Code Examples

### 3.1. Common Definitions

This is just a set of definitions common to the rest of this section.

```cpp
#define PIC1        0x20        /* IO base address for master PIC */
#define PIC2        0xA0        /* IO base address for slave PIC */
#define PIC1_COMMAND    PIC1
#define PIC1_DATA   (PIC1+1)
#define PIC2_COMMAND    PIC2
#define PIC2_DATA   (PIC2+1)
```

### 3.2. End of interrupt

Perhaps the most common command issued to the PIC chips is the `end of interrupt (EOI)` command (code 0x20). This is issued to the PIC chips at the end of IRQ-based interrupt routine. If the IRQ came from the Master PIC, it is sufficient to issue this command only to the Master PIC; However if the IRQ came from the Slave PIC, it is necessary to issue the command to both chips.

```cpp
#define PIC_EOI     0x20        /* End-of-interrupt command code */
 
void PIC_sendEOI(unsigned char irq)
{
    if(irq >= 8)
        outb(PIC2_COMMAND,PIC_EOI);
 
    outb(PIC1_COMMAND,PIC_EOI);
}
```

### 3.3. Initialization

When you enter protected mode (or even before hand, if you are not using GRUB) the first command you will need to give the two PICs is the initialize command (code 0x11). This command makes the pIC wait for 3 extra "initialization words" on the data port. These bytes give the PIC:

* Its vector offset. (ICW2)
* Tell it how it is wired to master/slaves. (ICW3)
* Gives additional information about the environment. (ICW4)

```cpp
/* reinitialize the PIC controllers, giving them specified vector offsets
   rather than 8h and 70h, as configured by default */
 
#define ICW1_ICW4   0x01        /* ICW4 (not) needed */
#define ICW1_SINGLE 0x02        /* Single (cascade) mode */
#define ICW1_INTERVAL4  0x04    /* Call address interval 4 (8) */
#define ICW1_LEVEL  0x08        /* Level triggered (edge) mode */
#define ICW1_INIT   0x10        /* Initialization - required! */
 
#define ICW4_8086   0x01        /* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO   0x02        /* Auto (normal) EOI */
#define ICW4_BUF_SLAVE  0x08    /* Buffered mode/slave */
#define ICW4_BUF_MASTER 0x0C    /* Buffered mode/master */
#define ICW4_SFNM   0x10        /* Special fully nested (not) */
 
/*
arguments:
    offset1 - vector offset for master PIC
        vectors on the master become offset1..offset1+7
    offset2 - same for slave PIC: offset2..offset2+7
*/
void PIC_remap(int offset1, int offset2)
{
    unsigned char a1, a2;
 
    a1 = inb(PIC1_DATA);                        // save masks
    a2 = inb(PIC2_DATA);
 
    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);  // starts the initialization sequence (in cascade mode)
    io_wait();
    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();
    outb(PIC1_DATA, offset1);                 // ICW2: Master PIC vector offset
    io_wait();
    outb(PIC2_DATA, offset2);                 // ICW2: Slave PIC vector offset
    io_wait();
    outb(PIC1_DATA, 4);                       // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
    io_wait();
    outb(PIC2_DATA, 2);                       // ICW3: tell Slave PIC its cascade identity (0000 0010)
    io_wait();
 
    outb(PIC1_DATA, ICW4_8086);
    io_wait();
    outb(PIC2_DATA, ICW4_8086);
    io_wait();
 
    outb(PIC1_DATA, a1);   // restore saved masks.
    outb(PIC2_DATA, a2);
}
```
