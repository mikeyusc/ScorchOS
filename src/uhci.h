//
//  uhci.h
//  
//
//  Created by Mike Evans on 5/28/15.
//
//

#ifndef ____uhci__
#define ____uhci__

#include <stdint.h>


#define UHCI_USBCMD     0x00
#define UHCI_USBSTS     0x02
#define UHCI_USBINTR    0x04
#define UHCI_FRNUM      0x06
#define UHCI_FRBASEADD  0x08
#define UHCI_SOFMOD     0x0C
#define UHCI_PORTSC1    0x10
#define UCHI_PORTSC2    0x12


typedef union tagUHCICommandReg
{
    struct
    {
        
        uint16_t RS:1; // Run/Stop
        uint16_t HCReset:1; // Host Controller Reset
        uint16_t GReset:1; // Global Reset
        uint16_t EGSM:1; // Enter Global Suspend Mode
        uint16_t FGR:1; // Force Global Resume
        uint16_t SWDBG:1; // Software Debug
        uint16_t CF:1; // Configure Flag
        uint16_t MAXP:1; // MAX Packet
        uint16_t resv:8;
        
        
    };
    uint16_t command_word;
    
} UHCICommandReg;


typedef union tagUHCIStatusReg
{
    struct
    {
        
        
        uint16_t USBInt:1;
        uint16_t USBErrorInt:1;
        uint16_t ResumeDetect:1;
        uint16_t HSystemError:1;
        uint16_t HCProcessError:1;
        uint16_t HCHalted:1;
        uint16_t resv:10;
        
        
        
    };
    
    uint16_t status_word;

    
} UHCIStatusReg;

typedef union tagUHCIINTREnableReg
{
    struct
    {
        uint16_t TimeoutCRCEnable:1;
        uint16_t ResumeInterruptEnable:1;
        uint16_t InterruptOnComplete:1;
        uint16_t ShortPacketInterruptEnable:1;
        uint16_t resv:12;
        
    };
    
    uint16_t intenable_word;
    
} UHCIIntrEnableReg;








extern void init_uhci(uint16_t basePort);


#endif /* defined(____uhci__) */
