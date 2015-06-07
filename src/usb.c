//
//  usb.c
//  
//
//  Created by Mike Evans on 5/28/15.
//
//

#include <system.h>
#include "usb.h"
#include <string.h>

// ==========================================================================
// CRC Generation Unit - Linear Feedback Shift Register implementation
// (c) Kay Gorontzi, GHSi.de, distributed under the terms of LGPL
// ==========================================================================
char *MakeCRC5(char *BitString)
{
    static char Res[6];                                 // CRC Result
    char CRC[5];
    int  i;
    char DoInvert;
    
    for (i=0; i<5; ++i)  CRC[i] = 0;                    // Init before calculation
    
    for (i=0; i<strlen(BitString); ++i)
    {
        DoInvert = ('1'==BitString[i]) ^ CRC[4];         // XOR required?
        
        CRC[4] = CRC[3];
        CRC[3] = CRC[2];
        CRC[2] = CRC[1] ^ DoInvert;
        CRC[1] = CRC[0];
        CRC[0] = DoInvert;
    }
    
    for (i=0; i<5; ++i)  Res[4-i] = CRC[i] ? '1' : '0'; // Convert binary to ASCII
    Res[5] = 0;                                         // Set string terminator
    
    return(Res);
}

