#ifndef ALPACA_UNIT_H
#define ALPACA_UNIT_H

#define ONE_KB  (1024)
#define FOUR_KB (1024*4)
#define TEN_KB  (1024*10)

int AlpacaUnit_memory_initSuite(void);
int AlpacaUnit_memory_cleanSuite(void);
void AlpacaUnit_buffer_base(void);
void AlpacaUnit_buffer_append(void);
void AlpacaUnit_buffer_resize(void);
void AlpacaUnit_buffer_ensureRoom(void);




#endif