#ifndef ALPACA_UNIT_H
#define ALPACA_UNIT_H

#define ONE_KB  (1024)
#define FOUR_KB (1024*4)
#define TEN_KB  (1024*10)

static inline int random_int(int min, int max){
   return min + rand() % (max+1 - min);
}
unsigned char *gen_rdm_bytestream (size_t num_bytes);

int AlpacaUnit_memory_initSuite(void);
int AlpacaUnit_memory_cleanSuite(void);

int AlpacaUnit_comms_initSuite(void);
int AlpacaUnit_comms_cleanSuite(void);


void AlpacaUnit_buffer_base(void);
void AlpacaUnit_buffer_append(void);
void AlpacaUnit_buffer_resize(void);
void AlpacaUnit_buffer_ensureRoom(void);
void AlpacaUnit_buffer_zero(void);

void AlpacaUnit_comms_base(void);
void AlpacaUnit_comms_connect(void);
void AlpacaUnit_comms_send(void);
void AlpacaUnit_comms_listen(void);
void AlpacaUnit_comms_recv(void);
void AlpacaUnit_comms_close(void);


#endif