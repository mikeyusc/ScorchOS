//
//  uhci.c
//
//
//  Created by Mike Evans on 5/28/15.
//
//

#include <uhci.h>
#include <system.h>


uint16_t _base;


void init_uhci(uint16_t basePort)
{
    _base=basePort;

    UHCIStatusReg stat;
    stat.status_word= inw(_base+UHCI_USBSTS);

    puts(" USB STATUS: ");
    putx(stat.status_word);
    puts("\n");




}
