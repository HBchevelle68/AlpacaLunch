#ifndef THREADPOOL_INTERFACE_H
#define THREADPOOL_INTERFACE_H

#include <stdint.h>
#include <pthread.h>

#include <threadpool/alpacaqueue.h>

/*
 * INTERFACE
 */
//#define threadpool_init 	AlpacaThreadpool_init
//#define threadpool_exit 	AlpacaThreadpool_exit
//#define threadpool_add_task AlpacaThreadpool_add_task





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
  pthread_mutex_t tp_m_lock; // threadpool mutex 
  pthread_cond_t q_cond;  // condition variable for queue of tasks
  uint8_t tp_status;
  uint8_t q_status;
  uint16_t t_size;

} ALtpool_t;



/* tpool_init
	@brief - Initializes a threadpool (ALtpool_t) and
	         returns a pointer to a allocated threadpool with
	         t_count threads available and able to queue up
	         q_size tasks.
	@return - Returns a pointer to a threadpool if successful
	          and returns NULL if an error occured.

	@t_count - Thread count. Number of threads this pool will generate.
*/
extern ALtpool_t* AlpacaThreadpool_init(unsigned int t_count);



/* add_task
	@brief - Adds a task (function) to the threadpool's queue to
	         be exececuted.
	@return - Returns 0 if successful. Returns 1 if error occured
            and task is not added to Queue.

	@tp - Threadpool to add task to.
	@routine - Function to add to queue
	@args - Arguments needed for function. If no args, then pass NULL;
*/
extern int AlpacaThreadpool_add_task(ALtpool_t *tp, void(*routine)(void*), void *args);



/* tpool_exit
	@brief - Begins teardown of threadpool. Sets SHUTDOWN flag which
	         waits for all threads to join. Then deallocates all
	         memory.
	@return - Returns 0 if successful. Returns 1 if error occured
            and cannot guarantee memory was deallocated.

	@tp - Threadpool to teardown.
*/
extern int AlpacaThreadpool_exit(ALtpool_t *tp);



#endif
