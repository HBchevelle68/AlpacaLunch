#define _XOPEN_SOURCE 700 //https://pubs.opengroup.org/onlinepubs/9699919799/
#include <stdio.h> 
#include <string.h>
#include <stdlib.h>
#include <errno.h>

// Internal
#include <interfaces/threading_interface.h>
#include <core/logging.h>

#define DEFAULTTHRDS 20

// For Atomics
//static const uint8_t FLAGON  = 1;
//static const uint8_t FLAGOFF = 0;

 
static alpaca_tmngr_t proc_pool;

/**
 * @brief Initialize threadmanager for process
 * 
 * @param max_thrds unsigned 1-byte value of max threads for pool
 *  
 * @return Returns the ALPACA_STATUS. If no errors and 
 *         threadmanager is usable ALPACA_SUCCESS returned.
 *         Otherwise, pool will be destroyed and an error 
 *         code will be returned.
 */
ALPACA_STATUS AlpacaThreadManager_initPool(uint8_t max_thrds){

    ALPACA_STATUS result = ALPACA_ERROR_UNKNOWN;
    ENTRY;

    // Can't have 0 threads
    if(!max_thrds){
        max_thrds = DEFAULTTHRDS;
    }

    proc_pool.pool = calloc(sizeof(alpaca_tmngr_t)*max_thrds, sizeof(uint8_t));
    if(!proc_pool.pool){
        LOGERROR("Error allocating threadmanager");
        result = ALPACA_ERROR_THRDINIT;
        goto exit;
    }
    proc_pool.status |= ALPACA_THRD_POOLINIT;
    for(uint8_t i = 0; i < max_thrds; i++) {
        proc_pool.pool[i].aptid = i;
    }
    
    
    /*
     * Init w/ default attributes. Interally, 
     * this is allocating and will need 
     * deallocation
     */
    result = pthread_mutexattr_init(&proc_pool.lock_attr);
    if(result){
        LOGERROR("Error initializing mutex attributes!");
        result = ALPACA_ERROR_THRDINIT;
        goto initfail;
    }
    proc_pool.status |= ALPACA_THRD_ATTRINIT;

    /* Set type to error checking 
     * Provides some deadlock protection
     * See man pages for details
     */ 
    result = pthread_mutexattr_settype(&proc_pool.lock_attr, PTHREAD_MUTEX_ERRORCHECK);
    if(result){
        LOGERROR("Error initializing mutex attributes!");
        result = ALPACA_ERROR_THRDINIT;
        goto initfail;
    }

    result = pthread_mutex_init(&proc_pool.lock, &proc_pool.lock_attr);
    if(result){
        LOGERROR("Error initializing mutex!");
        result = ALPACA_ERROR_THRDINIT;
        goto initfail;    
    }
    // Finish setting members
    proc_pool.max_threads = max_thrds;
    proc_pool.status |= (ALPACA_THRD_LOCKINIT | ALPACA_THRD_READY);

    /*
     * It is not entirely clear, but at this point
     * result must be 0 (ALPACA_SUCCESS). hop over
     * cleanup.
     */
    goto exit;

initfail: 
    if(proc_pool.status & ALPACA_THRD_POOLINIT) {

        if(proc_pool.status & ALPACA_THRD_ATTRINIT) {
            pthread_mutexattr_destroy(&proc_pool.lock_attr);
        }
        if(proc_pool.status & ALPACA_THRD_LOCKINIT) {
            pthread_mutex_destroy(&proc_pool.lock);
        }
        free(proc_pool.pool);
        proc_pool.pool = NULL;

    }
    // Clear status
    proc_pool.status >>= 4;

exit:
    
    LEAVING;
    return result;
}
/**
 * @brief Gracefull teardown of pool and deallocation of
 *        all heap memory
 * 
 * @note  All active threads will be told to cancel. As well,
 *        this call will block waiting for all threads to complete
 *  
 */
ALPACA_STATUS AlpacaThreadManager_teardownPool(void){
    
    ALPACA_STATUS result = ALPACA_SUCCESS;
    ENTRY;

    if(proc_pool.status & ALPACA_THRD_POOLINIT) {
        proc_pool.status ^= ALPACA_THRD_READY;

        // Cancel all
        // join all

        if(proc_pool.status & ALPACA_THRD_ATTRINIT) {
            pthread_mutexattr_destroy(&proc_pool.lock_attr);
        }
        if(proc_pool.status & ALPACA_THRD_LOCKINIT) {
            pthread_mutex_destroy(&proc_pool.lock);
        }
        free(proc_pool.pool);
        proc_pool.pool = NULL;

    }
    // Clear status
    proc_pool.status >>= 4;


    return result;
}