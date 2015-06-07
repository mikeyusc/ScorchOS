/*
** Copyright 2008-11 ScorchOS Developers
** See doc/License.txt for terms of use.
**
** Basic VGA Text Driver for Screen Access
*/

#include <system.h>
#include <string.h>

unsigned short *textmemptr;     // Pointer to VGA Base RAM
int attrib = 0x0F;              // Standard Color of screen
int csr_x = 0, csr_y = 0;       // Cursor x / y
int save_x=0, save_y=0;


void scroll(void)
    /// Scroll the screen up a line when we hit the bottom of the usable
    /// Screen, in this way the entire screen's contents scrolls up a line
{
    unsigned blank, temp;

    blank = 0x20 | (attrib << 8);   // Blank has a color / Charecter

    if(csr_y >= 25)                 // Row 25? Time to scroll.
    {
        temp = csr_y - 25 + 1;      // Move the cursor
        
        // Now we move the memory up by memcpy, this is FAST
        memcpy (textmemptr, textmemptr + temp * 80, (25 - temp) * 80 * 2);
        
        // Now the last line is mirrored on two lines, clear the last line
        memsetw (textmemptr + (25 - temp) * 80, blank, 80);
        
        csr_y = 24;                 // The cursor is now on the blank line.
    }
}

/// NOTE: What purpose does this function serve?
void shell_csr_fix()
    /// Fix the cursor back a line.
{
	csr_y--;        // Move cursor back a line
	move_csr();     // Tell the hardware to move it
}

void move_csr(void)
    /// VGA hardware has a built in sprite in text mode for a blinky cursor
    /// By default its a "_" and we're gonna keep it that way
    /// We're just gonna move it around so people know where they're typing...
{
    unsigned temp;

    temp = csr_y * 80 + csr_x;  // Position = (y * width) +  x

    outportb(0x3D4, 14);        // CRT Control Register: Select Cursor Location
    outportb(0x3D5, temp >> 8); // Send the high byte across the bus
    outportb(0x3D4, 15);        // CRT Control Register: Select Send Low byte
    outportb(0x3D5, temp);      // Send the Low byte of the cursor location
}

void moveto(int x, int y)
{
    csr_x=x; csr_y=y;
    move_csr();
    
}

void pushcsr()
{
    save_x=csr_x;
    save_y=csr_y;
    
}

void popcsr()
{
    csr_x=save_x;
    csr_y=save_y;
    move_csr();
}


void cls()
    /// Clear The screen
{
    unsigned blank;
    int i;

    blank = 0x20 | (attrib << 8);   // Make the "blank" (space + attribute)

    for(i = 0; i < 25; i++)         // Do it on every Line of the screen
        memsetw (textmemptr + i * 80, blank, 80);  // Clear a line on the screen

    csr_x = 0;  // Set Cursor Coords
    csr_y = 0;
    move_csr(); // Push the change to the VGA controller
}

void putch(char c)
    /// Output a single charecter to the screen
{
    unsigned short *where;
    unsigned att = attrib << 8;

    if(c == 0x08)   // Handle a Backspace as a control charecter
    {
        if(csr_x != 0) {
		csr_x--;									// Move it back one space
		where = textmemptr + (csr_y * 80 + csr_x);	// Find where we are
        *where = ' ' | att;							// Make it blank.
		}
    }

    else if(c == 0x09)  // Handle a Tab
    {
        csr_x = (csr_x + 8) & ~(8 - 1); // Move the cursor forward modulus 8
    }
    
    else if(c == '\r')  // Carriage return
    {
        csr_x = 0;      // Return the cursor to the beginning of the screen
    }

    else if(c == '\n')  // Newline!
    {
        csr_x = 0;      // Carriage return is included in the newline
        csr_y++;        // Next line on the screen
    }

    else if(c >= ' ')   // Any ASCII non-Controll code:
    {
        where = textmemptr + (csr_y * 80 + csr_x);
        *where = c | att;	// Character AND attributes: color
        csr_x++;
    }

    if(csr_x >= 80) // Check to see if we overflowed into next line
    {
        csr_x = 0;  // Adjust the line appropriately
        csr_y++;    // Increment line pointer
    }

    scroll();       // Check to see if we need to scroll the screen
    move_csr();     // Move the cursor to it's new home.
}

void puts(char *text)
    /// Output a string to the screen
{
    int i;

    for (i = 0; i < strlen(text); i++)  // Repeat until null charecter
    {
        putch(text[i]);                 // Put a charecter on the screen
    }
}

void putx(int n)
{
    char tembuf[64];
    
    itoa(n,16,tembuf);
    
    
    pad(&tembuf,4,'0');
    
    puts(tembuf);
    

}

void putlx(long long n)
{
    
    unsigned int high, low;
    
    low=n & 0xFFFF;
    high=n >> 16 & 0xFFFF;
    
    putx(high); putx(low);
    
}



void putd(int n)
    /// Output a base 10 number to the screen
{
	int i, acc = n, j = 0;
	char c1[32];
	char c2[32];

	if(n==0)
	{
		putch('0');     // Number is a 0, we lucked out.
		return;
	}
    
	for(i = 0; n; n /= 10, i++) // Loop, each time, divide by 10.
	{
		c1[i] = '0' + n % 10;   // Spit out the least significant digit
	}
	c1[i] = '\0';   // Set the end of the number as end of string 
	c2[i--] = 0;    // Not sure why this is here, Not gonna argue...
	while(i >= 0)   // Copy c1 to c2
	{
		c2[i--] = c1[j++];
	}
	puts(c2);       // Output c2 to screen
}

void pad (char * instr,int padnum, char padchar)
{
    int len = strlen(instr);
    if (len>=padnum) return;
    char temp[64];
    memset(temp,padchar,64);
    
    int i,j;
    j=0;
    
    
    for(i=padnum-len;i<padnum;i++,j++)
    {
        temp[i]=instr[j];
        
    }
    temp[i]=0;
    
    for(i=0;i<padnum;i++)
    {
        instr[i]=temp[i];
    }
    instr[i]=0;
    
    
}



char tbuf[32];
char bchars[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

void itoa(unsigned i,unsigned base,char* buf) {
    int pos = 0;
    int opos = 0;
    int top = 0;
    
    if (i == 0 || base > 16) {
        buf[0] = '0';
        buf[1] = '\0';
        
        return;
    }
    
    while (i != 0) {
        tbuf[pos] = bchars[i % base];
        pos++;
        i /= base;
    }
    top=pos--;
    for (opos=0; opos<top; pos--,opos++) {
        buf[opos] = tbuf[pos];
    }
    buf[opos] = 0;
}

void settextcolor(unsigned char forecolor, unsigned char backcolor)
    /// Set the color of the screen's output
{
    attrib = (backcolor << 4) | (forecolor & 0x0F);
}

void init_video(void)
    /// Set up the video driver (not that impressive...)
{
    textmemptr = (unsigned short *)0xB8000;
    cls();
}