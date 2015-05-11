/*
** Copyright 2008-12 ScorchOS Developers
** See doc/License.txt for terms of use.
** System Header File 
*/

#ifndef __SYSTEM_H
#define __SYSTEM_H

#include <mboot.h>
#include <command.h>
#include <stdint.h>

typedef int size_t;

/* This defines what the stack looks like after an ISR was running */
struct regs
{
    unsigned int gs, fs, es, ds;
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
    unsigned int int_no, err_code;
    unsigned int eip, cs, eflags, useresp, ss;    
};

/* COMMON.C */
extern unsigned short *memsetw(unsigned short *dest, unsigned short val, size_t count);
extern unsigned char inportb (unsigned short _port);
extern void outportb (unsigned short _port, unsigned char _data);
extern uint32_t inl (uint16_t port);
extern void outl (uint16_t port, uint32_t data);


/* SCRN.C */
extern void init_video(void);
extern void puts(char *text);
extern void move_csr(void);
extern void putch(char c);
extern void cls();
extern void shell_csr_fix();
extern void putd(int n);
extern void itoa(unsigned i,unsigned base,char* buf);
extern void pad (char * instr,int padnum, char padchar);
extern void putx(int n);

/* GDT.C */
extern void gdt_set_gate(int num, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran);
extern void gdt_install();

/* IDT.C */
extern void idt_set_gate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags);
extern void idt_install();

/* ISRS.C */
extern void isrs_install();

/* IRQ.C */
extern void irq_install_handler(int irq, void (*handler)(struct regs *r));
extern void irq_uninstall_handler(int irq);
extern void irq_install();

/* TIMER.C */
extern void timer_wait(int ticks);
extern void timer_install();

/* KB.C */
extern void keyboard_install();

/* SHELL.C */ 
typedef struct
{
	char command[64];	//command string
	char description[256];	//brief description
	void *address;		//the function it associates with
} commandEntry;
extern int atoi(const char *nptr);
extern int addCommand(char *command, char *description, void *address);
extern void initShell();			/* Populates the command list. 			*/
extern void runShell();				/* Runs one command on the shell. 		*/
extern void clearCommands();		/* Clear all entry's on the jump table. */
//the commands are in commands.h

#endif
