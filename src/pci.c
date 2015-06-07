//
//  pci.c
//  
//
//  Created by Mike Evans on 5/10/15.
//
//


#include <system.h>
#include "vmm.h"
#include "pci.h"
#include "rtl8139.h"
#include "ahci.h"
#include "uhci.h"



void checkBus(uint8_t bus);


uint16_t pciConfigReadWord (uint8_t bus, uint8_t slot,
                            uint8_t func, uint8_t offset)
{
    uint32_t address;
    uint32_t lbus  = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;
    uint16_t tmp = 0;
    
    /* create configuration address as per Figure 1 */
    address = (uint32_t)((lbus << 16) | (lslot << 11) |
                         (lfunc << 8) | (offset & 0xfc) | ((uint32_t)0x80000000));
    
    /* write out the address */
    outl (0xCF8, address);
    /* read in the data */
    /* (offset & 2) * 8) = 0 will choose the first word of the 32 bits register */
    tmp = (uint16_t)((inl (0xCFC) >> ((offset & 2) * 8)) & 0xffff);
    return (tmp);
}

uint32_t pciConfigReadDWord (uint8_t bus, uint8_t slot,
                            uint8_t func, uint8_t offset)
{
    uint32_t address;
    uint32_t lbus  = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;
    uint32_t tmp = 0;
    
    /* create configuration address as per Figure 1 */
    address = (uint32_t)((lbus << 16) | (lslot << 11) |
                         (lfunc << 8) | (offset & 0xfc) | ((uint32_t)0x80000000));
    
    /* write out the address */
    outl (0xCF8, address);
    /* read in the data */
    /* (offset & 2) * 8) = 0 will choose the first word of the 32 bits register */
    tmp = inl (0xCFC);
    return (tmp);
}

void pciEnableBusMaster(uint8_t bus, uint8_t slot,
                        uint8_t func,bool enable)
{
    pciConfigDoCommand(bus,slot,func,PCI_COMMAND_MASTER,enable);
    
}


void pciConfigWriteDWord(uint8_t bus, uint8_t slot,
                         uint8_t func, uint8_t offset,uint32_t inWord)
{
    uint32_t address;
    uint32_t lbus  = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;
    uint16_t tmp = 0;
    
    /* create configuration address as per Figure 1 */
    address = (uint32_t)((lbus << 16) | (lslot << 11) |
                         (lfunc << 8) | (offset & 0xfc) | ((uint32_t)0x80000000));
    
    /* write out the address */
    outl (0xCF8, address);
    outl (0xCFC,inWord);
    
    
}

void pciConfigDoCommand(uint8_t bus, uint8_t slot,
                         uint8_t func, uint16_t commandFlag, bool flag)
{
    uint32_t tmpValue=pciConfigReadDWord(bus,slot,func,0x04);
    if(flag)
    {
        tmpValue=tmpValue | commandFlag;
    }
    else
    {
        tmpValue = tmpValue & ~commandFlag;
    }
    
    pciConfigWriteDWord(bus,slot,func,0x04,tmpValue);
    
    
    
}



uint16_t getVendorID(uint8_t bus, uint8_t device, uint8_t function)
{
    uint16_t vendor;
    
    vendor = pciConfigReadWord(bus,device,function,0);
    
    return vendor;
    
}

uint16_t getDeviceID(uint8_t bus, uint8_t device, uint8_t function)
{
    uint16_t vendor;
    
    vendor = pciConfigReadWord(bus,device,function,2);
    
    return vendor;
    
}



uint8_t getHeaderType(uint8_t bus, uint8_t device, uint8_t function)
{
    return pciConfigReadWord(bus,device,function,0x0E) &0xFF ;
    
}

uint8_t getInterruptLine(uint8_t bus, uint8_t device, uint8_t function)
{
    return pciConfigReadWord(bus,device,function,0x3C) &0xFF ;
    
}

uint8_t getSecondaryBus(uint8_t bus, uint8_t device, uint8_t function)
{
    
    return pciConfigReadWord(bus,device,function,0x18) >>8 & 0xFF;
    
}

uint8_t getProgIF(uint8_t bus, uint8_t device, uint8_t function)
{
    return pciConfigReadWord(bus,device,function,0x08) >>8 & 0xFF;
}


uint8_t getBaseClass(uint8_t bus, uint8_t device, uint8_t function)
{
    return pciConfigReadWord(bus,device,function,0x0A) >>8 &0xFF ;
}

uint8_t getSubClass(uint8_t bus, uint8_t device, uint8_t function)
{
    return pciConfigReadWord(bus,device,function,0x0A) &0xFF ;
}



