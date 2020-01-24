#ifndef THREADPOOL_INTERFACE_H
#define THREADPOOL_INTERFACE_H

#include <stdint.h>
#include <pthread.h>

#include <threadpool/alpacaqueue.h>




/* **** TO DO ****
	
	in need of cleaning and tpool struct minor rework

 */


enum STATUS {
	NOTRUNNING  = 0x00,
	INITIALIZED = 0x01,
	GRACEFUL    = 0x02,
	SHUTDOWN    = 0x04
};



enum Q_STATUS{
	TODO = 	0,
	EMPTY = 1
};




// Struct that holds the function and arguments to execute
typedef struct AlpacaLunch_task{
    void (*routine)(void *);
    void *args;
} ALtask_t;



/*
    The core pool

    pthread_t *t_pool       - Thread array
    AL_queue_t *queue           - Queue holding tasks
    pthread_mutex_t q_lock  - Mutex to access queue of tasks
    pthread_mutex_t q_cond  - Condition variable for Queue
    uint8_t tp_status       - Threadpool status flags
    uint8_t q_status        - Queue status flags
    uint16_t t_size         - Number of threads for pool
    uint16_t q_size         - Number of tasks for pool
*/
typedef struct AlpacaLunch_threadpool {
  
  pthread_t *t_pool;
  AL_queue_t queue;
  pthread_mutex_t tp_m_lock;
  pthread_cond_t q_cond;  	 
  uint8_t tp_status;
  uint8_t q_status;
  uint16_t t_size;

} ALtpool_t;


extern ALtpool_t* AlpacaThreadpool_init(unsigned int t_count);

extern int AlpacaThreadpool_add_task(ALtpool_t *tp, void(*routine)(void*), void *args);

extern int AlpacaThreadpool_exit(ALtpool_t *tp);



#endif
