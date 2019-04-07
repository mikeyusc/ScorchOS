//
//  vmm.c
//
//
//  Created by Mike Evans on 5/9/15.
//
//
#include <system.h>

#include <string.h>
#include <vmm.h>
#include <pmm.h>

#define SATA_BASE 0xF0806000

//! page table represents 4mb address space
#define PTABLE_ADDR_SPACE_SIZE 0x400000

//! directory table represents 4gb address space
#define DTABLE_ADDR_SPACE_SIZE 0x100000000





//! current directory table
pdirectory*		_cur_directory=0;

//! current page directory base register
physical_addr	_cur_pdbr=0;

inline uint32_t vmmngr_ptable_virt_to_index (virtual_addr addr) {

    //! return index only if address doesnt exceed page table address space size
    return (addr >= PTABLE_ADDR_SPACE_SIZE) ? 0 : addr/PAGE_SIZE;
}

inline pt_entry* vmmngr_ptable_lookup_entry (ptable* p,virtual_addr addr) {

    if (p)
        return &p->m_entries[ vmmngr_ptable_virt_to_index (addr) ];
    return 0;
}

inline void vmmngr_ptable_clear (ptable* p) {

    if (p)
        memset ( p,0,sizeof (ptable) );
}

inline void vmmngr_pdirectory_clear (pdirectory* dir) {

    if (dir)
        memset ( dir,0,sizeof (pdirectory) );
}

inline uint32_t vmmngr_pdirectory_virt_to_index (virtual_addr addr) {

    //! return index only if address doesnt exceed 4gb (page directory address space size)
    return (addr >= DTABLE_ADDR_SPACE_SIZE) ? 0 : addr/PAGE_SIZE;
}

inline pd_entry* vmmngr_pdirectory_lookup_entry (pdirectory* p, virtual_addr addr) {

    if (p)
        return &p->m_entries[ vmmngr_ptable_virt_to_index (addr) ];
    return 0;
}

inline bool vmmngr_switch_pdirectory (pdirectory* dir) {

    if (!dir)
        return false;

    _cur_directory = dir;
    pmmngr_load_PDBR (_cur_pdbr);
    return true;
}

void vmmngr_flush_tlb_entry (virtual_addr addr) {

#ifdef _MSC_VER
    _asm {
        cli
        invlpg	addr
        sti
    }
#endif
}

pdirectory* vmmngr_get_directory () {

    return _cur_directory;
}

bool vmmngr_alloc_page (pt_entry* e) {

    //! allocate a free physical frame
    void* p = pmmngr_alloc_block ();
    if (!p)
        return false;

    //! map it to the page
    pt_entry_set_frame (e, (physical_addr)p);
    pt_entry_add_attrib (e, I86_PTE_PRESENT);

    return true;
}

void vmmngr_free_page (pt_entry* e) {

    void* p = (void*)pt_entry_pfn (*e);
    if (p)
        pmmngr_free_block (p);

    pt_entry_del_attrib (e, I86_PTE_PRESENT);
}

// chapter 21
#define PAGE_DIRECTORY_INDEX(x) (((x) >> 22) & 0x3ff)
#define PAGE_TABLE_INDEX(x) (((x) >> 12) & 0x3ff)
#define PAGE_GET_PHYSICAL_ADDRESS(x) (*x & ~0xfff)

