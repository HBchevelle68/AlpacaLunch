#ifndef CODES_H
#define CODES_H

#include <stdint.h>

typedef uint32_t ALPACA_STATUS;


#define ALPACA_SUCCESS (0)
#define ALPACA_FAILURE (1)

/*
 * Comms Error Codes 
 */
#define ALAPCA_ERROR_WOLFINIT	        (2)
#define ALPACA_ERROR_WOLFNOINIT         (3)
#define ALPACA_ERROR_WOLFSSLCREATE      (4)
#define ALPACA_ERROR_WOLFSSLCONNECT     (5)
#define ALPACA_ERROR_WOLFSSLWRITE       (6)
#define ALPACA_ERROR_WOLFSSLREAD        (7)
#define ALPACA_ERROR_WOLFSSLACCEPT      (8)
#define ALPACA_ERROR_WOLFSSLFDSET       (9)
#define ALPACA_ERROR_SOCKCREATE	        (10)
#define ALPACA_ERROR_SOCKSETPEER        (11)
#define ALPACA_ERROR_TCPBIND            (12)
#define ALPACA_ERROR_TCPLISTEN          (13)

#define ALPACA_ERROR_COMMSINIT 	        (15)
#define ALPACA_ERROR_COMMSCONNECT       (16)
#define ALPACA_ERROR_COMMSLISTEN        (17)
#define ALPACA_ERROR_COMMSCLOSED        (18)

#define ALPACA_ERROR_HDRBODYSIZE        (25)


/*
 * Config Codes
 */
#define ALPACA_ERROR_CONFINITBEHAV      (40)
#define ALPACA_ERROR_CONFINITPROTO      (41)
#define ALPACA_ERROR_CONFCORRUPT        (42)
#define ALPACA_ERROR_CONFBADPORT        (43)


/*
 * Crypto Error Codes 
 */
#define ALPACA_ERROR_CRYPTSETKEY  	    (60)


/*
 * Memory Error Codes
 */
#define ALPACA_ERROR_MEMINIT    	  	(70)
#define ALPACA_ERROR_MEMNOBUFFER	  	(71)
#define ALPACA_ERROR_MEMEXCEEDED	  	(72)
#define ALPACA_ERROR_MEMRESZFAIL	  	(73)
#define ALPACA_ERROR_MEMAPNDFAIL	  	(74)
#define ALPACA_ERROR_MEMFREEFAIL	  	(75)

/*
 * Threadpool Error Codes
 */
#define ALPACA_ERROR_THRDINIT  	  	    (80)


/*
 * Synchronization Error Codes
 */
#define ALPACA_ERROR_MTXATTRINIT	  	(100)
#define ALPACA_ERROR_MTXINIT	  	    (101)
#define ALPACA_ERROR_MTXBUSY	  	    (102)
#define ALPACA_ERROR_MTXLOCK	  	    (103)
#define ALPACA_ERROR_MTXUNLOCK	  	    (104)
#define ALPACA_ERROR_MTXOWNDED	  	    (105)
#define ALPACA_ERROR_MTXTIMEOUT	  	    (106)




/*
 * Generic Codes
 */
#define ALPACA_ERROR_UNKNOWN	        (200)
#define ALPACA_ERROR_OPEN	  	        (201)
#define ALPACA_ERROR_READ	  	        (202)
#define ALPACA_ERROR_WRITE	  	        (203)
#define ALPACA_ERROR_BADPARAM	        (204)
#define ALPACA_ERROR_MALLOC 	        (205)
#define ALPACA_ERROR_BADSTATE           (206)
#define ALPACA_ERROR_UNSUPPORTED        (207)



#endif