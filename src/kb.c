/*
** Copyright 2008-11 ScorchOS Developers
** See doc/License.txt for terms of use.
**
** PS/2 Driver & Keyboard Scancode Decoder 
** Improved by Primis
*/

#include <system.h>
#include <kb.h>

/// These are static Variables, They are the control lines of the keyboard;
    /// shift, caps, ctrl, ect.

unsigned char shift = 0;            // Shift Key Status 
unsigned char ctrl = 0;             // Ctrl Key Status
unsigned char alt = 0;              // Alt Key Status 
unsigned char caps = 0;             // Caps Lock Status
unsigned char num = 0;              // Num Lock Status 
unsigned char keyBuff[257];         // Keybuffer 
volatile unsigned char keyBuffEnd = 0;  // The Last key in the buffer
unsigned char asciiCode;            // The ASCII Code
unsigned char leds = 0;             // The Three LED's on the keyboard.
unsigned char echoON = 1;           // Echo keys
unsigned char kbScanCodes[512] =    
    /// Keyboard character maps (Look-up table)
{					
	0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
	'\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[' , ']', '\n', 0,
	'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,
	'\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
	'*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, '-', 0, 0, 0, '+', 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/* Shifted */
	0, 27, '!', '@', '#', '$', '%', '^', '&', '*', '(' , ')', '_' , '+', '\b',
	'\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{' , '}', '\n', 0,
	'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\'', '~', 0,
	'|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
	'*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, '7', '8', '9', 0, '4', '5', '6', 0,
	'1', '2', '3', '0', 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/* Caps, Normal */
	0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
	'\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{' , '}', '\n', 0,
	'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\'', '~', 0,
	'|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
	'*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, '-', 0, 0, 0, '+', 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/* Caps, Shifted */
	0, 27, '!', '@', '#', '$', '%', '^', '&', '*', '(' , ')', '_' , '+', '\b',
	'\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[' , ']', '\n', 0,
	'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,
	'\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
	'*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, '-', 0, 0, 0, '+', 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

void FlushBuffer()
    /// There is a hardware buffer for storing keypresses just incase you
    /// Wanted to use polling instead of interrupts, we have to clear this
    /// Before we set up the keyboard so random crap doesnt show up in the
    /// Software buffer
{
	unsigned temp;
	do
	{
		temp = inportb(0x64);
		if((temp & 0x01) != 0) 
		{
			(void)inportb(0x60);
			continue;
		}
	} while((temp & 0x02) != 0);
}


int kb_special(unsigned char key)
{
	static int specKeyUp = 1;	// Is a key already been or being pressed?
	switch(key) 
	{
		case 0x36: // R-Shift down
		case 0x2A: // L-Shift down
			shift = 1;
			break;
		case 0xB6: // R-Shift up
		case 0xAA: // L-Shift up
			shift = 0;
			break;
		case 0x1D: // Control down
			ctrl = 1;
			break;
		case 0x9D: // Control up
			ctrl = 0;
			break;
		case 0x38: // Alt down
			alt = 1;
			break;
		case 0xB8: // Alt up
			alt = 0;
			break;
		case 0x3A: // Caps down
			if(specKeyUp == 1) 
			{
				caps = !caps;
				UpdateLeds(CapsLock);
				specKeyUp = 0;
			}
			break;
		case 0x45: // Num down
			if(specKeyUp == 1)
			{
				num = !num;
				UpdateLeds(NumLock);
				specKeyUp = 0;
			}
			break;
		case 0x46: // Scroll down
			if(specKeyUp == 1)
			{
				num = !num;
				UpdateLeds(ScrollLock);
				specKeyUp = 0;
			}
			break;
		case 0xBA: // Caps Up
		case 0xC5: // Num Up
		case 0xC6: // Scroll Up
			specKeyUp = 1;
			break;
		case 0xE0:
			break;
		default:
			return(0);
	}
	return (1);
}


void keyboard_handler(struct regs *r)
    /// Get keyboard input during the keyboard interrupt
    /// Convert the scancode into ascii data.
{
	unsigned char scanCode;
	scanCode = inportb(0x60);
	unsigned char asciiCode;
	
	
	if(!(kb_special(scanCode) | (scanCode >= 0x80)))
	{
		if(shift)		//Determine key that has been pressed
		{
			if(!caps)
			{
				asciiCode = kbScanCodes[scanCode + 128];
			}
			else
			{
				asciiCode = kbScanCodes[scanCode + 384];
			}
		}
		else
		{
			if(!caps)
			{
				asciiCode = kbScanCodes[scanCode];
			}
			else
			{
				asciiCode = kbScanCodes[scanCode + 256];
			}
		}
		keyBuffEnd++;
		keyBuff[keyBuffEnd] = asciiCode;
		if (echoON){
			putch((int)keyBuff[keyBuffEnd]);
		}
	}
}

void keyboard_install()
    /// Install the Keyboard handler into the IDT
{
    FlushBuffer();                      // Anything in the buffer is crap
	irq_install_handler(1, &keyboard_handler);  // Install Handler
}

void waitKey()
    /// Wait for a keypress on the keyboard
{
	FlushBuffer();  // Empty the Buffer of erratic data
	while (getchar() == 0);
}

char *gets(char *s) 
    /// Get a String from stdin (keyboard)
{
	int i;
	char k = (char)getchar();
	if (k == 0)
		return 0;
	for (i = 0; k != 0 && k != '\n'; i++)
	{
		if (k == '\b')
		{
			i--;
			s[i] = 0;
			i--;
			k = (char)getchar();
			if(k == 0)
				return 0;
		}
		else
		{
			s[i] = k;
			k = (char)getchar();
			if(k == 0)
				return 0;
		}
	}
	s[i] = '\0';
	return s;
}
char getchar_int()
    /// Get a charecter from the stdin
{
	int i = 0;
	while(keyBuffEnd == 0);
	asm("cli");

	for(; i < keyBuffEnd; i++)
	{
		keyBuff[i] = keyBuff[i + 1];
	}
	keyBuffEnd--;

	asm("sti");

	return keyBuff[0];
}
int getchar()
    /// Get a charecter from the stdin (ANSI compliant)
{
	return ((int) getchar_int());
}

void UpdateLeds(char led)
    /// Keyboards have LED's for locks, update them so they match what the
    /// States of the keyboard are.
{
	if(led == 0)
	{
		leds = 0;
	}
	else
	{
	if (leds == (leds|led))	        // If led is already on
		{
			leds = leds^led;	    // We turn the led off
		}
		else
		{
			leds = leds | led;	    // Else, turn led on
		}
}
	while((inportb(0x64) &2) !=0);  // Loop until zero
	outportb(0x60, 0xED);
	while((inportb(0x64) &2) !=0 ); // Loop until zero
	outportb(0x60, leds);			// Update led status register
}