void vmmngr_initialize () {

    //taken from chapter 21

	   //! allocate default page table
    ptable* table = (ptable*) pmmngr_alloc_block ();
    if (!table)
        return;

    //! allocates 3gb page table
    ptable* table2 = (ptable*) pmmngr_alloc_block ();
    if (!table2)
        return;

    ptable* table3 = (ptable*) pmmngr_alloc_block ();
    if (!table3)
        return;



    //! clear page table
    memset (table, 0, sizeof (ptable));
    memset (table2, 0, sizeof (ptable));
    //memset (table3, 0, sizeof (ptable));

//    //! IO Mem idenitity mapped
//    for (int i=0, frame=SATA_BASE, virt=SATA_BASE; i<1024; i++, frame+=4096, virt+=4096) {
//
//        //! create a new page
//        pt_entry page=0;
//        pt_entry_add_attrib (&page, I86_PTE_PRESENT | I86_PTE_WRITABLE);
//        pt_entry_set_frame (&page, frame);
//
//        //! ...and add it to the page table
//        table3->m_entries [PAGE_TABLE_INDEX (virt) ] = page;
//    }
//

    //! 1st 4mb are idenitity mapped
    for (int i=0, frame=0x0, virt=0x00000000; i<1024; i++, frame+=4096, virt+=4096) {

        //! create a new page
        pt_entry page=0;
        pt_entry_add_attrib (&page, I86_PTE_PRESENT);
        pt_entry_set_frame (&page, frame);

        //! ...and add it to the page table
        table2->m_entries [PAGE_TABLE_INDEX (virt) ] = page;
    }

    //! map 1mb to 3gb (where we are at)
    for (int i=0, frame=0x100000, virt=0xc0100000; i<1024; i++, frame+=4096, virt+=4096) {

        //! create a new page
        pt_entry page=0;
        pt_entry_add_attrib (&page, I86_PTE_PRESENT);
        pt_entry_set_frame (&page, frame);

        //! ...and add it to the page table
        table->m_entries [PAGE_TABLE_INDEX (virt) ] = page;
    }

    //! create default directory table
    pdirectory*   dir = (pdirectory*) pmmngr_alloc_blocks (3);
    if (!dir)
        return;

    //! clear directory table and set it as current
    memset (dir, 0, sizeof (pdirectory));

    //! get first entry in dir table and set it up to point to our table
    pd_entry* entry = &dir->m_entries [PAGE_DIRECTORY_INDEX (0xc0100000) ];
    pd_entry_add_attrib (entry, I86_PDE_PRESENT);
    pd_entry_add_attrib (entry, I86_PDE_WRITABLE);
    pd_entry_set_frame (entry, (physical_addr)table);

    pd_entry* entry2 = &dir->m_entries [PAGE_DIRECTORY_INDEX (0x00000000) ];
    pd_entry_add_attrib (entry2, I86_PDE_PRESENT);
    pd_entry_add_attrib (entry2, I86_PDE_WRITABLE);
    pd_entry_set_frame (entry2, (physical_addr)table2);

//    pd_entry* entry3 = &dir->m_entries [PAGE_DIRECTORY_INDEX (SATA_BASE) ];
//    pd_entry_add_attrib (entry3, I86_PDE_PRESENT);
//    pd_entry_add_attrib (entry3, I86_PDE_WRITABLE);
//    pd_entry_set_frame (entry3, (physical_addr)table3);

    //! store current PDBR
    _cur_pdbr = (physical_addr) &dir->m_entries;

    //! switch to our page directory
    vmmngr_switch_pdirectory (dir);

    //! enable paging
    pmmngr_paging_enable (true);
}

/*
	Additions for Chapter 24
 */

/**
 * Allocates new page table
 * \param dir Page directory
 * \param virt Virtual address
 * \param flags Page flags
 * \ret Status code
 */
int vmmngr_createPageTable (pdirectory* dir, uint32_t virt, uint32_t flags) {

    pd_entry* pagedir = dir->m_entries;
    if (pagedir [virt >> 22] == 0) {
        void* block = pmmngr_alloc_block();
        if (!block)
            return 0; /* Should call debugger */
        pagedir [virt >> 22] = ((uint32_t) block) | flags;
        memset ((uint32_t*) pagedir[virt >> 22], 0, 4096);

        /* map page table into directory */
        vmmngr_mapPhysicalAddress (dir, (uint32_t) block, (uint32_t) block, flags);
    }
    return 1; /* success */
}

/**
 * Map physical address to virtual
 * \param dir Page directory
 * \param virt Virtual address
 * \param phys Physical address
 * \param flags Page flags
 */
