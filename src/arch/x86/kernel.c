/*
** Copyright 2008-12 ScorchOS Developers
** See doc/License.txt for terms of use.
**
** Central Kernel Code For initilization
*/

#include <system.h>
#include <kb.h>
#include <pmm.h>
#include <vmm.h>
#include <pci.h>
#include <flpydsk.h>
#include <fat12.h>
#include <rtl8139.h>









extern unsigned int endkernel;
extern unsigned int startkernel;
unsigned int sizekernel;

void cmd_read () {

    //! get pathname
    char path[]="test.txt";
    //DebugPrintf ("\n\rex: \"file.txt\", \"a:\\file.txt\", \"a:\\folder\\file.txt\"\n\rFilename> ");
    //get_cmd (path,32);
    puts(path);


    //! open file
    FILE file = volOpenFile (path);

    //! test for invalid file
    if (file.flags == FS_INVALID) {

        puts("\n\rUnable to open file");
        return;
    }

    //! cant display directories
    if (( file.flags & FS_DIRECTORY ) == FS_DIRECTORY) {

        puts("\n\rUnable to display contents of directory.");
        return;
    }



    //! top line
    puts ("\n\n\r-------[");
    puts(file.name);
    puts("]-------\n\r");

    //! display file contents
    while (file.eof != 1) {

        //! read cluster
        unsigned char buf[512];
        volReadFile ( &file, buf, 512);

        //! display file
        for (int i=0; i<512; i++)
            putch (buf[i]);

//        //! wait for input to continue if not EOF
//        if (file.eof != 1) {
//            puts ("\n\r------[Press a key to continue]------");
//            getch ();
//            DebugPrintf ("\r"); //clear last line
//        }
    }

    //! done :)
    puts("\n\n\r--------[EOF]--------");
}

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





    //! set drive 0 as current drive
    //flpydsk_set_working_drive (0);



    //! install floppy disk to IR 38, uses IRQ 6
    //flpydsk_install (6);

	//initShell();

    char tempbuf[64];


    sizekernel = (unsigned int)&endkernel - (unsigned int) &startkernel;
    puts("KERNEL SIZE : ");

    putd(sizekernel);



    puts("\nBIOS MEMORY INFO: ");
    unsigned int mem_kb = mbi->mem_lower + mbi->mem_upper;
    putd(mem_kb);
    puts("KB\n");

    pmmngr_init ((size_t) mem_kb, 0xC0100000 + sizekernel);



    if(mbi&&MULTIBOOT_INFO_MEM_MAP)
    {

        int region_count=mbi->mmap_length/sizeof(multiboot_memory_map_t);
        multiboot_memory_map_t * mmap_item = (multiboot_memory_map_t *) mbi->mmap_addr;

        for(int i=0;i<region_count;++i)
        {

            unsigned int lownib = (mmap_item[i].addr & 0xFFFFFFFF);
            unsigned int highnib=(mmap_item[i].addr>>32) & 0xFFFFFFFF;
            unsigned int startLo;
            unsigned int sizeLo;

            startLo=lownib;

            lownib = (mmap_item[i].len  & 0xFFFFFFFF);
            highnib=(mmap_item[i].len >>32) & 0xFFFFFFFF;
            sizeLo=lownib;

            if (mmap_item[i].type==1)
            {

                pmmngr_init_region (startLo, sizeLo);

            }
        }

    }

    pmmngr_deinit_region (0x100000, sizekernel);
    pmmngr_deinit_region (0x0, 0x10000);


    vmmngr_initialize();

    puts("\nTotal Blocks: ");
    putd(pmmngr_get_block_count());
    puts("\nFree Blocks: ");
    putd(pmmngr_get_free_block_count());
    puts("\nUsed Blocks: ");
    putd(pmmngr_get_use_block_count());
    puts("\n");


    init_heap();


    uint32_t * testvar = malloc(sizeof(uint32_t));

    *testvar=0x12345678;



    __asm__ __volatile__ ("sti");


    puts("\nFree Blocks: ");
    putd(pmmngr_get_free_block_count());
    puts("\nUsed Blocks: ");
    putd(pmmngr_get_use_block_count());
    puts("\n");


    init_pci();




    //fsysFatInitialize ();





   //	puts("\n\t\t\tWelcome to ScorchOS 0.1.5!\n");
   //	puts("\t\t\t(C)2008-12 ScorchOS Developers\n\n");


   	while(1) {
		runShell();
	}
}


