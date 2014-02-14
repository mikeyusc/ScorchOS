/*
** Copyright 2008-12 ScorchOS Developers
** See doc/License.txt for terms of use.
**
** Central Kernel Code For initilization
*/

#include <system.h>
#include <kb.h>

char username [32];
int prompt;
int main(void* mbd, unsigned int magic)
{

	multiboot_info_t *mbi;

	if ( magic != MULTIBOOT_BOOTLOADER_MAGIC )
   	/// Bootloader wasn't Multiboot...
    {
        puts("Muliboot header not found!\n");
        mbi = 0;
    }
    else
	/// Set MBI to address of multiboot info structure
	{
        mbi = (multiboot_info_t *) mbd;
    }
	gdt_install();
   	idt_install();
   	isrs_install();
   	irq_install();
   	init_video();
   	timer_install();
   	keyboard_install();
   	__asm__ __volatile__ ("sti");
	
	initShell(); 

   	puts("\n\t\t\tWelcome to ScorchOS 0.1.5!\n");
   	puts("\t\t\t(C)2008-12 ScorchOS Developers\n\n");

   	while(1) {
		runShell();
	}
}


