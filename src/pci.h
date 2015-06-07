//
//  pci.h
//  
//
//  Created by Mike Evans on 5/10/15.
//
//

#ifndef ____pci__
#define ____pci__

#include <stdint.h>



typedef struct pci_device_s
{
    uint16_t vendorID;
    uint16_t deviceID;
    uint8_t bus;
    uint8_t device;
    uint8_t function;
    
    uint16_t ioaddr;
    
    uint8_t classCode;
    uint8_t subClass;
    uint8_t progif;
    uint8_t revid;
    
    
    uint8_t irq;
    
     
    uint32_t bar0addr;
    uint32_t bar1addr;
    uint32_t bar2addr;
    uint32_t bar3addr;
    uint32_t bar4addr;
    uint32_t bar5addr;
    
    
    
} pci_device_t;

#define  PCI_COMMAND_IO                 0x0001   // Enable response in I/O space
#define  PCI_COMMAND_MEMORY             0x0002   // Enable response in Memory space
#define  PCI_COMMAND_MASTER             0x0004   // Enable bus mastering
#define  PCI_COMMAND_SPECIAL            0x0008   // Enable response to special cycles
#define  PCI_COMMAND_INVALIDATE         0x0010   // Use memory write and invalidate
#define  PCI_COMMAND_VGA_PALETTE        0x0020   // Enable palette snooping
#define  PCI_COMMAND_PARITY             0x0040   // Enable parity checking
#define  PCI_COMMAND_WAIT               0x0080   // Enable address/data stepping
#define  PCI_COMMAND_SERR               0x0100   // Enable SERR/
#define  PCI_COMMAND_FAST_BACK          0x0200   // Enable back-to-back writes



extern void init_pci(void);


extern void pciConfigDoCommand(uint8_t bus, uint8_t slot,
                               uint8_t func, uint16_t commandFlag, bool flag);

extern void pciEnableBusMaster(uint8_t bus, uint8_t slot,
                        uint8_t func,bool enable);




#endif /* defined(____pci__) */
