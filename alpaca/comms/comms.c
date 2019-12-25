// Implements interface 
#include <interfaces/comms_interface.h>

// Internal
#include <core/macros.h>
#include <core/logging.h>

ALPACA_STATUS AlpacaComms_create_listen_sock(ALLU_comms_ctx *ctx, uint16_t port, uint32_t listen_count){
	/* Init WolfSSL */
    FAIL_IF_TRUE(AlpacaComms_init_TLS(ctx));
 
    if ((ctx->sock = socket(AF_INET, SOCK_STREAM, 0)) == 0) { 
        LOGERROR("Socket creation failure\n");
        return ALPACA_FAILURE;
    } 

    ctx->serv_addr->sin_family = AF_INET;          
    ctx->serv_addr->sin_addr.s_addr = INADDR_ANY;  
    ctx->serv_addr->sin_port = BEU16(port);

    FAIL_IF_TRUE(REUSEADDR(ctx->sock));
    FAIL_IF_TRUE(BIND(ctx->sock, ctx->serv_addr));
    FAIL_IF_TRUE(LISTEN(ctx->sock,listen_count));
    
    LOGDEBUG(">>> Sock_FD: %d Bound to %s:%hu <<<\n", ctx->sock, 
                inet_ntoa(ctx->serv_addr->sin_addr), HU16(ctx->serv_addr->sin_port));

	return ALPACA_SUCCESS;
}

 
void AlpacaComms_connection_handler(ALLU_comms_ctx *ctx, uint32_t cli_sock, struct sockaddr_in* cliaddr){

    WOLFSSL* l_tls;
    uint32_t n;
    char* buff;
        
    buff = (char*)calloc(COMMSBUFSIZE, sizeof(unsigned char));
                                 
    /* 
     * Wrap socket 
     */ 
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