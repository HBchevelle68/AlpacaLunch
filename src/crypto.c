// Internal
#include <crypto.h>
#include <logging.h>


/*
 * TO DO
 * This needs to be configurable at runtime
 */

#define SCERT "../certs/cert.pem"
#define PKEY "../certs/prvt.pem"


NS_STATUS NS_init_TLS_1_2(NS_server_t *serv){

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