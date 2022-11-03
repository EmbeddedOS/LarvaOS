# Interrupt Descriptor table

The **Interrupt Descriptor Table (IDT)** is a binary data structure specific to the IA-32 and x86-64 architectures. It is the Protected Mode and Long Mode counterpart to the Real Mode Interrupt Vector table(IVT) telling the where [Interrupt Service Routines](interrupt_service_routines.md) are located (one per interrupt vector). It is similar to the **Global Descriptor Table** in structure.

The IDT entries are called gates. It can contain Interrupt Gates, Task Gates and Trap Gates.

Before you implement the IDT, make sure you have a working GDT.

## 1. IDTR

The location of the **IDT** is kept in the IDTR (IDT register). This is loaded using the **LIDT** assembly instruction, whose argument is a pointer to an IDT Descriptor structure:

**IDT Descriptor(IDTR)**:
|79 (64-bit Mode)   |     |
|48 (32-bit Mode) 16|15  0|
|-------------------|-----|
|Offset             |Size |
|63 (64-bit Mode)   |     |
|31 (32-bit Mode) 16|15  0|

* **Size**: One less than the size of the IDT in bytes.
* **Offset**: The linear address of the Interrupt Descriptor Table (not the physical address, paging applies).

Note that the amount of data loaded by **LIDT** differs in 32-bit and 64-bit modes, **Offset** is 4 bytes long in 32-bit mode and 8 bytes long in 64-bit mode.

This is similar to the GDT, except:

* The first entry (at zero offset) is used in the IDT.
* There are 256 interrupt vectors (0..255), so the IDT should have 256 entries, each entry corresponding to a specific interrupt vector.

* Although the IDT can contain more than 256 entries, they are ignored.
* Although the IDT can contain less than 256 entries, any entries that are not present (due to this or other reasons) will generate a **General Protection Fault** when an attempt to access them is made.

## 2. Structure on IA-32

**Table**
On 32-bit processors, the entries in the IDT are 8 bytes long and form a table like this:

Interrupt Descriptor Table (32-bit)

|Address|Content|
|-------|-------|
|IDTR Offset + 0|0|
|IDTR Offset + 8|1|
|IDTR Offset + 16|2|
|...|...|
|IDTR Offset + 2040|Entry 256|

The corresponding entry for a given Interrupt Vector is pointed to in memory by scaling the vector by 8 and adding it to the value in the Offset field of the IDTR.

**Gate Descriptor**
Each entry in the table has a complex structure:
Gate Descriptor (32-bit):

```text
|63                               48|47|46  45|44|43      40|39       32|
|Offset                             |P |DPL   |0 |Gate type |Reserved   |
|31                               16|  |1    0|  |3        0|           |
|31                               16|15                                0|
|Segment Selector                   |15                                0|
|15                                0|15                                 |
```

* Offset: A 32-bit value, split in two parts. It represents the address of the entry point of the Interrupt Service Routine.
* Selector: A Segment Selector with multiple fields which must point to a valid code segment in your GDT.
* Gate Type: A 4-bit value which defines the type of gate this Interrupt Descriptor represents. There are five valid type values:
  * 0b0101 or 0x5: Task Gate, note that in this case, the Offset value is unused and should be set to zero.
  * 0b0110 or 0x6: 16-bit Interrupt Gate
  * 0b0111 or 0x7: 16-bit Trap Gate
  * 0b1110 or 0xE: 32-bit Interrupt Gate
  * 0b1111 or 0xF: 32-bit Trap Gate
* DPL: A 2-bit value which defines the CPU Privilege Levels which are allowed to access this interrupt via the INT instruction. Hardware interrupts ignore this mechanism.
* P: Present bit. Must be set (1) for the descriptor to be valid.

**Example Code**
C Struct:

```c
struct InterruptDescriptor32 {
   uint16_t offset_1;        // offset bits 0..15
   uint16_t selector;        // a code segment selector in GDT or LDT
   uint8_t  zero;            // unused, set to 0
   uint8_t  type_attributes; // gate type, dpl, and p fields
   uint16_t offset_2;        // offset bits 16..31
} __attribute__((packed));
```

Example type_attributes values that people are likely to use (assuming DPL is 0):

* 32-bit Interrupt Gate: 0x8E (p=1, dpl=0b00, type=0b1110 => type_attributes=0b1000_1110=0x8E)
* 32-bit Trap Gate: 0x8F (p=1, dpl=0b00, type=0b1111 => type_attributes=1000_1111b=0x8F)
8 Task Gate: 0x85 (p=1, dpl=0b00, type=0b0101 => type_attributes=0b1000_0101=0x85)
