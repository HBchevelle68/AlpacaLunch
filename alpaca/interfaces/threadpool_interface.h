#ifndef THREADPOOL_INTERFACE_H
#define THREADPOOL_INTERFACE_H

#include <stdint.h>
#include <pthread.h>
//#include <stdatomic.h> // Needed?? 

#include <core/codes.h>
#include <threadpool/alpacaqueue.h>

#define ALPACA_TPOOL_INACTIVE 0
#define ALPACA_TPOOL_POOLINIT 1
#define ALPACA_TPOOL_ATTRINIT 2
#define ALPACA_TPOOL_LOCKINIT 4
#define ALPACA_TPOOL_READY    8  


/**
 * @brief Alpaca thread structure
 * 
 * thrd - posix thread
 * aptid - alapaca threadpool ID
 * active - bool, is the thread active
 */
typedef struct AlpacaThread {

  pthread_t thrd;
  uint8_t   aptid;
  uint8_t   active;

} Alpaca_thrd_t;

#define ALPACA_THRD_SIZE (sizeof(Alpaca_thrd_t))



/**
 * @brief Alpaca Threadpool structure
 *        
 * @note  Exepected to be used as an event based
 *        threadpool 
 * 
 * 
 * pool - allocated array of Alpaca_thrd_t's
 *        These are our actual threads
 * lock - mutex for pool access 
 * lock_attr - attricutes of aformentioned lock
 * max_threads - max alloed threads for this pool
 * active_threads - number of actively executing threads
 * status - bit flag of threadpool status 
 * 
 */
typedef struct AlpacaThreadPool {
  
  Alpaca_thrd_t*      pool;
  pthread_mutex_t     lock;
  pthread_mutexattr_t lock_attr;
  uint8_t  max_threads;
  uint8_t  active_threads;
  uint8_t  status;

} Alpaca_tPool_t;


// init threadpool
ALPACA_STATUS AlpacaThreadpool_initPool(uint8_t max_thrds);

// exit threadpool
ALPACA_STATUS AlpacaThreadpool_teardownPool(void);


// create thread
// cancel thread
// reap thread stacks



#endif