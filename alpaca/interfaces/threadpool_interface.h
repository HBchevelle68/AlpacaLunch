#ifndef THREADPOOL_INTERFACE_H
#define THREADPOOL_INTERFACE_H

#include <stdint.h>
#include <pthread.h>
//#include <stdatomic.h> // Needed?? 

#include <core/codes.h>
#include <threadpool/alpacaqueue.h>

#define ALPACA_TPOOL_INACTIVE 0
#define ALPACA_TPOOL_ATTRINIT 1
#define ALPACA_TPOOL_LOCKINIT 2
#define ALPACA_TPOOL_READY    4  



typedef struct AlpacaThread
{
  pthread_t   thrd;

  /* data */
}Alpaca_thrd_t;




typedef struct AlpacaThreadPool
{
  Alpaca_thrd_t*      pool;
  pthread_mutex_t     lock;
  pthread_mutexattr_t lock_attr;
  uint8_t  max_threads;
  uint8_t  active_threads;
  uint8_t  status;
} Alpaca_tPool_t;



// thread structure 

// threadpool structure

// init threadpool
ALPACA_STATUS AlpacaThreadpool_initPool(uint8_t max_thrds);

// exit threadpool
ALPACA_STATUS AlpacaThreadpool_teardownPool(void);


// create thread
// cancel thread



#endif