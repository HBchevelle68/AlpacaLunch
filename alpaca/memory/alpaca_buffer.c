#include <string.h>

#include <core/logging.h>
#include <interfaces/memory_interface.h>

/**
 *	@brief initializes a ALLU_Buffer_t structure which 
 *		   represnts a ALLU buffer
 *	
 *  @param allubuffer ptr to ALLU_Buffer_t ptr 
 *	@param size the size of the memory pool required
 *	@return ALPACA_STATUS
 */
ALPACA_STATUS AlpacaBuffer_init(ALLU_Buffer_t **alluBuffer, size_t size){

	ALPACA_STATUS result = ALPACA_SUCCESS;

	// Make sure to not go past the MAXMEM 
	if(size > (size_t)MAXMEM){
		result = ALPACA_ERROR_BADPARAM;
		goto exit;
	}

	// Alloc 
	(*alluBuffer) = (ALLU_Buffer_t *)calloc((sizeof(ALLU_Buffer_t) + size), sizeof(uint8_t));
	if(!(*alluBuffer)){
		LOGERROR("Failed to calloc\n");
		result = ALPACA_ERROR_MALLOC;
		goto exit;
	}
	// Set up vars
	(*alluBuffer)->size = size;
	(*alluBuffer)->index = 0;

exit:
	return result;
}

/**
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
		return ALPACA_ERROR_MEMNOBUFFER;
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

/**
 *	@brief Appends the data for a given size to the respective
 *		   ALLU_Buffer_t struct. 
 *
 *	@warning If there is not enough room
 *		   	 AlpacaBuffer_resize is called under the hood in 
 *		   	 AlpacaBuffer_ensureRoom. Be aware the pointer will
 *		   	 change!! 
 *		   
 *	
 *	@param alluBuffer ptr to ALLU_Buffer_t* struct
 *	@param data by ptr to src data to copy from
 *	@param size amount from data to copy from
 *
 *  @note  buffer and data param may overlap in memory
 *  
 *	@return ALPACA_STATUS
 */
ALPACA_STATUS AlpacaBuffer_append(ALLU_Buffer_t **alluBuffer, uint8_t *data, size_t size){

	ALPACA_STATUS result = ALPACA_SUCCESS;
	/*
	 * Check if invalid buffer passed
	 */
	if(!(*alluBuffer) || !data || size == 0){
		result = ALPACA_ERROR_BADPARAM;
		goto exit;
	}


	/*
	 * Make sure there is enough memory to handle the transfer
	 * acquire more memory if not
	 */
	if((AlpacaBuffer_ensureRoom(alluBuffer, size)) != ALPACA_SUCCESS){
		result = ALPACA_ERROR_MEMAPNDFAIL;
		goto exit;
	}
	LOGDEBUG("Buffer [%p] Size [%lu] Used [%lu]\n", *alluBuffer, (*alluBuffer)->size, (*alluBuffer)->index);
	
	// Perform the transfer 
	memmove(((*alluBuffer)->buffer + (*alluBuffer)->index), data, size);
	
	// Adjust the cursor accordingly 
	(*alluBuffer)->index += size;

exit:
	return result;	
}

/**
 *	@brief Resize the respective structure. This can be used
 *		   to SHRINK a ALLU_Buffer as well grow
 *	
 *	@param alluBuffer ptr to ALLU_Buffer_t* struct
 *	@param memNeeded amount of memory needed
 *	@return ALPACA_STATUS
 */
ALPACA_STATUS AlpacaBuffer_resize(ALLU_Buffer_t **alluBuffer, size_t memNeeded) {
 	
	ALPACA_STATUS result = ALPACA_SUCCESS; 

	/*
	 * Make sure pointer to buffer is valid
	 * Ensure that if a shrink is occuring
	 * that we are not shrinking to a size that
	 * cannot hold current data
	 */
	ALLU_Buffer_t* tmp = NULL;
	if(!(*alluBuffer) ||  memNeeded < (*alluBuffer)->index) {
		result = ALPACA_ERROR_BADPARAM;
		goto exit;
	}
	
	if((result = AlpacaBuffer_init(&tmp, memNeeded)) != ALPACA_SUCCESS){
		goto exit;
	}

	/*
	 * Copy old index 
	 * Then copy memory contents if any
	 */ 
	tmp->index = (*alluBuffer)->index;
	if((*alluBuffer)->index){
		memcpy(tmp->buffer, (*alluBuffer)->buffer, (*alluBuffer)->index);
	}

	/*
	 * Free the old structure 
	 */
	if((result = AlpacaBuffer_free(alluBuffer)) != ALPACA_SUCCESS) {
		LOGERROR("ERROR DURING FREE\n");
		// Free temp struct
		AlpacaBuffer_free(&tmp);
	}
	else {
		(*alluBuffer)=tmp;
	}

exit:
	return result;
}
/**
 *	@brief Verifies there is enough room left to write to the buffer
 *		   ensuring valid memory writes
 *	
 *	@note  If there is not enough room a reallocation will occur to 
 *         expand buffer
 * 
 * 	@warning  This function will always be called by AlpacaBuffer_append
 *	
 *	@param alluBuffer by ref ptr to ALLU_Buffer_t* struct
 *	@param memNeeded Amount of memory needed for successful write
 *	@return ALPACA_STATUS
 */
ALPACA_STATUS AlpacaBuffer_ensureRoom(ALLU_Buffer_t **alluBuffer, size_t memNeeded){

	ALPACA_STATUS result = ALPACA_SUCCESS;
	/*
	 * Check if invalid buffer passed
	 */
	if(!(*alluBuffer)){

		result = ALPACA_ERROR_MEMNOBUFFER;
		goto exit;

	}

	/*
	 * Check if the amount needed is greater than the amount
	 * avalaible. If so, rezize the structure accordingly
	 */
 	if( memNeeded > ((*alluBuffer)->size - (*alluBuffer)->index)){
 		size_t newSize = (memNeeded - (((*alluBuffer)->size - (*alluBuffer)->index)) + (*alluBuffer)->size);
 		result = AlpacaBuffer_resize(alluBuffer, newSize);

 	}

exit:
	return result;
}

/**
 *	@brief Verifies there is enough room left to write to the buffer
 *		   ensuring valid memory writes
 *	
 *	@note  If there is not enough room a reallocation will occur to 
 *         expand buffer
 *	
 *	@param alluBuffer by ref ptr to ALLU_Buffer_t* struct
 *	@param data by ptr to src data to copy from
 *	@param size amount from data to copy from
 *	@return ALPACA_STATUS
 */
ALPACA_STATUS  AlpacaBuffer_zero(ALLU_Buffer_t **alluBuffer){

	/*
	 * Check if invalid buffer passed
	 */
	if(!(*alluBuffer)){
		return ALPACA_ERROR_MEMNOBUFFER;
	}

	AlpacaMemory_zero((*alluBuffer)->buffer, (*alluBuffer)->size , ((*alluBuffer)->size - (*alluBuffer)->index));
	(*alluBuffer)->index = 0;

	return ALPACA_SUCCESS;
}