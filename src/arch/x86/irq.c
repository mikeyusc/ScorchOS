/*
** Copyright 2008-11 ScorchOS Developers
** See doc/License.txt for terms of use.
**
** IRQs - Hardware Interrupt Handlers
*/

#include <system.h>

/// IRQ or Interrupt Request line, are 16 lines that are hardware interrupts
/// Controlled by the PIC, Since these are hardware, they are a bit different
/// Than software (we have to acknowledge the interrupt so the chip knows we
/// Got it. So they have their own functions to handle that.
extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();

void *irq_routines[16] =
    /// This is a jump vector with addresses to functions to handle the irq's
{
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
};

void irq_install_handler(int irq, void (*handler)(struct regs *r))
    /// This installs an IRQ handler that will be run when an IRQ is triggered
{
    irq_routines[irq] = handler;
}

void irq_uninstall_handler(int irq)
    /// This uninstalls an IRQ handler function.
{
    irq_routines[irq] = 0;
}

void irq_remap(void)
    /// BIOS maps the IRQ's in such a way they conflict with the CPU exceptions
    /// We need to remap the PIC so this doesn't happen.
{
    outportb(0x20, 0x11);
    outportb(0xA0, 0x11);
    outportb(0x21, 0x20);
    outportb(0xA1, 0x28);
    outportb(0x21, 0x04);
    outportb(0xA1, 0x02);
    outportb(0x21, 0x01);
    outportb(0xA1, 0x01);
    outportb(0x21, 0x0);
    outportb(0xA1, 0x0);
}

void irq_install()
    /// This installs the IRQ subsytem and allows us to use them
{
    irq_remap();

    // Master
    idt_set_gate(32, (unsigned)irq0, 0x08, 0x8E);
    idt_set_gate(33, (unsigned)irq1, 0x08, 0x8E);
    idt_set_gate(34, (unsigned)irq2, 0x08, 0x8E);
    idt_set_gate(35, (unsigned)irq3, 0x08, 0x8E);
    idt_set_gate(36, (unsigned)irq4, 0x08, 0x8E);
    idt_set_gate(37, (unsigned)irq5, 0x08, 0x8E);
    idt_set_gate(38, (unsigned)irq6, 0x08, 0x8E);
    idt_set_gate(39, (unsigned)irq7, 0x08, 0x8E);
    // Slave
    idt_set_gate(40, (unsigned)irq8, 0x08, 0x8E);
    idt_set_gate(41, (unsigned)irq9, 0x08, 0x8E);
    idt_set_gate(42, (unsigned)irq10, 0x08, 0x8E);
    idt_set_gate(43, (unsigned)irq11, 0x08, 0x8E);
    idt_set_gate(44, (unsigned)irq12, 0x08, 0x8E);
    idt_set_gate(45, (unsigned)irq13, 0x08, 0x8E);
    idt_set_gate(46, (unsigned)irq14, 0x08, 0x8E);
    idt_set_gate(47, (unsigned)irq15, 0x08, 0x8E);
}

void irq_handler(struct regs *r)
    /// Because IRQ's have to send an End of Interrupt the the PIC,
    /// We need a special function for then so we continue to receive them
    /// After the initial interrupt is issued
    /// This code handles the interrupt, then tells the PIC we finished.
{
    void (*handler)(struct regs *r);        // Void Pointer Function Call

    handler = irq_routines[r->int_no - 32]; // Figure out what IRQ was Called
    if (handler)                            // Does the IRQ have a handler?
    {
        handler(r);                         // Run it then.
    }

    if (r->int_no >= 40)                    // Was this call from the slave PIC?
    {
        outportb(0xA0, 0x20);               // Send an EOI to it
    }

    outportb(0x20, 0x20);                   // Send an EOI to the master PIC.
}
