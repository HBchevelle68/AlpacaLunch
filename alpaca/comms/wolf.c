#include <interfaces/comms_interface.h>
#include <core/codes.h>

// For Atomics
__atribute__((unused)) static const uint8_t FLAGON  = 1;
__atribute__((unused)) static const uint8_t FLAGOFF = 0;

// Static Globals
static uint8_t wolfInitialized = 0;





AlpacaWolf_init(){

        /*
         * Init wolfSSL and create global wolf_CTX obj
         */
        wolfSSL_Init();
        if ((serverCtx->wolf_ctx = wolfSSL_CTX_new(wolfTLSv1_2_server_method())) == NULL){

            LOGERROR("wolfSSL_CTX_new ERROR: %d", ALPACA_COMMS_TLSINIT);
            return ALPACA_COMMS_TLSINIT;
        }


}

