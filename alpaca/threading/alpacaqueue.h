#ifndef ALPACAQUEUE_H
#define ALPACAQUEUE_H

typedef void* AlpacaQueue_item_t;

typedef struct AlpacaQueue_node_t{
  AlpacaQueue_item_t item;
  struct AlpacaQueue_node_t* next;
} AlpacaQueue_node_t;

typedef struct{
  AlpacaQueue_node_t* front;
  AlpacaQueue_node_t* back;
  int N;
}AlpacaQueue_t;


/* Initializes the data structure */
void AlpacaQueue_init(AlpacaQueue_t* thisAlpacaQueue);

/* Return 1 if empty, 0 otherwise */
int AlpacaQueue_isempty(AlpacaQueue_t* thisAlpacaQueue);

/* Returns the number of elements in the AlpacaQueue */
int AlpacaQueue_size(AlpacaQueue_t* thisAlpacaQueue);

/* Adds an element to the "back" of the AlpacaQueue */
void AlpacaQueue_enqueue(AlpacaQueue_t* thisAlpacaQueue, AlpacaQueue_item_t item);

/* Removes an element from the "front" of the AlpacaQueue */
AlpacaQueue_item_t AlpacaQueue_dequeue(AlpacaQueue_t* thisAlpacaQueue);

/* Removes the element on the "front" to the "back" of the AlpacaQueue */
void AlpacaQueue_cycle(AlpacaQueue_t* thisAlpacaQueue);

/* Returns the element at the "front" of the AlpacaQueue without removing it*/
AlpacaQueue_item_t AlpacaQueue_peek(AlpacaQueue_t* thisAlpacaQueue);

/* Empties the AlpacaQueue and performs any necessary memory cleanup */
void AlpacaQueue_destroy(AlpacaQueue_t* thisAlpacaQueue);

#endif
