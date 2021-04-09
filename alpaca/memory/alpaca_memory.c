/*
 * @file This file contains general memory i/o and debug functions 
 *
 */
#include <interfaces/memory_interface.h>



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

#ifdef TALKATIVE_ALPACA
#include <stdio.h>
void AlpacaMemory_dumpHex(const void* data, size_t size) {
    char ascii[17];
    size_t i, j;
    ascii[16] = '\0';
    for (i = 0; i < size; ++i) {
        printf("%02X ", ((unsigned char*)data)[i]);
        if (((unsigned char*)data)[i] >= ' ' && ((unsigned char*)data)[i] <= '~') {
            ascii[i % 16] = ((unsigned char*)data)[i];
        } else {
            ascii[i % 16] = '.';
        }
        if ((i+1) % 8 == 0 || i+1 == size) {
            printf(" ");
            if ((i+1) % 16 == 0) {
                printf("|  %s \n", ascii);
            } else if (i+1 == size) {
                ascii[(i+1) % 16] = '\0';
                if ((i+1) % 16 <= 8) {
                    printf(" ");
                }
                for (j = (i+1) % 16; j < 16; ++j) {
                    printf("   ");
                }
                printf("|  %s \n", ascii);
            }
        }
    }
}
#else
void AlpacaMemory_dumpHex(const void* data, size_t size){return;}
#endif