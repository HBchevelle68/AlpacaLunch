#ifndef MACROS_H
#define MACROS_H

#include <arpa/inet.h>



/*
 * Data manipulation
 */
#define BEU16(x) htons(x)
#define BEU32(x) htonl(x)

#define PUT_BEU16(dst,src) PUT_U16(dst,BEU16(src));
#define PUT_BEU32(dst,src) PUT_U32(dst,BEU32(src));

#define PUT_U16(dst,src) memset(&dst, (uint16_t)src, sizeof(uint16_t));
#define PUT_U32(dst,src) memset(&dst, (uint16_t)src, sizeof(uint32_t));



/*
 *   Error checking
 */
#define SERVFAIL_IF(ret, errstr) \
     if(ret < 0) {           \
        LOGERROR(errstr);    \
        NS_server_clean();   \
        return NS_FAILURE; } \



/*
 * Heap allocation and free
 * 
 */
#define BUFALLOC(buf, size) \
     if(size > 0) {  buf = (void*)malloc(size * sizeof(unsigned char)); }

#define BUFFREE(buf)                                        \
     if(buf != NULL) {                                      \
          memset(buf, (unsigned char)0x00, sizeof(*buf));   \
          free(buf);                                        \
          buf = NULL;                                       \
     }             



#endif