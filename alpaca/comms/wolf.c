#include <wolfssl/options.h>
#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/ssl.h>


// Internal
#include <interfaces/comms_interface.h>
#include <interfaces/utility_interface.h>
#include <core/codes.h>
#include <core/logging.h>
#include <comms/wolf.h>



// TEMPORARY
static const char TEMPCERTFILE[] = "/home/ap/AlpacaLunch/.testcerts/cert.pem";
static const char TEMPKEYFILE[] = "/home/ap/AlpacaLunch/.testcerts/private.pem";


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


ALPACA_STATUS AlpacaWolf_init(uint16_t version){

    ALPACA_STATUS result = ALPACA_ERROR_UNKNOWN;
    WOLFLOGGING;
    ENTRY;

    if(version > ALPACACOMMS_PROTO_SSH || version == 0){
        LOGERROR("Version number not in range: %d\n", version);
        result = ALPACA_ERROR_BADPARAM;
        goto exit;
    }

    /**
     * In order to keep this working  
     * shift bit down by 1. Since I use(d)
     * This as the index into the ALPACA_TLSVersion_t
     * 
     * The defines are
     * TLS1.2 == 0001
     * TLS1.3 == 0010
     * 
     * however indexes are: 
     * [0] == TLS1.2
     * [1] == TLS1.3
     * 
     * Bit shift >>1 will now yield:
     * TLS1.2 == 0000
     * TLS1.3 == 0001 
     * 
     * Bit shift is purely local
     */
    version = version>>1;

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
        
        /*
         * SERVER
         */
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
        
        /*
         * CLIENT
         */
        if ((procWolfClientCtx = wolfSSL_CTX_new(WOLFTLSVERSION[version].client_method())) == NULL){
            LOGERROR("procWolfClientCtx ERROR: %d\n", ALAPCA_ERROR_WOLFINIT);
            result = ALAPCA_ERROR_WOLFINIT;
            goto exit;
        }
        
        /* Load server key into WOLFSSL_CTX */
        if (wolfSSL_CTX_use_PrivateKey_file(procWolfClientCtx, TEMPKEYFILE, SSL_FILETYPE_PEM) != SSL_SUCCESS) {
            LOGERROR("ERROR: failed to load %s, please check the file.\n", TEMPKEYFILE);
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


ALPACA_STATUS AlpacaWolf_createSSL(Alpaca_commsCtx_t* ctx, uint16_t flags){

    ALPACA_STATUS result = ALPACA_ERROR_UNKNOWN;
    ENTRY;
    
    if(!ctx){
        result = ALPACA_ERROR_BADPARAM;
        LOGERROR(" Bad param(s) -> ctx:[%p]\n", ctx);
        goto exit;
    }

    if(NULL != ctx->protoCtx){
        result = ALPACA_ERROR_BADPARAM;
        LOGERROR(" Bad param(s) -> ctx->protoCtx:[%p]\n", ctx->protoCtx);
        goto exit;
    }

    switch(GET_COMMS_PROTO(flags)){

        case ALPACACOMMS_PROTO_TLS12:
            /**
             * Create SSL object
             */
            if(ALPACACOMMS_TYPE_CLIENT & flags){
                // CLIENT
                LOGDEBUG("Generating wolfSSL Client object\n");
                if ((ctx->protoCtx = wolfSSL_new(procWolfClientCtx)) == NULL) {
                    LOGERROR("Error from wolfSSL_new, no SSL object created\n");
                    result = ALPACA_ERROR_WOLFSSLCREATE;
                }
                result = ALPACA_SUCCESS;

            }
            else if (ALPACACOMMS_TYPE_SERVER & flags){
                // SERVER
                LOGDEBUG("Generating wolfSSL Server object\n");
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
    LOGDEBUG("Leaving with ctx[%p] ctx->ssl[%p] flags[0x%08x]\n",ctx, ctx->protoCtx, flags);
    LEAVING;
    return result;
}


ALPACA_STATUS AlpacaWolf_listen(Alpaca_commsCtx_t* ctx, uint16_t port){
    
    ENTRY;
	ALPACA_STATUS result = ALPACA_SUCCESS;
    struct sockaddr_in servAddr;
	socklen_t saddr_size = sizeof(servAddr);
    int ret = 0;
    
    /*
     * Port already verified 
     * Verify socket is not still valid
     */
    if(ctx->fd > 0){
        LOGERROR("Socket still valid...bad state\n");
        result = ALPACA_ERROR_BADPARAM;
        goto exit;
    }

    // Set up server address for bind
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(port);
    servAddr.sin_addr.s_addr = INADDR_ANY;

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
    setsockopt(ctx->fd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));

	/* Bind the server socket to our port */
    ret = bind(ctx->fd, (struct sockaddr*)&servAddr, saddr_size);
    if(-1 == ret) {
        LOGERROR("Error failed to bind to port [%d] errno [%d]\n", port, errno);
		result = ALPACA_ERROR_TCPBIND;
        goto exit;
    }

	/* 
     * Listen for a new connection we are only 
     * expecting at most a single connection 
     */
    ret = listen(ctx->fd, 1);
    if(-1 == ret) {
        LOGERROR("Error failed to listen errno [%d]\n", errno);
		result = ALPACA_ERROR_TCPLISTEN;
        goto exit;
    }
	LOGINFO("Successfully listening on port [%d] for TCP connection\n", port);

exit:
    LEAVING;
    return result;
}

static
void close_sleep(int32_t* fd, Alpaca_commsCtx_t* ctx) {

    if(*fd > 0){
        close(*fd);
        *fd = -1;
    } 
    if(NULL != ctx){
        AlpacaWolf_close(ctx);
    }    
	AlpacaUtilities_mSleep(THREE_SECONDS_MILLI);
}
ALPACA_STATUS AlpacaWolf_accept(Alpaca_commsCtx_t* ctx){
    
    ENTRY;
    ALPACA_STATUS result = ALPACA_SUCCESS;
	socklen_t saddr_size = sizeof(struct sockaddr_in);
    int32_t attempts = 0;
    int32_t clientFd = 0;

    /*
	 * Loop attempting to catch connect
	 * If a failure is detected sleep for 3 seconds before
	 * retrying. Retry up to MAX_RETRIES
	 */
	while(attempts < MAX_RETRIES && ctx->status == ALPACACOMMS_STATUS_NOTCONN) {

		LOGINFO("Attepmt #%d to catch TCP connection\n", attempts+1);
		/* 
         * TCP 
         * Accept client connections 
         */
        clientFd = accept(ctx->fd, (struct sockaddr*)&ctx->peer, &saddr_size);
        if (-1 == clientFd) {
            LOGERROR("Failed to accept... errno: %d\n", errno);
			close_sleep(&clientFd, ctx);
            attempts++;
        }
		/* 
         * TCP Connection established 
         */
		ctx->status |= ALPACACOMMS_STATUS_CONN;
		LOGDEBUG("TCP connection established!\n");	
		
        /*
         * Create Client ssl object
         */
        result = AlpacaWolf_createSSL(ctx, ctx->flags);
        if(NULL == ctx->protoCtx || ALPACA_SUCCESS != result ) {
            LOGERROR("Error generating ssl object\n");
            goto exit;
        }

		/*
         * Wrap bottom layer TCP socket in wolf
         * then perform TLS handshake 
         * 
         * @note need to use the newly created client
         * socket
         */
        if(SSL_SUCCESS != wolfSSL_set_fd((WOLFSSL*)ctx->protoCtx, clientFd)){
            LOGERROR("wolfSSL_set_fd error\n");
            close_sleep(&clientFd, ctx);
            ctx->status = ALPACACOMMS_STATUS_NOTCONN;
            attempts++;
            continue;
        }
        
        if (SSL_SUCCESS != wolfSSL_accept((WOLFSSL*)ctx->protoCtx)) {
            LOGERROR("Error failed to accept in wolfSSL\n");
            close_sleep(&clientFd, ctx);
            ctx->status = ALPACACOMMS_STATUS_NOTCONN;
            attempts++;
        }
    }

exit:
    /*
     * Any result not SUCCESS we need to 
     * clean our client connection
     */
    if(ALPACA_SUCCESS != result) {
        LOGERROR("TLS handshake failed!\n");
        if(0 < clientFd){
            close(clientFd);
        }
        AlpacaWolf_close(ctx);
        ctx->status = ALPACACOMMS_STATUS_NOTCONN;
    }
    else {

        LOGINFO("TLS established\n");
        ctx->status |= ALPACACOMMS_STATUS_TLSCONN;
        /*
        * SUCCESS Close listener
        * Swap socket descriptors
        */
	    close(ctx->fd);
	    ctx->fd = clientFd;
        clientFd = -1;
    }

    LEAVING;
    return result;
}


ALPACA_STATUS AlpacaWolf_connect(Alpaca_commsCtx_t* ctx){
    
    ALPACA_STATUS result = ALPACA_SUCCESS;
    int32_t ret = 0;
    ctx->status = ALPACACOMMS_STATUS_NOTCONN;
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
    if (0 != ret) {
        // Connection error or failure
        LOGERROR("**** Connection failure ****\n");
        result = ALPACA_ERROR_COMMSCONNECT;
        goto exit;
    }
    
    
    // TCP Connection established 
    ctx->status |= ALPACACOMMS_STATUS_CONN;
    LOGDEBUG("TCP connection established!\n");

    // Create Client ssl object    
    result = AlpacaWolf_createSSL(ctx, ctx->flags);
    if(NULL == ctx->protoCtx || ALPACA_SUCCESS != result ) {
        LOGERROR("Error generating ssl object\n");
        goto exit;
    }

    LOGDEBUG("Leaving with ctx[%p] ctx->protoCtx[%p] \n",ctx, ctx->protoCtx);
    
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
        ctx->status |= ALPACACOMMS_STATUS_TLSCONN;
    }
    /*
     * At this point TCP connection is possibly valid
     * Top layer will properly handle
     */
    return result;
}


ALPACA_STATUS AlpacaWolf_send(Alpaca_commsCtx_t* ctx, void* buf, size_t len, ssize_t* out){
    ALPACA_STATUS result = ALPACA_SUCCESS;
    ENTRY;

    if(NULL == ctx || NULL == buf || len == 0){
        LOGERROR("AlpacaWolf_send given bad params!\n");
        result = ALPACA_ERROR_BADPARAM;
        *out = -1;
        goto exit;
    }

    *out = wolfSSL_write((WOLFSSL*)ctx->protoCtx, buf, len);
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


ALPACA_STATUS AlpacaWolf_recv(Alpaca_commsCtx_t* ctx, void* buf, size_t len, ssize_t* out){
    ALPACA_STATUS result = ALPACA_SUCCESS;
    ENTRY;

    if(NULL == ctx || NULL == buf || len == 0){
        LOGERROR("AlpacaWolf_recv given bad params!\n");
        result = ALPACA_ERROR_BADPARAM;
        *out = -1;
        goto exit;
    }

    *out = wolfSSL_read((WOLFSSL*)ctx->protoCtx, buf, len);
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
    LOGDEBUG("Leaving with ctx[%p] ctx->ssl[%p] \n",ctx, ctx->protoCtx);
    if(ctx && ctx->protoCtx) {
        LOGDEBUG("Freeing SSL OBJ at [%p]\n", (ctx->protoCtx));
        wolfSSL_free((WOLFSSL*)ctx->protoCtx);
        ctx->protoCtx = NULL;
        goto done;
    }

    LOGINFO("No SSL object to free\n");

done:
    LEAVING;
    return result;   
}

