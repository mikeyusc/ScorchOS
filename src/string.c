/*
** Copyright 2008-12 ScorchOS Developers
** See doc/License.txt for terms of use.
**
** ANSI-C Compliant String functions
*/

#include <system.h>
#include <string.h>

void *memcpy(void *dst, const void *src, size_t len)
    /// Copy block of memory from src to dst, len bytes long
{
    char *sp = (char*) src;
    char *dp = (char*) dst;
    for(; len != 0; len--) *dp++ = *sp++;
	return dst;
}

void memset(void *dst, int val, size_t len)
    /// Set len bytes of memory at location dst to val
{
    char *temp = (char*) dst;
    for ( ; len != 0; len--) *temp++ = val;
}

int memcmp(const void *s1, const void *s2, size_t n)
    /// Compare n bytes of s1 to s2
{
	const unsigned char *us1 = (const unsigned char *)s1;
	const unsigned char *us2 = (const unsigned char *)s2;
	while(n-- != 0)
	{
		if(*us1 != *us2)
			return (*us1 < *us2) ? -1 : +1;
		us1++;
		us2++;
	}
	return 0;
}

char *strchr(const char *s, int c) 
    /// Break a string on a charecter
{
	while (*s != (char)c)
		if (!*s++)
			return 0;
	return (char *)s;
}

size_t strspn(const char *s1, const char *s2)
    /// Find length of string s2 in s1
{
	size_t ret=0;
	while(*s1 && strchr(s2,*s1++))
		ret++;
	return ret;
}

size_t strcspn(const char *s1, const char *s2)
    /// Find length of string without hitting s2
{
	size_t ret=0;
	while(*s1)
		if(strchr(s2,*s1))
			return ret;
		else s1++,ret++;
	return ret;
}

char *strtok(char *str, const char *delim)
    /// Cut a string into bite sized peices, yummy!
{
	static char* p=0;
	if(str)  	/* Does the string exist? */
		p=str;	/* If so, change p to current position. */
	else if(!p) 
		return 0; /* Otherwise, exit null. */
	str = p   + strspn(p,delim);
	p   = str + strcspn(str,delim);
	if(p==str)
		return p=0;
	p = *p ? *p=0,p+1 : 0;
	return str;
}

char *strstr(const char *s1, const char *s2)
    /// Find where s2 is in s1
{
	size_t s2len;
	if(*s2 == '\0')
		return (char *) s1;
	s2len = strlen(s2);
	for (; (s1 = strchr(s1, *s2)) != NULL; s1++)
		if (strncmp(s1, s2, s2len) == 0)
			return (char *) s1;
	return NULL;
}

size_t strlen(const char *s)
    /// Find the length of a string
{
   size_t i;
   for(i=0; s[i] != '\0'; i++);
   return i;
}

int strcmp(const char *s1, const char *s2)
    /// Compare two null ended strings
{
    for(; *s1 == *s2; ++s1, ++s2)
        if(*s1 == 0)
            return 0;
    return *(unsigned char *)s1 < *(unsigned char *)s2 ? -1 : 1;
}

int strncmp(const char *s1, const char *s2, size_t n)
    /// Compare n bytes in a s1 to s2
{
	unsigned char uc1, uc2;
	if (n==0)
		return 0;
	while (n-- > 0 && *s1 == *s2) {
		if ( n==0 || *s1 == '\0' )
			return 0;
		s1++;
		s2++;
	}
	uc1 = (*(unsigned char *) s1);
	uc2 = (*(unsigned char *) s2);
	return ((uc1 < uc2) ? -1 : (uc1 > uc2));
}

char *strcpy(char *dest, char *src)
    /// Copy null terminated src to dest 
{
    do
    {
      *dest++ = *src++;
    }
    while (*src != 0);
	return dest;
}

char *strcat(char *dst, char *src)
    /// Meow! (Concatinate dst to the end of src
{
   size_t len;

   len = strlen(dst);
   strcpy(dst + len, src);

   return dst;
}
