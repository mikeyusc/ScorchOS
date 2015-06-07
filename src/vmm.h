//
//  vmm.h
//  
//
//  Created by Mike Evans on 5/9/15.
//
//

#ifndef ____vmm__
#define ____vmm__

#include <stdint.h>
#include "pte.h"
#include "pde.h"

//! virtual address
typedef uint32_t virtual_addr;

//! i86 architecture defines 1024 entries per table--do not change
#define PAGES_PER_TABLE 1024
#define PAGES_PER_DIR	1024

//! page sizes are 4k
#define PAGE_SIZE 4096



//! page table
typedef struct {
    
    pt_entry m_entries[PAGES_PER_TABLE];
} ptable;

//! page directory
typedef struct {
    
    pd_entry m_entries[PAGES_PER_DIR];
} pdirectory;



//! initialize the memory manager
extern void vmmngr_initialize ();

//! allocates a page in physical memory
extern bool vmmngr_alloc_page (pt_entry*);

//! frees a page in physical memory
extern void vmmngr_free_page (pt_entry* e);

//! switch to a new page directory
extern bool vmmngr_switch_pdirectory (pdirectory*);

//! get current page directory
extern pdirectory* vmmngr_get_directory ();

//! flushes a cached translation lookaside buffer (TLB) entry
extern void vmmngr_flush_tlb_entry (virtual_addr addr);

//! clears a page table
extern void vmmngr_ptable_clear (ptable* p);

//! convert virtual address to page table index
extern uint32_t vmmngr_ptable_virt_to_index (virtual_addr addr);

//! get page entry from page table
extern pt_entry* vmmngr_ptable_lookup_entry (ptable* p,virtual_addr addr);

//! convert virtual address to page directory index
extern uint32_t vmmngr_pdirectory_virt_to_index (virtual_addr addr);

//! clears a page directory table
extern void vmmngr_pdirectory_clear (pdirectory* dir);

//! get directory entry from directory table
extern pd_entry* vmmngr_pdirectory_lookup_entry (pdirectory* p, virtual_addr addr);

//! identiy map region of memory
extern void vmmngr_identMap(uint32_t virt, uint32_t size);

//! map pages at virtual address
extern bool vmmngr_alloc_pages_at_virtual_address(uint32_t num_pages,uint32_t virt);

extern bool vmmngr_dealloc_pages_at_virtual_address(uint32_t num_pages,uint32_t virt);

/*
	New additions for chapter 24
 */

extern int         vmmngr_createPageTable      (pdirectory* dir, uint32_t virt, uint32_t flags);
extern void        vmmngr_mapPhysicalAddress   (pdirectory* dir, uint32_t virt, uint32_t phys, uint32_t flags);
extern void        vmmngr_unmapPageTable       (pdirectory* dir, uint32_t virt);
extern void        vmmngr_unmapPhysicalAddress (pdirectory* dir, uint32_t virt);
extern pdirectory* vmmngr_createAddressSpace   ();
extern void*       vmmngr_getPhysicalAddress   (pdirectory* dir, uint32_t virt);



#endif /* defined(____vmm__) */
