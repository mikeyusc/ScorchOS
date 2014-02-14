/*
** Copyright 2008-12 ScorchOS Developers
** See doc/License.txt for terms of use.
** ANSI C Stantard Subset: string manipulation functions <string.h>
*/

#ifndef _STRING_H
#define _STRING_H

#if !defined(NULL) 										/* Define NULL */
	#define NULL ((void*)0)
#endif

extern void *memcpy(void *dst, const void *src, size_t len);	/* Copy Memory 								*/
extern void memset(void *dst, int val, size_t len);				/* Set memory to a certial value            */
extern int memcmp(const void *s1, const void *s2, size_t n);	/* Compare memory. 							*/

extern size_t strcspn(const char *s1, const char *s2);	/* Find length of s1 without charecters in s2.      */
extern size_t strspn(const char *s1, const char *s2);	/* Find length of parts of s1 included in s2.		*/
extern char *strtok(char *str, const char *delim);		/* Tolkenize a string, delim = tolken to cut at.    */
extern char *strchr(const char *s, int c); 	    		/* Find first occurance of a charecter in a string. */ 
extern char *strstr(const char *s1, const char *s2);	/* Find s2 string in s1.							*/
extern size_t strlen(const char *s);					/* Find string length. 								*/
extern int strcmp(const char *s1, const char *s2);		/* Compare two strings 							  	*/
extern char *strcpy(char *dst, char *src);				/* Copy one string into another. 					*/
extern char *strcat(char *dst, char *src);				/* Add one string on to the end of another string. 	*/
int strncmp(const char *s1, const char *s2, size_t n);	/* Compare n bytes in a string.					 	*/

#endif
