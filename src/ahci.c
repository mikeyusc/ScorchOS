//
//  ahci.c
//  
//
//  Created by Mike Evans on 5/25/15.
//
//

#include <system.h>
#include <string.h>

#include "ahci.h"
#include "vmm.h"



HBA_MEM * mabar;

uint8_t hdbuf[8192];

uint32_t portbase[32];




#define	SATA_SIG_ATA	0x00000101	// SATA drive
#define	SATA_SIG_ATAPI	0xEB140101	// SATAPI drive
#define	SATA_SIG_SEMB	0xC33C0101	// Enclosure management bridge
#define	SATA_SIG_PM	0x96690101	// Port multiplier

#define AHCI_DEV_NULL 0
#define AHCI_DEV_SATA 1
#define AHCI_DEV_SATAPI 4
#define AHCI_DEV_SEMB 2
#define AHCI_DEV_PM 3

#define HBA_PORT_DET_PRESENT 3
#define HBA_PORT_IPM_ACTIVE 1

#define HBA_PxCMD_CR            (1 << 15) /* CR - Command list Running */
#define HBA_PxCMD_FR            (1 << 14) /* FR - FIS receive Running */
#define HBA_PxCMD_FRE           (1 <<  4) /* FRE - FIS Receive Enable */
#define HBA_PxCMD_SUD           (1 <<  1) /* SUD - Spin-Up Device */
#define HBA_PxCMD_ST            (1 <<  0) /* ST - Start (command processing) */

#define HBA_PxIS_TFES   (1 << 30)       /* TFES - Task File Error Status */

#define ATA_DEV_BUSY 0x80
#define ATA_DEV_DRQ 0x08


// Check device type
static int check_type(HBA_PORT *port)
{
    DWORD ssts = port->ssts;
    
    BYTE ipm = (ssts >> 8) & 0x0F;
    BYTE det = ssts & 0x0F;
    
    if (det != HBA_PORT_DET_PRESENT)	// Check drive status
        return AHCI_DEV_NULL;
    if (ipm != HBA_PORT_IPM_ACTIVE)
        return AHCI_DEV_NULL;
    
    switch (port->sig)
    {
        case SATA_SIG_ATAPI:
            return AHCI_DEV_SATAPI;
        case SATA_SIG_SEMB:
            return AHCI_DEV_SEMB;
        case SATA_SIG_PM:
            return AHCI_DEV_PM;
        default:
            return AHCI_DEV_SATA;
    }
}





// Find a free command list slot
int find_cmdslot(HBA_PORT *m_port)
{
    // If not set in SACT and CI, the slot is free
    DWORD slots = (m_port->sact | m_port->ci);
    int num_of_slots= (mabar->cap & 0x0f00)>>8 ; // Bit 8-12
    //puts("NUMSLOTS: "); putd(num_of_slots); puts(" ");
    
    
    for (int i=0; i<num_of_slots; i++)
    {
        if ((slots&1) == 0)
            return i;
        slots >>= 1;
    }
    puts("Cannot find free command list entry\n");
    return -1;
}


// Start command engine
void start_cmd(HBA_PORT *port)
{
    
    
    // Wait until CR (bit15) is cleared
    while (port->cmd & HBA_PxCMD_CR);
    
    // Set FRE (bit4) and ST (bit0)
    port->cmd |= HBA_PxCMD_FRE;
    port->cmd |= HBA_PxCMD_ST;
    
    
}

// Stop command engine
void stop_cmd(HBA_PORT *port)
{
    
    
    // Clear ST (bit0)
    port->cmd &= ~HBA_PxCMD_ST;
    
    // Wait until FR (bit14), CR (bit15) are cleared
    while(1)
    {
        if (port->cmd & HBA_PxCMD_FR)
        {
            
            continue;
        }
        
        if (port->cmd & HBA_PxCMD_CR)
        {
            
            
           continue;
        }
        
        break;
    }
    
    // Clear FRE (bit4)
    port->cmd &= ~HBA_PxCMD_FRE;
    
    
}


//FIS_REG_H2D fis;
//memset(&fis, 0, sizeof(FIS_REG_H2D));
//fis->fis_type = FIS_TYPE_REG_H2D;
//fis->command = ATA_CMD_IDENTIFY;	// 0xEC
//fis->device = 0;			// Master device
//fis->c = 1;

#define	AHCI_BASE	0x2000
#define AHCI_BUFFER 0x8000

#define AHCI_PORT_SIZE 12288

