//
//  arp.c
//  
//
//  Created by Mike Evans on 5/20/15.
//
//

#include "arp.h"

uint8_t packet[4096];
uint16_t arp_point;



void sp()
{
    arp_point=0;
    
    
}

void bp(uint8_t byte)
{
    packet[arp_point++]=byte;
    
}

void * ep(uint16_t * packetsize)
{
    *packetsize = arp_point;
    
    return packet;
    
}