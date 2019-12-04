#include <stdlib.h>
#include <stdio.h>
#include <threadpool/alpacaqueue.h>

void AL_queue_init(AL_queue_t *this){
  this->front = NULL;
  this->back = NULL;
  this->N = 0;
}

void AL_queue_enqueue(AL_queue_t* this, AL_item_t item){
  AL_queue_node_t* node;

  node = (AL_queue_node_t*) malloc(sizeof(AL_queue_node_t));
  node->item = item;
  node->next = NULL;

  if(this->back == NULL)
    this->front = node;
  else
    this->back->next = node;

  this->back = node;
  this->N++;
}


int AL_queue_size(AL_queue_t* this){
  return this->N;
}

int AL_queue_isempty(AL_queue_t *this){
  return this->N == 0;
}

AL_item_t AL_queue_dequeue(AL_queue_t* this){
  AL_item_t ans;
  AL_queue_node_t* node;

  if(this->front == NULL){
    fprintf(stderr, "Error: underflow in AL_queue_pop.\n");
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

void AL_queue_cycle(AL_queue_t* this){
  if(this->back == NULL)
    return;

  this->back->next = this->front;
  this->back = this->front;
  this->front = this->front->next;
  this->back->next = NULL;
}

AL_item_t AL_queue_peek(AL_queue_t* this){
  if(this->front == NULL){
    fprintf(stderr, "Error: underflow in AL_queue_front.\n");
    fflush(stderr);
    exit(EXIT_FAILURE);
  }

  return this->front->item;
}

void AL_queue_destroy(AL_queue_t* this){
  while(!AL_queue_isempty(this))
    AL_queue_dequeue(this);
}
