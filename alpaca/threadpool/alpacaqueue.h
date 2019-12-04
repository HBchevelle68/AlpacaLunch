#ifndef ALPACAQUEUE_H
#define ALPACAQUEUE_H

typedef void* AL_item_t;

typedef struct AL_queue_node_t{
  AL_item_t item;
  struct AL_queue_node_t* next;
} AL_queue_node_t;

typedef struct{
  AL_queue_node_t* front;
  AL_queue_node_t* back;
  int N;
}AL_queue_t;


/* Initializes the data structure */
void AL_queue_init(AL_queue_t* thisAL_queue);

/* Return 1 if empty, 0 otherwise */
int AL_queue_isempty(AL_queue_t* thisAL_queue);

/* Returns the number of elements in the AL_queue */
int AL_queue_size(AL_queue_t* thisAL_queue);

/* Adds an element to the "back" of the AL_queue */
void AL_queue_enqueue(AL_queue_t* thisAL_queue, AL_item_t item);

/* Removes an element from the "front" of the AL_queue */
AL_item_t AL_queue_dequeue(AL_queue_t* thisAL_queue);

/* Removes the element on the "front" to the "back" of the AL_queue */
void AL_queue_cycle(AL_queue_t* thisAL_queue);

/* Returns the element at the "front" of the AL_queue without removing it*/
AL_item_t AL_queue_peek(AL_queue_t* thisAL_queue);

/* Empties the AL_queue and performs any necessary memory cleanup */
void AL_queue_destroy(AL_queue_t* thisAL_queue);

#endif
