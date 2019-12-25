#ifndef MACROS_H
#define MACROS_H

#include <arpa/inet.h>

// Internal
#include <core/allu.h>



/*
 * Data manipulation
 */

#define HU16(x)  ntohs(x)

#define BEU16(x) htons(x)
#define BEU32(x) htonl(x)

#define PUT_BEU16(dst,src) PUT_U16(dst,BEU16(src));
#define PUT_BEU32(dst,src) PUT_U32(dst,BEU32(src));

//#define PUT_U8(dst,src) memset(&dst, (uint8_t)src, sizeof(uint8_t));
#define PUT_U16(dst,src) memset(&dst, (uint16_t)src, sizeof(uint16_t));
#define PUT_U32(dst,src) memset(&dst, (uint32_t)src, sizeof(uint32_t));
//#define PUT_U64(dst,src) memset(&dst, (uint64_t)src, sizeof(uint64_t));



/*
 *   Error checking
 */

#define FAIL_IF_TRUE(ret)        \
     if(ret != ALPACA_SUCCESS) { \
        return ALPACA_FAILURE;   \
     }

#define FAIL_UNINSTALL_IF(ret)   \
     if(ret != ALPACA_SUCCESS) { \
        alpacacore_exit();       \
        return ALPACA_FAILURE;   \
     }

#define FAIL_UNINSTALL_IF_PRINT(ret, errstr) \
     if(ret != ALPACA_SUCCESS) {   \
        LOGERROR(errstr);          \
        alpacacore_exit();         \
        return ALPACA_FAILURE;     \
     }




#endif