/*
** Copyright 2008-11 ScorchOS Developers
**   See doc/License.txt for terms of use.
**
** ScorchOS Shell
** Parses input and executes shell functions
*/

// *REWRITE* Get rid of static cmdSwitches and replace with a dynamic array

#include <system.h>
#include <string.h>
#include <kb.h>
#include <command.h>

#define KERN_VER "0.1.5"
#define SHELL_VER "0.6"
#define MaxCommands 16

int numCommands = -1;   /* Number of Registered Commands, we start at -1 so the first one registered is 0. */
char *cmdString;
char *cmdSwitch0;
char *cmdSwitch1;
char *cmdSwitch2;
char *cmdSwitch3;
char *cmdSwitch4;
char *cmdSwitch5;
char *cmdSwitch6;
commandEntry CommandTable[MaxCommands];
extern char prompt [32];
void runShell()
{
	int i;
	char inputString[512];
	putch('\n');
	puts(prompt);
	gets(inputString);
	cmdString = strtok(inputString, " ");
	cmdSwitch0 = strtok(NULL, " ");
	cmdSwitch1 = strtok(NULL, " ");
	cmdSwitch2 = strtok(NULL, " ");
	cmdSwitch3 = strtok(NULL, " ");
	cmdSwitch4 = strtok(NULL, " ");
	cmdSwitch5 = strtok(NULL, " ");
	cmdSwitch6 = strtok(NULL, " ");
	void (*commandJump)(void);
	i = findCMD(cmdString);
	if(i >= 0)
	{
		commandJump = CommandTable[i].address;
		commandJump();
	}
	return;
}

int findCMD(char *cmd)
{
	int i;
	int c;
	for(i=0; i < numCommands + 1; i++)
	{
		c = strcmp(CommandTable[i].command, cmd);
		if (c == 0)
		{
			return i;
		}
	}
	return -1;
}

void initShell()
{
	clearCommands();
	puts("Username: ");
	gets(prompt);
	strcat(prompt, "$ ");
	cls();
	cmdString[0] = 0;
	cmdSwitch0[0] = 0;
	cmdSwitch1[0] = 0;
	cmdSwitch2[0] = 0;
	cmdSwitch3[0] = 0;
	cmdSwitch4[0] = 0;
	cmdSwitch5[0] = 0;
	cmdSwitch6[0] = 0;
	addCommand("help", "Lists all available commands.", help);
	addCommand("ahelp", "Lists all available commands with descriptions", ahelp);
	addCommand("print", "Outputs your chosen text", outputMe);
	addCommand("version", "Displays version information for ScorchOS kernel and shell", version);
	addCommand("example", "Quick string to int conversion demo", example);
	addCommand("add", "Adds two numbers together", addMe);
	addCommand("sub", "Subtracts one number from another", subtractMe);
	addCommand("exp", "Multiplies a number against itself an exponential amount of times", exponentMe);
	addCommand("reset", "Reset shell", initShell);
	puts("\nScorchOS Shell ");
	puts(SHELL_VER);
	putch('\n');
}

void clearCommands() /* Clear all address pointers of our Commands. */
{
	int i;
	for(i=0;i<MaxCommands;i++)
	{
		CommandTable[i].address = NULL;
		numCommands = -1;
	}
}


int addCommand(char *command, char *description, void *address)
{
	if(numCommands < MaxCommands)
	{
		numCommands++;
		strcpy(CommandTable[numCommands].command, command);
		strcpy(CommandTable[numCommands].description, description);
		CommandTable[numCommands].address = address;
		return numCommands;
	}
	return -1;
}

/* ScorchOS Commands - This is where all commands for the shell are stored */

void help()
{
	int i;
	puts("\nCommand List: ");
	for(i=0;i<=numCommands;i++)
	{
		if(CommandTable[i].address != NULL)
		{
			puts(CommandTable[i].command);
			putch(',');
			putch(' ');
		}
	}
	putch('\n');
}

void ahelp()
{
	int i;
	for(i=0; i <= numCommands; i++)
	{
		puts(CommandTable[i].command);
		puts(" - ");
		puts(CommandTable[i].description);
		putch('\n');
	}
	return;
}

void version()
{
	puts("\nKernel Version:\t");
	puts(KERN_VER);
	puts("\nShell Version:\t");
	puts(SHELL_VER);
}

void example()
{
	char a[9], b[9];
	int c,d;
	puts("Input a number(1): ");
	gets(a);
	puts("Input a number(2): ");
	gets(b);
	c = atoi(a);
	d = atoi(b);
	puts("Addition: ");
	putd(c+d);
	puts("\nSubtraction: ");
	putd(c-d);
	puts("\nMultiply: ");
	putd(c*d);
	puts("\nDivide: ");
	putd(c/d);
	putch('\n');
}

void outputMe()
{
	puts("\nYou typed:");
	putch('\n');
	puts(cmdSwitch0);
	putch(' ');
	puts(cmdSwitch1);
	putch(' ');
	puts(cmdSwitch2);
	putch(' ');
	puts(cmdSwitch3);
	putch(' ');
	puts(cmdSwitch4);
	putch(' ');
	puts(cmdSwitch5);
	putch(' ');
	puts(cmdSwitch6);
}

int atoi(const char *nptr)
{
  int c;
  int total;
  int sign;

  c = (int)(unsigned char) *nptr++;
  sign = c;
  if (c == '-' || c == '+') c = (int)(unsigned char) *nptr++;

  total = 0;
  while (('0'<= c) && (c <='9'))
  {
    total = 10 * total + (c - '0');
    c = (int)(unsigned char) *nptr++;
  }

  if (sign == '-')
    return -total;
  else
    return total;
}

void exponentMe()
{
	int a, b, c;
	a = atoi(cmdSwitch0);
	b = atoi(cmdSwitch0);
	c = atoi(cmdSwitch1);

	if (c == 0)
	{
		putd(1);
	}
	else
	{
		for (; c > 1; c--)
		{
			a = a * b;
		}
		putd(a);
	}
}

void addMe()
{
	int a, b;
	a = atoi(cmdSwitch0);
	b = atoi(cmdSwitch1);
	putch('\n');
	puts(cmdSwitch0);
	puts(" + ");
	puts(cmdSwitch1);
	puts(" = ");
	putd(a+b);
}

void subtractMe()
{
	int a, b;
	a = atoi(cmdSwitch0);
	b = atoi(cmdSwitch1);
	putch('\n');
	puts(cmdSwitch0);
	puts(" - ");
	puts(cmdSwitch1);
	puts(" = ");
	putd(a-b);
}
