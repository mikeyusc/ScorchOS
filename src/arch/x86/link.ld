/* Copyright 2008-11 ScorchOS Developers  */
/* See doc/License.txt for terms of use. */
/* Link.ld Linker script for the kernel ensure everything is in the right place */

ENTRY (loader)

SECTIONS{
    . = 0xC0100000;
	_startkernel = .;

    .text  :AT(ADDR(.text) - 0xC0000000) {
        *(.text)
	*(.rodata*)
    }

    .data ALIGN (0x1000) : AT(ADDR(.data) - 0xC0000000){
        *(.data)
    }

    .bss : AT(ADDR(.bss) - 0xC0000000) {
        sbss = .;
        *(COMMON)
        *(.bss)
        ebss = .;
	_endkernel = .;
	
    }

 	
}


