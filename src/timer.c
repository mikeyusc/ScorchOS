/*
** Copyright 2008-11 ScorchOS Developers
** See doc/License.txt for terms of use.
**
** Kernel Timekeeping Functions
*/

#include <system.h>

volatile int PIC_ticks = 0; // Volatile to avoid infinite loops in optimization.

void PIC_handler(struct regs *r)
/// Callback from ISR 0 
{ 
	PIC_ticks++;            // Increment Tick count
} 

void wait(int ticks)
/// Hold the system until specified time is reached
{
    unsigned long eticks;

    eticks = PIC_ticks + ticks;
    while(PIC_ticks < eticks);
}

void timer_install()
/// Sets up the system clock by installing the timer handler into IRQ0
{
    unsigned long hz = 1000;                                // 1 tick / mSec
    asm("cli");                                             // Stop interrupts
    unsigned long divisor = 1193180 / hz;                   // Find the divisor
    outportb(0x43, 0x36);              // Tell PIC we're setting it's frequency
    unsigned char l = (unsigned char)(divisor & 0xFF);      // Bitmap Low byte
    unsigned char h = (unsigned char)(divisor >> 8) & 0xFF; // Bitmap High byte
    outportb(0x40, l);                                      // Send Low Byte!
    outportb(0x40, h);                                      // Send High Byte!
    irq_install_handler(0, PIC_handler);  // Install Pointer to the ISRS Table
    asm("sti"); // Restore interrupts
}
