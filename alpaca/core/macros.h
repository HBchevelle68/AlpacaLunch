#ifndef MACROS_H
#define MACROS_H

#include <arpa/inet.h>

// Internal
#include <core/allu.h>


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