#define _XOPEN_SOURCE 700 //https://pubs.opengroup.org/onlinepubs/9699919799/
#include <pthread.h>
#include <errno.h>
#include <time.h>

#include <core/logging.h>
#include <interfaces/threading_interface.h>


ALPACA_STATUS AlpacaSync_init(alpaca_mtx_t* mtx){

    ALPACA_STATUS result = ALPACA_SUCCESS;
    ENTRY;


    /*
     * Verify paramters
     */
    if(NULL == mtx) {
        LOGERROR("Bad lock variable passed, make sure its not still valid mtx[%p]\n", mtx );
        result = ALPACA_ERROR_BADPARAM;
        goto exit;
    } 

    if(ALPACA_SYNC_INACTIVE != mtx->status){
        LOGERROR("Mutex is either active or in bad state mtx->status[%u]!\n", mtx->status);
        result = ALPACA_ERROR_BADPARAM;
        goto exit;
    }

    /*
     * Init w/ default attributes. Interally, 
     * this is allocating and will need 
     * deallocation
     */
    if(pthread_mutexattr_init(&mtx->lock_attr)){
        LOGERROR("Error initializing mutex attributes! [%d]\n", errno);
        result = ALPACA_ERROR_MTXATTRINIT;
        goto exit;
    }
    mtx->status |= ALPACA_SYNC_ATTRINIT;

    /* Set type to error checking 
     * Provides some deadlock protection
     * See man pages for details
     */ 
    if(pthread_mutexattr_settype(&mtx->lock_attr, PTHREAD_MUTEX_ERRORCHECK)){
        LOGERROR("Error setting mutex type attributes! [%d]\n", errno);
        result = ALPACA_ERROR_MTXATTRINIT;
        goto exit;
    }
    /* Set robustness to Robust 
     * Alerts if orig owner died with lock held
     * See man pages for details
     */ 
    if(pthread_mutexattr_setrobust(&mtx->lock_attr, PTHREAD_MUTEX_ROBUST)){
        LOGERROR("Error setting mutex robustness attributes! [%d]\n", errno);
        result = ALPACA_ERROR_MTXATTRINIT;
        goto exit;
    }

    if(pthread_mutex_init(&mtx->lock, &mtx->lock_attr)){
        LOGERROR("Error initializing mutex![%d]\n", errno);
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
        LOGERROR("Bad lock variable passed, make sure its not still valid mtx[%p]\n", mtx );
        result = ALPACA_ERROR_BADPARAM;
        goto exit;
    } 

    if(ALPACA_SYNC_INACTIVE == mtx->status){
        LOGERROR("Mutex is inactive cannot destroy mtx[%p] mtx->status[%u]!\n", mtx, mtx->status);
        result = ALPACA_ERROR_BADPARAM;
        goto exit;
    }

    /*
     * Check status flags to prevent any 
     * double free's
     */
    if(ALPACA_SYNC_ATTRINIT && mtx->status){
        LOGDEBUG("Cleaning mutex [%p] attribute structure\n", mtx);
        pthread_mutexattr_destroy(&mtx->lock_attr);
    }

    if(ALPACA_SYNC_LOCKINIT && mtx->status){
        LOGDEBUG("Cleaning mutex [%p]\n", mtx);
        pthread_mutex_destroy(&mtx->lock);
    }

    mtx->status = ALPACA_SYNC_INACTIVE;

exit:
    LEAVING;
    return result;
}

static
void syncCheckError(void){
    ENTRY;

    switch (errno) {
        case EINVAL:
            LOGERROR("Lock or unlock attempted on uninitialized mutex\n");
            break;

        case EBUSY:
            LOGERROR("Lock is currently owned by another thread\n");
            break;

        case EDEADLK:
            LOGERROR("Deadlock detected or This thread attempted to lock an already owned lock");
            break;

        case EPERM:
            LOGERROR("Current thread does not own mutex");
            break;

        case ETIMEDOUT:
            LOGERROR("Timed out waiting for lock\n");
            break;

        case EOWNERDEAD:
            LOGERROR("*** Lock was left locked by previous thread when thread terminated ***\n");
            break;

        default:
            LOGERROR("Error occured during lock or unlock with unknown calue [%d]\n", errno);    
            break;
    }

    LEAVING;
}

