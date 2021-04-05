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

    alpaca_config.initial_behavior = 1;
    
    printf("here\n");

    AlpacaConfig_init();
    /*
     * Initialize process wide comms
     */
    result = AlpacaComms_init(commsFlags);
    if(result){
        LOGERROR("Error initializing Alpaca Comms\n");
        goto exit;
    }
    
    /*
     * Initialize comms context for connect/listen
     * This context is for our connection to the controller
     * 
     * TODO - flags should be pulled from a configuration
     *        hardcoded for now
     */
    result = AlpacaComms_initCtx(&coreComms, ALPACACOMMS_PROTO_TLS12 | ALPACACOMMS_TYPE_CLIENT);
    if(result != ALPACA_SUCCESS){
        LOGERROR("Global comms ctx error! [%u]\n", result);
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
    //AlpacaUtilities_daemonize();

    /** 
     * When debug is enabled this is an empty function
     */
    result = AlpacaCore_init(ALPACACOMMS_PROTO_TLS12);
    if(ALPACA_SUCCESS != result){
        LOGERROR("Global initialization error! [%u]\n", result);
        goto done;
    }

    // Need to perform initial comms


    result = AlpacaComms_initialCallback(coreComms);
    if(ALPACA_SUCCESS != result){
        LOGERROR("Initial Comms exited with result [%u]\n", result);
        goto done;
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
        goto done;
    }

   
done:
    AlpacaCore_exit();
    LOGINFO("Exiting with exit code: %u\n", result);
    return 0; 
}
