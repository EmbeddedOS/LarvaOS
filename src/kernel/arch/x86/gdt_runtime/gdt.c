#include <gdt.h>
#include <panic.h>

/**
 * Segment descriptor is an 8-byte structure that define the following properties of a protected-mode segment:
 * + Base address (32 bits), which define where the segment begins in physical memory.
 * + Segment Limit (20 bits), which define the size of the segment.
 * + Various flags, which affect how the CPU interprets the segment, such as privilege level of code, read- or write-only.
 * Structure of the segment descriptor:
 *      |31					24|23		20|19		16|15		12|11		8|7					0|
 *      |Base address (24-31) |G |DB|  |A |Limit 16-19|P |DPL  |S | Type	 |Base address(16-23)|
 *      |			Base address ( Bit 0-15)		  |				Segment Limit (Bit 0-15)	 |
 *
 *  G -- Granularity.
 *  P -- Segment Present.
 *  S -- Descriptor type (0 = system; 1 = code or data).
 *  A -- Available for use by system software.
 *  DPL -- Descriptor privilege level.
 *  DB -- Default operation size (0 = 16-bit segment; 1 = 32-bit segment).
 *  Type -- Segment type.
 *  For code:
 *      + Code: 1 for code, since this is a code segment.
 *      + Conforming: 0, by not conforming it means code in a segment with a lower privilege may not call code in this segment,
 *                    this a key to memory protection.
 *      + Readable: 1, 1 if readable, 0 if execute-only. Readable allows us to read constants defined in the code.
 *      + Accessed: 0 This is often used for debugging and virtual memory techniques,
 *                  since the CPU sets the bit when it accesses the segment.
 *  For data:
 *      + Code: 0 for data
 *      + Expand down: 0. This allows the segment to expand down - TODO: explain this.
 *      + Writable: 1. This allows the data segment to be written to, otherwise it would be read only.
 *      + Accessed: 0 This is often used for debugging and virtual memory techniques,
 *                  since the CPU sets the bit when it accesses the segment.
 */

static void extract_structured_gdt_to_gdt(uint8_t *dst,
                                          struct structured_segment_descriptor structured_gdt)
{
    if ((structured_gdt.limit > 0x10000) && ((structured_gdt.limit & 0xFFF) != 0xFFF))
    { // Segment Limit (20 bits), which define the size of the segment.
        panic("Can not load GDT entry!");
    }

    dst[6] = 0x40;
    if (structured_gdt.limit > 0x10000)
    {
        structured_gdt.limit = structured_gdt.limit >> 12;
        dst[6] = 0xC0;
    }

    // Extract limit.
    dst[0] = structured_gdt.limit & 0xFF;
    dst[1] = (structured_gdt.limit >> 8) & 0xFF;
    dst[6] |= (structured_gdt.limit >> 16) & 0x0F;

    // Extract base.
    dst[2] = structured_gdt.base & 0xFF;
    dst[3] = (structured_gdt.base >> 8) & 0xFF;
    dst[4] = (structured_gdt.base >> 16) & 0xFF;
    dst[7] = (structured_gdt.base >> 24) & 0xFF;

    // Extract type.
    dst[5] = structured_gdt.type;
}

void extract_structured_gdt(struct segment_descriptor *gdt,
                            struct structured_segment_descriptor *structured_gdt,
                            int number_of_entry)
{
    for (int i = 0; i < number_of_entry; i++)
    {
        extract_structured_gdt_to_gdt((uint8_t *)&gdt[i], structured_gdt[i]);
    }
}