ALPACA_STATUS AlpacaSync_lock(alpaca_mtx_t* mtx){
    ALPACA_STATUS result = ALPACA_SUCCESS;
    ENTRY;
    /*
     * Verify paramters
     */
    if(NULL != mtx && (ALPACA_SYNC_LOCKINIT && mtx->status)) {
        LOGERROR("Bad lock variable passed, make sure its not still valid mtx[%p]\n", mtx );
        result = ALPACA_ERROR_BADPARAM;
        goto exit;
    }

    if(0 != pthread_mutex_lock(&mtx->lock)){
        syncCheckError();
        result = ALPACA_ERROR_MTXLOCK;
        goto exit;
    }

exit:
    LEAVING;
    return result;
}
ALPACA_STATUS AlpacaSync_trylock(alpaca_mtx_t* mtx){
    ALPACA_STATUS result = ALPACA_SUCCESS;
    ENTRY;
    /*
     * Verify paramters
     */
    if(NULL != mtx && (ALPACA_SYNC_LOCKINIT && mtx->status)) {
        LOGERROR("Bad lock variable passed, make sure its not still valid mtx[%p]\n", mtx );
        result = ALPACA_ERROR_BADPARAM;
        goto exit;
    }

    if(0 != pthread_mutex_trylock(&mtx->lock)){
        if(EBUSY == errno){
            /* This is part of the expected behavior 
             * of trylock, no need to check errors
             * for print
             */
            result = ALPACA_ERROR_MTXBUSY;
        }
        else {
            syncCheckError();
        }
        goto exit;
    }


exit:
    LEAVING;
    return result;
}

ALPACA_STATUS AlpacaSync_timelock(alpaca_mtx_t* mtx, time_t sec, long nanosec){
    ALPACA_STATUS result = ALPACA_SUCCESS;
    struct timespec timeout = {.tv_sec=sec, .tv_nsec=nanosec};
    ENTRY;

    LOGDEBUG("If lock unavailable will sleep for %lld.%.9ld", (long long)timeout.tv_sec, timeout.tv_nsec);

    /*
     * Verify paramters
     */
    if(NULL != mtx && (ALPACA_SYNC_LOCKINIT && mtx->status)) {
        LOGERROR("Bad lock variable passed, make sure its not still valid mtx[%p]\n", mtx );
        result = ALPACA_ERROR_BADPARAM;
        goto exit;
    }

    if(0 != pthread_mutex_timedlock(&mtx->lock, &timeout)){
        if(ETIMEDOUT == errno){
            /* This is part of the expected behavior 
             * of timedlock, no need to check errors
             */
            result = ALPACA_ERROR_MTXTIMEOUT;
        }
        else {
            syncCheckError();
        }
        goto exit;
    }

exit:
    LEAVING;
    return result;
}

ALPACA_STATUS AlpacaSync_unlock(alpaca_mtx_t* mtx){
    ALPACA_STATUS result = ALPACA_SUCCESS;
    ENTRY;
    /*
     * Verify paramters
     */
    if(NULL != mtx && (ALPACA_SYNC_LOCKINIT && mtx->status)) {
        LOGERROR("Bad lock variable passed, make sure its not still valid mtx[%p]\n", mtx );
        result = ALPACA_ERROR_BADPARAM;
        goto exit;
    }

    if(0 != pthread_mutex_unlock(&mtx->lock)){
        syncCheckError();
        result = ALPACA_ERROR_MTXUNLOCK;
        goto exit;
    }

exit:
    LEAVING;
    return result;
}