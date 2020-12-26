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
#include <comms/sock.h>

#define DEFAULTPORT  44444
#define MAXRETRIES   5
#define THREESECONDS (3*1000)

// Process level variables 
Alpaca_commsCtx_t *coreComms;



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

	result = AlpacaComms_initCtx(&coreComms, version);
	if(result != ALPACA_SUCCESS){
		LOGERROR("Error creating core comms context. Result:%d Version:%d\n", result, version);
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
 *  @param ctx Double Pointer to caller comms context object to 
 * 			   allocate and initialize
 * 	@param type Comms type
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
	LOGINFO("Initializing Comms Ctx...\n");

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
		goto exit;
	}	

	result = AlpacaSock_create((*ctx)->AlpacaSock);
	if(result != ALPACA_SUCCESS){
		LOGERROR("Error creating Alpaca Socket");
		goto exit;
	}


	switch(type) {

		case ALPACA_COMMSTYPE_TLS12:
				LOGINFO("Creating TLS 1.2 ssl obj\n");
				/**
				 * Create Client ssl object
				 */
				result = AlpacaWolf_createClientSSL((*ctx)->AlpacaSock, ALPACA_COMMSTYPE_TLS12);
				if(((*ctx)->AlpacaSock) == NULL) {
					LOGERROR("Error generating ssl object");
					goto exit;
				}
				(*ctx)->connect = AlpacaWolf_connect;
				//(*ctx)->read    = AlpacaWolf_recv;
				(*ctx)->write   = AlpacaWolf_send;
				(*ctx)->close   = AlpacaWolf_close;

				// Set status 
				(*ctx)->status  = ALPACA_COMMSSTATUS_NOTCONN;
				break;
		
		case ALPACA_COMMSTYPE_TLS13:
			result = ALPACA_FAILURE;
			LOGERROR("TLS 1.3 not supported yet");
			break;

		case ALPACA_COMMSTYPE_UDP:
			result = ALPACA_FAILURE;
			LOGERROR("UDP not supported yet");
			break;

		case ALPACA_COMMSTYPE_SSH:
			result = ALPACA_FAILURE;
			LOGERROR("SSH not supported yet");
			break;

		default:
			result = ALPACA_ERROR_UNKNOWN;
			LOGERROR("Invalid comms type passed -> %d", type);
	}

exit:
	/**
	 * If any failure occurs clean memory allocated
	 * and set to NULL 
	 */
	if((result != ALPACA_SUCCESS) && (*ctx)){
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

	if((*ctx)) {
		/*
		* If our custom socket is allocated clean up
		*/
		if((*ctx)->AlpacaSock){

			/* If we are connected, close both layers */
			if((*ctx)->status & (ALPACA_COMMSSTATUS_CONN | ALPACA_COMMSSTATUS_TLSCONN)){
				AlpacaComms_close(ctx);
			}
			
			AlpacaSock_close((*ctx)->AlpacaSock);
			free((*ctx)->AlpacaSock);
			(*ctx)->AlpacaSock = NULL;
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
ALPACA_STATUS AlpacaComms_connect(Alpaca_commsCtx_t** ctx, char* ipstr, uint16_t port){
	
	ENTRY;
	ALPACA_STATUS result = ALPACA_SUCCESS;
	int32_t ret = 0;
	uint8_t attempts = 0;
	struct pollfd pfd;
	/*
	 * Eliminates the need for (*ctx) in every
	 * reference to the AlpacaSock
	 */
	//Alpaca_sock_t* as_ptr = (*ctx)->AlpacaSock;

	if(!(*ctx) || !ipstr || !port ){
		LOGERROR("Invalid parameters passed: ctx[%p], IP[%s], port[%d]\n",(*ctx), ipstr, port);
		result = ALPACA_ERROR_BADPARAM;
		goto exit;
	}
	
	/**
	 * Set the peer information
	 */
	(*ctx)->AlpacaSock->peer.sin_family = AF_INET;
    (*ctx)->AlpacaSock->peer.sin_port   = htons(port);

    /* convert IPv4 from string to network byte order */
    if(inet_pton(AF_INET, ipstr, &((*ctx)->AlpacaSock->peer.sin_addr)) != 1){
		LOGERROR("");
	}

	/** 
	 * Loop attempting to connect to supplied peer
	 * If a failure is detected sleep for 3 seconds before
	 * retrying. Retry up to MAXRETRIES
	 */
	while(attempts < MAXRETRIES){
		ret = 0;

		/* Wait for writeability */
		pfd.fd = (*ctx)->AlpacaSock->fd;
		pfd.events = POLLOUT;

		ret = poll(&pfd, 1, 10*1000);
		if(ret == -1){
			perror("poll");
			break;
		}

		if(pfd.events & POLLOUT){
			/* Connect to the server */
			LOGINFO("Attepmt #%d to establish TCP connection to %s:%d\n", attempts+1, ipstr, port);
			ret = connect((*ctx)->AlpacaSock->fd, (struct sockaddr*)&(*ctx)->AlpacaSock->peer, sizeof(struct sockaddr_in));
			if (ret == -1)
			{
				LOGERROR("Failed to connect... errno: %d\n", errno	);
				attempts++;
				AlpacaUtilities_mSleep(THREESECONDS);
				continue;
			}
			/* Connection established */
			(*ctx)->status = ALPACA_COMMSSTATUS_CONN;
			break;
		}
		
				
	}

	if(ret != 0){
		result = ALPACA_ERROR_COMMSCONNECT;
		LOGERROR("Could not establish TCP connection\n");
		goto exit;
	}

	/* TLS handhake */
	result = (*ctx)->connect((*ctx)->AlpacaSock);
	if(result){
		LOGERROR("TLS handshake failed!\n");
		goto exit;
	}
	LOGINFO("TLS established\n");
	(*ctx)->status = ALPACA_COMMSSTATUS_TLSCONN;

exit:
	
	if(!((*ctx)->status & ALPACA_COMMSSTATUS_TLSCONN)){
		/* 
		 * TCP + TLS was not established 
		 */
		LOGERROR("TCP + TLS was not able to be established\n");
		AlpacaComms_close(ctx);
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
ALPACA_STATUS AlpacaComms_close (Alpaca_commsCtx_t** ctx){
	ENTRY;
	ALPACA_STATUS result = ALPACA_SUCCESS;
	Alpaca_sock_t* ptr = (*ctx)->AlpacaSock;

	if(!(*ctx) || !(*ctx)->AlpacaSock){
		LOGERROR("Invalid params passed to AlpacaComms_close: ctx:%p, ctx->AlpacaSock:%p ", (*ctx), (*ctx)->AlpacaSock);
		result = ALPACA_ERROR_BADPARAM;
		goto exit;
	}

	// Close top layer comms
	result = (*ctx)->close(ptr->ssl);
	if(result != ALPACA_SUCCESS){
		LOGERROR("Failure to close security comms layer");
		goto exit;
	}

	// Close bottom layer 
	result = AlpacaSock_close((*ctx)->AlpacaSock);
	if(result != ALPACA_SUCCESS){
		LOGERROR("Failure to close security comms layer");
	}

exit:
	LEAVING;
	return result;	
}






/*
ALPACA_STATUS AlpacaComms_read	  (Alpaca_commsCtx_t* ctx, void* buf, size_t len, ssize_t* out);
ALPACA_STATUS AlpacaComms_write	  (Alpaca_commsCtx_t* ctx, void* buf, size_t len, ssize_t* out);
*/