/*
** Copyright 2008-11 ScorchOS Developers
** See doc/License.txt for terms of use.
**
** IDT - Interrupt Descriptor Table
*/

#include <system.h>
#include <string.h>

extern void idt_load();     // From 'start.asm', Loads the IDT to the CPU

struct idt_entry
    /// IDT table entry so the CPU knows how to handle a specific interrupt
{
    unsigned short base_lo;
    unsigned short sel;
    unsigned char always0;
    unsigned char flags;
    unsigned short base_hi;
} __attribute__((packed));

struct idt_ptr
    /// Pointer to the Interrupt Vector Table to give to the CPU
{
    unsigned short limit;
    unsigned int base;
} __attribute__((packed));

/***
Create a static table of 256 interrupts, this will be used as the vector table
That the CPU uses to handle the ISR's, also the CPU takes the pointer in a very
Specific format, so we need an instance of that too.
***/
struct idt_entry idt[256];
struct idt_ptr idtp;

void idt_set_gate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags)
    /// Set an interrupt's entry in the table
{
    /* The interrupt routine's base address */
    idt[num].base_lo = (base & 0xFFFF);
    idt[num].base_hi = (base >> 16) & 0xFFFF;

    /* The segment or 'selector' that this IDT entry will use
    *  is set here, along with any access flags */
    idt[num].sel = sel;
    idt[num].always0 = 0;
    idt[num].flags = flags;
}

void idt_install()
    /// Create and install the IDT
{
    idtp.limit = (sizeof (struct idt_entry) * 256) - 1; // Set up the pointer
    idtp.base = (unsigned int)&idt;                     // Base of pointer...
    memset(&idt, 0, sizeof(struct idt_entry) * 256);    // Clear IDT to all 0's
    idt_load();                                         // Load it to the CPU.
}

