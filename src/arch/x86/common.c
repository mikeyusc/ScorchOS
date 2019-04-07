/*
** Copyright 2008-12 ScorchOS Developers
** See doc/License.txt for terms of use.
**
** Commonly Used Functions that don't fit anywhere else...
*/

#include <system.h>
#include <stdint.h>

unsigned short *memsetw(unsigned short *dest, unsigned short val, size_t count)
{
    	unsigned short *temp = (unsigned short *)dest;
    	for( ; count != 0; count--) *temp++ = val;
    	return dest;
}

unsigned char inportb (unsigned short _port)
{
    	unsigned char rv;
    	__asm__ __volatile__ ("inb %1, %0" : "=a" (rv) : "dN" (_port));
    	return rv;
}

void outportb (unsigned short _port, unsigned char _data)
{
	__asm__ __volatile__ ("outb %1, %0" : : "dN" (_port), "a" (_data));
}

uint8_t inb (uint16_t port)
{
    /* See [IA32-v2a] "IN". */
    uint8_t data;
    asm volatile ("inb %w1, %b0" : "=a" (data) : "Nd" (port));
    return data;
}

void outb (uint16_t port, uint8_t data)
{
    /* See [IA32-v2b] "OUT". */
    asm volatile ("outb %b0, %w1" : : "a" (data), "Nd" (port));
}


uint16_t
inw (uint16_t port)
{
    uint16_t data;
    /* See [IA32-v2a] "IN". */
    asm volatile ("inw %w1, %w0" : "=a" (data) : "Nd" (port));
    return data;
}

void outw (uint16_t port, uint16_t data)
{
    /* See [IA32-v2b] "OUT". */
    asm volatile ("outw %w0, %w1" : : "a" (data), "Nd" (port));
}


uint32_t
inl (uint16_t port)
{
    /* See [IA32-v2a] "IN". */
    uint32_t data;
    asm volatile ("inl %w1, %0" : "=a" (data) : "Nd" (port));
    return data;
}

void
outl (uint16_t port, uint32_t data)
{
    /* See [IA32-v2b] "OUT". */
    asm volatile ("outl %0, %w1" : : "a" (data), "Nd" (port));
}



