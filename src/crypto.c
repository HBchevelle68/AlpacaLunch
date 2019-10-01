// Internal
#include <crypto.h>
#include <logging.h>


/*
 * TO DO
 * This needs to be configurable at runtime
 */
#define SCERT "../certs/cert.pem"
#define PKEY "../certs/prvt.pem"


NS_STATUS NS_init_TLS(NS_server_t *serv){

    NS_STATUS result = NS_SUCCESS;

    wolfSSL_Init();
    if ((serv->tls_ctx = wolfSSL_CTX_new(wolfTLSv1_2_server_method())) == NULL){
        LOGERROR("wolfSSL_CTX_new ERROR: %d", NS_TLSINIT);
        return NS_TLSINIT;
    }

    /* Load server cert into wolfSSL_CTX */
	if (wolfSSL_CTX_use_certificate_file(serv->tls_ctx, SCERT, SSL_FILETYPE_PEM) != SSL_SUCCESS) {
	    LOGERROR("wolfSSL_CTX_use_certificate_file ERROR: %d", NS_TLSCERT);
	    return NS_TLSCERT;
	}

	/* Load server key into wolfSSL_CTX */
	if (wolfSSL_CTX_use_PrivateKey_file(serv->tls_ctx, PKEY, SSL_FILETYPE_PEM) != SSL_SUCCESS) {
        LOGERROR("wolfSSL_CTX_use_PrivateKey_file ERROR: %d", NS_TLSKEY);
	    return NS_TLSKEY;
	}

    return result;
}

/*
 * TLS wrap a socket 
 */
WOLFSSL* NS_gen_local_TLS(NS_server_t *serv, uint16_t cli_sock){

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


NS_STATUS NS_init_RSA(NS_server_t *serv){

    RsaKey priv;
    RsaKey pub;
    WC_RNG rng;
    int ret = 0;
    long e = 65537; // standard value to use for exponent

    wc_InitRsaKey(&priv, NULL); // not using heap hint. No custom memory
    wc_InitRng(&rng);

    // generate 2048 bit long private key
    ret = wc_MakeRsaKey(&priv, 2048, e, &rng);
    if( ret != 0 ) {
        // error generating private key
    }
}

/*
NS_STATUS NS_gen_hash(char* buf){
    
    int ret;
    unsigned char digest[SHA224_DIGEST_SIZE];
    Sha256 sha;

    if ((ret = wc_InitSha256(&sha)) != 0) {
        LOGERROR("wc_InitSha256 failed");
        return NS_SUCCESS;
    }
    
}
*/