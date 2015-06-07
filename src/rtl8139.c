//
//  rtl8139.c
//  
//
//  Created by Mike Evans on 5/15/15.
//
//

#include "rtl8139.h"
#include <system.h>
#include "pci.h"
#include <string.h>
#include "arp.h"
#include "inet.h"




pci_device_t n8139device;

uint8_t macAddress[6];
uint16_t bufferPtr;



#define RX_BUF 0x8000
#define TX_BUF 0x10000

#define RX_BUF_SIZE 0x2000





void test_arp()
{
    sp();
    
    for(int i = 0;i<6;i++)
    {
        bp(0xFF);
    }
    for(int i = 0;i<6;i++)
    {
        bp(macAddress[i]);
        
    }
    bp(0x08);
    bp(0x06);
    
    bp(0x00);
    bp(0x01);
    
    bp(0x08);
    bp(0x00);
    
    bp(0x06);
    bp(0x04);
    
    bp(0x00);
    bp(0x01);
    
    for(int i = 0;i<6;i++)
    {
        bp(macAddress[i]);
        
    }
    
    bp(0x00);
    bp(0x00);
    bp(0x00);
    bp(0x00);
    
    for(int i = 0;i<6;i++)
    {
        bp(0x00);
    }
    
    bp(0xC0);
    bp(0xA8);
    bp(0x0A);
    bp(0x01);
    
    for(int i = 0;i<18;i++)
    {
        bp(0x00);
    }
    
    uint16_t packsize = 0;
    void * outbuf = ep(&packsize);
    
    puts("ARP Output Buffer has size: ");
    putd(packsize);
    
    puts("\n");
    
    rtl8139_send(outbuf,packsize);
    
    
    
    
}


void recv_packet(uint16_t* read_ofs)
{
    uint8_t  *rxbuf;
    uint8_t * destmac;
    uint8_t * srcmac;
    uint16_t * ethertype;
    
    
    
    rxbuf=(uint8_t*)RX_BUF;

    uint16_t	len;
    uint16_t status;
    status=*(uint16_t*)&rxbuf[*read_ofs];
    len =*(uint16_t*)&rxbuf[*read_ofs+2];
    
    uint8_t * pkt = &rxbuf[*read_ofs+4];
    
    
    
    
    if(len>3000)
    {
        puts("  LEN TOO BIG!! ");
        wait(50000);
        
        return;
        
    }
  
    
    // Packet Processing goes here!!
    // TODO: USE THE PACKET!!
    destmac=pkt;
    srcmac=pkt+6;
    ethertype = pkt+12;
    
//    for(int i =0;i<6;i++)
//    {
//        putx(destmac[i]);
//        puts(":");
//        
//    }
//    puts(" ");
//    
//    for(int i =0;i<6;i++)
//    {
//        putx(srcmac[i]);
//        puts(":");
//        
//    }
//    puts(" ");
    uint16_t etype = NTOHS(*ethertype);
  
    
//    switch (etype) {
//        case 0x0800:
//            puts(" IP ");
//            break;
//        case 0x0806:
//            puts(" ARP ");
//            break;
//        case 0x86DD:
//            puts(" IPV6 ");
//            break;
//            
//            
//        default:
//            putx(etype);
//            break;
//    }
//    puts("\n");
//    
//    
    
    
    
    
    
    
    
    
    
    *read_ofs+=len+4;
    *read_ofs=(*read_ofs + 3) & ~3; //Align
   
    
    
    
}


