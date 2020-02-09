#ifndef THREADPOOL_INTERFACE_H
#define THREADPOOL_INTERFACE_H

#include <stdint.h>
#include <pthread.h>
#include <stdatomic.h>

#include <core/codes.h>
#include <threadpool/alpacaqueue.h>




// Struct that holds the function and arguments to execute
typedef struct AlpacaLunch_task{
    void (*routine)(void *);
    void *args;
    char name[256];
} ALtask_t;

typedef struct AlpacaLunch_thread {
  pthread_t thread;
  char name[256]; 
} ALthread_t;



/*
    The core pool

    pthread_t *t_pool       - Thread array
    AL_queue_t *queue           - Queue holding tasks
    pthread_mutex_t q_lock  - Mutex to access queue of tasks
    pthread_mutex_t q_cond  - Condition variable for Queue
    uint16_t t_size         - Number of threads for pool
    uint16_t q_size         - Number of tasks for pool
*/
typedef struct AlpacaLunch_threadpool {
  
  ALthread_t *t_pool;
  AL_queue_t queue;
  pthread_mutex_t tp_m_lock;
  pthread_cond_t q_cond;  	 
  uint16_t t_size;
  uint8_t teardown;
} ALtpool_t;


extern ALtpool_t* AlpacaThreadpool_init(uint32_t t_count);

extern int32_t AlpacaThreadpool_addTask(ALtpool_t *tp, void (*routine)(void*), void *args, char* name);

extern int32_t AlpacaThreadpool_listThreads(ALtpool_t *tp);

extern int32_t AlpacaThreadpool_exit(ALtpool_t *tp);



#endif
