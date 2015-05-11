//
//  pci.c
//  
//
//  Created by Mike Evans on 5/10/15.
//
//


#include <system.h>

#include "pci.h"



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

uint16_t getVendorID(uint8_t bus, uint8_t device, uint8_t function)
{
    uint16_t vendor;
    
    vendor = pciConfigReadWord(bus,device,function,0);
    
    return vendor;
   
    
}

uint8_t getHeaderType(uint8_t bus, uint8_t device, uint8_t function)
{
    return pciConfigReadWord(bus,device,function,0x0E) &0xFF ;
    
}

int8_t getBaseClass(uint8_t bus, uint8_t device, uint8_t function)
{
    return pciConfigReadWord(bus,device,function,0x0A) >>8 &0xFF ;
}
int8_t getSubClass(uint8_t bus, uint8_t device, uint8_t function)
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
//        secondaryBus = getSecondaryBus(bus, device, function);
//        checkBus(secondaryBus);
              
    }
    
    uint16_t vendorID;
    
    vendorID = getVendorID(bus, device, function);
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


