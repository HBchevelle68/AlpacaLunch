// Internal
#include <crypto.h>
#include <logging.h>


#define SCERT "../certs/cert.pem"
#define PKEY "../certs/prvt.pem"


NS_STATUS init_TLS_1_2(){

    NS_STATUS result = NS_SUCCESS;
    WOLFSSL_CTX *ctx;

    wolfSSL_Init();
    if ((ctx = wolfSSL_CTX_new(wolfTLSv1_2_server_method())) == NULL){
        LOGERROR("wolfSSL_CTX_new error.");
        return NS_TLSINIT;
    }

    /* Load server certificate into CYASSL_CTX */
	if (wolfSSL_CTX_use_certificate_file(ctx, SCERT, SSL_FILETYPE_PEM) != SSL_SUCCESS) {
	    LOGERROR("wolfSSL_CTX_use_certificate_file error");
	    return NS_TLSCERT;
	}

	/* Load server key into CYASSL_CTX */
	if (wolfSSL_CTX_use_PrivateKey_file(ctx, PKEY, SSL_FILETYPE_PEM) != SSL_SUCCESS) {
        LOGERROR("wolfSSL_CTX_use_PrivateKey_file error");
	    return NS_TLSCERT;
	}



    return result;
}