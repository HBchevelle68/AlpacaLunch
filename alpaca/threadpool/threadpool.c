#define _XOPEN_SOURCE 700
#include <stdio.h> 
#include <string.h>
#include <stdlib.h>

// Internal
#include <interfaces/threadpool_interface.h>
#include <core/logging.h>

#define MAXTHREADS 255

// For Atomics
//static const uint8_t FLAGON  = 1;
//static const uint8_t FLAGOFF = 0;


Alpaca_tPool_t allu_pool;

// init threadpool
ALPACA_STATUS AlpacaThreadpool_initPool(uint8_t max_thrds){

    ALPACA_STATUS result = ALPACA_ERROR_UNKNOWN;
    
    ENTRY;

    // Let's set some boundries here
    if(max_thrds > MAXTHREADS){
        LOGERROR("Cannot support more than %u threads", MAXTHREADS);
        result = ALPACA_ERROR_BADPARAM;
        goto exit;
    }

    allu_pool.pool = calloc(sizeof(Alpaca_tPool_t), sizeof(uint8_t));
    if(!allu_pool.pool){
        LOGERROR("Error allocating threadpool");
        result = ALPACA_ERROR_TPOOLINIT;
        goto exit;
    }
    
    /*
     * Set default attributes. Interally, 
     * this is allocating space and will need 
     * deallocation
     */
    result = pthread_mutexattr_init(&allu_pool.lock_attr);
    if(result){
        LOGERROR("Error initializing mutex attributes");
        result = ALPACA_ERROR_TPOOLINIT;
        goto initfail;
    }
    allu_pool.status |= ALPACA_TPOOL_ATTRINIT;

    result = pthread_mutexattr_settype(&allu_pool.lock_attr), PTHREAD_MUTEX_ERRORCHECK_NP);


    

initfail:
    if(pool){

        
    }


exit:
    LEAVING;
    return result;
}