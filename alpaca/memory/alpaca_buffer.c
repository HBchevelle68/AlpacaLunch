#include <interfaces/memory_interface.h>


ALLU_Buffer_t * AlpacaBuffer_init(size_t size){

	ALLU_Buffer_t *buff = NULL;

	if(size > (size_t)MAXMEM){
		return NULL;
	} 	

	return buff;
}

ALPACA_STATUS AlpacaBuffer_free(ALLU_Buffer_t **alluBuffer){
	return ALPACA_SUCCESS;
}

 ALPACA_STATUS  AlpacaBuffer_append(ALLU_Buffer_t **alluBuffer, uint8_t *data);
 ALPACA_STATUS  AlpacaBuffer_resize(ALLU_Buffer_t **alluBuffer, size_t newSize);
 ALPACA_STATUS  AlpacaBuffer_ensureRoom(ALLU_Buffer_t **alluBuffer);
 ALPACA_STATUS  AlpacaBuffer_zero(ALLU_Buffer_t **alluBuffer);