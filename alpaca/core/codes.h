#ifndef CODES_H
#define CODES_H

#include <stdint.h>

typedef uint32_t ALPACA_STATUS;

/*
 * Function Return Error codes
 * Maintain posix compliance, this also ensures portability
 */
#define ALPACA_SUCCESS  	(0)
#define ALPACA_FAILURE  	(1)

/*
 * Tool Error Codes 
 */
#define ALPACA_TLSINIT  	(2)
#define ALPACA_TLSCERT  	(3)
#define ALPACA_TLSKEY   	(4)
#define ALPACA_MEM_NOBUFFER	(5)
#define ALPACA_MEM_EXCEEDED	(6)
#define ALPACA_MEM_RESZFAIL	(7)
#define ALPACA_MEM_APNDFAIL	(8)


#endif