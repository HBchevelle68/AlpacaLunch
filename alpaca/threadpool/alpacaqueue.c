#include <stdlib.h>
#include <stdio.h>
#include <threadpool/alpacaqueue.h>

void AlpacaQueue_queue_init(AlpacaQueue_t *this){
  this->front = NULL;
  this->back = NULL;
  this->N = 0;
}

void AlpacaQueue_queue_enqueue(AlpacaQueue_t* this, AlpacaQueue_item_t item){
  AlpacaQueue_node_t* node;

  node = (AlpacaQueue_node_t*) malloc(sizeof(AlpacaQueue_node_t));
  node->item = item;
  node->next = NULL;

  if(this->back == NULL)
    this->front = node;
  else
    this->back->next = node;

  this->back = node;
  this->N++;
}


int AlpacaQueue_queue_size(AlpacaQueue_t* this){
  return this->N;
}

int AlpacaQueue_queue_isempty(AlpacaQueue_t *this){
  return this->N == 0;
}

AlpacaQueue_item_t AlpacaQueue_queue_dequeue(AlpacaQueue_t* this){
  AlpacaQueue_item_t ans;
  AlpacaQueue_node_t* node;

  if(this->front == NULL){
    fprintf(stderr, "Error: underflow in AlpacaQueue_queue_pop.\n");
    fflush(stderr);
    exit(EXIT_FAILURE);
  }

  node = this->front;
  ans = node->item;

  this->front = this->front->next;
  if (this->front == NULL) this->back = NULL;
  free(node);

  this->N--;

  return ans;
}

void AlpacaQueue_queue_cycle(AlpacaQueue_t* this){
  if(this->back == NULL)
    return;

  this->back->next = this->front;
  this->back = this->front;
  this->front = this->front->next;
  this->back->next = NULL;
}

AlpacaQueue_item_t AlpacaQueue_queue_peek(AlpacaQueue_t* this){
  if(this->front == NULL){
    fprintf(stderr, "Error: underflow in AlpacaQueue_queue_front.\n");
    fflush(stderr);
    exit(EXIT_FAILURE);
  }

  return this->front->item;
}

void AlpacaQueue_queue_destroy(AlpacaQueue_t* this){
  while(!AlpacaQueue_queue_isempty(this))
    AlpacaQueue_queue_dequeue(this);
}
