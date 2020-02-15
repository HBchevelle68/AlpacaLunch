/*
 * AlpacaLunch Memory Interface 
#ifdef TALKATIVELLAMA
 */
#ifndef MEMORY_INTERFACE_H
#define MEMORY_INTERFACE_H
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include <core/codes.h>

#define MAXMEM (1024*1024*750) // 750 MB

typedef struct AlpacaBuffer{
	size_t size;
	size_t index;
	uint8_t padding[7];
	uint8_t buffer[0];
} ALLU_Buffer_t;



/*
 * Implemented in alpaca_memory.c
 */
extern void AlpacaMemory_dumpHex(const void* data, size_t size);
extern void* AlpacaMemory_zero(void* buf, size_t bufsize, size_t len);


/*
 * Implemented in alpaca_buffer.c
 */
extern ALLU_Buffer_t* AlpacaBuffer_init(size_t size);
extern ALPACA_STATUS  AlpacaBuffer_free(ALLU_Buffer_t **alluBuffer);
extern ALPACA_STATUS  AlpacaBuffer_append(ALLU_Buffer_t **alluBuffer, uint8_t *data);
extern ALPACA_STATUS  AlpacaBuffer_resize(ALLU_Buffer_t **alluBuffer, size_t newSize);
extern ALPACA_STATUS  AlpacaBuffer_ensureRoom(ALLU_Buffer_t **alluBuffer);
extern ALPACA_STATUS  AlpacaBuffer_zero(ALLU_Buffer_t **alluBuffer);




#endif