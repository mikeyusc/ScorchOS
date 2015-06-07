
//
//  rtl8139.h
//  
//
//  Created by Mike Evans on 5/15/15.
//
//

#ifndef ____rtl8139__
#define ____rtl8139__


#include <stdint.h>
#include "pci.h"


// === CONSTANTS ===
#define VENDOR_ID	0x10EC
#define DEVICE_ID	0x8139


extern pci_device_t n8139device;


enum eRTL8139_Regs
{
    // MAC Address
    MAC0, MAC1, MAC2,
    MAC3, MAC4, MAC5,
    
    // Multicast Registers
    MAR0 = 0x08, MAR1, MAR2, MAR3,
    MAR4, MAR5, MAR6, MAR7,
    
    // Transmit status of descriptors 0 - 3
    TSD0 = 0x10,	TSD1 = 0x14,
    TSD2 = 0x18,	TSD3 = 0x1C,
    // Transmit start addresses
    TSAD0 = 0x20,	TSAD1 = 0x24,
    TSAD2 = 0x28,	TSAD3 = 0x2C,
    
    RBSTART = 0x30,	//!< Recieve Buffer Start (DWord)
    // Early Recieve Byte Count
    ERBCR = 0x34,	// 16-bits
    // Early RX Status Register
    ERSR = 0x36,
    
    // -, -, -, RST, RE, TE, -, BUFE
    CMD 	= 0x37,
    
    CAPR	= 0x38,	// Current address of packet read
    CBA 	= 0x3A,	// Current Buffer Address - Total byte count in RX buffer
    
    IMR 	= 0x3C,	// Interrupt mask register
    ISR 	= 0x3E,	// Interrupt status register
    
    TCR 	= 0x40,	// Transmit Configuration Register
    RCR 	= 0x44,	// Recieve Configuration Register
    TCTR	= 0x48,	// 32-bit timer (count)
    MPC 	= 0x4C,	// Missed packet count (due to RX overflow)
    
    CR_9346 = 0x50,
    CONFIG0 = 0x51,
    CONFIG1	= 0x52,
    // 0x53 resvd
    TIMERINT = 0x54,	// Fires a timeout when TCTR equals this value
    
};

#define FLAG_ISR_SERR	0x8000	// System error
#define FLAG_ISR_TIMEO	0x4000	// Timer timeout (See TIMERINT)
#define FLAG_ISR_LENCHG	0x2000	// Cable length changed
#define FLAG_ISR_FOVW	0x0040	// Rx FIFO Underflow
#define FLAG_ISR_PUN	0x0020	// Packet Underrung
#define FLAG_ISR_RXOVW	0x0010	// Rx Buffer Overflow
#define FLAG_ISR_TER	0x0008	// Tx Error
#define FLAG_ISR_TOK	0x0004	// Tx OK
#define FLAG_ISR_RER	0x0002	// Rx Error
#define FLAG_ISR_ROK	0x0001	// Rx OK




void init_rtl8139(void);
void rtl8139_send(void* data,uint32_t length);



#endif /* defined(____rtl8139__) */