void port_rebase(HBA_PORT *port, int portno)
{
    
    mabar->ghc=(DWORD)(1<<31);
    mabar->ghc=(DWORD)(1<<0);
    mabar->ghc=(DWORD)(1<<31);
    mabar->ghc=(DWORD)(1<<1);
    
    
    if(portbase[portno]==0)
    {
        portbase[portno]=AHCI_BASE + (AHCI_PORT_SIZE*portno);
//        if(!vmmngr_alloc_pages_at_virtual_address(3,portbase[portno]))
//        {
//            puts("CAN'T ALLOCATE AHCI PORT MEMORY FOR PORT ");
//            putd(portno);
//            puts(" BASE: ");
//            putlx(portbase[portno]);
//            portbase[portno]=0;
//            
//            return;
//        }
        
    }
    //puts("ALLOCED REBASE\n");
    
    
    
    
    stop_cmd(port);	// Stop command engine
    
    // Command list offset: 1K*portno
    // Command list entry size = 32
    // Command list entry maxim count = 32
    // Command list maxim size = 32*32 = 1K per port
    
    
    port->clb = portbase[portno] & 0xFFFFFFFF;
    port->clbu = 0;
    
    memset((void*)(port->clb), 0, 1024);
    
    //puts("CLB SET\n");
    
    // FIS offset: 32K+256*portno
    // FIS entry size = 256 bytes per port
    port->fb = portbase[portno] + (1<<10) & 0xFFFFFFFF;
    port->fbu = 0;
    memset((void*)(port->fb), 0, 256);
    
    //puts("FB SET\n");
    
    // Command table offset: 40K + 8K*portno
    // Command table size = 256*32 = 8K per port
    HBA_CMD_HEADER *cmdheader = (HBA_CMD_HEADER*)(port->clb);
    for (int i=0; i<32; i++)
    {
        cmdheader[i].prdtl = 8;	// 8 prdt entries per command table
        // 256 bytes per command table, 64+16+48+16*8
        // Command table offset: 40K + 8K*portno + cmdheader_index*256
        cmdheader[i].ctba = portbase[portno] + (1<<10) + ((i+1)<<8) & 0xFFFFFFFF;
        cmdheader[i].ctbau = 0;
        memset((void*)cmdheader[i].ctba, 0, 256);
    }
    //puts("HEADER SET\n");
    
    
    start_cmd(port);	// Start command engine
    
    //puts("LEAVING REBASE");
    
}