void vmmngr_mapPhysicalAddress (pdirectory* dir, uint32_t virt, uint32_t phys, uint32_t flags) {

    pd_entry* pagedir = dir->m_entries;
    if (pagedir [virt >> 22] == 0)
        vmmngr_createPageTable (dir, virt, flags);
    ((uint32_t*) (pagedir[virt >> 22] & ~0xfff))[virt << 10 >> 10 >> 12] = phys | flags;
}

/**
 * Unmap page table
 * \param dir Page directory
 * \param virt Virtual address
 */
void vmmngr_unmapPageTable (pdirectory* dir, uint32_t virt) {
    pd_entry* pagedir = dir->m_entries;
    if (pagedir [virt >> 22] != 0) {

        /* get mapped frame */
        void* frame = (void*) (pagedir [virt >> 22] & 0x7FFFF000);

        /* unmap frame */
        pmmngr_free_block (frame);
        pagedir [virt >> 22] = 0;
    }
}



bool vmmngr_alloc_pages_at_virtual_address(uint32_t num_pages,uint32_t virt)
{

    pdirectory * dir = _cur_directory;
    if (vmmngr_getPhysicalAddress(dir,virt)!=0)
    {
        return false;
    }

    void * blocks = pmmngr_alloc_blocks(num_pages);

    if(!blocks) return false;
    uint32_t start = virt;

    for(uint32_t i = 0;i<num_pages; i++, start+=4096, blocks+=4096)
    {
        vmmngr_mapPhysicalAddress(dir,start,blocks,I86_PTE_PRESENT | I86_PTE_WRITABLE);
    }


    return true;
}

bool vmmngr_dealloc_pages_at_virtual_address(uint32_t num_pages,uint32_t virt)
{
    pdirectory * dir = _cur_directory;
    if (vmmngr_getPhysicalAddress(dir,virt)!=0)
    {
        return false;
    }

    uint32_t start = virt;

    for(uint32_t i = 0;i<num_pages; i++, start+=4096)
    {
        //physical_addr * phys = vmmngr_getPhysicalAddress(dir,virt);


        vmmngr_unmapPhysicalAddress(dir,start);
    }

    return true;



}



void vmmngr_identMap(uint32_t virt, uint32_t size)
{
    pdirectory * dir = _cur_directory;
    uint32_t start = virt;

    uint32_t num_pages = (size/PAGE_SIZE) + ((size % PAGE_SIZE)>0? 1:0);

    for(uint32_t i = 0;i<num_pages; i++, start+=4096)
    {
        vmmngr_mapPhysicalAddress(dir,start,start,I86_PTE_PRESENT | I86_PTE_WRITABLE);
    }

}

/**
 * Unmap address
 * \param dir Page directory
 * \param virt Virtual address
 */
void vmmngr_unmapPhysicalAddress (pdirectory* dir, uint32_t virt) {
    /* note: we don't unallocate physical address here; callee does that */
    pd_entry* pagedir = dir->m_entries;
    if (pagedir [virt >> 22] != 0)
        vmmngr_unmapPageTable (dir, virt);
    //      ((uint32_t*) (pagedir[virt >> 22] & ~0xfff))[virt << 10 >> 10 >> 12] = 0;
}

/**
 * Create address space
 * \ret Page directory
 */
pdirectory* vmmngr_createAddressSpace () {
    pdirectory* dir = 0;

    /* allocate page directory */
    dir = (pdirectory*) pmmngr_alloc_block ();
    if (!dir)
        return 0;

    /* clear memory (marks all page tables as not present) */
    memset (dir, 0, sizeof (pdirectory));
    return dir;
}

/**
 * Get physical address from virtual
 * \param dir Page directory
 * \param virt Virtual address
 * \ret Physical address
 */
void* vmmngr_getPhysicalAddress (pdirectory* dir, uint32_t virt) {
    pd_entry* pagedir = dir->m_entries;
    if (pagedir [virt >> 22] == 0)
        return 0;
    return (void*) ((uint32_t*) (pagedir[virt >> 22] & ~0xfff))[virt << 10 >> 10 >> 12];
}


