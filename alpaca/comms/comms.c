/**
 * @file
 */


// Implements interface 
#include <interfaces/comms_interface.h>


// Internal
#include <interfaces/memory_interface.h>
#include <core/logging.h>
#include <core/codes.h>
#include <comms/wolf.h>
#include <comms/sock.h>

#define DEFAULTPORT 12345


// Process level variables 
Alpaca_commsCtx_t coreComms;



/**
 *	@brief  Initialize Process level comms. This should be called 
 *  	    from main and only be called once per process
 *  
 *  @param  version - Selction of tls version 1.2/1.3 (comms_interface.h)
 *  @return ALPACA_STATUS
 */
ALPACA_STATUS AlpacaComms_init(Alpaca_tlsVersion_t version){
	ALPACA_STATUS result = ALPACA_SUCCESS;
	ENTRY;
	/*
	 * Initilize the upper level comms layer
	 */
	result = AlpacaWolf_init(version);
	if(result != ALPACA_SUCCESS){
		LOGERROR("Error during TLS layer init Version:%d\n", version);
		goto exit;
	}

exit:
	LEAVING;
	return result;
}

/**
 *	@brief  Tear down global comms. Called when process exiting
 *  	    
 *  @return ALPACA_STATUS
 */
ALPACA_STATUS AlpacaComms_cleanUp (void){
	ALPACA_STATUS result = ALPACA_SUCCESS;
	ENTRY;

	result = AlpacaWolf_cleanUp();
	if(result != ALPACA_SUCCESS){
		LOGERROR("Error during comms cleanup\n");
	}

	LEAVING;
	return result;
}

/**
 *	@brief Initialize a generic Alpaca Comms context object
 *
 *  @param ctx  - Double Pointer to caller comms context object to 
 * 			      allocate and initialize
 * 	@param type - Comms type
 *  @return ALPACA_STATUS
 */
ALPACA_STATUS AlpacaComms_initCtx(Alpaca_commsCtx_t** ctx, uint8_t type) {
	
	ALPACA_STATUS result = ALPACA_SUCCESS;
	ENTRY;
	/*
	 * Verify that the pointer given doesn't actually point to something
	 * if it does, it may still be valid memory and allocation can cause 
	 * a leak
	 */
	if((*ctx) != NULL){
		result = ALPACA_ERROR_BADPARAM;
		LOGERROR("Pointer to Comms CTX appears invalid...points to %p...will not allocate\n", ctx);
		goto exit;
	}

	/*
	 * We have a safe pointer so we can allocate the base structure
	 */
	(*ctx) = calloc(sizeof(Alpaca_commsCtx_t), sizeof(uint8_t));
	if((*ctx) == NULL) {
		LOGERROR("Error during allocation of comms ctx\n");
		result = ALPACA_ERROR_MALLOC;
		goto exit;
	}

	/*
	 * Now we can allocate the socket structure
	 */
	(*ctx)->AlpacaSock = calloc(sizeof(Alpaca_sock_t), sizeof(uint8_t));
	if(!(*ctx)){
		LOGERROR("Error during allocation of netCtx ctx\n");
		result = ALPACA_ERROR_MALLOC;
	}	

	result = AlpacaSock_create((*ctx)->AlpacaSock);
	if(result != ALPACA_SUCCESS){
		LOGERROR("Error creating Alpaca Socket");
		goto exit;
	}

	switch(type) {

		case ALPACA_COMMSTYPE_TLS12:
				result = AlpacaWolf_create((*ctx)->AlpacaSock->ssl, ALPACA_COMMSTYPE_TLS12);
				if(((*ctx)->AlpacaSock->ssl) == NULL){
					LOGERROR("Error generating ssl object");
					goto exit;
				}
				(*ctx)->connect = NULL;
				(*ctx)->read    = NULL;
				(*ctx)->write   = NULL;
				(*ctx)->close   = NULL;
		
		case ALPACA_COMMSTYPE_TLS13:
			result = ALPACA_FAILURE;
			LOGERROR("TLS 1.3 not supported yet");

		case ALPACA_COMMSTYPE_UDP:
			result = ALPACA_FAILURE;
			LOGERROR("UDP not supported yet");

		case ALPACA_COMMSTYPE_SSH:
			result = ALPACA_FAILURE;
			LOGERROR("SSH not supported yet");
		default:
			result = ALPACA_ERROR_UNKNOWN;
			LOGERROR("Invalid comms type passed -> %d", type);
	}


exit:
	// Clean up if required
	if((result != ALPACA_SUCCESS) && (*ctx)){
		free((*ctx));
		(*ctx) = NULL;
	}

	LEAVING;
	return result;
}


/**
 *	@brief Tear down of an Alpaca Comms context object to
 * 		   incudle memory clean up
 * 	@param  ctx - pointer to Alpaca_commsCtx_t object to tear down
 *  @return ALPACA_STATUS  
 */
ALPACA_STATUS AlpacaComms_destroyCtx(Alpaca_commsCtx_t** ctx){
	
	ENTRY;
	ALPACA_STATUS result = ALPACA_SUCCESS;

	if(ctx){
		/*
		* If our custom socket is allocated clean up
		*/
		if(ctx->AlpacaSock){
			AlpacaSock_close(ctx->AlpacaSock);
		}
		goto exit;
	}
	// Invalid param
	result = ALPACA_ERROR_BADPARAM;
	LOGERROR("Pointer to Comms CTX appears invalid...points to %p...will not allocate\n", *ctx);
	
exit:
	LEAVING;
	return result;
}


/*
// Network I/O
ALPACA_STATUS AlpacaComms_connect (Alpaca_commsCtx_t* ctx);
ALPACA_STATUS AlpacaComms_read	  (Alpaca_commsCtx_t* ctx, void* buf, size_t len, ssize_t* out);
ALPACA_STATUS AlpacaComms_write	  (Alpaca_commsCtx_t* ctx, void* buf, size_t len, ssize_t* out);
ALPACA_STATUS AlpacaComms_close   (Alpaca_commsCtx_t* ctx);
*/