void rtl_irq_handler(struct regs *r)
{
//    puts("NETWORK IRQ\n");
    uint16_t base;
    uint16_t	status;
    uint16_t    imrs;
    uint16_t read_ofs, end_ofs;
    uint16_t  packet_count;
    
    
    
    

    
    base = n8139device.ioaddr &= ~1;
    
    status = inw(base + ISR);
    imrs = inw(base + IMR);
//    
//    puts("NET STATUS:");
//    putx(status);
//    puts("\n");
    
    if( status & FLAG_ISR_TOK )
    {
        
        
        outw(base + ISR, FLAG_ISR_TOK);
    }
    
    // Recieve OK, inform read
    if( status & FLAG_ISR_ROK )
    {
        
        packet_count=0;
        
        
        
        end_ofs = inw(base + CBA);
        read_ofs=bufferPtr;
        //uint16_t readfromcard = inw(base+CAPR);
        
        //readfromcard += 0x10;

        
        
        if(read_ofs>end_ofs)
        {
            // We're behind in the ring.. need to catch up!
            
            
            while (read_ofs<RX_BUF_SIZE)
            {
                packet_count++;
                
                recv_packet(&read_ofs);
                
            }
            
            read_ofs-=RX_BUF_SIZE;
            
        }
        
        while(read_ofs<end_ofs)
        {
            packet_count++;
            
            recv_packet(&read_ofs);
            
            
            
        }
        
        if(read_ofs!=end_ofs)
        {
            puts("\nREAD != END\n");
            
        }
        
        bufferPtr=read_ofs;
        
        outw(base + CAPR, read_ofs-0x10);
        outw(base + ISR, FLAG_ISR_ROK);
        
     
        return;
        
            
      
        
    }
    

    
    
    
    // Transmit error, ... oops
    if( status & FLAG_ISR_TER )
    {
        puts("Tx Error, dunno what to do");
        outw(base + ISR, FLAG_ISR_TER);
    }
    
    
    // Recieve error, ... oops
    if( status & FLAG_ISR_RER )
    {
        puts("Rx Error, dunno what to do");
        outw(base + ISR, FLAG_ISR_RER);
    }
    
    // Packet Underrun/Link Change
    if( status & FLAG_ISR_PUN )
    {
        // Set when CAPR is written but Rx is empty, OR when the link status changes
        puts("ISR[PUN] set... hmmm");
        outw(base + ISR, FLAG_ISR_PUN);
    }
    
    // Rx Overflow
    if( status & FLAG_ISR_RXOVW )
    {
        puts("Rx Overflow... oh fsck\nOFS:");
        putx(read_ofs);
        
        outw(base + ISR, FLAG_ISR_RXOVW);
    }
    
    // Rx FIFO Overflow
    if( status & FLAG_ISR_FOVW )
    {
        puts("Rx FIFO Overflow... huh?");
        outw(base + ISR, FLAG_ISR_FOVW);
    }

    

    
}

void rtl8139_send(void* data,uint32_t length)
{
    int td;
    
    
    uint32_t status;
    uint16_t base;
    base = n8139device.ioaddr &= ~1;
    
    
    if(length>1500) return; // Greater than MTU
    td=0;
    
    
    outl(base+TSAD0+td*4,TX_BUF+(0x800*td));
    
    memcpy(TX_BUF+(0x800*td),data,length);
    
    
    
    
    
    // Start
    status = 0;
    status |= length & 0x1FFF;	// 0-12: Length
    status |= 0 << 13;	// 13: OWN bit
    status |= (0 & 0x3F) << 16;	// 16-21: Early TX threshold (zero atm, TODO: check)
    //LOG("status = 0x%08x", status);
    outl(base + TSD0 + td*4, status);

    
    
}

void init_rtl8139(void)
{
    uint8_t * rxbuf;
    
    rxbuf=RX_BUF;
    bufferPtr=0;
    
    
    
    puts("NETWORK INIT\n");
    
    puts("NETWORK IRQ: ");
    putd(n8139device.irq);
    puts("\n");
    
    
    irq_install_handler (n8139device.irq , &rtl_irq_handler);
    uint16_t base;
    base = n8139device.ioaddr &= ~1;
    
    pciEnableBusMaster(n8139device.bus,n8139device.device,n8139device.function,true);
    
    
    
    outb(base+CONFIG1,0x0);
    outb(base+CMD,0x10);    
    while((inb(base+CMD) & 0x10) !=0) {}
    
    outw(base + IMR, 0xE07F);
    
    puts("NETWORK CONTINUE\n");
    
    outl(base+RBSTART,rxbuf);
    
    outl(base + CBA, 0);
    outl(base + CAPR, 0);
    
    outl(base+TSAD0,TX_BUF);
    outl(base+TSAD1,TX_BUF+(0x800*1));
    outl(base+TSAD2,TX_BUF+(0x800*2));
    outl(base+TSAD3,TX_BUF+(0x800*3));
    
    
    
 
    outl(base + RCR, (6<<13)|(6<<8)|0x80|0x1F);	// All valid + runt packets
    
    outw(base + IMR, 0xE07F);
    
    outb(base+CMD,0x0C);
    puts("NETWORK DONE\n");
    
 
    macAddress[0]=inb(base);
    macAddress[1]=inb(base+1);
    macAddress[2]=inb(base+2);
    macAddress[3]=inb(base+3);
    macAddress[4]=inb(base+4);
    macAddress[5]=inb(base+5);
    

    
    
    
    test_arp();
    
    
    rtl_irq_handler(0);
    
    
    
    
    
    
    
    
}

