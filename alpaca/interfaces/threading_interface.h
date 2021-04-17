#ifndef THREADPOOL_INTERFACE_H
#define THREADPOOL_INTERFACE_H

#include <stdint.h>
#include <pthread.h>
//#include <stdatomic.h> // Needed?? 

#include <core/codes.h>
#include <threading/alpacaqueue.h>

#define ALPACA_THRD_INACTIVE 0
#define ALPACA_THRD_POOLINIT 1
#define ALPACA_THRD_ATTRINIT 2
#define ALPACA_THRD_LOCKINIT 4
#define ALPACA_THRD_READY    8  

/*
 * Remade for clarity
 */ 
#define ALPACA_SYNC_INACTIVE ALPACA_THRD_INACTIVE
#define ALPACA_SYNC_ATTRINIT ALPACA_THRD_ATTRINIT
#define ALPACA_SYNC_LOCKINIT ALPACA_THRD_LOCKINIT


/**
 * @brief Alpaca Mutex structure
 * 
 * lock - mutex
 * lock_attr - mutex attributes
 */
typedef struct AlpacaMutex {
    pthread_mutex_t     lock;
    pthread_mutexattr_t lock_attr;
    uint8_t status;
} alpaca_mtx_t;

#define ALPACA_MTX_SIZE sizeof(struct AlpacaMutex)

ALPACA_STATUS AlpacaSync_init(alpaca_mtx_t* lock);
ALPACA_STATUS AlpacaSync_lock(alpaca_mtx_t* lock);
ALPACA_STATUS AlpacaSync_trylock(alpaca_mtx_t* lock, size_t timeout);
ALPACA_STATUS AlpacaSync_unlock(alpaca_mtx_t* lock);
ALPACA_STATUS AlpacaSync_destroy(alpaca_mtx_t* lock);




/**
 * @brief Alpaca thread structure
 * 
 * thrd - posix thread
 * aptid - alapaca threadmanager ID
 * active - bool, is the thread active
 */
typedef struct AlpacaThread {

  pthread_t thrd;
  uint8_t   aptid;
  uint8_t   active;

} alpaca_thrd_t;

#define ALPACA_THRD_SIZE (sizeof(struct AlpacaThread))



/**
 * @brief Alpaca Threadpool structure
 *        
 * @note  Exepected to be used as an event based
 *        threadmanager 
 * 
 * 
 * pool - allocated array of alpaca_thrd_t's
 *        These are our actual threads
 * lock - mutex for pool access 
 * lock_attr - attricutes of aformentioned lock
 * max_threads - max alloed threads for this pool
 * active_threads - number of actively executing threads
 * status - bit flag of threadmanager status 
 * 
 */
typedef struct AlpacaThreadManager {
  
  alpaca_thrd_t*      pool;
  pthread_mutex_t     lock;
  pthread_mutexattr_t lock_attr;
  uint8_t  max_threads;
  uint8_t  active_threads;
  uint8_t  status;

} alpaca_tmngr_t;


// init threadmanager
ALPACA_STATUS AlpacaThreadManager_initPool(uint8_t max_thrds);

// exit threadmanager
ALPACA_STATUS AlpacaThreadManager_teardownPool(void);


// create thread
// cancel thread
// reap thread stacks



#endif