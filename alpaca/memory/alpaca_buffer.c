
#include <core/logging.h>
#include <interfaces/memory_interface.h>


ALLU_Buffer_t * AlpacaBuffer_init(size_t size){

	ALLU_Buffer_t *buff = NULL;

	// Make sure to not go past the MAXMEM 
	if(size > (size_t)MAXMEM){
		return NULL;
	}

	// Alloc 
	buff = (ALLU_Buffer_t *)calloc((sizeof(ALLU_Buffer_t) + size), sizeof(uint8_t));
	if(!buff){
		LOGERROR("Failed to calloc\n");
		return NULL;
	}
	// Set up vars
	buff->size = size;
	buff->index = 0;

	return buff;
}


ALPACA_STATUS AlpacaBuffer_free(ALLU_Buffer_t **alluBuffer){

	/*
	 * Invalid buffer passed
	 */
	if(!(*alluBuffer)){
		return ALPACA_MEM_NOBUFFER;
	}
	/*
	 * Zero out the memory section for some added security
	 * Free the structure to include the buffer itself
	 * Set the ptr to NULL
	 */ 
	AlpacaMemory_zero((*alluBuffer)->buffer, (*alluBuffer)->size , ((*alluBuffer)->size - (*alluBuffer)->index));
	free(*alluBuffer);
	*alluBuffer = NULL;

	return ALPACA_SUCCESS;
}

 ALPACA_STATUS  AlpacaBuffer_append(ALLU_Buffer_t **alluBuffer, uint8_t *data);
 ALPACA_STATUS  AlpacaBuffer_resize(ALLU_Buffer_t **alluBuffer, size_t newSize);
 ALPACA_STATUS  AlpacaBuffer_ensureRoom(ALLU_Buffer_t **alluBuffer);

 ALPACA_STATUS  AlpacaBuffer_zero(ALLU_Buffer_t **alluBuffer){
 		/*
	 * Invalid buffer passed
	 */
	if(!(*alluBuffer)){
		return ALPACA_MEM_NOBUFFER;
	}
	AlpacaMemory_zero((*alluBuffer)->buffer, (*alluBuffer)->size , ((*alluBuffer)->size - (*alluBuffer)->index));
	
	return ALPACA_SUCCESS;
 }