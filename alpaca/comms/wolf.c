#include <wolfssl/options.h>
#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/ssl.h>

// Internal
#include <interfaces/comms_interface.h>
#include <core/codes.h>
#include <core/logging.h>

static const char TEMPCERTFILE[] = "/home/chris/repos/AlpacaLunch/controller/hutch/hutch-cert-dungpile.pem";
static const char TEMPKEYFILE[] = "/home/chris/repos/AlpacaLunch/controller/hutch/hutch-key-dungpile.pem";

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
ALPACA_STATUS AlpacaWolf_init(uint16_t version){

    ALPACA_STATUS result = ALPACA_ERROR_UNKNOWN;
    ENTRY;

    if(version > ALPACA_COMMSPROTO_SSH){
        LOGERROR("Version number not in range: %d\n", version);
        result = ALPACA_ERROR_BADPARAM;
        goto exit;
    }

    if(!wolfInitialized){
        LOGINFO("Initializing global wolfCTX's with version [%x]\n", version);
         /*
         * Init wolfSSL and create global wolfCTX obj
         * Currently this is only support TLS 1.2, 
         */
        wolfSSL_Init();
        wolfInitialized = 1;

        procWolfClientCtx = NULL;
        procWolfServerCtx = NULL;
        
        
        if ((procWolfServerCtx = wolfSSL_CTX_new(WOLFTLSVERSION[version].server_method())) == NULL){

            LOGERROR("procWolfServerCtx ERROR: %d\n", ALAPCA_ERROR_WOLFINIT);
            result = ALAPCA_ERROR_WOLFINIT;
            goto exit;
        }
        
        /* Load server certificates into WOLFSSL_CTX */
        if (wolfSSL_CTX_use_certificate_file(procWolfServerCtx, TEMPCERTFILE, SSL_FILETYPE_PEM)
            != SSL_SUCCESS) {
            LOGERROR("ERROR: failed to load %s, please check the file.\n", TEMPCERTFILE);
            result = ALAPCA_ERROR_WOLFINIT;
            goto exit;
        }

        /* Load server key into WOLFSSL_CTX */
        if (wolfSSL_CTX_use_PrivateKey_file(procWolfServerCtx, TEMPKEYFILE, SSL_FILETYPE_PEM) != SSL_SUCCESS) {
            LOGERROR("ERROR: failed to load %s, please check the file.\n", TEMPKEYFILE);
            result = ALAPCA_ERROR_WOLFINIT;
            goto exit;
        }
        wolfSSL_CTX_set_verify(procWolfServerCtx, SSL_VERIFY_NONE, 0);
        
        if ((procWolfClientCtx = wolfSSL_CTX_new(WOLFTLSVERSION[version].client_method())) == NULL){
            LOGERROR("procWolfClientCtx ERROR: %d\n", ALAPCA_ERROR_WOLFINIT);
            result = ALAPCA_ERROR_WOLFINIT;
            goto exit;
        }
        
        wolfSSL_CTX_set_verify(procWolfClientCtx, SSL_VERIFY_NONE, 0);
        
        result = ALPACA_SUCCESS;
    }

exit:
    LEAVING;
    return result;
}

/**
 *  @brief
 */ 
ALPACA_STATUS AlpacaWolf_cleanUp(void){

    ALPACA_STATUS result = ALPACA_SUCCESS;
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
    }
    else {
        LOGDEBUG("Wolf was either already cleaned or never initialized...nothing to do\n");
    }

    LEAVING;
    return result;
}

/**
 *  @brief Create a underlying SSL object of the type specified
 *  
 *  @param alpacasock Alpaca_sock_t pointer to custom socket
 *  @param type unsigned byte int representing the ssl type
 * 
 *  @return ALPACA_STATUS
 */
