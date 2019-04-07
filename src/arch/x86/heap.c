#include <system.h>
#include <stdint.h>
#include <pmm.h>
#include <vmm.h>

#define KHEAP 0xC8000000

uint32_t heaptop;

bool init_heap()
{


    heaptop=KHEAP;


    return true;

}




int liballoc_lock()
{
    //TODO !

	//pthread_mutex_lock( &mutex );
	return 0;
}

int liballoc_unlock()
{
	//TODO !
    //pthread_mutex_unlock( &mutex );
	return 0;
}


void* liballoc_alloc( int pages )
{

	unsigned int size = pages * PAGE_SIZE;
    void  * outmem;

    bool valid= vmmngr_alloc_pages_at_virtual_address(pages,heaptop);
    if(valid)
    {
        outmem=(void*)heaptop;

        heaptop+=PAGE_SIZE*pages;

    }





	return outmem;
}

int liballoc_free( void* ptr, int pages )
{
    if(vmmngr_dealloc_pages_at_virtual_address(pages,ptr)) return 1;

    return 0;
}



