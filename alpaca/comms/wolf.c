#include <wolfssl/options.h>
#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/ssl.h>

// Internal
#include <interfaces/comms_interface.h>
#include <core/codes.h>
#include <core/logging.h>

static const char TEMPCERTFILE[] = "/home/ap/AlpacaLunch/.testcerts/cert.pem";
static const char TEMPKEYFILE[] = "/home/ap/AlpacaLunch/.testcerts/private.pem";

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

    if(version > ALPACACOMMS_PROTO_SSH){
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
ALPACA_STATUS AlpacaWolf_createSSL(Alpaca_commsCtx_t* ctx, uint16_t flags){

    ALPACA_STATUS result = ALPACA_ERROR_UNKNOWN;
    ENTRY;
    
    if(!ctx || ctx->protoCtx){
        result = ALPACA_ERROR_BADPARAM;
        LOGERROR(" Bad param(s) -> ctx:[%p]\n", ctx->protoCtx);
        goto exit;
    }

    switch(GET_COMMS_PROTO(flags)){

        case ALPACACOMMS_PROTO_TLS12:
            /**
             * Create SSL object
             */
            if(ALPACACOMMS_TYPE_CLIENT & flags){
                // CLIENT
                if ((ctx->protoCtx = wolfSSL_new(procWolfClientCtx)) == NULL) {
                    LOGERROR("Error from wolfSSL_new, no SSL object created\n");
                    result = ALPACA_ERROR_WOLFSSLCREATE;
                }
                result = ALPACA_SUCCESS;

            }
            else if (ALPACACOMMS_TYPE_SERVER & flags){
                // SERVER
                if ((ctx->protoCtx = wolfSSL_new(procWolfServerCtx)) == NULL) {
                    LOGERROR("Error from wolfSSL_new, no SSL object created\n");
                    result = ALPACA_ERROR_WOLFSSLCREATE;
                }
                result = ALPACA_SUCCESS;

            }
            else {
                // ERROR
                LOGERROR("Error, invalid flags used %04x\n", flags);
                result = ALPACA_ERROR_WOLFSSLCREATE;
            }
            break;

        case ALPACACOMMS_PROTO_TLS13:
			result = ALPACA_FAILURE;
			LOGERROR("TLS 1.3 not supported yet\n");
            break;

		default:
			result = ALPACA_ERROR_UNKNOWN;
			LOGERROR("Invalid comms type passed -> %d\n", flags);
    }
    
exit:
    LOGDEBUG("Leaving with ctx[%p] ctx->ssl[%p] flags[%X]\n",ctx, ctx->protoCtx, flags);
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
ALPACA_STATUS AlpacaWolf_connect(Alpaca_commsCtx_t* ctx){
    
    ALPACA_STATUS result = ALPACA_SUCCESS;
    int32_t ret = 0;
    ENTRY;

    /*
     * Create socket and verify
     * Socket created is set to non-blocking as well as setting the
     * Close on Exec (CLOEXEC) flag to prevent file descriptor
     * leaking into child processes 
     */
    ctx->fd = socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC, 0); 
    if(-1 == ctx->fd) { 
        LOGERROR("Failure to create socket\n");
        result = ALPACA_ERROR_SOCKCREATE;
        goto exit;
    } 

    // TCP Connect
    ret = connect(ctx->fd, (struct sockaddr*)&ctx->peer, sizeof(struct sockaddr_in));
    if (-1 == ret) {
        // Connection error or failure
        LOGERROR("**** Connection failure ****");
        result = ALPACA_ERROR_COMMSCONNECT;
        goto exit;
    }
    
    // TCP Connection established 
    ctx->status = ALPACACOMMS_STATUS_CONN;
    LOGDEBUG("TCP connection established!\n");

    // Create Client ssl object    
    result = AlpacaWolf_createSSL((WOLFSSL*)ctx->protoCtx, ctx->flags);
    if(NULL == ctx->protoCtx || ALPACA_SUCCESS != result ) {
        LOGERROR("Error generating ssl object\n");
        goto exit;
    }
    
    /*
     * Wrap bottom layer TCP socket in wolf
     * then perform TLS handshake 
     */
    if(SSL_SUCCESS != wolfSSL_set_fd(ctx->protoCtx, ctx->fd)){
        LOGERROR("ERROR wolfSSL_set_fd\n");
        result = ALPACA_ERROR_WOLFSSLCREATE;
        goto exit;
    }
    
    if (SSL_SUCCESS != wolfSSL_connect(ctx->protoCtx)) {
        LOGERROR("ERROR failed to connect in wolfSSL\n");
        result = ALPACA_ERROR_WOLFSSLCONNECT;
    }


exit:
    LEAVING;
    if(result == ALPACA_SUCCESS){
        ctx->status = ALPACACOMMS_STATUS_TLSCONN;
    }
    /*
     * At this point TCP connection is possibly valid
     * Top layer will properly handle
     */
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
ALPACA_STATUS AlpacaWolf_close(Alpaca_commsCtx_t* ctx) {
    
    ALPACA_STATUS result = ALPACA_SUCCESS;
    ENTRY;
    LOGDEBUG("Freeing SSL OBJ at [%p]\n", (*sslCtx));
    if(ctx && ctx->protoCtx) {
        wolfSSL_free((WOLFSSL*)ctx->protoCtx);
        ctx->protoCtx = NULL;
        goto done;
    }

    LOGINFO("No SSL object to free\n");

done:
    LEAVING;
    return result;   
}

