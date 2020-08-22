/**/


// Implements interface 
#include <interfaces/comms_interface.h>


// Internal
#include <interfaces/memory_interface.h>
#include <core/logging.h>
#include <core/codes.h>
#include <comms/wolf.h>
#include <comms/sock.h>

#define DEFAULTPORT 12345

static
int32_t __attribute__ ((unused))setNonblocking(int fd)
{
    int flags = -1;

    /* If system have O_NONBLOCK, use the Posix way to do it */
#if defined(O_NONBLOCK)
    if (-1 == (flags = fcntl(fd, F_GETFL, 0)))
        flags = 0;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
#else
    /* Otherwise, use the old way of doing it */
    flags = 1;
    return ioctl(fd, FIOBIO, &flags);
#endif
} 

/**
 *	@brief
 *  
 *  @param
 *	@return
 */
ALPACA_STATUS AlpacaComms_init(Alpaca_tlsVersion_t v){
	ALPACA_STATUS result = ALPACA_SUCCESS;
	ENTRY;

	result = AlpacaWolf_init(v);
	if(result != ALPACA_SUCCESS){
		LOGERROR("Error during TLS layer init Version:%d\n", v);
	}

	LEAVING;
	return result;
}

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

ALPACA_STATUS AlpacaComms_initCtx(Alpaca_commsCtx_t** ctx){
	
	ALPACA_STATUS result = ALPACA_SUCCESS;
	ENTRY;

	/*
	 * Verify that the pointer given doesn't actually point to something
	 * if it does, it may still be valid memory and allocation can cause 
	 * a leak
	 */
	if(*ctx != NULL){
		result = ALPACA_ERROR_BADPARAM;
		LOGERROR("Pointer to Comms CTX appears invalid...points to %p...will not allocate\n", *ctx);
		goto exit;
	}

	/*
	 * We have a safe pointer so we can allocate the base structure
	 */
	*ctx = calloc(sizeof(Alpaca_commsCtx_t), sizeof(uint8_t));
	if(!(*ctx)){
		LOGERROR("Error during allocation of comms ctx\n");
		result = ALPACA_ERROR_MALLOC;
		goto exit;
	}

	/*
	 * Now we can allocate the socket structure
	 */
	(*ctx)->netCtx = calloc(sizeof(Alpaca_sock_t), sizeof(uint8_t));
	if(!(*ctx)){
		LOGERROR("Error during allocation of netCtx ctx\n");
		result = ALPACA_ERROR_MALLOC;
	}	

	/*
	 *	At this point we are not sure what type of comms
	 *  we'll have
	 */
	(*ctx)->connect = NULL;
	(*ctx)->read  = NULL;
	(*ctx)->write = NULL;
	(*ctx)->close = NULL;


exit:
	// Clean up if required
	if((result != ALPACA_SUCCESS) && *ctx){
		free(*ctx);
		*ctx = NULL;
	}

	LEAVING;
	return result;
}

ALPACA_STATUS AlpacaComms_destroyCtx(Alpaca_commsCtx_t** ctx){
	
	ALPACA_STATUS result = ALPACA_SUCCESS;
	ENTRY;

	if(*ctx == NULL){
		result = ALPACA_ERROR_BADPARAM;
		LOGERROR("Pointer to Comms CTX appears invalid...points to %p...will not allocate\n", *ctx);
		goto exit;
	}

	if((*ctx)->netCtx){
		// AlpacaSock_close(((*ctx)->netCtx));
	}


exit:
	LEAVING;
	return result;
}