/*
 * AlpacaLunch Memory Interface 
 *
 */
#ifndef MEMORY_INTERFACE_H
#define MEMORY_INTERFACE_H
#include <stddef.h>
#include <stdint.h>

#include <core/codes.h>


typedef struct AlpacaBuffer{
	size_t size;
	size_t index;
	uint8_t buffer[0];
} ALLU_Buffer_t;

extern void* AlpacaMemory_zero(void* buf, size_t bufsize, size_t len);

extern ALLU_Buffer_t * AlpacaBuffer_init(size_t size);

extern ALPACA_STATUS AlpacaBuffer_free(size_t size);


#endif