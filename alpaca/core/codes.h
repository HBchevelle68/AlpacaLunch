#ifndef CODES_H
#define CODES_H

#include <stdint.h>

typedef uint32_t ALPACA_STATUS;


#define ALPACA_SUCCESS  	  	(0)
#define ALPACA_FAILURE  	  	(1)

/*
 * Comms Error Codes 
 */
#define ALPACA_ERROR_COMMSINIT 	(2)
#define ALAPCA_ERROR_WOLFINIT	(3)
#define ALPACA_ERROR_WOLFNOINIT (4) 
#define ALPACA_ERROR_SOCKCREATE	(5)


/*
 * Memory Error Codes
 */
#define ALPACA_MEM_NOBUFFER	  	(20)
#define ALPACA_MEM_EXCEEDED	  	(21)
#define ALPACA_MEM_RESZFAIL	  	(22)
#define ALPACA_MEM_APNDFAIL	  	(23)


/*
 * Generic Codes
 */
#define ALPACA_ERROR_UNKNOWN	(100)
#define ALPACA_ERROR_OPEN	  	(101)
#define ALPACA_ERROR_READ	  	(102)
#define ALPACA_ERROR_WRITE	  	(103)
#define ALPACA_ERROR_BADPARAM	(104)
#define ALPACA_ERROR_MALLOC 	(104)




#endif