void checkFunction(uint8_t bus, uint8_t device, uint8_t function) {
    uint8_t baseClass;
    uint8_t subClass;
    uint8_t secondaryBus;
    
    baseClass = getBaseClass(bus, device, function);
    subClass = getSubClass(bus, device, function);
    if( (baseClass == 0x06) && (subClass == 0x04) ) {
        secondaryBus = getSecondaryBus(bus, device, function);
        checkBus(secondaryBus);
        //puts("SECONDARY BUS\n");
              
    }
    
    uint16_t vendorID;
    uint16_t deviceID = getDeviceID(bus,device,function);
    uint8_t headerType = getHeaderType(bus, device, function);
    uint32_t bar0 = pciConfigReadDWord(bus,device,function,0x10);
    uint32_t bar1 = pciConfigReadDWord(bus,device,function,0x14);
    uint32_t bar2 = pciConfigReadDWord(bus,device,function,0x18);
    uint32_t bar3 = pciConfigReadDWord(bus,device,function,0x1C);
    uint32_t bar4 = pciConfigReadDWord(bus,device,function,0x20);
    uint32_t bar5 = pciConfigReadDWord(bus,device,function,0x24);
    
    

    uint8_t interruptLine = getInterruptLine(bus,device,function);
    uint8_t progif = getProgIF(bus,device,function);
    
    
    
    vendorID = getVendorID(bus, device, function);
    
    if(vendorID!=0x15AD || deviceID!=0x07A0)
    {
    
        putx(vendorID);
        puts(":");
        putx(deviceID);
        puts(" ");
        putx(baseClass);
        puts(" ");
        putx(subClass);
        puts(" ");
        putx(headerType);
  
        puts(" ");
        putx(interruptLine);
        puts(" ");
        putx(progif);
        
        
        puts(" ");
        putx(bus);
        puts(":");
        putx(device);
        puts(":");
        putx(function);
        
        
        
        
        puts("\n");
    }
    
    
    if(deviceID==0x8139 && vendorID==0x10EC)
    {
        n8139device.vendorID=vendorID;
        n8139device.deviceID=deviceID;
        n8139device.bus=bus;
        n8139device.device=device;
        n8139device.function=function;
        n8139device.irq=interruptLine;
        n8139device.ioaddr=bar0 & 0xFFFF;
        
        //puts("Ethernet Controller Found: ");
        //putlx(bar0);
        
        
        //init_rtl8139();
        //puts("\n");
        
        
        
    }
    
    if(baseClass==1 && subClass==6)
    {
        pciConfigDoCommand(bus,device,function,PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER,true);

        
//        puts("SATA Controller Found: ");
//        
//        putlx(bar5);
//        puts("\n");
        
        bar5 = pciConfigReadDWord(bus,device,function,0x24);
        
        
        vmmngr_identMap(bar5,AHCI_MEM_SIZE);
        
        
        
        ahci_init(bar5);
        
        
        
        
    }
    
    if(baseClass==0xC && subClass == 0x03)
    {
        puts("USB Controller found: ");
        switch(progif)
        {
            case 0x00:
                puts("USB1/UHCI\n");
                puts("BASE Port: ");
                putlx(bar4);
                
                init_uhci(bar4 & ~1);
                
                puts("\n");
                
                
                break;
            case 0x10:
                puts("USB1/OHCI\n");
                break;
                
            case 0x20:
                puts("USB2\n");
                break;
            case 0x30:
                puts("USB3\n");
                break;
            default:
                puts("Unsupported\n");
                break;
                
                
        };
        
        
        
    }
    
    
    
    
    }


void checkDevice(uint8_t bus, uint8_t device) {
    
    
    
    
    uint8_t function = 0;
    uint16_t vendorID;
    
    vendorID = getVendorID(bus, device, function);
    
    
    
    if(vendorID == 0xFFFF) return;        // Device doesn't exist
    
        

    
    
    checkFunction(bus, device, function);
    uint8_t headerType = getHeaderType(bus, device, function);
    if( (headerType & 0x80) != 0) {
        /* It is a multi-function device, so check remaining functions */
        for(function = 1; function < 8; function++) {
            if(getVendorID(bus, device, function) != 0xFFFF) {
                checkFunction(bus, device, function);
            }
        }
    }
 
    
}


void checkBus(uint8_t bus) {
    uint8_t device;
    
    
    for(device = 0; device < 32; device++) {
        checkDevice(bus, device);
    }
}


void checkAllBuses(void) {
    uint8_t function,headerType;
    uint8_t bus;
    
    headerType = getHeaderType(0, 0, 0);
    if( (headerType & 0x80) == 0) {
        /* Single PCI host controller */
        checkBus(0);
    } else {
        /* Multiple PCI host controllers */
        for(function = 0; function < 8; function++) {
            if(getVendorID(0, 0, function) != 0xFFFF) break;
            bus = function;
            checkBus(bus);
        }
    }
}

void init_pci(void)
{
    
    checkAllBuses();
    
}


