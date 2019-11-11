// Internal
#include <crypto.h>
#include <logging.h>


/*
 * TO DO
 * This needs to be configurable at runtime
 */
#define SCERT "../certs/cert.pem"
#define PKEY "../certs/prvt.pem"


ALPACA_STATUS alpacacore_init_TLS(allu_server_t *serv){

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

/*
 * TLS wrap a socket 
 */
WOLFSSL* alpacacore_wrap_sock(allu_server_t *serv, uint16_t cli_sock){

    WOLFSSL* tls;
    if((tls = wolfSSL_new(serv->tls_ctx)) == NULL) {
		   LOGERROR("wolfSSL_new error\n");
		   return NULL;
	}
	if(wolfSSL_set_fd(tls, cli_sock) != SSL_SUCCESS){
           LOGERROR("wolfSSL_set_fd error\n");
		   return NULL;
    }
    return tls;
}


/*
ALPACA_STATUS alpacacore_gen_hash(char* buf){
    
    int ret;
    unsigned char digest[SHA224_DIGEST_SIZE];
    Sha256 sha;

    if ((ret = wc_InitSha256(&sha)) != 0) {
        LOGERROR("wc_InitSha256 failed");
        return NS_SUCCESS;
    }
    
}
*/