//
//  inet.h
//  
//
//  Created by Mike Evans on 5/20/15.
//
//

#ifndef _inet_h
#define _inet_h

#define HTONS(n) (((((unsigned short)(n) & 0xFF)) << 8) | (((unsigned short)(n) & 0xFF00) >> 8))
#define NTOHS(n) (((((unsigned short)(n) & 0xFF)) << 8) | (((unsigned short)(n) & 0xFF00) >> 8))

#define HTONL(n) (((((unsigned long)(n) & 0xFF)) << 24) | \
((((unsigned long)(n) & 0xFF00)) << 8) | \
((((unsigned long)(n) & 0xFF0000)) >> 8) | \
((((unsigned long)(n) & 0xFF000000)) >> 24))

#define NTOHL(n) (((((unsigned long)(n) & 0xFF)) << 24) | \
((((unsigned long)(n) & 0xFF00)) << 8) | \
((((unsigned long)(n) & 0xFF0000)) >> 8) | \
((((unsigned long)(n) & 0xFF000000)) >> 24))



#endif
