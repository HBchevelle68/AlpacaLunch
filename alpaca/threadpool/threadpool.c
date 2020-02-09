#include <stdio.h> 
#include <string.h>
#include <stdlib.h>

// Internal
#include <core/logging.h>


// Implements Interface 
#include <interfaces/threadpool_interface.h>


// For Atomics
static const uint8_t FLAGON  = 1;
static const uint8_t FLAGOFF = 0;



/*
 * This function is broken. Undefined behavior when unlocking and
 * the calling thread does not posses the lock
 * Atomics??? 
 */
static 
void threadpool_thread_safe_exit(ALtpool_t *tp){
    pthread_mutex_unlock(&(tp->tp_m_lock));
    pthread_exit(NULL);
}



/*
    ALL threads will be constantly running this function
    They will loop and check to see if a job
    is in Queue. If there is a job it will grab the job
    and execute.
*/
static 
void *thread_loop(void *threadpool){

    ALtpool_t *tp = (ALtpool_t *)threadpool;
    ALtask_t *to_execute;
    uint8_t tearDownRet;

    while(1) {
       
        pthread_mutex_lock(&(tp->tp_m_lock));
        while(AL_queue_isempty(&(tp->queue))){
            // Clear flag out variable
            tearDownRet = 0;

            pthread_cond_wait(&(tp->q_cond), &(tp->tp_m_lock));

            /*
             * Atomically load flag and check if teardown requested 
             */
            __atomic_load(&(tp->teardown), &tearDownRet, __ATOMIC_SEQ_CST);
            if(tearDownRet){
                // Exit
                threadpool_thread_safe_exit(tp);
            }
        }

        // Pop task from Queue and release lock
        to_execute = AL_queue_dequeue(&(tp->queue));
        pthread_mutex_unlock(&(tp->tp_m_lock));

        // Begin execution of function
        (to_execute->routine)(to_execute->args);
        free(to_execute);
    }

    // Only gets here on 1st IF statement
    pthread_exit(NULL);
}

static
ALPACA_STATUS createThreads(ALtpool_t *tp){

    for(int i = 0; i < tp->t_size; i++) {
        sprintf( &(tp->t_pool[i].name[0]), "Thread--%d", i);
        // If any threads fail to be created, time to go
        if(pthread_create(&(tp->t_pool[i].thread), NULL, thread_loop, (void*)tp) != 0) {
            AlpacaThreadpool_exit(tp);
            return ALPACA_FAILURE;
        }
    }

    return ALPACA_SUCCESS;
} 


/* tpool_init
 *    @brief - Initializes a threadpool (ALtpool_t) and
 *             returns a pointer to a allocated threadpool with
 *             t_count threads available and able to queue up
 *             q_size tasks.
 *    @return - Returns a pointer to a threadpool if successful
 *              and returns NULL if an error occured.
 *
 *    @param t_count - Thread count. Number of threads this pool will generate.
 */
ALtpool_t* AlpacaThreadpool_init(unsigned int t_count){

    ALtpool_t *tp;

    // ALLOCATE ALtpool_t struct and zero memory
    if((tp = malloc(sizeof(ALtpool_t))) == NULL) {
        return NULL;
    }
    memset(tp, 0, sizeof(ALtpool_t));


    // INIT THREADS MEM
    if((tp->t_pool = malloc(sizeof(ALthread_t) * t_count)) == NULL){
        AlpacaThreadpool_exit(tp);
        return NULL;
    }
    memset(tp->t_pool, 0, (sizeof(ALthread_t) * t_count));


    // INIT QUEUE
    AL_queue_init(&(tp->queue));


    // INIT LOCK/COND VARIABLE
    pthread_mutex_init(&(tp->tp_m_lock), NULL);
    pthread_cond_init(&(tp->q_cond), NULL);

    // SET THREAD COUNT
    tp->t_size = t_count;
    __atomic_fetch_and(&(tp->teardown), &FLAGOFF, __ATOMIC_SEQ_CST);

    // START threads
    if(createThreads(tp) != ALPACA_SUCCESS){
        AlpacaThreadpool_exit(tp);
        return NULL;
    }

    return tp;
}


/* add_task
    @brief - Adds a task (function) to the threadpool's queue to
             be exececuted.
    @return - Returns 0 if successful. Returns 1 if error occured
            and task is not added to Queue.

    @param tp - Threadpool to add task to.
    @param routine - Function to add to queue
    @param args - Arguments needed for function. If no args, then pass NULL;
*/
int AlpacaThreadpool_addTask(ALtpool_t *tp, void (*routine)(void*), void *args, char* name){

    if(!tp){
        return ALPACA_FAILURE;
    }
    
    ALtask_t *task = malloc(sizeof(ALtask_t));

    /*
     * Set task func ptr and func args 
     */
    task->routine = routine;
    task->args = args;
    strcpy(task->name, name);
   

    pthread_mutex_lock(&(tp->tp_m_lock));
    AL_queue_enqueue(&(tp->queue), (AL_item_t)task);

    pthread_mutex_unlock(&(tp->tp_m_lock)); // UNLOCK
    pthread_cond_broadcast(&(tp->q_cond));  // BCAST

    return ALPACA_SUCCESS;
}



/*
    @brief - Private function called by threadpool_exit()
             waits for threads to join and frees memory
             and locks in proper order.

    @param tp - Threadpool to teardown

*********************************************************************
    @bug this currently assumes tp is fully allocated and running
         does not account for if a failue occured durin INIT
*/
static
int threadpool_free_pool(ALtpool_t *tp){


    /* 
     * Make sure thread pool got allocated
     * Repeat process of checking for allocated memory
     * Free if memory allocated
     */ 
    if(tp != NULL){
       /*
        * Atomically store 0x01 into threadpool teardown flag
        * then broadcast to sleeping threads to wake.
        * After threads join, atomically store 0x00 into
        * threadpool teardown flag
        */
        if(tp->t_pool != NULL){
        

            __atomic_store(&(tp->teardown), &FLAGON, __ATOMIC_SEQ_CST);

 
            for(int i = 0; i < tp->t_size; i++) {
               /*
                * Wait for threads to join
                */
                pthread_cond_broadcast(&(tp->q_cond));
                pthread_join(tp->t_pool[i].thread, NULL);

            }
            __atomic_fetch_and(&(tp->teardown), &FLAGOFF, __ATOMIC_SEQ_CST);

            /*
             * Clean thread memory
             */ 
            free(tp->t_pool);
        }

        /*
         * All threads are done, now safe to destroy locks
         */ 
        pthread_mutex_destroy(&(tp->tp_m_lock));
        pthread_cond_destroy(&(tp->q_cond));
        
        /*
         * Clean queue memory
         */ 
        AL_queue_destroy(&(tp->queue));

        /*
         * Finally lets clean up threadpool memory 
         */ 

        free(tp);
        tp = NULL;
    }

    return ALPACA_SUCCESS;
}


/* tpool_exit
    @brief - Begins teardown of threadpool. Sets SHUTDOWN flag which
             waits for all threads to join. Then deallocates all
             memory.
    @return - Returns 0 if successful. Returns 1 if error occured
            and cannot guarantee memory was deallocated.

    @param tp - Threadpool to teardown.
*/
int AlpacaThreadpool_exit(ALtpool_t *tp){


    if(threadpool_free_pool(tp)){
        printf("ERROR in free_pool()\n");
        return 1;
    }
    return ALPACA_SUCCESS;
}
