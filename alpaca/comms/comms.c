// Implements interface 
#include <interfaces/comms_interface.h>

// Internal
#include <core/logging.h>

short AlpacaComms_create_listen_sock(uint16_t port, uint32_t listen_count){
	return 0;
}

 
void AlpacaComms_connection_handler(ALLU_comms_ctx *ctx, uint32_t cli_sock, struct sockaddr_in* cliaddr){

    WOLFSSL* l_tls;
    uint32_t n;
    char* buff;
    
    
    buff = (char*)calloc(COMMSBUFSIZE, sizeof(unsigned char));
                                 

    /* Wrap socket */ 
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


    /*
     * Free buffers and free wolfssl structs
     */
    if(buff != NULL) {
          memset(buff, (unsigned char)0, COMMSBUFSIZE);
          free(buff);
          buff = NULL;
    }
    wolfSSL_free(l_tls);
}

/*
 * TLS wrap a socket 
 */
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

    wolfSSL_Init();
    if ((serv->tls_ctx = wolfSSL_CTX_new(wolfTLSv1_2_server_method())) == NULL){
        LOGERROR("wolfSSL_CTX_new ERROR: %d", ALPACA_TLSINIT);
        return ALPACA_TLSINIT;
    }

    /* Load server cert into wolfSSL_CTX */
	if (wolfSSL_CTX_use_certificate_file(serv->tls_ctx, SCERT, SSL_FILETYPE_PEM) != SSL_SUCCESS) {
	    LOGERROR("wolfSSL_CTX_use_certificate_file ERROR: %d", ALPACA_TLSCERT);
	    return ALPACA_TLSCERT;
	}

	/* Load server key into wolfSSL_CTX */
	if (wolfSSL_CTX_use_PrivateKey_file(serv->tls_ctx, PKEY, SSL_FILETYPE_PEM) != SSL_SUCCESS) {
        LOGERROR("wolfSSL_CTX_use_PrivateKey_file ERROR: %d", ALPACA_TLSKEY);
	    return ALPACA_TLSKEY;
	}

    return result;
}

void AlpacaComms_clean_comms_ctx(ALLU_comms_ctx *ctx){
	
	if(ctx != NULL){
        
        if(ctx->sock > 0) {
            close(ctx->sock);
        }
        
        if(ctx->tls_ctx != NULL){
            wolfSSL_CTX_free(ctx->tls_ctx);
        }
        
        if(ctx->serv_addr != NULL) {
			memset(ctx->serv_addr, (unsigned char)0, sizeof(struct sockaddr_in));
			free(ctx->serv_addr);
			ctx->serv_addr = NULL;
    	}
        
        if(ctx != NULL) {
			memset(ctx, (unsigned char)0, sizeof(*ctx));
			free(ctx);
			ctx = NULL;
    	}
    }
}