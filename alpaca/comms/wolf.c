#include <wolfssl/options.h>
#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/ssl.h>

#include <interfaces/comms_interface.h>
#include <core/codes.h>
#include <core/logging.h>


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



ALPACA_STATUS AlpacaWolf_init(Alpaca_tlsVersion_t v){

    ALPACA_STATUS result = ALPACA_ERROR_UNKNOWN;
    ENTRY;

    if(!wolfInitialized){
         /*
         * Init wolfSSL and create global wolfCTX obj
         * Currently this is only support TLS 1.2, 
         */
        wolfSSL_Init();
        wolfInitialized = 1;

        if ((procWolfServerCtx = wolfSSL_CTX_new(WOLFTLSVERSION[v].server_method())) == NULL){

            LOGERROR("procWolfServerCtx ERROR: %d", ALAPCA_ERROR_WOLFINIT);
            result = ALAPCA_ERROR_WOLFINIT;
            goto done;
        }
        if ((procWolfClientCtx = wolfSSL_CTX_new(WOLFTLSVERSION[v].client_method())) == NULL){

            LOGERROR("procWolfClientCtx ERROR: %d", ALAPCA_ERROR_WOLFINIT);
            result = ALAPCA_ERROR_WOLFINIT;
            goto done;
        }
        
        
        result = ALPACA_SUCCESS;
    }

done:
    LEAVING;
    return result;
}



ALPACA_STATUS AlpacaWolf_cleanUp(void){

    ALPACA_STATUS result = ALPACA_ERROR_WOLFNOINIT;
   
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

        result = ALPACA_SUCCESS;
    }

    LEAVING;
    return result;
}