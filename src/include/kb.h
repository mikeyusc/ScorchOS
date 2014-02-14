/*
** Copyright 2008-12 ScorchOS Developers
** See doc/License.txt for terms of use.
** Keyboard Header File
*/

#ifndef __KB_H
#define __KB_H

#define ScrollLock (unsigned char)0x01
#define NumLock (unsigned char)0x02
#define CapsLock (unsigned char)0x04

int kb_special(unsigned char key);
void UpdateLeds(char led);
void FlushBuffer();
extern char getchar_int();
extern int getchar();
extern char *gets (char *s);
extern void waitKey();

#endif
