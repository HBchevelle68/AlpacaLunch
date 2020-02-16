
#include <core/logging.h>
#include <interfaces/memory_interface.h>

/*
 *	@brief initializes a ALLU_Buffer_t structure which 
 *		   represnts a ALLU buffer
 *	
 *	@param size the size of the memory pool required
 *	@return ALLU_Buffer_t pointer to struct
 */
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

/*
 *	@brief free's the respective buffer and set ptr to NULL
 *	
 *	@param alluBuffer by ref ptr to ALLU_Buffer_t* struct
 *	@return ALPACA_STATUS
 */
ALPACA_STATUS AlpacaBuffer_free(ALLU_Buffer_t **alluBuffer){
	
	/*
	 * Check if invalid buffer passed
	 */
	if(!(*alluBuffer)){
		return ALPACA_MEM_NOBUFFER;
	}

	/*
	 * Zero out the memory section for some added security
	 * Free the structure to include the buffer itself
	 */ 
	LOGDEBUG("Buffer [%p] Size [%lu] Used [%lu]\n", *alluBuffer, (*alluBuffer)->size, (*alluBuffer)->index);
	AlpacaMemory_zero((*alluBuffer)->buffer, (*alluBuffer)->size , ((*alluBuffer)->size - (*alluBuffer)->index));
	free(*alluBuffer);

	/*
	 * Set the ptr to NULL
	 */
	*alluBuffer = NULL;

	return ALPACA_SUCCESS;
}

/*
 *	@brief Appends the data for a given size to the respective
 *		   ALLU_Buffer_t struct. 
 *
 *		   If there is not enough room
 *		   AlpacaBuffer_resize is called under the hood in 
 *		   AlpacaBuffer_ensureRoom. Be aware the pointer will
 *		   change!! 
 *		   
 *	
 *	@param alluBuffer by ref ptr to ALLU_Buffer_t* struct
 *	@param data by ptr to src data to copy from
 *	@param size amount from data to copy from
 *	@return ALPACA_STATUS
 */
ALPACA_STATUS AlpacaBuffer_append(ALLU_Buffer_t **alluBuffer, uint8_t *data, size_t size){

	/*
	 * Check if invalid buffer passed
	 */
	if(!(*alluBuffer)){
		return ALPACA_MEM_NOBUFFER;
	}
	/*
	 * Make sure there is enough memory to handle the transfer
	 * acquire more memory if not
	 */
	if((AlpacaBuffer_ensureRoom(alluBuffer, size)) != ALPACA_SUCCESS){

		return ALPACA_MEM_APNDFAIL;
	}
	LOGDEBUG("Buffer [%p] Size [%lu] Used [%lu]\n", *alluBuffer, (*alluBuffer)->size, (*alluBuffer)->index);
	
	// Perform the transfer 
	memmove(((*alluBuffer)->buffer + (*alluBuffer)->index), data, size);
	
	// Adjust the cursor accordingly 
	(*alluBuffer)->index += size;

	return ALPACA_SUCCESS;	
}

/*
 *	@brief Resize the respective structure. This can be used
 *		   to SHRINK a ALLU_Buffer as well grow
 *	
 *	@warning if used to shrink, no data is taken from 
 *			 previous buffer state
 *	
 *	@param alluBuffer by ref ptr to ALLU_Buffer_t* struct
 *	@param memNeeded amount of memory needed
 *	@return ALPACA_STATUS
 */
ALLU_Buffer_t * AlpacaBuffer_resize(ALLU_Buffer_t **alluBuffer, size_t memNeeded){
 	/*
	 * Check for invalid buffer passed
	 * Init new buffer and check for success
	 */
	ALLU_Buffer_t* tmp = NULL;
	if(!(*alluBuffer) || !(tmp = AlpacaBuffer_init(memNeeded))) {
		return NULL;
	}
	
	/*
	 * Update the new index to the proper position
	 * Copy over the old buffer into the new
	 */
	if(tmp->size > (*alluBuffer)->size){
		/*
		 * Only here if the buffer did NOT shrink
		 */
		tmp->index = (*alluBuffer)->index;
		memcpy(tmp->buffer, (*alluBuffer)->buffer, (*alluBuffer)->index);
	}
	else{
		tmp->index = 0;
	}
	/*
	 * Free the old structure 
	 */
	if(AlpacaBuffer_free(alluBuffer) != ALPACA_SUCCESS){
		LOGERROR("ERROR DURING FREE\n");
		AlpacaBuffer_free(&tmp);
		return NULL;
	}

	return tmp;
}
 
ALPACA_STATUS AlpacaBuffer_ensureRoom(ALLU_Buffer_t **alluBuffer, size_t memNeeded){

	/*
	 * Check if invalid buffer passed
	 */
	if(!(*alluBuffer)){
		return ALPACA_MEM_NOBUFFER;
	}

	/*
	 * Check if the amount needed is greater than the amount
	 * avalaible. If so, rezize the structure accordingly
	 */
 	if( memNeeded > ((*alluBuffer)->size - (*alluBuffer)->index)){
 		
 		size_t newSize = (memNeeded - (((*alluBuffer)->size - (*alluBuffer)->index)) + (*alluBuffer)->size);
 		if(((*alluBuffer) = AlpacaBuffer_resize(alluBuffer, newSize)) == NULL){
 			
 			// Failure occured during resize
 			return ALPACA_MEM_RESZFAIL;
 		}
 	}

	return ALPACA_SUCCESS;
}

ALPACA_STATUS  AlpacaBuffer_zero(ALLU_Buffer_t **alluBuffer){

	/*
	 * Check if invalid buffer passed
	 */
	if(!(*alluBuffer)){
		return ALPACA_MEM_NOBUFFER;
	}

	AlpacaMemory_zero((*alluBuffer)->buffer, (*alluBuffer)->size , ((*alluBuffer)->size - (*alluBuffer)->index));
	(*alluBuffer)->index = 0;

	return ALPACA_SUCCESS;
}