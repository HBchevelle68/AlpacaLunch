#ifndef CODES_H
#define CODES_H

#include <stdint.h>

typedef uint32_t ALPACA_STATUS;


#define ALPACA_SUCCESS  	  	(0)
#define ALPACA_FAILURE  	  	(1)

/*
 * Comms Error Codes 
 */
#define ALAPCA_ERROR_WOLFINIT	        (2)
#define ALPACA_ERROR_WOLFNOINIT         (3)
#define ALPACA_ERROR_WOLFSSLCREATE      (4)
#define ALPACA_ERROR_WOLFSSLCONNECT     (5)
#define ALPACA_ERROR_COMMSINIT 	        (10)
#define ALPACA_ERROR_COMMSCONNECT       (11)
#define ALPACA_ERROR_SOCKCREATE	        (20)
#define ALPACA_ERROR_SOCKSETPEER        (21)



/*
 * Crypto Error Codes 
 */
#define ALPACA_ERROR_CRYPTSETKEY  	    (30)


/*
 * Memory Error Codes
 */
#define ALPACA_ERROR_MEMNOBUFFER	  	(50)
#define ALPACA_ERROR_MEMEXCEEDED	  	(51)
#define ALPACA_ERROR_MEMRESZFAIL	  	(52)
#define ALPACA_ERROR_MEMAPNDFAIL	  	(53)

/*
 * Threadpool Error Codes
 */
#define ALPACA_ERROR_TPOOLINIT  	  	(60)


/*
 * Generic Codes
 */
#define ALPACA_ERROR_UNKNOWN	        (100)
#define ALPACA_ERROR_OPEN	  	        (101)
#define ALPACA_ERROR_READ	  	        (102)
#define ALPACA_ERROR_WRITE	  	        (103)
#define ALPACA_ERROR_BADPARAM	        (104)
#define ALPACA_ERROR_MALLOC 	        (105)
#define ALPACA_ERROR_BADSTATE           (106)





#endif