bool identify(HBA_PORT *port)
{
    //puts("IDENTIFY\n");
    
    port->is = (DWORD)-1;		// Clear pending interrupt bits
    int spin = 0; // Spin lock timeout counter
    int slot = find_cmdslot(port);
    memset(AHCI_BUFFER,0,2048);
    
    
    
    //puts("SLOT: "); putd(slot); puts("\n");
    if (slot == -1)
        return false;
    
    HBA_CMD_HEADER *cmdheader = (HBA_CMD_HEADER*)port->clb;
    cmdheader += slot;
    cmdheader->cfl = sizeof(FIS_REG_H2D)/sizeof(DWORD);	// Command FIS size
    cmdheader->w = 0;		// Read from device
    cmdheader->prdtl =  2;	// PRDT entries count
    
    HBA_CMD_TBL *cmdtbl = (HBA_CMD_TBL*)(cmdheader->ctba);
    memset(cmdtbl, 0, sizeof(HBA_CMD_TBL) +
           (cmdheader->prdtl-1)*sizeof(HBA_PRDT_ENTRY));
    
    cmdtbl->prdt_entry[0].dba = (DWORD)AHCI_BUFFER;
    cmdtbl->prdt_entry[0].dbc = 512-1;	// 512 bytes per sector
    cmdtbl->prdt_entry[0].i = 1;
    
    FIS_REG_H2D *cmdfis = (FIS_REG_H2D*)(&cmdtbl->cfis);
    cmdfis->fis_type= FIS_TYPE_REG_H2D;
    cmdfis->pmport=0;
    cmdfis->featurel=1;
    cmdfis->command = 0xEC;	// 0xEC
    cmdfis->device = 0;			// Master device
    cmdfis->c = 1;
    
    while ((port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000)
    {
        spin++;
    }
    if (spin == 1000000)
    {
        puts("Port is hung\n");
        return false;
    }
    
    port->ci = 1<<slot;	// Issue command
 
    while (1)
    {
        // In some longer duration reads, it may be helpful to spin on the DPS bit
        // in the PxIS port field as well (1 << 5)
        if ((port->ci & (1<<slot)) == 0)
            break;
        if (port->is & HBA_PxIS_TFES)	// Task file error
        {
            puts("Read disk error\n");
            return false;
        }
    }
    
    // Check again
    if (port->is & HBA_PxIS_TFES)
    {
        puts("Read disk error\n");
        return false;
    }
    
    hdparam * hdp = (hdparam *) AHCI_BUFFER;
    
    
    
    return true;
    
    
    
    
}
#define LOBYTE(x) ((unsigned char) ((x) & 0xff))
#define HIBYTE(x) ((unsigned char) ((x) >> 8 & 0xff))



bool read(HBA_PORT *port, DWORD startl, DWORD starth, DWORD count, WORD *buf)
{
    port->is = (DWORD)-1;		// Clear pending interrupt bits
    int spin = 0; // Spin lock timeout counter
    int slot = find_cmdslot(port);
    if (slot == -1)
        return false;
    
    HBA_CMD_HEADER *cmdheader = (HBA_CMD_HEADER*)port->clb;
    cmdheader += slot;
    cmdheader->cfl = sizeof(FIS_REG_H2D)/sizeof(DWORD);	// Command FIS size
    cmdheader->w = 0;		// Read from device
    cmdheader->prdtl = (WORD)((count-1)>>4) + 1;	// PRDT entries count
    
    HBA_CMD_TBL *cmdtbl = (HBA_CMD_TBL*)(cmdheader->ctba);
    memset(cmdtbl, 0, sizeof(HBA_CMD_TBL) +
           (cmdheader->prdtl-1)*sizeof(HBA_PRDT_ENTRY));
    
    // 8K bytes (16 sectors) per PRDT
    int i;
    
    for (i=0; i<cmdheader->prdtl-1; i++)
    {
        cmdtbl->prdt_entry[i].dba = (DWORD)buf;
        cmdtbl->prdt_entry[i].dbc = 8*1024;	// 8K bytes
        cmdtbl->prdt_entry[i].i = 1;
        buf += 4*1024;	// 4K words
        count -= 16;	// 16 sectors
    }
    // Last entry
    cmdtbl->prdt_entry[i].dba = (DWORD)buf;
    cmdtbl->prdt_entry[i].dbc = count<<9;	// 512 bytes per sector
    cmdtbl->prdt_entry[i].i = 1;
    
    // Setup command
    FIS_REG_H2D *cmdfis = (FIS_REG_H2D*)(&cmdtbl->cfis);
    
    cmdfis->fis_type = FIS_TYPE_REG_H2D;
    cmdfis->c = 1;	// Command
    cmdfis->command = ATA_CMD_READ_DMA_EXT;
    
    cmdfis->lba0 = (BYTE)startl;
    cmdfis->lba1 = (BYTE)(startl>>8);
    cmdfis->lba2 = (BYTE)(startl>>16);
    cmdfis->device = 1<<6;	// LBA mode
    
    cmdfis->lba3 = (BYTE)(startl>>24);
    cmdfis->lba4 = (BYTE)starth;
    cmdfis->lba5 = (BYTE)(starth>>8);
    
    cmdfis->countl = LOBYTE(count);
    cmdfis->counth = HIBYTE(count);
    
    // The below loop waits until the port is no longer busy before issuing a new command
    while ((port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000)
    {
        spin++;
    }
    if (spin == 1000000)
    {
        puts("Port is hung\n");
        return false;
    }
    
    port->ci = 1<<slot;	// Issue command
    
    // Wait for completion
    while (1)
    {
        // In some longer duration reads, it may be helpful to spin on the DPS bit
        // in the PxIS port field as well (1 << 5)
        if ((port->ci & (1<<slot)) == 0)
            break;
        if (port->is & HBA_PxIS_TFES)	// Task file error
        {
            puts("Read disk error\n");
            return false;
        }
    }
    
    // Check again
    if (port->is & HBA_PxIS_TFES)
    {
        puts("Read disk error\n");
        return false;
    }
    
    return true;
}




void probe_port(HBA_MEM *abar)
{
    // Search disk in impelemented ports
    DWORD pi = abar->pi;
    DWORD ver = abar->vs;
    
    
    int i = 0;
    
    
    while (i<32)
    {
        
        
        
        if (pi & 1)
        {
            
            
            int dt = check_type(&abar->ports[i]);
            if (dt == AHCI_DEV_SATA)
            {
                puts("SATA drive found at port ");
                putd(i);
                puts("\n");
                
                port_rebase(&abar->ports[i],i);
                
                identify(&abar->ports[i]);
                
                
                //read(&abar->ports[i],0,0,512,(WORD *)AHCI_BUFFER);
                
                
            }
            else if (dt == AHCI_DEV_SATAPI)
            {
                puts("SATAPI drive found at port ");
                putd(i);
                puts("\n");
            }
            else if (dt == AHCI_DEV_SEMB)
            {
                puts("SEMB drive found at port ");
                putd(i);
                puts("\n");
            }
            else if (dt == AHCI_DEV_PM)
            {
                puts("PM drive found at port ");
                putd(i);
                puts("\n");
            }
            else
            {
//                puts("No drive found at port ");
//                putd(i);
//                puts(" ");
            }
        }
        
        pi >>= 1;
        i ++;
    }
}


void ahci_init(uint32_t _abar)
{
    
    for(int i=0;i<32;i++)
    {
        portbase[i]=0;
        
    }
    
    mabar=(HBA_MEM*) _abar;
    
    
    
    probe_port(mabar);
    
    
    
    
}


