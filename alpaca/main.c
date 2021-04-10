//#define _GNU_SOURCE // Temp for test
// Standard
#include <stdio.h> 
#include <time.h>

// Internal
#include <core/logging.h> 
#include <core/coreloop.h>

// Interfaces 
#include <interfaces/comms_interface.h>
#include <interfaces/utility_interface.h>
#include <core/config.h>
/*
 * Comms context leading back to home
 * See comms/comms.c
 */
extern Alpaca_commsCtx_t *coreComms;
//extern Alpaca_config_t alpaca_config;




ALPACA_STATUS AlpacaCore_init(uint16_t commsFlags){
    ENTRY;
    ALPACA_STATUS result = ALPACA_SUCCESS;

    DEBUGWARNING();

    /*
     * Initialize configuration
     */
    result = AlpacaConfig_init();
    if(result){
        LOGERROR("Error initializing Alpaca Config\n");
        goto exit;
    }
    /*
     * Initialize comms
     */
    result = AlpacaComms_init(commsFlags);
    if(result){
        LOGERROR("Error initializing Alpaca Comms\n");
        goto exit;
    }
    
   
exit:
    LEAVING;
    return result;
}


/*
 * Small for now. Called by sighandler
 * will get expandaed on
 */
ALPACA_STATUS AlpacaCore_exit(void){
    ENTRY;
    ALPACA_STATUS result = ALPACA_SUCCESS;
    
    result = AlpacaComms_cleanUp();
    if(result){
        LOGERROR("Error cleaning process comms\n");
    }

    LEAVING;
    return result;
}



int main(int argc, char** argv){
    
    ENTRY;
    ALPACA_STATUS result = ALPACA_SUCCESS;

    /** 
     * Immediately daemonize ourself according to *nix spec
     * See more in daemon(7)
     * 
     * When debug is enabled this is an empty function
     */
    AlpacaUtilities_daemonize();

    /** 
     * When debug is enabled this is an empty function
     */
    result = AlpacaCore_init(ALPACACOMMS_PROTO_TLS12);
    if(ALPACA_SUCCESS != result){
        LOGERROR("Global initialization error! [%u]\n", result);
        goto exit;
    }

    // Need to perform initial comms

    if(ALPACACOMMS_INIT_LISTEN == alpaca_config.initial_behavior){
        // Init core comms
        result = AlpacaComms_initCtx(&coreComms, ALPACACOMMS_TYPE_SERVER | ALPACACOMMS_PROTO_TLS12);
        if(ALPACA_SUCCESS != result){
            LOGERROR("Core Comms initialization failure with result [%u]\n", result);
            goto exit;
        }

        // Listen
        result = AlpacaComms_initialCallback(coreComms);    
        if(ALPACA_SUCCESS != result){
            LOGERROR("Initial listen exited with result [%u]\n", result);
            goto exit;
        }
    }
    else if(ALPACACOMMS_INIT_CALLBACK == alpaca_config.initial_behavior) {

        // Init core comms
        result = AlpacaComms_initCtx(&coreComms, ALPACACOMMS_TYPE_CLIENT | ALPACACOMMS_PROTO_TLS12);
        if(ALPACA_SUCCESS != result){
            LOGERROR("Core Comms initialization failure with result [%u]\n", result);
            goto exit;
        }

        // Callback
        result = AlpacaComms_initialCallback(coreComms);    
        if(ALPACA_SUCCESS != result){
            LOGERROR("Initial callback exited with result [%u]\n", result);
            goto exit;
        }
    }
    else {
        result = ALPACA_ERROR_CONFINITBEH;
        goto exit;
    }
    
    /*
     * Launch coreloop
     * 
     * This likely will need to be more complex 
     * as time goes on. For now its simple
     */
    result = AlpacaCore_coreLoop();
    if(ALPACA_SUCCESS != result){
        LOGERROR("Coreloop exited with result [%u]\n", result);
        goto exit;
    }

   
exit:
    AlpacaCore_exit();
    LOGINFO("Exiting with exit code: %u\n", result);
    return 0; 
}
