/**
 * @file
 */
#include <sys/poll.h>

// Implements interface 
#include <interfaces/comms_interface.h>


// Internal
#include <interfaces/utility_interface.h>
#include <interfaces/memory_interface.h>
#include <core/logging.h>
#include <core/codes.h>
#include <comms/wolf.h>



// Process level variables 
Alpaca_commsCtx_t *coreComms;



/**
 *	@brief  Initialize Process level comms. This should be called 
 *  	    from main and only be called once per process
 *  
 *  @param  version - Selction of tls version 1.2/1.3 (comms_interface.h)
 *  @return ALPACA_STATUS
 */
ALPACA_STATUS AlpacaComms_init(uint16_t flags){
	ALPACA_STATUS result = ALPACA_SUCCESS;
	ENTRY;
	/*
	 * Initilize the upper level comms layer
	 */
	LOGINFO("Initializing global comms with flags: 0x%08X\n",flags);
	

	result = AlpacaWolf_init(GET_COMMS_PROTO(flags));
	if(result != ALPACA_SUCCESS){
		LOGERROR("Error during TLS layer init Version:%d\n", GET_COMMS_PROTO(flags));
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
ALPACA_STATUS AlpacaComms_cleanUp (void) {
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
 *  @param ctx Double Pointer to caller comms context object to 
 * 			   allocate and initialize
 * 	@param type Comms type. Requires bit mask of ALPACACOMMS_TYPE and 
 * 				ALPACA_COMMSPROTO
 *  @return ALPACA_STATUS
 */
ALPACA_STATUS AlpacaComms_initCtx(Alpaca_commsCtx_t **ctx, uint16_t flags) {

	ALPACA_STATUS result = ALPACA_SUCCESS;
	ENTRY;

	LOGINFO("Initializing Comms Ctx with params ctx[%p] flags[0x%08x]\n", (*ctx), flags);

	/*
	 * Verify that the pointer given doesn't actually point to something
	 * if it does, it may still be valid memory and allocation can cause 
	 * a leak
	 */
	if ((*ctx) != NULL || !GET_COMMS_TYPE(flags) || !GET_COMMS_PROTO(flags)) {
		result = ALPACA_ERROR_BADPARAM;
		LOGERROR("Invalid params passed\n");
		goto exit;
	}

	/*
	 * We have a safe pointer so we can allocate the base structure
	 */
	(*ctx) = calloc(sizeof(Alpaca_commsCtx_t), sizeof(uint8_t));
	if ((*ctx) == NULL) {
		LOGERROR("Error during allocation of comms ctx\n");
		result = ALPACA_ERROR_MALLOC;
		goto exit;
	}

	switch (GET_COMMS_PROTO(flags)) {

	case ALPACACOMMS_PROTO_TLS12:
		LOGINFO("TLS_1.2 was selected\n");
		(*ctx)->connect = AlpacaWolf_connect;
		(*ctx)->listen = AlpacaWolf_listen;
		(*ctx)->accept = AlpacaWolf_accept;
		(*ctx)->read = AlpacaWolf_recv;
		(*ctx)->write = AlpacaWolf_send;
		(*ctx)->close = AlpacaWolf_close;
		(*ctx)->flags = flags;
		break;

	case ALPACACOMMS_PROTO_TLS13:
		result = ALPACA_ERROR_UNSUPPORTED;
		LOGERROR("TLS 1.3 not supported yet\n");
		break;

	case ALPACACOMMS_PROTO_UDP:
		result = ALPACA_ERROR_UNSUPPORTED;
		LOGERROR("UDP not supported yet\n");
		break;

	case ALPACACOMMS_PROTO_SSH:
		result = ALPACA_ERROR_UNSUPPORTED;
		LOGERROR("SSH not supported yet\n");
		break;

	default:
		result = ALPACA_ERROR_UNKNOWN;
		LOGERROR("Invalid comms type passed -> %d\n", flags);
	}

exit:
	/**
	 * If any failure occurs clean memory allocated
	 * and set to NULL 
	 */
	if ((ALPACA_SUCCESS != result) && (*ctx)) {
		LOGERROR("Error [%u] occured during ctx init...cleaning\n", result);
		AlpacaComms_destroyCtx(ctx);
	}

	LEAVING;
	return result;
}

/**
 *	@brief Tear down of an Alpaca Comms context object to
 * 		   incudle memory clean up
 * 	@param ctx pointer to Alpaca_commsCtx_t object to tear down
 * 
 *  @return ALPACA_STATUS  
 */
ALPACA_STATUS AlpacaComms_destroyCtx(Alpaca_commsCtx_t** ctx){
	
	ALPACA_STATUS result = ALPACA_SUCCESS;
	ENTRY;
	LOGDEBUG("Cleaning comms ctx at [%p]\n", (*ctx));
	if((*ctx)) {
		/*
		* If our custom socket is valid
		* clean up
		*/
		if((*ctx)->fd > 0){			
			AlpacaComms_close(*ctx);
		}
		free((*ctx));
		*ctx = NULL;
	}

	LEAVING;
	return result;
}


/**
 *	@brief Perform TCP handshake and then perform underlying comms connect
 * 	
 *  @param ctx Pointer to allocated Alpaca_commsCtx_t object 
 *  @param ipstr IP address in string format of peer to connect to
 *  @param port port to connect to
 *  
 *  @return ALPACA_STATUS  
 */
ALPACA_STATUS AlpacaComms_connect(Alpaca_commsCtx_t* ctx, char* ipstr, uint16_t port){
	
	ENTRY;
	ALPACA_STATUS result = ALPACA_SUCCESS;
	//int32_t ret = 0;
	int32_t attempts = 0;
	
	// Check params
	if(!ctx || !ipstr || !port ) {
		LOGERROR("Invalid parameters passed: ctx[%p], IP[%s], port[%d]\n", ctx, ipstr, port);
		result = ALPACA_ERROR_BADPARAM;
		goto exit;
	}
	
	// Make sure we are not already in a connected state
	if(ctx->status & (ALPACACOMMS_STATUS_CONN)) {
		LOGERROR("Comms ctx at [%p] is already connected...state[%02X]...disconnect before connecting again\n", ctx, ctx->status);
		result = ALPACA_ERROR_BADSTATE;
		goto exit;
	}

	// Verify function pointer
	if (!ctx->connect) {
		// Missing function pointer
		LOGERROR("Error: Missing underlayer connect function pointer");
		result = ALPACA_ERROR_BADSTATE;
		goto exit;
	}
	
	memset(&ctx->peer, 0, sizeof(ctx->peer));
	// Convert IPv4 from string to network byte order 
    if(inet_pton(AF_INET, ipstr, &ctx->peer.sin_addr) != 1) {
		LOGERROR("Error converting ip addr\n");
		result = ALPACA_ERROR_UNKNOWN;
		goto exit;
	}
	// Set up peer address
    ctx->peer.sin_family = AF_INET;
    ctx->peer.sin_port = htons(port);

	/** 
	 * Loop attempting to connect to supplied peer
	 * If a failure is detected sleep for 3 seconds before
	 * retrying. Retry up to MAX_RETRIES
	 */
	while(attempts < MAX_RETRIES && ctx->status == ALPACACOMMS_STATUS_NOTCONN) {

		LOGINFO("Attepmt #%d to establish TCP connection to %s:%d\n", attempts+1, ipstr, port);

		// Call underlying connect
		result = ctx->connect(ctx);
		if(ALPACA_SUCCESS != result){
			/*
			 * Error occured
			 * Clean connection/socket
			 * Sleep and retry if more attempts exist
			 */
			AlpacaComms_close(ctx);
			ctx->status = ALPACACOMMS_STATUS_NOTCONN;
			AlpacaUtilities_mSleep(THREE_SECONDS_MILLI);
			attempts++;
			continue;
		}
		// Success
		LOGINFO("Connection fully established!\n");
	}

exit:
	if(ctx && !(ctx->status & ALPACACOMMS_STATUS_CONN)){

		LOGERROR("Connection was not able to be established\n");
		AlpacaComms_close(ctx);
		memset(&ctx->peer, 0, sizeof(ctx->peer));
	}
	LEAVING;
	return result;
}

/**
 *	@brief Start tcp listen at specified port
 * 	       
 * 
 *  @warning No memory is free'd
 * 
 *  @param ctx Pointer to allocated Alpaca_commsCtx_t object 
 *  @return ALPACA_STATUS  
 */
ALPACA_STATUS AlpacaComms_listen (Alpaca_commsCtx_t* ctx, uint16_t port){
	ENTRY;
	ALPACA_STATUS result = ALPACA_SUCCESS;
	
	LOGDEBUG("Attempting to setup listener with params ctx[%p] port[%u]\n", ctx, port);
	if(!ctx || port < 1024) {
		LOGERROR("Error bad params passed ctx[%p] port[%u]\n", ctx, port);
		result = ALPACA_ERROR_BADPARAM;
		goto exit;
	}

	/*
	 * Make sure that this ctx has been 
	 * set up correctly
	 */
	if(!ctx->listen || !ctx->accept) {
		LOGERROR("Error Comms CTX bad state! listen [%p] accept [%p]\n",ctx->listen, ctx->accept);
		result = ALPACA_ERROR_BADSTATE;
		goto exit;
	}

	/*
	 * The underlying listen call can be virtually
	 * any protocol. Any specific listen behavior
	 * is executed in this call
	 */
	result = ctx->listen(ctx, port);
	if(ALPACA_SUCCESS != result) {
		// Underlying listen call failed
		goto exit;
	}

	/*
	 * The underlying accept call can be virtually
	 * any protocol. Any specific accept behavior
	 * is executed in this call
	 * 
	 * Connectionless protocols will not truly have 
	 * an accept and instead will likely have a 
	 * filler stub instead
	 */
	result = ctx->accept(ctx);
	if(ALPACA_SUCCESS != result) {
		// Underlying accept call failed
		goto exit;
	}	

	// If we hit here we are successfull
	
exit:
	if(ALPACA_SUCCESS != result ){
		if(ctx){
			LOGINFO("Cleaning listener...");
			AlpacaComms_close(ctx);
		}
	}

	LEAVING;
	return result;
}

/**
 *	@brief Tear down Alpaca Socket to include any underlying comms
 * 	       contexts such as tls, udp, etc. 
 * 
 *  @warning No memory is free'd
 * 
 *  @param ctx Pointer to allocated Alpaca_commsCtx_t object 
 *  @return ALPACA_STATUS  
 */
ALPACA_STATUS AlpacaComms_close(Alpaca_commsCtx_t* ctx) {
	ENTRY;
	ALPACA_STATUS result = ALPACA_SUCCESS;
	

	if(!ctx){
		LOGERROR("Invalid params passed to AlpacaComms_close: ctx:%p\n", ctx);
		result = ALPACA_ERROR_BADPARAM;
		goto exit;
	}

	/*
	 * It is possible that an error could have 
	 * occured prior to function pointers being set
	 * verify pointers exist
	 */
	if(ctx->close && NULL != ctx->protoCtx){
		// Close underlying layer	
		result = ctx->close(ctx);
		if(result != ALPACA_SUCCESS){
			LOGERROR("Failure to close security comms layer\n");
			goto exit;
		}
	}
	if(0 < ctx->fd){
		// Close socket
		close(ctx->fd);
		ctx->fd = -1;
	}
	ctx->status = ALPACACOMMS_STATUS_NOTCONN;

exit:
	LEAVING;
	return result;	
}



ALPACA_STATUS AlpacaComms_recv(Alpaca_commsCtx_t* ctx, void* buf, size_t len, ssize_t* out){
	ENTRY;
	ALPACA_STATUS result = ALPACA_SUCCESS;
	ssize_t temp = 0;

	if(!ctx || !buf || len == 0){
		LOGERROR("Invalid params passed to AlpacaComms_send ctx:%p buf:%p  len:%lu\n", ctx, buf, len);
		result = ALPACA_ERROR_BADPARAM;
		*out = 0;
		goto exit;
	}

	/*
	 * If non-blocking sockets end up being used
	 * this will need to loop and have additional error checking
	 * will also need polling
	 */
	result = ctx->read(ctx, buf, len, &temp);
	if(result != ALPACA_SUCCESS || temp <= 0){
		LOGERROR("Error attempting read: %d\n", result);
		*out = temp;
		goto exit;
	}
	*out = temp;
	LOGDEBUG("Recv'd %lu bytes\n", *out);

exit:
	LEAVING;
	return result;	
}

/**
 * @brief 
 * 
 */
ALPACA_STATUS AlpacaComms_send(Alpaca_commsCtx_t* ctx, void* buf, size_t len, ssize_t* out){
	ENTRY;
	ALPACA_STATUS result = ALPACA_SUCCESS;
	ssize_t temp = 0;

	
	if(!ctx || !buf || len == 0){
		LOGERROR("Invalid params passed to AlpacaComms_send ctx:%p, buf:%p len:%lu\n", ctx, buf, len);
		result = ALPACA_ERROR_BADPARAM;
		*out = 0;
		goto exit;
	}

	/*
	 * If non-blocking sockets end up being used
	 * this will need to loop and have additional error checking
	 * will also need polling 
	 */
	result = ctx->write(ctx, buf, len, &temp);
	if(result != ALPACA_SUCCESS || temp <= 0){
		LOGERROR("Error attempting send: %d\n", result);
		*out = temp;
		goto exit;
	}
	*out = temp;
	LOGDEBUG("Sent %lu bytes\n", *out);

exit:
	LEAVING;
	return result;	
}

int32_t AlpacaComms_setNonBlocking(int fd) {
    int flags;

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