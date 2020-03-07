// Implements interface 
#include <interfaces/comms_interface.h>


// Internal
#include <interfaces/memory_interface.h>
#include <core/macros.h>
#include <core/logging.h>
#include <core/codes.h>


// TO DO
// Needs to be dynamic
#define SCERT "/home/ap/AlpacaLunch/.testcerts/cert.pem"
#define PKEY "/home/ap/AlpacaLunch/.testcerts/private.pem"
#define DEFAULTPORT 12345

// For Atomics
static const uint8_t FLAGON  = 1;
static const uint8_t FLAGOFF = 0;

// Static Globals
static uint8_t wolfInitialized = 0;

// Globals
ALLU_server_ctx* serverCtx = NULL; 



/*
 * @brief initialize 
 */
ALPACA_STATUS AlpacaComms_initServerComms(void){


    if(!(__atomic_load_n(&wolfInitialized, __ATOMIC_SEQ_CST))){

        /*
         * Create global Alpaca Server Context obj
         */
        serverCtx = (ALLU_server_ctx*)calloc(sizeof(ALLU_server_ctx), sizeof(uint8_t));
        if(!serverCtx){
            return ALPACA_COMMS_INITFAIL;
        }

        /*
         * Init wolfSSL and create global wolf_CTX obj
         */
        wolfSSL_Init();
        if ((serverCtx->wolf_ctx = wolfSSL_CTX_new(wolfTLSv1_2_server_method())) == NULL){

            LOGERROR("wolfSSL_CTX_new ERROR: %d", ALPACA_COMMS_TLSINIT);
            return ALPACA_COMMS_TLSINIT;
        }

        /* 
         * Load server cert into wolfSSL_CTX 
         */
        LOGDEBUG("Opening cert file %s\n", SCERT);
        if (wolfSSL_CTX_use_certificate_file(serverCtx->wolf_ctx, SCERT, SSL_FILETYPE_PEM) != SSL_SUCCESS) {

            LOGERROR("wolfSSL_CTX_use_certificate_file ERROR: %d\n", ALPACA_COMMS_TLSCERT);
            return ALPACA_COMMS_TLSCERT;
        }

        /*
         * Load server key into wolfSSL_CTX 
         */
        LOGDEBUG("Opening cert file %s\n", PKEY);
        if (wolfSSL_CTX_use_PrivateKey_file(serverCtx->wolf_ctx, PKEY, SSL_FILETYPE_PEM) != SSL_SUCCESS) {

            LOGERROR("wolfSSL_CTX_use_PrivateKey_file ERROR: %d\n", ALPACA_COMMS_TLSKEY);
            return ALPACA_COMMS_TLSKEY;
        }

        /*
         * Create underlying server socket
         */
        if(AlpacaSock_createServSock(&serverCtx) != ALPACA_SUCCESS){
            return ALPACA_COMMS_SOCKERR;
        }

        LOGDEBUG(">>> Sock_FD: %d Bound to %s:%hu <<<\n", serverCtx->serverSock, 
                inet_ntoa(serverCtx->servAddr->sin_addr), ntohs(serverCtx->servAddr->sin_port));

        // Set global flag
        //wolfInitialized = 1;
        __atomic_store(&wolfInitialized, &FLAGON, __ATOMIC_SEQ_CST);
    }

    return ALPACA_SUCCESS;
}


ALPACA_STATUS AlpacaComms_cleanServerComms(void){

    if(__atomic_load_n(&wolfInitialized, __ATOMIC_SEQ_CST) && serverCtx != NULL){
        
        /*
         * Make sure we have a socket first 
         * close and set to 0
         */    
        if(serverCtx->serverSock > 0) {
            close(serverCtx->serverSock);
            serverCtx->serverSock = 0;
        }
        
        /*
         * Clean up wolf ctx obj and set to NULL
         */            
        if(serverCtx->wolf_ctx != NULL){
            wolfSSL_CTX_free(serverCtx->wolf_ctx);
            serverCtx->wolf_ctx = NULL;
        }

        /*
         * Clean up servAddr structure and set to NULL
         */                    
        if(serverCtx->servAddr != NULL) {
            memset(serverCtx->servAddr, (unsigned char)0, sizeof(struct sockaddr_in));
            free(serverCtx->servAddr);
            serverCtx->servAddr = NULL;
        }

        /*
         * Clean up global server context
         */                                       
        memset(serverCtx, (uint8_t)0, sizeof(*serverCtx));
        free(serverCtx);
        serverCtx = NULL;
        
        /*
         * Lastly, teardwon wolfSSL
         */                    
        wolfSSL_Cleanup();

        //wolfInitialized = 0;
         __atomic_fetch_and(&wolfInitialized, &FLAGOFF, __ATOMIC_SEQ_CST);
    }
    return ALPACA_SUCCESS;
}




/*
 
void AlpacaComms_connection_handler(ALLU_comms_ctx *ctx, uint32_t cli_sock, struct sockaddr_in* cliaddr){

    WOLFSSL* l_tls;
    uint32_t n;
    char* buff;
        
    buff = (char*)calloc(COMMSBUFSIZE, sizeof(unsigned char));
                                 
 
    l_tls = AlpacaComms_wrap_sock(ctx, cli_sock);
    

    LOGDEBUG("Reading from tls1.2 socket\n");
    while ( (n = wolfSSL_read(l_tls, buff, COMMSBUFSIZE) ) > 0 ){
        
        if( n < 0 ){
            LOGERROR("wolfSSL_read error = %d\n", wolfSSL_get_error(l_tls,n));
            break;
        }
        
        LOGDEBUG("FROM CONNECTION: %s\n", buff);
        LOGTESTFILE(buff);

        memset(buff, 0, COMMSBUFSIZE);
    }


    if(buff != NULL) {
          memset(buff, (unsigned char)0, COMMSBUFSIZE);
          free(buff);
          buff = NULL;
    }
    wolfSSL_free(l_tls);
}



WOLFSSL* AlpacaComms_wrap_sock(ALLU_comms_ctx *ctx, uint16_t cli_sock){

    WOLFSSL* tls;
    if((tls = wolfSSL_new(ctx->tls_ctx)) == NULL) {
		   LOGERROR("wolfSSL_new error\n");
		   return NULL;
	}
	if(wolfSSL_set_fd(tls, cli_sock) != SSL_SUCCESS){
           LOGERROR("wolfSSL_set_fd error\n");
		   return NULL;
    }
    return tls;
}

ALPACA_STATUS AlpacaComms_init_TLS(ALLU_comms_ctx *serv){

    ALPACA_STATUS result = ALPACA_SUCCESS;


    return result;
}
*/
