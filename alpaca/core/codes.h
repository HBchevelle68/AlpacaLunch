#ifndef CODES_H
#define CODES_H

#include <stdint.h>

typedef uint32_t ALPACA_STATUS;

/*
 * Function Return Error codes
 * Maintain posix compliance, this also ensures portability
 */
#define ALPACA_SUCCESS  	  (0)
#define ALPACA_FAILURE  	  (1)

/*
 * Comms Error Codes 
 */
#define ALPACA_COMMS_INITFAIL (2)
#define ALPACA_COMMS_TLSINIT  (3)
#define ALPACA_COMMS_TLSCERT  (4)
#define ALPACA_COMMS_TLSKEY   (5)
#define ALPACA_COMMS_SOCKERR  (6)

/*
 * Memory Error Codes
 */
#define ALPACA_MEM_NOBUFFER	  (20)
#define ALPACA_MEM_EXCEEDED	  (21)
#define ALPACA_MEM_RESZFAIL	  (22)
#define ALPACA_MEM_APNDFAIL	  (23)



#endif