ALPACA_STATUS AlpacaWolf_createSSL(Alpaca_sock_t* alpacasock , uint16_t flags){

    ALPACA_STATUS result = ALPACA_ERROR_UNKNOWN;
    ENTRY;
    LOGDEBUG("Entering with params alpacasock[%p] alpacasock->ssl[%p] flags[%X]\n",alpacasock, alpacasock->ssl, flags);
    if(!alpacasock || (alpacasock->ssl)){
        result = ALPACA_ERROR_BADPARAM;
        LOGERROR(" Bad param(s) -> alpacasock:[%p] alpacasock->ssl:[%p]\n",alpacasock, alpacasock->ssl);
        goto exit;
    }

    switch(GET_COMMS_PROTO(flags)){

        case ALPACA_COMMSPROTO_TLS12:
            /**
             * Create ssl object
             */
            if(ALPACA_COMMSTYPE_CLIENT & flags){

                if ((alpacasock->ssl = wolfSSL_new(procWolfClientCtx)) == NULL) {
                    LOGERROR("Error from wolfSSL_new, no SSL object created\n");
                    result = ALPACA_ERROR_WOLFSSLCREATE;
                }
                result = ALPACA_SUCCESS;
            }
            else if (ALPACA_COMMSTYPE_SERVER & flags){

                if ((alpacasock->ssl = wolfSSL_new(procWolfServerCtx)) == NULL) {
                    LOGERROR("Error from wolfSSL_new, no SSL object created\n");
                    result = ALPACA_ERROR_WOLFSSLCREATE;
                }
                result = ALPACA_SUCCESS;
            }
            else {

                LOGERROR("Error, invalid flags used %04x\n", flags);
                result = ALPACA_ERROR_WOLFSSLCREATE;
            }
            break;

        case ALPACA_COMMSPROTO_TLS13:
			result = ALPACA_FAILURE;
			LOGERROR("TLS 1.3 not supported yet\n");
            break;

		default:
			result = ALPACA_ERROR_UNKNOWN;
			LOGERROR("Invalid comms type passed -> %d\n", flags);
    }
    
exit:
    LOGDEBUG("Leaving with alpacasock[%p] alpacasock->ssl[%p] flags[%X]\n",alpacasock, alpacasock->ssl, flags);
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
ALPACA_STATUS AlpacaWolf_accept(Alpaca_sock_t* alpacasock, int16_t fd){
    ALPACA_STATUS result = ALPACA_SUCCESS;
    ENTRY;

    /* Verify pointers */
    if(alpacasock && alpacasock->ssl && fd > 2){
        /*
         * Wrap bottom layer TCP socket in wolf
         * then perform TLS handshake 
         */
        if(wolfSSL_set_fd(alpacasock->ssl, fd) != SSL_SUCCESS){
            LOGERROR("wolfSSL_set_fd error\n");
            result = ALPACA_ERROR_WOLFSSLCREATE;
        }
        
        if (wolfSSL_accept(alpacasock->ssl) != SSL_SUCCESS) {
            LOGERROR("ERROR failed to accept wolfSSL\n");
            result = ALPACA_ERROR_WOLFSSLCONNECT;
        }
    }
    else {
        LOGERROR("Error param(s) passed alpacasock[%p] ssl[%p] fd[%d]\n", alpacasock, alpacasock->ssl, fd);
        result = ALPACA_ERROR_BADPARAM;
    }

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
    ENTRY;

    /* Verify pointers */
    if(alpacasock && alpacasock->ssl){
        /*
         * Wrap bottom layer TCP socket in wolf
         * then perform TLS handshake 
         */
        if(wolfSSL_set_fd(alpacasock->ssl, alpacasock->fd) != SSL_SUCCESS){
            LOGERROR("wolfSSL_set_fd error\n");
            result = ALPACA_ERROR_WOLFSSLCREATE;
        }
        
        if (wolfSSL_connect(alpacasock->ssl) != SSL_SUCCESS) {
            int err = 0;
            char buffer[80];
            err = wolfSSL_get_error(alpacasock->ssl, 0);
            wolfSSL_ERR_error_string(err, buffer);
            LOGDEBUG("WolfSSL Error %d: %s\n", err, buffer);

            result = ALPACA_ERROR_WOLFSSLCONNECT;
        }
    }
    else {
        LOGERROR("Error invalid pointer(s) passed alpacasock:%p ssl:%p\n", alpacasock, alpacasock->ssl);
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
ALPACA_STATUS AlpacaWolf_send(WOLFSSL* sslCtx, void* buf, size_t len, ssize_t* out){
    ALPACA_STATUS result = ALPACA_SUCCESS;
    ENTRY;

    if(NULL == sslCtx || NULL == buf || len == 0){
        LOGERROR("AlpacaWolf_send given bad params!\n");
        result = ALPACA_ERROR_BADPARAM;
        *out = -1;
        goto exit;
    }

    *out = wolfSSL_write(sslCtx, buf, len);
    if(*out <= 0){
        LOGERROR("Failure to send!\n");
        result = ALPACA_ERROR_WOLFSSLWRITE;
    }
    /*
     * If ever switched to non-blocking some extra work
     * needs to be done here
     */

exit:
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
ALPACA_STATUS AlpacaWolf_recv(WOLFSSL* sslCtx, void* buf, size_t len, ssize_t* out){
    ALPACA_STATUS result = ALPACA_SUCCESS;
    ENTRY;

    if(NULL == sslCtx || NULL == buf || len == 0){
        LOGERROR("AlpacaWolf_recv given bad params!\n");
        result = ALPACA_ERROR_BADPARAM;
        *out = -1;
        goto exit;
    }

    *out = wolfSSL_read(sslCtx, buf, len);
    if(*out <= 0){
        LOGERROR("Failure to read!\n");
        result = ALPACA_ERROR_WOLFSSLREAD;
    }
    /*
     * If ever switched to non-blocking some extra work
     * needs to be done here
     */

exit:
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
ALPACA_STATUS AlpacaWolf_close(WOLFSSL** sslCtx) {
    
    ALPACA_STATUS result = ALPACA_SUCCESS;
    ENTRY;
    LOGDEBUG("Freeing SSL OBJ at [%p]\n", (*sslCtx));
    if((*sslCtx)) {
        wolfSSL_free((*sslCtx));
        (*sslCtx) = NULL;
        goto done;
    }

    LOGERROR("No SSL object to free\n");

done:
    LEAVING;
    return result;   
}

