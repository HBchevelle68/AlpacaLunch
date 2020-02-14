#include <interfaces/memory_interface.h>

// TODO

void* AlpacaMemory_zero(void* buf, size_t bufsize, size_t len){
	
	if(len > bufsize){
		len = bufsize;
	}
	volatile uint8_t * ptr = buf;
	while(len--){
		*ptr++ = (uint8_t)0;
	}
	return buf;
}