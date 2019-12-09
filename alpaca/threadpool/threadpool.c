#include <stdio.h>    // std io
#include <string.h>   // memset
#include <stdlib.h>   // malloc and other mem functions

// Internal
#include <threadpool/threadpool.h>


/*
 * This function is broken. Undefined behavior when unlocking and
 * the calling thread does not posses the lock
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

    while(1) {

        if((tp->tp_status & SHUTDOWN)){ 
            break;
        }
        
        pthread_mutex_lock(&(tp->tp_m_lock));
        while(AL_queue_isempty(&(tp->queue)) == 1){
            if((tp->tp_status == GRACEFUL)){
                 threadpool_thread_safe_exit(tp);
            }

            pthread_cond_wait(&(tp->q_cond), &(tp->tp_m_lock));
            if((tp->tp_status == SHUTDOWN)){ 
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


ALtpool_t *tpool_init(unsigned int t_count){

    ALtpool_t *tp;

    // ALLOCATE ALtpool_t struct and zero memory
    if((tp = malloc(sizeof(ALtpool_t))) == NULL) {
        return NULL;
    }
    memset(tp, 0, sizeof(ALtpool_t));


    // INIT THREADS
    if((tp->t_pool = malloc(sizeof(pthread_t) * t_count)) == NULL){
        return NULL;
    }
    memset(tp->t_pool, 0, (sizeof(pthread_t) * t_count));


    // INIT QUEUE
    AL_queue_init(&(tp->queue));


    // INIT LOCK/COND VAR
    pthread_mutex_init(&(tp->tp_m_lock), NULL);
    pthread_cond_init(&(tp->q_cond), NULL);
    

    // START threads
    for(int i = 0; i < t_count; i++) {
        if(pthread_create(&(tp->t_pool[i]), NULL, thread_loop, (void*)tp) != 0) {
            return NULL;
        }
    }


    // Finish up setting other vars
    pthread_mutex_lock(&(tp->tp_m_lock));
    tp->t_size = t_count;
    tp->q_status = EMPTY;
    tp->tp_status = INITIALIZED;
    pthread_mutex_unlock(&(tp->tp_m_lock));


    return tp;
}



int threadpool_add_task(ALtpool_t *tp, void (*routine)(void*), void *args){

    if(!tp){
        return -1;
    }
    
    //AL_item_t item;
    ALtask_t *task = malloc(sizeof(ALtask_t));

    /*
     * Set task func ptr and func args 
     */
    task->routine = routine;
    task->args = args;
    //item = (void*)task;

    pthread_mutex_lock(&(tp->tp_m_lock));
    AL_queue_enqueue(&(tp->queue), (AL_item_t)task);
    tp->q_status = TODO;

    pthread_mutex_unlock(&(tp->tp_m_lock)); // UNLOCK
    pthread_cond_broadcast(&(tp->q_cond));  // BCAST

    return 0;
}



/*
    @brief - Private function called by threadpool_exit()
             waits for threads to join and frees memory
             and locks in proper order.

    @tp - Threadpool to teardown
*/
int threadpool_free_pool(ALtpool_t *tp){

    /* 
     * Make sure thread pool got allocated
     * Repeat process of checking for allocated memory
     * Free if memory allocated
     */ 
    if(tp != NULL){
        
        pthread_cond_broadcast(&(tp->q_cond));
        // Wait for threads to join
        for(int i = 0; i < tp->t_size; i++) {
            pthread_join(tp->t_pool[i], NULL);
        }

        // All threads are done, now safe to destroy locks`
        pthread_mutex_destroy(&(tp->tp_m_lock));
        pthread_cond_destroy(&(tp->q_cond));


        // Clean thread memory
        if(tp->t_pool != NULL){
            free(tp->t_pool);
        }

        // Clean queue memory
        AL_queue_destroy(&(tp->queue));

        // Finally lets clean up threadpool memory
        free(tp);
    }

    return 0;
}



int threadpool_exit(ALtpool_t *tp){

    tp->tp_status = SHUTDOWN;
    if(threadpool_free_pool(tp)){
        printf("ERROR in free_pool()\n");
        return 1;
    }
    return 0;
}
