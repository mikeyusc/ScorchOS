/* Copyright 2008-11 ScorchOS Developers  */
/* See doc/License.txt for terms of use. */
/* Link.ld Linker script for the kernel ensure everything is in the right place */

ENTRY (loader)

SECTIONS{
    . = 0x00100000;

    .text :{
        *(.text)
    }

    .rodata ALIGN (0x1000) : {
        *(.rodata)
    }

    .data ALIGN (0x1000) : {
        *(.data)
    }

    .bss : {
        sbss = .;
        *(COMMON)
        *(.bss)
        ebss = .;
	endkernel = .;
    }
}


