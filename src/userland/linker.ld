ENTRY(kernel_entry_point)   /* Entry point of an application, 
                               that being specifically the first executable instruction in the output file.*/

OUTPUT_FORMAT(binary)   /* Output format of the executable.*/

SECTIONS 
{ /** You use 'SECTIONS' command to describe the memory layout of the output file.
    * The SECTIONS command tells the linker how to map input sections into output sections,
    * and how to place the output sections in memory.
    * The format of the SECTIONS command is:
    *   SECTIONS
    *   {
    *       section-command
    *       section-command
    *       ...
    *   }
    */

    . = 0x400000;   /** This command sets the value of the specical symbol '.', 
                      * which is the location counter.
                      * If you do not specify the address of an output section in some other way,
                      * the address is set from the current value of the location counter.
                      * The location counter is then incremented by the size of the output section.
                      * At the start of the 'SECTIONS' command, the location counter has the value '0'.
                      *
                      * This command say that the code should be loaded at address 0x400000,
                      * so, we will Load all user programs at address 0x400000 in its virtual memory.
                      */

    .text : ALIGN(0x1000)
    { /** We will set the address of the '.text' output section to the 
        * current value of the location counter.
        * And align the section on a 0x1000 byte boundary, 
        * this work because ALIGN() returns the current location counter 
        * aligned upward to the specified value.
        */

        *(.text) /** An input section description consists of a file name optionally
                   * followed by a list of section names in parentheses.
                   * 
                   * In here, we include all input '.text' sections.
                   * Here the '*' is a wildcard which matches any file name.
                   */
    }

	.data : ALIGN (0x1000)
    { /** GNU C++ will will normally arrange to put the addresses of 
        * global constructors and destructors into the .ctors and .dtors 
        * sections. We need to get the lists to call constructors and
        * destructors of global objects before and after using main().
        */
	   __CTOR_LIST__ = .;
	   *(.ctor*)
	   __CTOR_END__ = .;
	   __DTOR_LIST__ = .;
	   *(.dtor*)
	   __DTOR_END__ = .;
	   *(.data)
	}

    .asm : ALIGN(0x1000)
    {
        *(.asm)
    }

    .rodata : ALIGN(0x1000)
    {
        *(.rodata)
    }

    .data : ALIGN(0x1000)
    {
        *(.data)
    }

    .bss : ALIGN(0x1000)
    {
        *(COMMON)
        *(.bss)
    }
}