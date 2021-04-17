#define _XOPEN_SOURCE 700 //https://pubs.opengroup.org/onlinepubs/9699919799/
#include <pthread.h>
#include <errno.h>

#include <core/logging.h>
#include <interfaces/threading_interface.h>


ALPACA_STATUS AlpacaSync_init(alpaca_mtx_t* mtx){

    ALPACA_STATUS result = ALPACA_SUCCESS;
    ENTRY;


    /*
     * Verify paramters
     */
    if(NULL == mtx) {
        LOGERROR("Bad lock variable passed, make sure its not still valid mtx[%p]", mtx );
        result = ALPACA_ERROR_BADPARAM;
        goto exit;
    } 

    if(ALPACA_SYNC_INACTIVE != mtx->status){
        LOGERROR("Mutex is either active or in bad state mtx->status[%u]!", mtx->status);
        result = ALPACA_ERROR_BADPARAM;
        goto exit;
    }

    /*
     * Init w/ default attributes. Interally, 
     * this is allocating and will need 
     * deallocation
     */
    if(pthread_mutexattr_init(&mtx->lock_attr)){
        LOGERROR("Error initializing mutex attributes! [%d]", errno);
        result = ALPACA_ERROR_MTXATTRINIT;
        goto exit;
    }
    mtx->status |= ALPACA_SYNC_ATTRINIT;

    /* Set type to error checking 
     * Provides some deadlock protection
     * See man pages for details
     */ 
    if(pthread_mutexattr_settype(&mtx->lock_attr, PTHREAD_MUTEX_ERRORCHECK)){
        LOGERROR("Error setting mutex type attributes! [%d]", errno);
        result = ALPACA_ERROR_MTXATTRINIT;
        goto exit;
    }

    if(pthread_mutex_init(&mtx->lock, &mtx->lock_attr)){
        LOGERROR("Error initializing mutex![%d]", errno);
        result = ALPACA_ERROR_MTXINIT;
        goto exit;
    }
    mtx->status |= ALPACA_SYNC_LOCKINIT;


exit:
    if(ALPACA_SUCCESS != result && ALPACA_ERROR_BADPARAM != result){
        if(ALPACA_SYNC_ATTRINIT & mtx->status){
            pthread_mutexattr_destroy(&mtx->lock_attr);
        }
        /*
         * There isn't a way to get here
         * and have a valid lock 
         */
    }

    LEAVING;
    return result;
}

ALPACA_STATUS AlpacaSync_destroy(alpaca_mtx_t* mtx){
    ALPACA_STATUS result = ALPACA_SUCCESS;
    ENTRY;
    /*
     * Verify paramters
     */
    if(NULL == mtx) {
        LOGERROR("Bad lock variable passed, make sure its not still valid mtx[%p]", mtx );
        result = ALPACA_ERROR_BADPARAM;
        goto exit;
    } 

    if(ALPACA_SYNC_INACTIVE == mtx->status){
        LOGERROR("Mutex is inactive cannot destroy mtx[%p] mtx->status[%u]!", mtx, mtx->status);
        result = ALPACA_ERROR_BADPARAM;
        goto exit;
    }

    /*
     * Check status flags to prevent any 
     * double free's
     */
    if(ALPACA_SYNC_ATTRINIT && mtx->status){
        LOGDEBUG("Cleaning mutex [%p] attribute structure ", mtx);
        pthread_mutexattr_destroy(&mtx->lock_attr);
    }

    if(ALPACA_SYNC_LOCKINIT && mtx->status){
        LOGDEBUG("Cleaning mutex [%p]", mtx);
        pthread_mutex_destroy(&mtx->lock);
    }

    mtx->status = ALPACA_SYNC_INACTIVE;

exit:
    LEAVING;
    return result;
}
//ALPACA_STATUS AlpacaSync_lock(alpaca_mtx_t*);
//ALPACA_STATUS AlpacaSync_trylock(alpaca_mtx_t*, size_t timeout);
//ALPACA_STATUS AlpacaSync_unlock(alpaca_mtx_t*);