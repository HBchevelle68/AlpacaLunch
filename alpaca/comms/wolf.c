#include <wolfssl/options.h>
#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/ssl.h>

// Internal
#include <interfaces/comms_interface.h>
#include <core/codes.h>
#include <core/logging.h>


// For Atomics
//__atribute__((unused)) static const uint8_t FLAGON  = 1;
//__atribute__((unused)) static const uint8_t FLAGOFF = 0;

// Static Globals
static WOLFSSL_CTX* procWolfClientCtx;
static WOLFSSL_CTX* procWolfServerCtx;
static uint8_t 		wolfInitialized = 0;


typedef struct WolfSSL_TLS_Version {

	WOLFSSL_METHOD* (*server_method)(void); 
	WOLFSSL_METHOD* (*client_method)(void);

} ALPACA_TLSVersion_t;

static const ALPACA_TLSVersion_t WOLFTLSVERSION[2] = 
{
	{
		wolfTLSv1_2_server_method, 
		wolfTLSv1_2_client_method
	},
	{
		wolfTLSv1_3_server_method, 
		wolfTLSv1_3_client_method
	}
};


/**
 *  @brief Initializes the global WolfSSL contexts. These wolf contexts
 *         can be found in wolf.c. contexts are used to generate new
 *         ssl objects for client or server objects
 *  
 *  @param version version of wolfssl comms  
 *  
 *  @return ALPACA_STATUS
 */
ALPACA_STATUS AlpacaWolf_init(Alpaca_tlsVersion_t version){

    ALPACA_STATUS result = ALPACA_ERROR_UNKNOWN;
    ENTRY;

    if(!wolfInitialized){
         /*
         * Init wolfSSL and create global wolfCTX obj
         * Currently this is only support TLS 1.2, 
         */
        wolfSSL_Init();
        wolfInitialized = 1;

        if ((procWolfServerCtx = wolfSSL_CTX_new(WOLFTLSVERSION[version].server_method())) == NULL){

            LOGERROR("procWolfServerCtx ERROR: %d", ALAPCA_ERROR_WOLFINIT);
            result = ALAPCA_ERROR_WOLFINIT;
            goto exit;
        }
        if ((procWolfClientCtx = wolfSSL_CTX_new(WOLFTLSVERSION[version].client_method())) == NULL){

            LOGERROR("procWolfClientCtx ERROR: %d", ALAPCA_ERROR_WOLFINIT);
            result = ALAPCA_ERROR_WOLFINIT;
            goto exit;
        }
        
        
        result = ALPACA_SUCCESS;
    }

exit:
    LEAVING;
    return result;
}

/**
 *  @brief Create a client side underlying SSL object of the type specified
 *  
 *  @param alpacasock Alpaca_sock_t pointer to custom socket
 *  @param type unsigned byte int representing the ssl type
 * 
 *  @return ALPACA_STATUS
 */
ALPACA_STATUS AlpacaWolf_createClientSSL(Alpaca_sock_t* alpacasock ,uint8_t type){

    ALPACA_STATUS result = ALPACA_ERROR_UNKNOWN;
    ENTRY;
    if(!alpacasock || !(alpacasock->ssl)){
        result = ALPACA_ERROR_BADPARAM;
        goto exit;
    }

    switch(type){

        case ALPACA_COMMSTYPE_TLS12:
            /**
             * Create ssl object
             */
            if ((alpacasock->ssl = wolfSSL_new(procWolfClientCtx)) == NULL) {
                LOGERROR("Error from wolfSSL_new, no SSL object created");
                result = ALPACA_ERROR_WOLFSSLCREATE;
            }
            result = ALPACA_SUCCESS;
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
    LEAVING;
    return result;
}







/**
 *  @brief Perform TLS handshake. Should only be called after 
 *         TCP handshake successful.
 * 
 *  @param alpacasock Alpaca_sock_t pointer to custom socket
 * 
 *  @return ALPACA_STATUS 
 */
ALPACA_STATUS AlpacaWolf_connect(Alpaca_sock_t* alpacasock){
    ALPACA_STATUS result = ALPACA_SUCCESS;
    int ret = 0;
    ENTRY;

    /* Verify pointers */
    if(alpacasock && alpacasock->ssl){
        /*
         * Wrap std TCP socket in wolf
         * then perform TLS handshake 
         */
        if(wolfSSL_set_fd(alpacasock->ssl, alpacasock->fd) != SSL_SUCCESS){
            LOGERROR("wolfSSL_set_fd error\n");
            result = ALPACA_ERROR_WOLFSSLCREATE;
        }
        
        if ((ret = wolfSSL_connect(alpacasock->ssl)) != SSL_SUCCESS) {
            LOGERROR("ERROR failed to connect to wolfSSL");
            result = ALPACA_ERROR_WOLFSSLCONNECT;
        }
    }
    else {
        LOGERROR("Error invalid pointer passed");
        result = ALPACA_ERROR_BADPARAM;
    }

    LEAVING;
    return result;
}

/**
 *  @brief Send data through set SSL connection   
 *         
 *  @param alpacasock Alpaca_sock_t pointer to custom socket
 *  @param buf pointer to buffer to send
 *  @param len length of data to send
 *  @param out out variable of bytes successfully sent
 * 
 *  @return ALPACA_STATUS 
 */
ALPACA_STATUS AlpacaWolf_send (WOLFSSL* sslCtx, void* buf, size_t len, ssize_t* out){
    ALPACA_STATUS result = ALPACA_SUCCESS;
    ENTRY;




    LEAVING;
    return result;
}




/**
 *  @brief Close and free underlying SSL object  
 *         
 *  @param alpacasock Alpaca_sock_t pointer to custom socket
 * 
 *  @return ALPACA_STATUS 
 */
ALPACA_STATUS AlpacaWolf_close(WOLFSSL* sslCtx) {
    
    ALPACA_STATUS result = ALPACA_SUCCESS;
    ENTRY;
    
    if(sslCtx) {
        wolfSSL_free(sslCtx);
        sslCtx = NULL;
    }

    LOGERROR("No SSL object to free");

    LEAVING;
    return result;   
}

/**
 *  @brief
 */
ALPACA_STATUS AlpacaWolf_cleanUp(void){

    ALPACA_STATUS result = ALPACA_ERROR_WOLFNOINIT;
    ENTRY;

    if(wolfInitialized){
    
		/* 
		 *	Free the wolfSSL context object
		 *  Cleanup the wolfSSL environment 
		 */
		if(procWolfServerCtx){
        	wolfSSL_CTX_free(procWolfServerCtx);
    		procWolfServerCtx = NULL;
		}

		if(procWolfClientCtx){
	        wolfSSL_CTX_free(procWolfClientCtx);    
	    	procWolfClientCtx = NULL;
		}

    	wolfSSL_Cleanup();
        wolfInitialized = 0;

        result = ALPACA_SUCCESS;
    }

    LEAVING;
    return result;
}